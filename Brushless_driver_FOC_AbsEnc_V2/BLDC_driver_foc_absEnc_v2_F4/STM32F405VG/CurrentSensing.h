#ifndef CURRENTSENSING_H
#define CURRENTSENSING_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "trigon_math.h"
#include "ticks.h"

void current_sensing_init(u32 init_ticks);

void cal_zero_mean(u32* zeromean);

void current_1000_update(void);

s16 get_instant_current_A(void);
s16 get_instant_current_B(void);
s16 get_instant_current_C(void);

void print_zero_mean(u32* zeromean);

void abc_to_dq(u16 elec_angle, s16* a, s16* b, s16* c, s16* d, s16* q);

void dq_to_abc(u16 elec_angle, s16* a, s16* b, s16* c, s16* d, s16* q);

#endif
