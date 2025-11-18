/*
 * Copyright 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _APP_H_
#define _APP_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LPSPI_MASTER_BASEADDR         (LPSPI0)
#define LPSPI_MASTER_PCS_FOR_INIT     (kLPSPI_Pcs0)
#define LPSPI_MASTER_PCS_FOR_TRANSFER (kLPSPI_MasterPcs0)
#define LPSPI_DMA         DMA0
#define LPSPI_DMA_RX_CHANNEL   1U
#define LPSPI_DMA_TX_CHANNEL   0U
#define LPSPI_TRANSMIT_EDMA_CHANNEL     kDma0RequestLPSPI0Tx
#define LPSPI_RECEIVE_EDMA_CHANNEL      kDma0RequestLPSPI0Rx

#define TRANSFER_BAUDRATE 100000U

#define NCP_HOST_GPIO_RX            GPIO3
#define NCP_HOST_GPIO_PIN_RX        14U
#define NCP_HOST_GPIO_RX_READY      GPIO3
#define NCP_HOST_GPIO_PIN_RX_READY  1U
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_InitHardware(void);

#endif /* _APP_H_ */
