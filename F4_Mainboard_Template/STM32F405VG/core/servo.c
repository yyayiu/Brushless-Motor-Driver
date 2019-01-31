#include "servo.h"

void servo_init(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable; 
	TIM_OCInitStructure.TIM_Pulse = 1000;
	
	for (uint8_t i=0; i<SERVO_SIZE; i++){
		const ServoStruct* servo = &(ServoPorts[i]);
		
		gpio_rcc_init(servo->gpio);
		
		if (servo->rcc_line == 1){
			RCC_APB1PeriphClockCmd(servo->tim_rcc, ENABLE);
			TIM_TimeBaseStructure.TIM_Prescaler = (168/2)-1;
		}else if(servo->rcc_line == 2){
			RCC_APB2PeriphClockCmd(servo->tim_rcc, ENABLE);
			TIM_TimeBaseStructure.TIM_Prescaler = 168-1;
		}else{
			//Error
			while(1);
		}
		
		//Warning: IS_RCC_APB1/2_PERIPH does not mean the input is a APB1/2 device, sometimes it just so happens to be 
//		if (IS_RCC_APB1_PERIPH(servo->tim_rcc)){
//			RCC_APB1PeriphClockCmd(servo->tim_rcc, ENABLE);
//		}else if(IS_RCC_APB2_PERIPH(servo->tim_rcc)){
//			RCC_APB2PeriphClockCmd(servo->tim_rcc, ENABLE);
//		}else{
//			//Error
//			while(1);
//		}
		
		gpio_af_init(servo->gpio, GPIO_OType_PP, GPIO_PuPd_NOPULL, servo->tim_af);
		
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
		TIM_TimeBaseStructure.TIM_Period = 20000;
		
		TIM_TimeBaseInit(servo->tim, &TIM_TimeBaseStructure);

		switch (servo->channel){
			case 1:
				TIM_OC1Init(servo->tim, &TIM_OCInitStructure);
				TIM_OC1PreloadConfig(servo->tim, ENABLE);
				break;
			
			case 2:
				TIM_OC2Init(servo->tim, &TIM_OCInitStructure);
				TIM_OC2PreloadConfig(servo->tim, ENABLE);
				break;
			
			case 3:
				TIM_OC3Init(servo->tim, &TIM_OCInitStructure);
				TIM_OC3PreloadConfig(servo->tim, ENABLE);
				break;
			
			case 4:
				TIM_OC4Init(servo->tim, &TIM_OCInitStructure);
				TIM_OC4PreloadConfig(servo->tim, ENABLE);
				break;
		}
		
		TIM_ARRPreloadConfig(servo->tim, ENABLE);
		TIM_Cmd(servo->tim, ENABLE);
		TIM_CtrlPWMOutputs(servo->tim, ENABLE);
	}
}

/**
  * @brief  Controlling the CCR value for servos
  * @param  servo_id: Port of Motor to be used
  * @param  val: Any value from 0~20000.
  * @retval None
  */
void servo_ccr_control(ServoID servo_id , u32 ccr_val) {
	switch(ServoPorts[servo_id].channel){
		case 1:
			TIM_SetCompare1(ServoPorts[servo_id].tim, ccr_val);
			break;
		
		case 2:
			TIM_SetCompare2(ServoPorts[servo_id].tim, ccr_val);
			break;
		
		case 3:
			TIM_SetCompare3(ServoPorts[servo_id].tim, ccr_val);
			break;
		
		case 4:
			TIM_SetCompare4(ServoPorts[servo_id].tim, ccr_val);
			break;
	}
}

/**
  * @brief  Control the degree of the servo
  * @param  servo_id: The servo id to be used
  * @param  val: Any value from MIN_DEG to MAX_DEG (defined in @ServoConfigs_TABLE)
  * @retval None
  */

#define getCCR(i, d) (ServoConfigs[i].min_ccr + d*(ServoConfigs[i].max_ccr-ServoConfigs[i].min_ccr)/(ServoConfigs[i].max_deg-ServoConfigs[i].min_deg))
void servo_deg_control(ServoID servo_id , int32_t degree){
	u16 ccrVal = getCCR(servo_id, degree);
	servo_ccr_control(servo_id, ccrVal);
}
#undef getCCR
