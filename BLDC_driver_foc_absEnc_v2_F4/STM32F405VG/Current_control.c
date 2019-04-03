#include "Current_control.h"

//pid constant for current control(dq)
	s16 p_const_d = 10;	//10
	s16 i_const_d = 30;	//30
	s16 d_const_d = 20;	//20
	s16 p_const_q = 10;//0;	//10
	s16 i_const_q = 20;//20;	//30
	s16 d_const_q = 0;//0;	//30
	
// pid error for current control(dq)
	s16   error_current_d = 0;
	s32 i_error_current_d = 0;
	s16 d_error_current_d = 0;
	s16   error_current_q = 0;
	s32 i_error_current_q = 0;
	s16 d_error_current_q = 0;
	
	
// FOC current_control_dq-frame
void Current_control_FOC_dq_Frame(u16 elec_angle, s16 target_current_d, s16 target_current_q, s16 current_d, s16 current_q, s16* pwm_A, s16* pwm_B, s16* pwm_C){
	
	s16 pwm_d;
	s16 pwm_q;
	
	//current error update
		d_error_current_d = (current_d-target_current_d) - error_current_d;
		d_error_current_q = (current_q-target_current_q) - error_current_q;
			error_current_d = current_d-target_current_d;
			error_current_q = current_q-target_current_q;
		i_error_current_d += error_current_d;
		i_error_current_q += error_current_q;
	
	//PWM pid control
		pwm_d = 1*(p_const_d*error_current_d + i_const_d*i_error_current_d + d_const_d*d_error_current_d)/1000;//151215
		pwm_q = 1*(p_const_q*error_current_q + i_const_q*i_error_current_q + d_const_q*d_error_current_q)/1000;//002000
	
		dq_to_abc(elec_angle, pwm_A, pwm_B, pwm_C, &pwm_d, &pwm_q);
	
}
