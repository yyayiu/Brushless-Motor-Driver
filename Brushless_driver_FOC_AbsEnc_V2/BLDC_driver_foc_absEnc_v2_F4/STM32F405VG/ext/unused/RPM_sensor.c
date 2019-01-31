/**RPM Sensor for STM32F103
** @Author James Mok
**/

#include "RPM_sensor.h"


static volatile u16 past_val = 0;
static volatile u16 now_val = 0;
static volatile u32 diff = 0;
static u16 tar_val = 650;
static u8 PID_FLAG = PID_OFF;
static u16 fail_time = 0;
s32 PID;
u16 cur_pwm;


void RPMs_init(void){	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStructure);																		// Set to default
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;									// Set Timer Pin 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;										// Set Pin mode to floating
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Init for RPMs_In_Tim
	TIM_DeInit(RPMs_In_TIM);																								// clear
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00; 														// No prescaling
	TIM_TimeBaseStructure.TIM_Period = CountPerRo;													// Max Count
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(RPMs_In_TIM, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(RPMs_In_TIM, TIM_EncoderMode_TI12,
													 TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	TIM_SetCounter(RPMs_In_TIM, 6);																					// Reset Count as 6
	TIM_ClearITPendingBit(RPMs_In_TIM, TIM_IT_Update);
	TIM_ITConfig(RPMs_In_TIM, TIM_IT_Update, ENABLE);
	TIM_Cmd(RPMs_In_TIM, ENABLE);// Enable counter timer
	
	//Init for RPMs_Count_TIM
	TIM_DeInit(RPMs_Count_TIM);
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;	                 		     				// Timer period, 4.5ms to trigger the event
	TIM_TimeBaseStructure.TIM_Prescaler = RPMs_TIM_Period;     							// 72M/1e5 - 1 = 710
	TIM_TimeBaseInit(RPMs_Count_TIM, &TIM_TimeBaseStructure);      					// this part feeds the parameter we set above
	
	TIM_ClearITPendingBit(RPMs_Count_TIM, TIM_IT_Update);									 	// Clear Interrupt bits
	TIM_ITConfig(RPMs_Count_TIM, TIM_IT_Update, ENABLE);										// Enable TIM Interrupt
	TIM_Cmd(RPMs_Count_TIM, ENABLE);																				// Counter Enable
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

/**
``* @Brief 	Return the RPS
``*	@Param 	None
	*	@Retval RPS
 **/
u32 get_RPS(void){
	return 1 * diffScale * 10 / diff;
}

u32 get_diff(void){
	return diff;
}

u8 get_PID_FLAG(void){
	return PID_FLAG;
}

/**
	*	@Breif 	For setting the target RPS. Note that the initial value must be 50 for stablize the system.
	*	@Param	Target RPS value
	*	@Retval	None
 **/
void set_tar_val(u16 tar){
	tar_val = tar <= (RPMs_DownLim + 10) ? 0 : tar;
	tar_val = tar >= RPMs_UpLim ? RPMs_UpLim : tar;
}

/**
	* @Breif 	For Turning the brushless PID ON/OFF
	*	@Param 	State: PID_ON, PID_OFF
	*	@Retval None
 **/
void set_PID_FLAG(u8 state){
	PID_FLAG = state;
}

/**
	* @Breif 	PID for burshless, to be called in ticks handler with 4 ticks (4ms)
	*	@Param 	P I D constant. For RED BRUSHLESS, use (50,10, 20) and target value 80 for initialization. For Silver use (40, 2, 30)
	*	@Retval None
 **/
void RPMs_update(s32 P, s32 I, s32 D){
	static s16 now_error = 0;
	static s16 past_error = 0;
	static s16 last_past_error = 0;
	static u32 now_pwm = 750;
	
	if(PID_FLAG == PID_OFF){
		//servo_control(SERVO3, 450);
		return;
	}
	else{
		now_error = tar_val - diff;
	    
		PID = (P* now_error+ I*(past_error + now_error + last_past_error) + D*(now_error - past_error) /2) / PIDScale;
	
		now_pwm += PID;
		
		last_past_error = past_error;
		past_error = now_error;
		

		now_pwm = now_pwm >= PWM_UpLim ? PWM_UpLim : now_pwm;
		now_pwm = now_pwm <= PWM_DownLim ? PWM_DownLim : now_pwm;
		
		cur_pwm = now_pwm;
		servo_control(RPMs_servo, now_pwm);
	}
}

void TIM5_IRQHandler(void){
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET){
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
		diff = 1 * diffScale * RoMulti/TIM_GetCounter(RPMs_Count_TIM);
		
		diff = diff <= RPMs_DownLim ? 0 : diff;
		diff = diff > RPMs_UpLim ? 0 : diff;
		
		TIM_SetCounter(RPMs_Count_TIM, 0);
	}
}

void TIM6_IRQHandler(void){
	if(TIM_GetITStatus(RPMs_Count_TIM, TIM_IT_Update) != RESET){
		TIM_ClearFlag(RPMs_Count_TIM, TIM_FLAG_Update);
		fail_time++;
		
		if(fail_time >= 500)		//please test more to find the optimal value
			PID_FLAG = PID_OFF;
	}
}
