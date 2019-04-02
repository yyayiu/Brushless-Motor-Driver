#ifndef CURRENT_CONTROL_H
#define CURRENT_CONTROL_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "uart.h"

//pid turning
void mech_angle_p_inc(void);
void mech_angle_p_dec(void);
void mech_angle_i_inc(void);
void mech_angle_i_dec(void);
void mech_angle_d_inc(void);
void mech_angle_d_dec(void);

void print_mech_angle_pid(void);

void elec_angle_control(u16 target_angle, u16 elec_angle, s16* target_current_q);

s32 mech_angle_control(u16 target_angle, u16 this_AbsEnc, s16* target_current_q);

void d_const_mech_angle_change(s16 speed);

#endif
