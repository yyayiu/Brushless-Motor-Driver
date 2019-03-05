#ifndef CURRENT_CONTROL_H
#define CURRENT_CONTROL_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "uart.h"

void elec_angle_control(u16 target_angle, u16 elec_angle, s16* target_current_q);

void mech_angle_control(u16 target_angle, u16 this_AbsEnc, s16* target_current_q);

#endif
