#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"

/**
* This library handles encoder count with interrupt to count beyond 65535
* Note that this library is poorly written (by me) so you also need to change the interrupt handler if you want to change to another timer
*
* Rex Cheng
*/

#define ENCODER_COUNT 2

#define ENCODER1_EXT_RCC_INIT							RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
#define ENCODER1_IRQ											EXTI9_5_IRQn
#define ENCODER1_IRQ_HANDLER							EXTI9_5_IRQHandler

#define ENCODER1_GPIO_RCC_INIT			    	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
#define ENCODER1_PORT1										GPIO_Pin_8
#define ENCODER1_PORT2										GPIO_Pin_9
#define ENCODER1_GPIOx										GPIOC
#define ENCODER1_EXTPORTSOURCE						EXTI_PortSourceGPIOC
#define ENCODER1_EXTPINSOURCE1						EXTI_PinSource8
#define ENCODER1_EXTPINSOURCE2						EXTI_PinSource9
#define ENCODER1_EXT_LINE1								EXTI_Line8
#define ENCODER1_EXT_LINE2								EXTI_Line9

#define ENCODER1_MODE											GPIO_PuPd_UP

#define ENCODER2_TIMER										TIM3
#define ENCODER2_AF                       GPIO_AF_TIM3
#define ENCODER2_TIMER_RCC_INIT						RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
#define ENCODER2_IRQ											TIM3_IRQn
#define ENCODER2_IRQ_HANDLER							TIM3_IRQHandler

#define ENCODER2_GPIO_RCC_INIT			    	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
#define ENCODER2_PORT1										GPIO_Pin_6
#define ENCODER2_PORT2										GPIO_Pin_7
#define ENCODER2_GPIOx										GPIOC
#define ENCODER2_GPIO_PINSOURCE1          GPIO_PinSource6
#define ENCODER2_GPIO_PINSOURCE2          GPIO_PinSource7

#define ENCODER2_MODE											GPIO_PuPd_UP

//Using non-interrupt mode might reduce interference from noise
//but the user is respondible for calling encoder_2_update() such that the count wouldn't exceed 32767 in each interval.
#define ENCODER_USES_INTERRUPT_MODE false

typedef enum{
	ENCODER_1 = 0, 
	ENCODER_2
}EncoderID;

void encoder_1_init(void);
void encoder_2_init(void);

/** Get the encoder reading. Overflow after 65535.
	@return encoder count
*/
int32_t get_encoder_count(EncoderID id);

// Reset the encoder count
void reset_encoder(EncoderID id);

#if !ENCODER_USES_INTERRUPT_MODE
	void encoder_2_update(void);
#endif

#endif
