#include "Driving_Sensing_Mode.h"

void HS_opamp_init(void){
	
	//PC11, 12, 12 ==> OD
		GPIO_InitTypeDef gpioStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		gpioStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
		gpioStructure.GPIO_Mode = GPIO_Mode_OUT;
		gpioStructure.GPIO_OType = GPIO_OType_OD;
		gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &gpioStructure);
	
}

void HS_opamp(enum Mode mode){	
	
	if(mode == driving){
		//driving: PC11, 12, 12 ==> high-Z			(enable opamp)
			GPIO_SetBits(GPIOC, GPIO_Pin_11);
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
			GPIO_SetBits(GPIOC, GPIO_Pin_13);
	}
	if(mode == sensing){	
		//sensing:PC11, 12, 12 ==> pull-down		(disable opamp)
			GPIO_ResetBits(GPIOC, GPIO_Pin_11);
			GPIO_ResetBits(GPIOC, GPIO_Pin_12);
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	}
	
}

void Sensing_MOSFET_init(void){
	
	//PC10 ==> PP
		GPIO_InitTypeDef gpioStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		gpioStructure.GPIO_Pin = GPIO_Pin_10;
		gpioStructure.GPIO_Mode = GPIO_Mode_OUT;
		gpioStructure.GPIO_OType = GPIO_OType_PP;
		gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &gpioStructure);
	
}

void Sensing_MOSFET(enum Mode mode){
	
	if(mode == driving){
		//driving: PC10 ==> high				(turn on MOSFET)
			GPIO_SetBits(GPIOC, GPIO_Pin_10);
	}
	if(mode == sensing){	
		//sensing: PC10 ==> low					(turn off MOSFET)
			GPIO_ResetBits(GPIOC, GPIO_Pin_10);
	}

}

void Injection_opamp_init(void){
	
	//PC3, 4, 5 ==> PP
		GPIO_InitTypeDef gpioStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		gpioStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
		gpioStructure.GPIO_Mode = GPIO_Mode_OUT;
		gpioStructure.GPIO_OType = GPIO_OType_PP;
		gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &gpioStructure);
	
}

void Injection_opamp(enum Mode mode){
	
	if(mode == driving){
		//driving: PC3, 4, 5 ==> low		(disable opamp)
			GPIO_ResetBits(GPIOC, GPIO_Pin_3);
			GPIO_ResetBits(GPIOC, GPIO_Pin_4);
			GPIO_ResetBits(GPIOC, GPIO_Pin_5);
	}
	if(mode == sensing){
		//sensing: PC3, 4, 5 ==> high		(enable opamp)
			GPIO_SetBits(GPIOC, GPIO_Pin_3);
			GPIO_SetBits(GPIOC, GPIO_Pin_4);
			GPIO_SetBits(GPIOC, GPIO_Pin_5);
	}
	
}