#include "ticks.h"

volatile u32 msTicks = 0;
void ticks_init() {
	RCC_APB1PeriphClockCmd(TICKS_RCC, ENABLE);
	
	//Setup timer
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
  TIM_TimeBaseStructure.TIM_Prescaler = 86 - 1;
  TIM_TimeBaseStructure.TIM_Period = 1000;
  TIM_TimeBaseInit(TICKS_TIM, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TICKS_TIM, TIM_IT_Update, ENABLE);
	
	//Setup interrupt
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TICKS_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TICKS_TIM, ENABLE);
	
	msTicks = 0;
}

void TICKS_IRQ_HANDLER(void){
	if (TIM_GetITStatus(TICKS_TIM, TIM_IT_Update) != RESET){
		msTicks++;
		TIM_ClearITPendingBit(TICKS_TIM, TIM_IT_Update);
	}
}

//Generate a delay in us  (inaccurate when value too small)
void _delay_us(uint16_t us){
	volatile uint32_t count = us*28;
	while(count--){
		__asm__("nop");
	}
}

//Generate a delay in ms
void _delay_ms(uint16_t ms){
	volatile uint32_t count = ms*28000;
	while(count--){
		__asm__("nop");
	}
}
