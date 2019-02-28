#ifndef ABSENC_H
#define ABSENC_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
 
//Absolute encoder data
void AbsEnc_init(void);

u16 AbsEnc_data(void);

//Computing the electrical degree
u16 get_elec_angle(u16 AbsEnc_data);

//compute the velocity
s16 get_velcity(u16 this_AbsEnc, u16 last_AbsEnc);

#endif
