#include "AbsEnc.h"

void AbsEnc_init(){
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

u16 AbsEnc_data(){
	
	u16 gray_data = (~GPIOB->IDR)&0x03FF;
	u16 bin_data = 0x0000;
	
	bin_data = bin_data | (gray_data&0x0200);		//Bit 9
	bin_data = bin_data | ((gray_data&0x0100)^((bin_data>>1)&0x0100));		//Bit 8
	bin_data = bin_data | ((gray_data&0x0080)^((bin_data>>1)&0x0080));		//Bit 7
	bin_data = bin_data | ((gray_data&0x0040)^((bin_data>>1)&0x0040));		//Bit 6
	bin_data = bin_data | ((gray_data&0x0020)^((bin_data>>1)&0x0020));		//Bit 5
	bin_data = bin_data | ((gray_data&0x0010)^((bin_data>>1)&0x0010));		//Bit 4
	bin_data = bin_data | ((gray_data&0x0008)^((bin_data>>1)&0x0008));		//Bit 3
	bin_data = bin_data | ((gray_data&0x0004)^((bin_data>>1)&0x0004));		//Bit 2
	bin_data = bin_data | ((gray_data&0x0002)^((bin_data>>1)&0x0002));		//Bit 1
	bin_data = bin_data | ((gray_data&0x0001)^((bin_data>>1)&0x0001));		//Bit 0
	
	if(bin_data%2 == 1){bin_data -= 1;}
	
	return bin_data;		
}

u16 A_phase_min = 82;
u16 A_phase_angle[7] = {82, 228, 374, 520, 666, 812, 958};
u16 A_phase_angle_offseted[8] = {0, 146, 292, 438, 584, 730, 876, 1024};	//form [0, 1024)
u16 get_elec_angle(u16 AbsEnc_data){
	
	//offset AbsEnc_data
		if(AbsEnc_data < A_phase_min){ AbsEnc_data = (AbsEnc_data + 1024) - A_phase_min;}
		else{ AbsEnc_data -= A_phase_min;}
		
	//map to electrical angle
		u16 elec_angle = 0;
		if(AbsEnc_data>=A_phase_angle_offseted[0] && AbsEnc_data<A_phase_angle_offseted[1]){
			elec_angle = ( ((AbsEnc_data-A_phase_angle_offseted[0])*360)/(A_phase_angle_offseted[1]-A_phase_angle_offseted[0]) );	}
		else if(AbsEnc_data>=A_phase_angle_offseted[1] && AbsEnc_data<A_phase_angle_offseted[2]){
			elec_angle = ( ((AbsEnc_data-A_phase_angle_offseted[1])*360)/(A_phase_angle_offseted[2]-A_phase_angle_offseted[1]) );	}
		else if(AbsEnc_data>=A_phase_angle_offseted[2] && AbsEnc_data<A_phase_angle_offseted[3]){
			elec_angle = ( ((AbsEnc_data-A_phase_angle_offseted[2])*360)/(A_phase_angle_offseted[3]-A_phase_angle_offseted[2]) );	}
		else if(AbsEnc_data>=A_phase_angle_offseted[3] && AbsEnc_data<A_phase_angle_offseted[4]){
			elec_angle = ( ((AbsEnc_data-A_phase_angle_offseted[3])*360)/(A_phase_angle_offseted[4]-A_phase_angle_offseted[3]) );	}
		else if(AbsEnc_data>=A_phase_angle_offseted[4] && AbsEnc_data<A_phase_angle_offseted[5]){
			elec_angle = ( ((AbsEnc_data-A_phase_angle_offseted[4])*360)/(A_phase_angle_offseted[5]-A_phase_angle_offseted[4]) );	}
		else if(AbsEnc_data>=A_phase_angle_offseted[5] && AbsEnc_data<A_phase_angle_offseted[6]){
			elec_angle = ( ((AbsEnc_data-A_phase_angle_offseted[5])*360)/(A_phase_angle_offseted[6]-A_phase_angle_offseted[5]) );	}
		else if(AbsEnc_data>=A_phase_angle_offseted[6] && AbsEnc_data<A_phase_angle_offseted[7]){
			elec_angle = ( ((AbsEnc_data-A_phase_angle_offseted[6])*360)/(A_phase_angle_offseted[7]-A_phase_angle_offseted[6]) );	}
		else{return 361;}
		
		//reverse direct
//			elec_angle = -1*(elec_angle-180) + 180;
//			if (elec_angle==360){elec_angle = 0;}
		
		return elec_angle;
}

s16 get_velcity(u16 this_AbsEnc, u16 last_AbsEnc){
	
	s16 velocity = this_AbsEnc - last_AbsEnc;
	
	if(velocity > 512){
		velocity -= 1024;
	}
	if(velocity < -512){
		velocity += 1024;
	}
	
	return velocity;
	
}
