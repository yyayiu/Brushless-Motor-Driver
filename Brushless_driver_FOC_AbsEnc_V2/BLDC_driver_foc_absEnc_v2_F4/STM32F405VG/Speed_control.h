#ifndef SPEED_CONTROL_H
#define SPEED_CONTROL_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "uart.h"

void rpm_to_t_n(u16* t, s16* n, s16 target_speed);

// Low speed control, 1~50 rpm
void low_speed_control(s16 target_speed, s16* target_angle, u32 this_ticks);


#endif
