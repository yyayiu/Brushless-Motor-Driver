#include "Speed_control.h"
#include "Angle_control.h"

//pid constant for speed control(high speed)
	s16	p_const_speed = 100;// 
	s16	i_const_speed = 40;
	s16	d_const_speed = 0;

// speed pid error
	s16		error_speed = 0;
	s32 i_error_speed = 0;
	s16 d_error_speed = 0;


void rpm_to_t_n(u16* t, s16* n, s16 target_speed){
	
	u16 remainder = -1; // reminder = inf.
	s16 direct = 1;
	u8 max_index = 0;
	
	// check the target_speed
		if(target_speed==0){*t=1;	*n=0;	return;}
		if(target_speed<0){direct = -1; target_speed *= -1;}
	
	// control max. index
		if(target_speed<=27){max_index = 3;}
		else{max_index = 5;}
	
	//determine the error for each n, compute the n, t
		for(u8 index = 1; index <= max_index; index++){
			// find the remainder
				u16 remainder_of_index = (1875*index)%(8*target_speed);
						/*	above equation obtain from:
							w rpm
							1024w counts / 60s
							1024w counts / 60*1000*4 ticks	(1 ticks = 250 us)
							1024w n-counts / 60*1000*4*n ticks	
							60*1000*4*n / 1024w		ticks/n-counts
							8175*n/8*w	ticks/n-counts
					*/

			// check if the remainder is more closer to the next integer
			u8 add_one = 0;
				if (remainder_of_index > 4*target_speed){
					remainder_of_index = 8*target_speed - remainder_of_index;
					add_one = 1;
				}
				else{
					add_one = 0;
				}
			// update if the current index have the min. error
				if( remainder_of_index < remainder ){
					//get the t and n
						*t = (1875*(index))/(8*target_speed) + add_one;
						*n = index * direct;
						
					remainder = remainder_of_index;
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
	
	//if(target_speed > 50){return;}
	
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

// high speed control
void speed_pid_control(s16 target_speed, s16 speed, s16* target_current_q){
	
	//speed error update
		d_error_speed = (speed-target_speed) - error_speed;
			error_speed = speed-target_speed;
		i_error_speed += error_speed;
	//target q pid control
		*target_current_q = -1*(p_const_speed*error_speed + i_const_speed*i_error_speed + d_const_speed*d_error_speed) / 100;	
	
	if (target_speed > 0){*target_current_q += 60;}
	else if (target_speed < 0){*target_current_q -= 60;}
	
}





// Speed control for all speed range
s32 stored_AbsEnc_increase_10000 = 0;
s32 count_per_ticks_10000 = 0;
void speed_control(s16 target_speed, s16* target_angle, u32 this_ticks){
	
	//d_const_mech_angle_change(target_speed);
	
	static u32 last_mech_angle_ticks = 0;
	if (this_ticks - last_mech_angle_ticks >= 1){
		count_per_ticks_10000 = (1024*10*target_speed)/(60*4);		//10000* (1024*w)/(60*1000*4) (10000*count/ticks)
		stored_AbsEnc_increase_10000 += count_per_ticks_10000;
		
		*target_angle += (stored_AbsEnc_increase_10000/10000);
		stored_AbsEnc_increase_10000 = (stored_AbsEnc_increase_10000%10000);
		
		if(*target_angle>=1024){
			*target_angle -= 1024;
		}
		if(*target_angle<0){
			*target_angle += 1024;
		}
		
		last_mech_angle_ticks = this_ticks;
	}
	
}

void print_speed_control_data(void){
	uart_tx_blocking(COM3, " %d %d\n", stored_AbsEnc_increase_10000, count_per_ticks_10000);
}


