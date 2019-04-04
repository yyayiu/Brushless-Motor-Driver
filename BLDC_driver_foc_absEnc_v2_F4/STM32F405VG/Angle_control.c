#include "Angle_control.h"

//pid constant for position control(elec angle)
	s16	p_const_elec_angle = 1200;
	s16	i_const_elec_angle = 10;
	s16	d_const_elec_angle = 0;
	
//pid constant for position control(mech angle)
	s16	p_const_mech_angle = 3000;//3000
	s16	i_const_mech_angle = 20;//20
	s32	d_const_mech_angle = 6500;//6500
	//  0~20  6500
	// 21~40 11000
	// 41~   25000
	
// pid error for angle control(elec and mech)
	s16		error_angle = 0;
	s32 i_error_angle = 0;
	s16 d_error_angle = 0;


// elec_angle control
void elec_angle_control(u16 target_angle, u16 elec_angle, s16* target_current_q){
	
	//elec angle error update
		d_error_angle = (elec_angle-target_angle) - error_angle;
			error_angle = elec_angle-target_angle;
				if(error_angle>180){error_angle -= 360;		d_error_angle -= 360;}
		i_error_angle += error_angle;
	//target q pid control
		*target_current_q = -1*(p_const_elec_angle*error_angle + i_const_elec_angle*i_error_angle + d_const_elec_angle*d_error_angle)/100;	
	//reduce the target_q when angle reach the target
		if(error_angle>0){*target_current_q -= 60;}
		if(error_angle<0){*target_current_q += 60;}
	
}


void mech_angle_p_inc(void){
	p_const_mech_angle = p_const_mech_angle + 50;
}
void mech_angle_p_dec(void){
	p_const_mech_angle = p_const_mech_angle - 50;
}
void mech_angle_i_inc(void){
	i_const_mech_angle++;
}
void mech_angle_i_dec(void){
	i_const_mech_angle--;
}
void mech_angle_d_inc(void){
	d_const_mech_angle = d_const_mech_angle + 100;
}
void mech_angle_d_dec(void){
	d_const_mech_angle = d_const_mech_angle - 100;
}
void print_mech_angle_pid(void){
	uart_tx_blocking(COM3, "   P:%d  I:%d  D:%d\n", p_const_mech_angle, i_const_mech_angle, d_const_mech_angle);
}


s32 mech_angle_control(u16 target_angle, u16 this_AbsEnc, s16* target_current_q){
	
	//mechinical angle error update
		d_error_angle = (this_AbsEnc-target_angle) - error_angle;
			error_angle =  this_AbsEnc-target_angle;
				if(error_angle>512){error_angle -= 1024;		d_error_angle -= 1024;}
				if(error_angle<-512){error_angle += 1024;		d_error_angle += 1024;}
		i_error_angle += error_angle;
	//bound the i_error
		if (i_error_angle > 18000){i_error_angle = 18000;}
		if (i_error_angle < -18000){i_error_angle = -18000;}
	//target q pid control
		*target_current_q = 1*(p_const_mech_angle*error_angle + i_const_mech_angle*i_error_angle + d_const_mech_angle*d_error_angle)/100;
	//offset target_current_q for static friction
		if(error_angle>0){*target_current_q -= 0;}
		if(error_angle<0){*target_current_q += 0;}
	//reduce target_current_q if torque of target_current_q is smaller than static friction
//		if(error_angle==0 && *target_current_q<130 && *target_current_q>-130){
//			*target_current_q = 0;//(*target_current_q*50)/100;
//		}
	
		return i_error_angle;
}

void d_const_mech_angle_change(s16 target_speed){
	if(target_speed >= -20 && target_speed <= 20){d_const_mech_angle = 8000;}
	else if(target_speed >= -40 && target_speed <= 40){d_const_mech_angle = 12000;}
	else {d_const_mech_angle = 25000;}
}
