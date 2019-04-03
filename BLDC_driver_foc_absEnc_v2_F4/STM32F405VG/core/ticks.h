#ifndef _TICKS_H
#define _TICKS_H

#include "stm32f4xx_tim.h"
#include "stm32f4xx_it.h"

extern volatile u32 msTicks;

#define TICKS_TIM 				TIM2
#define TICKS_RCC 				RCC_APB1Periph_TIM2
#define TICKS_IRQ 				TIM2_IRQn
#define TICKS_IRQ_HANDLER	TIM2_IRQHandler

void ticks_init(void);

//Get the total ms passed
#define get_ticks() (msTicks)

//Get the total seconds passed
#define get_seconds() (msTicks/1000)

//Get the ticks passed since last seconds
#define get_sub_ticks() (msTicks%1000)

//The following delay functions are not accurate

//Generate a delay in us
void _delay_us(uint16_t us);
//Generate a delay in ms
void _delay_ms(uint16_t ms);

#endif
