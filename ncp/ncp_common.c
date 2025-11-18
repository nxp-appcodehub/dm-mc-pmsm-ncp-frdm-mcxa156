/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ncp_common.h"
#include "fsl_lpspi_edma.h"
#include "fsl_lpspi.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "app.h"
#include "checksum_generator.h"
#include "fsl_debug_console.h"

extern lpspi_master_edma_handle_t g_s_edma_handle;

SemaphoreHandle_t g_tx_sem;
SemaphoreHandle_t g_rx_sem;
SemaphoreHandle_t g_shared_resources_sem;

void ncp_api_init(void)
{
	g_tx_sem = xSemaphoreCreateBinary();
	g_rx_sem = xSemaphoreCreateBinary();
	g_shared_resources_sem = xSemaphoreCreateBinary();
	xSemaphoreGive(g_shared_resources_sem);
}

// Communication SPI Functions -------------------------------------------------------------------------------------
void ncp_rx_gpio_callback(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
	xSemaphoreGiveFromISR(g_rx_sem, &pxHigherPriorityTaskWoken);
}

void ncp_tx_gpio_callback(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
	xSemaphoreGiveFromISR(g_tx_sem, &pxHigherPriorityTaskWoken);
}

void ncp_wait_ncp_device_ready_for_reception(void)
{
	xSemaphoreTake(g_rx_sem, portMAX_DELAY);
}

void ncp_wait_ncp_device_ready_for_transmision(void)
{
	xSemaphoreTake(g_tx_sem, portMAX_DELAY);
}

void ncp_send(uint8_t *buff, uint32_t buff_size)
{
	uint8_t c_send_array[4] = {'s', 'e', 'n', 'd'};
    uint32_t extra_header_data;
    uint32_t transfer_len;
    lpspi_transfer_t dma_transfer;
    uint32_t checksum;
    uint8_t *aux_buff;

	xSemaphoreTake(g_shared_resources_sem, portMAX_DELAY);
    extra_header_data = buff_size - sizeof(NCP_COMMAND);

    // Start send command -------------------------------------------------------------------
	dma_transfer.txData      = c_send_array;
	dma_transfer.rxData      = NULL;
	dma_transfer.dataSize    = sizeof(c_send_array);
	dma_transfer.configFlags = LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterByteSwap;
    LPSPI_MasterTransferEDMA(LPSPI_MASTER_BASEADDR, &g_s_edma_handle, &dma_transfer);
	ncp_wait_ncp_device_ready_for_reception();
    // --------------------------------------------------------------------------------------

    // Send header --------------------------------------------------------------------------
	dma_transfer.txData      = buff;
	dma_transfer.rxData      = NULL;
	dma_transfer.dataSize    = sizeof(NCP_COMMAND);
	dma_transfer.configFlags = LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterByteSwap;
    LPSPI_MasterTransferEDMA(LPSPI_MASTER_BASEADDR, &g_s_edma_handle, &dma_transfer);
	ncp_wait_ncp_device_ready_for_reception();
    // --------------------------------------------------------------------------------------

    // Send data ----------------------------------------------------------------------------
	aux_buff = buff+sizeof(NCP_COMMAND);
    checksum = checksum_generator(buff, buff_size);
    *((uint32_t*)(aux_buff+extra_header_data)) = checksum;
	extra_header_data += sizeof(checksum);
    while(0 < extra_header_data)
    {
    	transfer_len = (1024 < extra_header_data)? 1024:extra_header_data;
        extra_header_data -= transfer_len;
    	dma_transfer.txData      = aux_buff;
    	dma_transfer.rxData      = NULL;
    	dma_transfer.dataSize    = transfer_len;
    	dma_transfer.configFlags = LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterByteSwap;
        LPSPI_MasterTransferEDMA(LPSPI_MASTER_BASEADDR, &g_s_edma_handle, &dma_transfer);
    	ncp_wait_ncp_device_ready_for_reception();
    	aux_buff += transfer_len;

    }
    // --------------------------------------------------------------------------------------
	xSemaphoreGive(g_shared_resources_sem);
}


void ncp_receive(uint8_t *buff, uint32_t *buff_size)
{
	uint8_t c_rcv_array[4] = {'r', 'e', 'c', 'v'};
    NCP_COMMAND *header;
    uint32_t reception_size;
    uint32_t extra_header_reception;
    uint32_t transfer_len;
    lpspi_transfer_t dma_transfer;
    uint8_t *aux_buff;
    uint32_t checksum;
    uint32_t checksum_rcv;

    ncp_wait_ncp_device_ready_for_transmision();
	xSemaphoreTake(g_shared_resources_sem, portMAX_DELAY);
    // Start recv command -------------------------------------------------------------------
	dma_transfer.txData      = c_rcv_array;
	dma_transfer.rxData      = NULL;
	dma_transfer.dataSize    = sizeof(c_rcv_array);
	dma_transfer.configFlags = LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterByteSwap;
    LPSPI_MasterTransferEDMA(LPSPI_MASTER_BASEADDR, &g_s_edma_handle, &dma_transfer);
	ncp_wait_ncp_device_ready_for_reception();
    // --------------------------------------------------------------------------------------

    // Receive header -----------------------------------------------------------------------
    aux_buff = buff;
	dma_transfer.rxData      = aux_buff;
	dma_transfer.dataSize    = sizeof(NCP_COMMAND);
	dma_transfer.configFlags = LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterByteSwap;
    LPSPI_MasterTransferEDMA(LPSPI_MASTER_BASEADDR, &g_s_edma_handle, &dma_transfer);
	ncp_wait_ncp_device_ready_for_reception();
    // --------------------------------------------------------------------------------------

    // Receive data -------------------------------------------------------------------------
    header = (NCP_COMMAND*)aux_buff;
    aux_buff += sizeof(NCP_COMMAND);
	reception_size = header->size;
	*buff_size = reception_size;
	extra_header_reception = (reception_size - sizeof(NCP_COMMAND) + sizeof(checksum));
    while(0 < extra_header_reception)
    {
    	transfer_len = (1024 < extra_header_reception)? 1024:extra_header_reception;
    	extra_header_reception -= transfer_len;
    	dma_transfer.txData      = NULL;
    	dma_transfer.rxData      = aux_buff;
    	dma_transfer.dataSize    = transfer_len;
    	dma_transfer.configFlags = LPSPI_MASTER_PCS_FOR_TRANSFER | kLPSPI_MasterByteSwap;
        LPSPI_MasterTransferEDMA(LPSPI_MASTER_BASEADDR, &g_s_edma_handle, &dma_transfer);
		ncp_wait_ncp_device_ready_for_reception();
		aux_buff += transfer_len;
    }
	checksum_rcv = *((uint32_t*)(aux_buff - sizeof(checksum)));
	checksum = checksum_generator(buff, reception_size);
	if(checksum_rcv != checksum)
	{
		PRINTF("Error in Checksum\n\r");
	}
    // --------------------------------------------------------------------------------------
	xSemaphoreGive(g_shared_resources_sem);
}
// -----------------------------------------------------------------------------------------------------------------
