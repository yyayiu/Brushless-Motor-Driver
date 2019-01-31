#include "linear_ccd.h"
#include "adc.h"	

void CLK(u8 state){//self make clock to control ccd

	if (state == 1){
		GPIO_SetBits(CLK_PORT, CLK_PIN);
		state=0;
	}else if (state == 0){
		GPIO_ResetBits(CLK_PORT,CLK_PIN);
		state=1;
	}
}

void SI(u8 bit){ //controlling Linear_ccd 1 si

	if(bit == 1){
		GPIO_SetBits(SI_PORT,SI_PIN);
	}else{
		GPIO_ResetBits(SI_PORT,SI_PIN);
	}
}

u32 AO(){  // getting data from ccd1 ao
	u32 temp;
	int get_times = 200;
	for(u8 i=0; i<get_times; i++){
		temp += adc_get(ADC_CHANNEL);
	}
	temp=(u32)(temp/get_times);
	return temp;
}

u32 linear_ccd_buffer[128];
u8 flag = 0;

void linear_ccd_read(){
	SI(0);
	CLK(0);
	_delay_us(1);
	SI(1);
	for(u8 n=0;n<128;n++){
		_delay_us(1);
		CLK(1);		
		SI(0);
		_delay_us(1);
		linear_ccd_buffer[n]=AO();
		CLK(0);
	}

	for(u8 y=0;y<128;y++){
		linear_ccd_buffer[y] = (linear_ccd_buffer[y])*160 / 4095;
		if(linear_ccd_buffer[y] >= 160){
			linear_ccd_buffer[y] = 159;
		}
	}
}

void linear_ccd_init(){
	gpio_init(SI_PORT, GPIO_Mode_OUT, GPIO_Fast_Speed, GPIO_OType_PP, GPIO_PuPd_NOPULL, false);
	gpio_init(CLK_PORT, GPIO_Mode_OUT, GPIO_Fast_Speed, GPIO_OType_PP, GPIO_PuPd_NOPULL, false);
}
