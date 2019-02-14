#include "PWM.h"

void PWM_init(){
	//init GPIO A
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);//TIM1, Ch1
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);//TIM1, Ch2
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);//TIM1, Ch3
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1);//TIM1, Ch4

    GPIO_InitTypeDef gpioStructure;
    gpioStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
		gpioStructure.GPIO_Mode = GPIO_Mode_AF;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioStructure);
	//init GPIO C
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);//TIM3, Ch3
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM3);//TIM3, Ch4

    gpioStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    gpioStructure.GPIO_Mode = GPIO_Mode_AF;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioStructure);
	
	//init TIM1, TIM3
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);		//168 MHz
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		// 84 MHz
	 
		TIM_TimeBaseInitTypeDef timerInitStructure;
		timerInitStructure.TIM_Prescaler = 42-1;//42 - 1;	//4kHz, 250us
		timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
		timerInitStructure.TIM_Period = 1000;
		timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		timerInitStructure.TIM_RepetitionCounter = 0;
		
		TIM_TimeBaseInit(TIM1, &timerInitStructure);
		TIM_Cmd(TIM1, ENABLE);
		
		timerInitStructure.TIM_Prescaler = 21-1;//21 - 1;	//4kHz, 250us
		TIM_TimeBaseInit(TIM3, &timerInitStructure);
		TIM_Cmd(TIM3, ENABLE);
		
	//init TIM1 Ch1,2,3,4
		TIM_OCInitTypeDef outputChannelInit;
		outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
		outputChannelInit.TIM_Pulse = 20;
		outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
		outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC1Init(TIM1, &outputChannelInit);							//TIM1, CH1
		TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
		TIM_OC2Init(TIM1, &outputChannelInit);							//TIM1, CH2
		TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
		TIM_OC3Init(TIM1, &outputChannelInit);							//TIM1, CH3
		TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
		TIM_OC4Init(TIM1, &outputChannelInit);							//TIM1, CH4
		TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
		TIM_OC3Init(TIM3, &outputChannelInit); 							//TIM3, CH3
		TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
		TIM_OC4Init(TIM3, &outputChannelInit);							//TIM4, CH4
		TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
		
		//need to add for Advande timer TIM1 or TIM8
		TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
		TIM_BDTRStructInit(&TIM_BDTRInitStruct);
		TIM_BDTRConfig(TIM1, &TIM_BDTRInitStruct);
		TIM_CCPreloadControl(TIM1, ENABLE);
		TIM_CtrlPWMOutputs(TIM1, ENABLE);
} 

void PWM_AL(u16 pwm){TIM_SetCompare3(TIM1, pwm);}
void PWM_NAH(u16 pwm){TIM_SetCompare4(TIM1, pwm);}
void PWM_BL(u16 pwm){TIM_SetCompare2(TIM1, pwm);}
void PWM_NBH(u16 pwm){TIM_SetCompare1(TIM1, pwm);}
void PWM_CL(u16 pwm){TIM_SetCompare3(TIM3, pwm);}
void PWM_NCH(u16 pwm){TIM_SetCompare4(TIM3, pwm);}

void PWM_A(u16 pwm){PWM_AL(pwm);	PWM_NAH(pwm);}
void PWM_B(u16 pwm){PWM_BL(pwm);	PWM_NBH(pwm);}
void PWM_C(u16 pwm){PWM_CL(pwm);	PWM_NCH(pwm);}

void set_PWM(s16 A, s16 B, s16 C){
	if(A>1000){A=1000;}
	if(B>1000){B=1000;}
	if(C>1000){C=1000;}
	if(A<0){A=0;}
	if(B<0){B=0;}
	if(C<0){C=0;}
	PWM_A((u16)A);		PWM_B((u16)B);		PWM_C((u16)C);
}
