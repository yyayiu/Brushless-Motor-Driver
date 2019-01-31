#ifndef	_TIMER_H
#define	_TIMER_H

#include "stm32f4xx_tim.h"
#include <stdbool.h>
#include "ticks.h"

/***************************************************************************************************************************************
** TIMER - STM32F4
**
** This library provides functions for scheduling tasks using timer.
** // I guess it will be faster when implemented with priority queue but well I'm too lazy.
** Yes I have implementated it with priority queue. [I am not that lazy (-_-)]
**
** ROBOCON 2017
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
**
** v1.0 January 2017
**
** "I've seen the future. You're not in it." ~Faceless void
****************************************************************************************************************************************/

#define TIMER_SIZE 50 //Max number of actions that can be pended

#define TIMER_TIM 				TIM7
#define TIMER_RCC 				RCC_APB1Periph_TIM7
#define TIMER_IRQ 				TIM7_IRQn
#define TIMER_IRQ_HANDLER	TIM7_IRQHandler

//+1 count every 0.5 ms (42000), /1000->ms /2-> Clock division /2->0.5ms
#define TIMER_PRESCALER (SystemCoreClock/1000/4)

//Each quantum = 0.5ms
#define QUANTUM_MULTIPLER 2

typedef void(*TimerAction)(void);

typedef struct{
	volatile TimerAction action;
	vu32 trig_time;
}TimerActionStruct;

//Init timer
void timer_init(void);

/**
* The following functions can be used to schedule tasks.
* @example Buzzer beeps for five times. 
*							do_after_for(buzzer_on, 1000, 1000, 5);
*							do_after_for(buzzer_off, 1200, 1000, 5);
*
* @exmaple Servo move at specific pattern
*							do_after(servo_lift, 800);
*							do_after(servo_put, 1200);
*							do_after(servo_release, 2000);
*
*/

/**
* Register a event call that will happen after some time.
* @param action: The function to be called
* @param ms: The time to be waited (0 ~ 2^32/QUANTUM_MULTIPLER)
*/
void do_after(TimerAction action, uint32_t ms);

/**
* Register a event call that will happen after some time.
* @param action: The function to be called
* @param ms: The time to be waited (0 ~ 2^32/QUANTUM_MULTIPLER)
* @param reload: The time ms to be reloaded when repeated
* @param repeat: How many times this function will be repeated
*/
void do_after_for(TimerAction action, uint32_t ms, uint32_t reload, uint16_t repeat);

// Return the current size of timer array
uint16_t get_timer_size(void);

#endif
