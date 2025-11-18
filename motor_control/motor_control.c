/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "motor_control.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mc_periph_init.h"
#include "m1_sm_snsless_enc.h"
#include "mid_sm_states.h"
#include "board.h"


#define M1_END_OF_ISR \
    {                 \
        __DSB();      \
        __ISB();      \
    }

#define FEATURE_ENC (1)               /* Encoder feature flag */
#define FEATURE_FIELD_WEAKENING (0)   /* Field weakening feature flag */
#define FEATURE_S_RAMP (0)            /* S-ramp feature flag */

#define FEATURE_SET (FEATURE_ENC << (0) | \
                     FEATURE_FIELD_WEAKENING << (1) | \
                     FEATURE_S_RAMP << (2))

#define PEAK_DETECTOR_DECREASE 0.01

/* Demo mode enabled/disabled */
static bool_t bDemoModeSpeed = false;
static uint32_t g_rpm;

static mid_app_cmd_t g_eMidCmd;                  /* Start/Stop MID command */
static ctrl_m1_mid_t g_sSpinMidSwitch;           /* Control Spin/MID switching */

static uint32_t g_actual_rpm;
static float g_current_peak_detector;


void motor_control_init(void)
{    
    uint32_t ui32PrimaskReg;

    /* Disable all interrupts before peripherals are initialized */
    ui32PrimaskReg = DisableGlobalIRQ();
    /* Disable demo mode after reset */
    bDemoModeSpeed = false;
    /* Initialize peripheral motor control driver for motor M1 */
    MCDRV_Init_M1();
    /* Turn off application */
    M1_SetAppSwitch(false);
    g_rpm = 0;

    /* Init application state - Spin or MID */
    g_sSpinMidSwitch.eAppState = kAppStateMID;

    if(g_sSpinMidSwitch.eAppState == kAppStateMID)
    {
      MID_Init_AR();
    }

    /* Spin state machine is default */
    g_sSpinMidSwitch.eAppState = kAppStateSpin;
    /* Enable interrupts  */
    EnableGlobalIRQ(ui32PrimaskReg);
    /* Start PWM */
    g_sM1Pwm3ph.pui32PwmBaseAddress->MCTRL |= PWM_MCTRL_RUN(0xF);
}

void motor_control_set_speed(uint32_t rpm)
{
    if(rpm >= 500 && rpm <= 4000)
    {
        bDemoModeSpeed = true;
        g_rpm = rpm;
    }
    else
    {
        bDemoModeSpeed = false;
        g_rpm = 0;
    }
}

float motor_control_get_speed(void)
{
	 return g_sM1Drive.sSpeed.fltSpeedFilt*M1_N_ANGULAR_MAX;
}

float motor_control_get_voltage(void)
{
	 return g_sM1Drive.sFocPMSM.fltUDcBusFilt;
}

float motor_control_get_current(void)
{
    return g_current_peak_detector;
}

sm_app_state_t motor_control_get_app_state(void)
{
    return (sm_app_state_t)M1_GetAppState();
}

void motor_control_app_task(void *arg)
{
    while(1)
    {
        switch(g_sSpinMidSwitch.eAppState)
        {
            case kAppStateSpin:
                /* M1 state machine */
                if(g_sSpinMidSwitch.bCmdRunMid == TRUE)
                {
                if((kSM_AppStop == M1_GetAppState()) && (FALSE == M1_GetAppSwitch()) )
                {
                    MID_Init_AR();
                    g_sSpinMidSwitch.sFaultCtrlM1_Mid &= ~(FAULT_APP_SPIN);
                    g_eMidCmd = kMID_Cmd_Stop;                          /* Reset MID control command */
                    g_sSpinMidSwitch.eAppState = kAppStateMID;          /* MID routines will be processed */
                }
                else
                    g_sSpinMidSwitch.sFaultCtrlM1_Mid |= FAULT_APP_SPIN;

                g_sSpinMidSwitch.bCmdRunMid = FALSE;                  /* Always clear request */
                }

                g_sSpinMidSwitch.bCmdRunM1 = FALSE;
                break;
            default:
                /* MID state machine */
                if(g_sSpinMidSwitch.bCmdRunM1 == TRUE)
                {
                if((g_eMidCmd == kMID_Cmd_Stop) && (kMID_Stop == MID_GetActualState()))
                {
                    g_sSpinMidSwitch.sFaultCtrlM1_Mid &= ~(FAULT_APP_MID);
                    g_sM1Ctrl.eState = kSM_AppInit;                      /* Set Init state for M1 state machine */
                    g_sSpinMidSwitch.eAppState = kAppStateSpin;          /* Switch application state to Spin */
                }
                else
                    g_sSpinMidSwitch.sFaultCtrlM1_Mid |= FAULT_APP_MID;

                /* Always clear request */
                g_sSpinMidSwitch.bCmdRunM1 = FALSE;
                g_sSpinMidSwitch.bCmdRunMid = FALSE;
                break;
                }

                g_sSpinMidSwitch.bCmdRunMid = FALSE;
                MID_Process_BL(&g_eMidCmd);
                break;
        }
	    vTaskDelay(pdMS_TO_TICKS(100));
    } 
}



void ADC0_IRQHandler(void)
{
    switch(g_sSpinMidSwitch.eAppState)
    {
    case kAppStateSpin:
        /* M1 state machine */
        SM_StateMachineFast(&g_sM1Ctrl);
      break;
    default:
        /* MID state machine */
        MID_ProcessFast_FL();
      break;
    }

    g_current_peak_detector = (g_current_peak_detector > PEAK_DETECTOR_DECREASE)? (g_current_peak_detector-PEAK_DETECTOR_DECREASE):0;
    g_current_peak_detector = (g_current_peak_detector < g_sM1Drive.sFocPMSM.sIABC.fltA)? g_sM1Drive.sFocPMSM.sIABC.fltA:g_current_peak_detector;
    g_current_peak_detector = (g_current_peak_detector < g_sM1Drive.sFocPMSM.sIABC.fltB)? g_sM1Drive.sFocPMSM.sIABC.fltB:g_current_peak_detector;
    g_current_peak_detector = (g_current_peak_detector < g_sM1Drive.sFocPMSM.sIABC.fltC)? g_sM1Drive.sFocPMSM.sIABC.fltC:g_current_peak_detector;

    /* Clear the TCOMP INT flag */
    ADC0->STAT |= ADC_STAT_TCOMP_INT(1);

    /* Add empty instructions for correct interrupt flag clearing */
    M1_END_OF_ISR;
}


void CTIMER0_IRQHandler(void)
{
    static uint16_t fault_blink_counter = 0;
    uint16_t state;

    state = M1_GetAppState();

    /* M1 Slow StateMachine call */
    SM_StateMachineSlow(&g_sM1Ctrl);

    g_actual_rpm = M1_GetSpeed();

    if (state == kSM_AppStop)
    {
        if(bDemoModeSpeed)
        {
            M1_SetAppSwitch(true);
            g_sM1Drive.eControl                  = kControlMode_SpeedFOC;
            g_sM1Drive.sMCATctrl.ui16PospeSensor = MCAT_SENSORLESS_CTRL;
        }
        LED_RED_ON();
        LED_GREEN_OFF();
    }
    else if (state == kSM_AppFault)
    {
        if (fault_blink_counter >= 125)
        {
            LED_RED_TOGGLE();
            bDemoModeSpeed = false;
            fault_blink_counter = 0;
        }
        else
        {
            fault_blink_counter ++;
        }
        LED_GREEN_OFF();
    }
    else
    {
        if(bDemoModeSpeed)
        {
            M1_SetSpeed((float_t)g_rpm);
        }
        else
        {
            M1_SetSpeed(0);
            M1_SetAppSwitch(false);
        }
        LED_RED_OFF();
        LED_GREEN_ON();
    }

    /* Clear the match interrupt flag. */
    CTIMER0->IR |= CTIMER_IR_MR0INT(1U);

    /* Add empty instructions for correct interrupt flag clearing */
    M1_END_OF_ISR;
    
}
