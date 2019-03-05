#ifndef CURRENT_CONTROL_H
#define CURRENT_CONTROL_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "uart.h"
#include "CurrentSensing.h"
#include "PWM.h"

// FOC current_control_dq-frame
void Current_control_FOC_dq_Frame(u16 elec_angle, s16 target_current_d, s16 target_current_q, s16 current_d, s16 current_q, s16* pwm_A, s16* pwm_B, s16* pwm_C);

#endif
