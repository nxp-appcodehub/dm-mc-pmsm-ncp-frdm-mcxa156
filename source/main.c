/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "clock_config.h"
#include "board.h"
#include "motor_control.h"
#include "app.h"
#include "ncp_wifi_api.h"
#include "fsl_lpspi_edma.h"
#include "fsl_lpspi.h"
#include "commands.h"
#include "checksum_generator.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define AP_SSID     "NXP_NCP_Motor_Control"
#define AP_PASSWORD "0123456789"


/*******************************************************************************
 * Variables
 ******************************************************************************/
lpspi_master_edma_handle_t g_s_edma_handle;

static edma_handle_t g_edma_rx_handle;
static edma_handle_t g_edma_tx_handle;

static uint8_t g_transfer_buff_receive[RECEIVE_BUFF_SIZE] = {0};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void ncp_init(void);
uint32_t string_to_int(uint8_t *buff, uint8_t buff_size);
uint32_t int_to_string(uint8_t *buff, uint32_t max_buff_size, float number, uint8_t isFloat);

/*******************************************************************************
 * Code
 ******************************************************************************/

void ncp_socket_connection(uint32_t *socket_handler_ret, uint32_t *socket_accept_handler_ret)
{
    NCPCmd_DS_COMMAND *ncp_command;
    uint8_t ip_addr[IP_ADDR_LEN] = {0};
	uint32_t buff_size;
    uint32_t command;
    uint32_t socket_handler;

    ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_receive;
    // Socket Open --------------------------------------------------------
	ncp_wlan_socket_open(k_socket_tcp);
    do 
    {
        ncp_receive(g_transfer_buff_receive, &buff_size);
        command = ncp_command->header.cmd;
    }while(command != NCP_RSP_SOCKET_OPEN);
    if(ncp_command->header.result == NCP_CMD_RESULT_OK)
    {
        PRINTF("Socket opened\n\r");
    }
    else
    {
        PRINTF("Error oppening socket\n\r");
        vTaskSuspend(NULL);
    }
    socket_handler = ncp_command->params.wlan_socket_open.opened_handle;
    *socket_handler_ret = socket_handler;
    // --------------------------------------------------------------------

    // Socket Bind --------------------------------------------------------
    strcpy((char*)ip_addr, "192.168.1.1");
	ncp_wlan_socket_bind(socket_handler, ip_addr, 8000);
    do 
    {
        ncp_receive(g_transfer_buff_receive, &buff_size);
        command = ncp_command->header.cmd;
    }while(command != NCP_RSP_SOCKET_BIND);
    if(ncp_command->header.result == NCP_CMD_RESULT_OK)
    {
        PRINTF("Socket bind success\n\r");
    }
    else
    {
        PRINTF("Error in socket bind\n\r");
        vTaskSuspend(NULL);
    }
	PRINTF("Socket IP 	: %s \n\r", ip_addr);
	PRINTF("Socket Port	: %d \n\r", 8000);
    // --------------------------------------------------------------------

    // Socket Listen ------------------------------------------------------
	ncp_wlan_socket_listen(socket_handler, 1);
    do 
    {
        ncp_receive(g_transfer_buff_receive, &buff_size);
        command = ncp_command->header.cmd;
    }while(command != NCP_RSP_SOCKET_LISTEN);
    if(ncp_command->header.result == NCP_CMD_RESULT_OK)
    {
        PRINTF("Socket listen success\n\r");
    }
    else
    {
        PRINTF("Error in socket listen\n\r");
        vTaskSuspend(NULL);
    }
    // --------------------------------------------------------------------

    // Socket Accept ------------------------------------------------------
    do
    {
		vTaskDelay(pdMS_TO_TICKS(500));
        PRINTF("Trying to accept socket\n\r");
	    ncp_wlan_socket_accept(socket_handler, 1);
        ncp_receive(g_transfer_buff_receive, &buff_size);
    }while(ncp_command->header.result != NCP_CMD_RESULT_OK || ncp_command->header.cmd != NCP_RSP_SOCKET_ACCEPT);
    socket_handler = ncp_command->params.wlan_socket_accept.accepted_handle;
    *socket_accept_handler_ret = socket_handler;
    PRINTF("Connected\n\r");
    // --------------------------------------------------------------------
    vTaskDelay(pdMS_TO_TICKS(500));
}

void ncp_task(void *args)
{
    uint8_t send_buff[20] = "0000,00.00,0.00";
    NCPCmd_DS_COMMAND *ncp_command;
	uint32_t buff_size;
    uint32_t command;
    uint32_t socket_handler;
    uint32_t socket_handler_accept;
    float rpm;
    float voltage;
    float current;
    uint32_t offset_string;

    ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_receive;
    
    vTaskDelay(pdMS_TO_TICKS(3000));
    PRINTF("NCP Task\n\r");
    
    // Configure Access Point ---------------------------------------------
	ncp_wlan_uap_provider_set_config((uint8_t*)AP_SSID, strlen(AP_SSID),
									 (uint8_t*)AP_PASSWORD, strlen(AP_PASSWORD),
									 k_wlan_security_wpa2);
    do 
    {
        ncp_receive(g_transfer_buff_receive, &buff_size);
        command = ncp_command->header.cmd;
    }while(command != NCP_RSP_WLAN_BASIC_WLAN_UAP_PROV_SET_UAPCFG);
    if(ncp_command->header.result == NCP_CMD_RESULT_OK)
    {
        PRINTF("Access point configured\n\r");
    }
    else
    {
        PRINTF("Error configuring Access point\n\r");
        vTaskSuspend(NULL);
    }
    // --------------------------------------------------------------------

    // Start Access Point -------------------------------------------------
    ncp_wlan_uap_provider_start();
    do 
    {
        ncp_receive(g_transfer_buff_receive, &buff_size);
        command = ncp_command->header.cmd;
    }while(command != NCP_RSP_WLAN_BASIC_WLAN_UAP_PROV_START);
    if(ncp_command->header.result == NCP_CMD_RESULT_OK)
    {
        PRINTF("Access point started\n\r");
    }
    else
    {
        PRINTF("Error starting Access point\n\r");
        vTaskSuspend(NULL);
    }
    // --------------------------------------------------------------------
    
    ncp_socket_connection(&socket_handler, &socket_handler_accept);

    while(1)
    {
        // Socket Receive -------------------------------------------------
        PRINTF("Receiving\n\r");
		ncp_wlan_socket_receive(socket_handler_accept, 50, 2000);
        ncp_receive(g_transfer_buff_receive, &buff_size);
        if((ncp_command->header.cmd == NCP_RSP_SOCKET_RECV)&&(ncp_command->header.result == NCP_CMD_RESULT_OK))
        {
            if(ncp_command->params.wlan_socket_receive.recv_size <= 5)
            {
                rpm = (float)string_to_int((uint8_t*)ncp_command->params.wlan_socket_receive.recv_data,ncp_command->params.wlan_socket_receive.recv_size);
                PRINTF("RPM Reception: %f\n\r", rpm);
                motor_control_set_speed(rpm);
            }
            else
            {
                PRINTF("Update Reception\n\r");
                // Socket Send ----------------------------------------------------
                rpm = (float)((uint32_t)motor_control_get_speed());
                voltage = motor_control_get_voltage();
                current = motor_control_get_current();
                offset_string = 0;

                offset_string = int_to_string(send_buff, 4, rpm, 0);
                send_buff[offset_string] = ',';
                offset_string ++;

                offset_string += int_to_string(send_buff+offset_string, 5, voltage, 1);
                send_buff[offset_string] = ',';
                offset_string ++;

                offset_string += int_to_string(send_buff+offset_string, 4, current, 1);
                send_buff[offset_string] = 0;
                
                PRINTF("Sending\n\r");
		        ncp_wlan_socket_send(socket_handler_accept, (uint8_t*)send_buff, offset_string);
                ncp_receive(g_transfer_buff_receive, &buff_size);
                // ----------------------------------------------------------------
            }
        }
        else if((ncp_command->header.cmd == NCP_RSP_SOCKET_RECV)&&(ncp_command->header.result != NCP_CMD_RESULT_OK))
        {
            PRINTF("Communication losted\n\r");
            motor_control_set_speed(0);
            ncp_wlan_socket_close(socket_handler);
            ncp_receive(g_transfer_buff_receive, &buff_size);
            ncp_socket_connection(&socket_handler, &socket_handler_accept);
        }
        // ----------------------------------------------------------------

    }
}



int main(void)
{
    BOARD_InitHardware();
    motor_control_init();
    ncp_init();

    PRINTF("NCP Motor Control Demo\n\r");

    xTaskCreate(motor_control_app_task, "motor_control_app", 512, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(ncp_task, "ncp", 512, NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();

    while (1);
    return 0;
}

void ncp_init(void)
{
	uint32_t srcClock_Hz;
    edma_config_t userConfig = {0};
    lpspi_master_config_t master_config;

    // SPI with DMA init -------------------------------------------------------------------------------------------
    EDMA_GetDefaultConfig(&userConfig);
    EDMA_Init(LPSPI_DMA, &userConfig);

    LPSPI_MasterGetDefaultConfig(&master_config);
    master_config.baudRate = TRANSFER_BAUDRATE;
    master_config.whichPcs = LPSPI_MASTER_PCS_FOR_INIT;
    master_config.pcsToSckDelayInNanoSec        = 1000000000U / (master_config.baudRate * 2U);
    master_config.lastSckToPcsDelayInNanoSec    = 1000000000U / (master_config.baudRate * 2U);
    master_config.betweenTransferDelayInNanoSec = 1000000000U / (master_config.baudRate * 2U);

    srcClock_Hz = CLOCK_GetLpspiClkFreq(0u);
    LPSPI_MasterInit(LPSPI_MASTER_BASEADDR, &master_config, srcClock_Hz);

    memset(&(g_edma_rx_handle), 0, sizeof(g_edma_rx_handle));
    memset(&(g_edma_tx_handle), 0, sizeof(g_edma_tx_handle));

    EDMA_CreateHandle(&(g_edma_rx_handle), LPSPI_DMA, LPSPI_DMA_RX_CHANNEL);
    EDMA_CreateHandle(&(g_edma_tx_handle), LPSPI_DMA, LPSPI_DMA_TX_CHANNEL);

    EDMA_SetChannelMux(LPSPI_DMA, LPSPI_DMA_TX_CHANNEL, LPSPI_TRANSMIT_EDMA_CHANNEL);
    EDMA_SetChannelMux(LPSPI_DMA, LPSPI_DMA_RX_CHANNEL, LPSPI_RECEIVE_EDMA_CHANNEL);

    LPSPI_MasterTransferCreateHandleEDMA(LPSPI_MASTER_BASEADDR, &g_s_edma_handle, NULL, NULL,
                                        &g_edma_rx_handle, &g_edma_tx_handle);
    // -------------------------------------------------------------------------------------------------------------
    ncp_tlv_chksum_init();
    ncp_api_init();
    ncp_wifi_api_init();
}

uint32_t string_to_int(uint8_t *buff, uint8_t buff_size)
{
    uint32_t result;
    uint8_t index;

    result = 0;
    for(index = 0; index < buff_size; index ++)
    {
        result *= 10;
        result += (buff[index]-'0');
    }
    return result;
}

uint32_t int_to_string(uint8_t *buff, uint32_t max_buff_size, float number, uint8_t isFloat)
{
    uint32_t elements, n_int, index;
    float aux_number;
    float aux_int;
    uint32_t int_value;

    elements = 0;
    n_int = 0;

    if(number != 0)
    {
        aux_number = number;
        aux_int = (float)((uint32_t)aux_number);
        aux_number = aux_number - aux_int;

        while((uint32_t)aux_int > 0)
        {
            aux_int /= 10.0;
            n_int ++;
        }

        if(max_buff_size < n_int)
        {
            aux_int = 0;
            aux_number = 0;
        }

        if(n_int == 0 && max_buff_size > 0)
        {
            buff[elements] = '0';
            elements ++;
        }

        for(index = 0; index < n_int; index ++)
        {
            aux_int *= 10.0;
            int_value = (uint32_t)aux_int;
            aux_int -= int_value;
            buff[elements] = int_value + '0';
            elements ++;
        }

        if((elements < (max_buff_size-2)) && isFloat)
        {
            buff[elements] = '.';
            elements ++;
            if(aux_number == 0.0)
            {
                buff[elements] = '0';
                elements ++;
            }
            while((elements < max_buff_size) && (aux_number > 0.0))
            {
                aux_number *= 10.0;
                int_value = (uint32_t)aux_number;
                aux_number -= int_value;
                buff[elements] = int_value + '0';
                elements ++;
            }
        }
    }
    else if(max_buff_size != 0)
    {
        buff[elements] = '0';
        elements ++;
        if(isFloat && max_buff_size >= 3)
        {
            buff[elements] = '.';
            elements ++;
            buff[elements] = '0';
            elements ++;
        }
    }
    return elements;
}

void GPIO3_IRQHandler(void)
{
	uint32_t interrupts;
	interrupts = GPIO_GpioGetInterruptFlags(GPIO3);
	if(interrupts & (1<<NCP_HOST_GPIO_PIN_RX_READY))
	{
		ncp_rx_gpio_callback();
		GPIO_GpioClearInterruptFlags(GPIO3, 1 << NCP_HOST_GPIO_PIN_RX_READY);
	}
	if(interrupts & (1<<NCP_HOST_GPIO_PIN_RX))
	{
		ncp_tx_gpio_callback();
		GPIO_GpioClearInterruptFlags(GPIO3, 1 << NCP_HOST_GPIO_PIN_RX);
	}
}