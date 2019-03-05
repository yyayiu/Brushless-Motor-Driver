#include "Angle_control.h"

//pid constant for position control(elec angle)
	s16	p_const_elec_angle = 1200;
	s16	i_const_elec_angle = 10;
	s16	d_const_elec_angle = 0;
	
// pid error for angle control(elec and mech)
	s16		error_angle = 0;
	s32 i_error_angle = 0;
	s16 d_error_angle = 0;

void elec_angle_control(u16 target_angle, u16 elec_angle, s16* target_current_q){
	
	//elec angle error update
		d_error_angle = (elec_angle-target_angle) - error_angle;
			error_angle = elec_angle-target_angle;
				if(error_angle>180){error_angle -= 360;		d_error_angle -= 360;}
		i_error_angle += error_angle;
	//target q pid control
		*target_current_q = -1*(p_const_elec_angle*error_angle + i_const_elec_angle*i_error_angle + d_const_elec_angle*d_error_angle)/100;	
	//reduce the target_q when angle reach the target
		if(error_angle>0){target_current_q -= 60;}
		if(error_angle<0){target_current_q += 60;}
	
}
