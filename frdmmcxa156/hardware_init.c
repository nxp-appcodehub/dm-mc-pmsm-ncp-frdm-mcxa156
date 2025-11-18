/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "pin_mux.h"
#include "fsl_clock.h"
#include "fsl_reset.h"
#include "board.h"
#include <stdbool.h>
#include "app.h"
/*${header:end}*/

/*${function:start}*/
void BOARD_InitHardware(void)
{
    gpio_pin_config_t gpio_in_config = {
		kGPIO_DigitalInput,
		0,
	};

    CLOCK_SetClockDiv(kCLOCK_DivLPSPI0, 1u);
    CLOCK_AttachClk(kFRO12M_to_LPSPI0);

    CLOCK_EnableClock(kCLOCK_GateGPIO0);
    CLOCK_EnableClock(kCLOCK_GateGPIO1);
    CLOCK_EnableClock(kCLOCK_GateGPIO2);
    CLOCK_EnableClock(kCLOCK_GateGPIO3);
    CLOCK_EnableClock(kCLOCK_GateGPIO4);

    CLOCK_EnableClock(kCLOCK_GatePORT0);
    CLOCK_EnableClock(kCLOCK_GatePORT1);
    CLOCK_EnableClock(kCLOCK_GatePORT2);
    CLOCK_EnableClock(kCLOCK_GatePORT3);
    CLOCK_EnableClock(kCLOCK_GatePORT4);
    CLOCK_EnableClock(kCLOCK_GateFLEXIO0);

    RESET_ReleasePeripheralReset(kDMA_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kFLEXIO0_RST_SHIFT_RSTn);

    RESET_ReleasePeripheralReset(kGPIO0_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO1_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO2_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO3_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO4_RST_SHIFT_RSTn);

    RESET_ReleasePeripheralReset(kPORT0_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kPORT1_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kPORT2_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kPORT3_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kPORT4_RST_SHIFT_RSTn);

    RESET_PeripheralReset(kINPUTMUX0_RST_SHIFT_RSTn);
    CLOCK_EnableClock(kCLOCK_GateINPUTMUX0);

	GPIO_PinInit(NCP_HOST_GPIO_RX, NCP_HOST_GPIO_PIN_RX, &gpio_in_config);
	GPIO_PinInit(NCP_HOST_GPIO_RX_READY, NCP_HOST_GPIO_PIN_RX_READY, &gpio_in_config);

	GPIO_SetPinInterruptConfig(NCP_HOST_GPIO_RX_READY, NCP_HOST_GPIO_PIN_RX_READY, kGPIO_InterruptFallingEdge);
	GPIO_SetPinInterruptConfig(NCP_HOST_GPIO_RX, NCP_HOST_GPIO_PIN_RX, kGPIO_InterruptFallingEdge);

    NVIC_SetPriority(GPIO3_IRQn, 3U);
    NVIC_EnableIRQ(GPIO3_IRQn);
    
    LED_RED_INIT(LOGIC_LED_OFF);
    LED_GREEN_INIT(LOGIC_LED_OFF);
    LED_BLUE_INIT(LOGIC_LED_OFF);

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    
    FMU0->FCTRL = FMU_FCTRL_RWSC(0x1);
}
/*${function:end}*/
