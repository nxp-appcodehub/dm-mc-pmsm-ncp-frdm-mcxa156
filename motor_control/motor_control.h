/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "state_machine.h"

void motor_control_init(void);

void motor_control_set_speed(uint32_t rpm);

float motor_control_get_speed(void);

float motor_control_get_voltage(void);

float motor_control_get_current(void);

sm_app_state_t motor_control_get_app_state(void);

void motor_control_app_task(void *arg);