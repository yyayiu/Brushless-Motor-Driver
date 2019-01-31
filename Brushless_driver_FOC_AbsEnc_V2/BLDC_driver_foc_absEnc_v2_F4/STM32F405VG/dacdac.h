#ifndef _DAC_H
#define _DAc_H

#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"

#define FET_A 	0x04
#define FET_B 	0x02
#define FET_C 	0x01
#define NO_FET 	0x00


enum DAC_Enable{DAC_DISABLE=0,DAC_A,DAC_B,DAC_C};



void DAC_enable_init(void);

 // 0 -> all off
 // 1 -> on a phase 
 //	2 -> on b phase 
 // 3 -> on c phase 
 
void DAC_enable(u8 channel);

void DAC_TIM4_Config(u16 period);

void dac_init(u16 freq,u8 level);

u32 get_voltage(void);


void FET_gnd(u16 input);

u16 get_freq(void);

u16 get_interval(void);




#endif