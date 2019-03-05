#include "Speed_control.h"

void rpm_to_t_n(u16* t, s16* n, s16 target_speed){
	
	u16 reminder = -1; // reminder = inf.
	s16 direct = 1;
	u8 max_index = 5;
	
	// check the target_speed
	if(target_speed==0){*t=1;	*n=0;	return;}
	if(target_speed<0){direct = -1; target_speed *= -1;}
	
	// control max. index
	if(target_speed<=27){max_index = 3;}
	
	for(u8 index = 1; index <= max_index; index++){
		// find the error
		u16 reminder_of_index = (1875*index)%(16*target_speed);
		u8 add_one = 0;
		if (reminder_of_index > 8*target_speed){
			reminder_of_index = 16*target_speed - reminder_of_index;
			add_one = 1;
		}
		else{
			add_one = 0;
		}
		
		// get the n with min. error
		if( reminder_of_index < reminder ){
			// update if error is smaller
			*t = (1875*(index))/(16*target_speed) + add_one;
			*n = index * direct;
			
			reminder = reminder_of_index;
		}
	}
	
	/* debug message 
	uart_tx_blocking(COM3, "%d %d %d\n", target_speed, *t, *n);
	*/
}



// Low speed control, 1~50 rpm
u16 time_step = 117;
s16 AbsEnc_step = 1;
void low_speed_control(s16 target_speed, s16* target_angle, u32 this_ticks){
	
	if(target_speed > 50){return;}
	
	//calculate time step and AbsEnc step
		rpm_to_t_n(&time_step, &AbsEnc_step, target_speed);
	//increase target_mech_angle for each time step
		static u32 last_target_mech_angle_increase_ticks = 0;
		
	if (this_ticks - last_target_mech_angle_increase_ticks >= time_step) {
		*target_angle = *target_angle + AbsEnc_step;
		
		if(*target_angle>=1024){
			*target_angle -= 1024;
		}
		if(*target_angle<0){
			*target_angle += 1024;
		}
		last_target_mech_angle_increase_ticks = this_ticks;
	}
	
	//debug
	//uart_tx_blocking(COM3, "%d %d\n", time_step, AbsEnc_step);
}

