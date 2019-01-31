#include "encoder.h"

#if ENCODER_USES_INTERRUPT_MODE
	static volatile s16 loopCountEnc2 = 0;
#else
	static volatile s32 enc2Count = 0;
#endif
static volatile s32 enc1Count = 0; 

//Disable encoder 1 because counting encoder with external interupt is not reliable when there is noise -By Emmett

//Init encoder
/*void encoder_1_init(void){
	//Init RCC Clock
	ENCODER1_GPIO_RCC_INIT;
	ENCODER1_EXT_RCC_INIT;
	
	//GPIO Init
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	
	//Init encoder 1
	GPIO_InitStructure.GPIO_Pin = ENCODER1_PORT1 | ENCODER1_PORT2;
	GPIO_InitStructure.GPIO_PuPd = ENCODER1_MODE;
	GPIO_Init(ENCODER1_GPIOx, &GPIO_InitStructure);

	//Custom interrupt and encoder event for encoder 1
	SYSCFG_EXTILineConfig(ENCODER1_EXTPORTSOURCE, ENCODER1_EXTPINSOURCE1);
	SYSCFG_EXTILineConfig(ENCODER1_EXTPORTSOURCE, ENCODER1_EXTPINSOURCE2);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	
	EXTI_InitStructure.EXTI_Line = ENCODER1_EXT_LINE1 | ENCODER1_EXT_LINE2;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = ENCODER1_IRQ;
	NVIC_Init(&NVIC_InitStructure);
	
	//Let's go!
	enc1Count = 0;
}*/

void encoder_2_init(void){
	ENCODER2_TIMER_RCC_INIT;
	ENCODER2_GPIO_RCC_INIT;
	
	GPIO_PinAFConfig(ENCODER2_GPIOx, ENCODER2_GPIO_PINSOURCE1, ENCODER2_AF);
	GPIO_PinAFConfig(ENCODER2_GPIOx, ENCODER2_GPIO_PINSOURCE2, ENCODER2_AF);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	
	//Init encoder 2
	GPIO_InitStructure.GPIO_Pin = ENCODER2_PORT1 | ENCODER2_PORT2;
	GPIO_InitStructure.GPIO_PuPd = ENCODER2_MODE;
	GPIO_Init(ENCODER2_GPIOx, &GPIO_InitStructure);
	
	//Timer init
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ENCODER2_TIMER, &TIM_TimeBaseStructure);
	
	#if ENCODER_USES_INTERRUPT_MODE
		//Interrupt init
		TIM_ITConfig(ENCODER2_TIMER, TIM_IT_Update, ENABLE);
		
		NVIC_InitTypeDef NVIC_InitStructure;

		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannel = ENCODER2_IRQ;
		NVIC_Init(&NVIC_InitStructure);
		
		loopCountEnc2 = 0;
	#else
		enc2Count = 0;
	#endif
	
	//Encoder interface init for encoder 2
	TIM_EncoderInterfaceConfig(ENCODER2_TIMER, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	//Let's go!
	TIM_SetCounter(ENCODER2_TIMER, 0);
	TIM_Cmd(ENCODER2_TIMER, ENABLE);
}

//Handler for encoder 1
/*void ENCODER1_IRQ_HANDLER(){
	static u8 lastAReading = 0, lastBReading = 0;
	
	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
		u8 aReading = GPIO_ReadInputDataBit(ENCODER1_GPIOx, ENCODER1_PORT1);
		u8 bReading = GPIO_ReadInputDataBit(ENCODER1_GPIOx, ENCODER1_PORT2);
		
		if (bReading ^ lastAReading){
			enc1Count++;
		}
		
		if (aReading ^ lastBReading){
			enc1Count--;
		}
		
		lastAReading = aReading;
		lastBReading = bReading;
		EXTI_ClearITPendingBit(EXTI_Line8);
		
	}else if(EXTI_GetITStatus(EXTI_Line9) != RESET){
		u8 aReading = GPIO_ReadInputDataBit(ENCODER1_GPIOx, ENCODER1_PORT1);
		u8 bReading = GPIO_ReadInputDataBit(ENCODER1_GPIOx, ENCODER1_PORT2);
		
		if (bReading ^ lastAReading){
			enc1Count++;
		}
		
		if (aReading ^ lastBReading){
			enc1Count--;
		}
		
		lastAReading = aReading;
		lastBReading = bReading;
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}*/

#if ENCODER_USES_INTERRUPT_MODE
	//Handler for encoder 2
	void ENCODER2_IRQ_HANDLER(){
		if (TIM_GetITStatus(ENCODER2_TIMER, TIM_IT_Update) != RESET){
			TIM_ClearITPendingBit(ENCODER2_TIMER, TIM_IT_Update);
			if (TIM_GetCounter(ENCODER2_TIMER) < 32768){
				//Overflow
				loopCountEnc2++;
			}else{
				//Underflow
				loopCountEnc2--;
			}
		}
	}
#else
	//Manually update the encoder count without using interrupt
	void encoder_2_update(){
		s16 cnt = (s16) TIM_GetCounter(ENCODER2_TIMER);
		enc2Count += cnt;
		TIM_SetCounter(ENCODER2_TIMER, 0);
	}
#endif

int32_t get_encoder_count(EncoderID id){
	if (id == ENCODER_1){
		return enc1Count;
	}else{
		#if ENCODER_USES_INTERRUPT_MODE
			return loopCountEnc2*0xFFFF + TIM_GetCounter(ENCODER2_TIMER);
		#else
			return enc2Count;
		#endif
	}
}

void reset_encoder(EncoderID id){
	if (id == ENCODER_1){
		enc1Count = 0;
	}else{
		#if ENCODER_USES_INTERRUPT_MODE
			loopCountEnc2 = 0;
		#else
			enc2Count = 0;
		#endif
		TIM_SetCounter(ENCODER2_TIMER, 0);
	}
}

