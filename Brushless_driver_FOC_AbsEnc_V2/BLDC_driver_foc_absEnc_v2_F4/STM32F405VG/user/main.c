/**
** H K U S T
** Robocon 2018
**
** F4 Library
** Please do not edit this project directly, copy first.
**
*/
#include "main.h"

#include "../AbsEnc.h"
#include "../PWM.h"
#include "../CurrentSensing.h"
#include "../dacdac.h"
#include "../Speed_control.h"
#include "../Current_control.h"
#include "led.h"

//pid constant
	//pid constant for current control(abc) 420 000 090 /1000
		s16 p_const_abc = 420;
		s16 i_const_abc = 0;
		s16 d_const_abc = 90;



//starting variable
	u16 start = 0;
	u32 start_ticks = 0;

//Current Sensing	
	s32 current_cumulationA = 0;
	s32 current_cumulationB = 0;
	s32 current_cumulationC = 0;
	u16 cumulating_count = 0;

//data buffer
	s32 buffer_current_A[300];
	s16 buffer_current_B[300];
	s16 buffer_current_C[300];
	s16 buffer_current_D[300];
	s16 buffer_current_Q[300];
	s32 buffer_angle[300];
	
	s16 buffer_count = 0;
	u8 sd_down_sampling_var = 0;

//PWM data
	s16 pwm_A = 1000;
	s16 pwm_B = 1000;
	s16 pwm_C = 1000;

//data for control	
	//Angle data
		u16 last_AbsEnc = 0;
		u16 this_AbsEnc = 0;
		s16 speed = 0;
		u16 elec_angle = 0;
	
	//Current data
		s16 current_A = 0;
		s16 current_B = 0;
		s16 current_C = 0;
		s16 current_error_mean = 0;
		s16 current_d = 0;
		s16 current_q = 0;
	
	//Error data
		s16   error_current_A = 0;
		s16 i_error_current_A = 0;
		s16 d_error_current_A = 0;		
		s16   error_current_B = 0;
		s16 i_error_current_B = 0;
		s16 d_error_current_B = 0;
		s16   error_current_C = 0;
		s16 i_error_current_C = 0;
		s16 d_error_current_C = 0;
		
		s32 change_A_1000 = 0;
		s32 change_B_1000 = 0;
		s32 change_C_1000 = 0;
	
	//target_data
		s16 target_current_A = 0;
		s16 target_current_B = 0;
		s16 target_current_C = 0;
		s16 target_current_d = 0;
		s16 target_current_q = 150;	//max = 270
		s16 target_angle = 0;
		s16 target_speed = 0;	//rpm

		
//uart
u8 pd_const_index[12] = {1, 5, 1, 2, 1, 5, 0, 0, 2, 0, 0, 0};
u8 pd_const_ptr = 0;
void Uart_listener(uint8_t byte){
	if(byte=='n'){uart_tx_blocking(COM3, "\n");}
	
	if(byte>=48 && byte<=57){
		pd_const_index[pd_const_ptr%12] = byte-48;
		pd_const_ptr++;
	}
	if(byte=='.'){
//		p_const_d = pd_const_index[0]*10 + pd_const_index[1]*1;
//		i_const_d = pd_const_index[2]*10 + pd_const_index[3]*1;
//		d_const_d = pd_const_index[4]*10 + pd_const_index[5]*1;
//		p_const_q = pd_const_index[6]*10 + pd_const_index[7]*1;
//		i_const_q = pd_const_index[8]*10 + pd_const_index[9]*1;
//		d_const_q = pd_const_index[10]*10 + pd_const_index[11]*1;
//		uart_tx_blocking(COM3, "%% d:\np=%d;\ni=%d;\nd=%d;\n", p_const_d, i_const_d, d_const_d);
//		uart_tx_blocking(COM3, "%% q:\np=%d;\ni=%d;\nd=%d;\n", p_const_q, i_const_q, d_const_q);
		start=1;
	}
	if(byte=='a'){
		set_PWM(930, 1000, 1000);
	}
	if(byte=='s'){
		set_PWM(1000, 930, 1000);
	}
	if(byte=='d'){
		set_PWM(1000, 1000, 930);
	}
	if(byte=='f'){
		set_PWM(1000, 1000, 1000);
	}
	if(byte=='1'){
		target_angle += 2;
		if(target_angle>=1024){target_angle-=1024;}
		if(target_angle<0){target_angle+=1024;}
	}
	if(byte=='2'){
		target_angle -= 2;
		if(target_angle>=1024){target_angle-=1024;}
		if(target_angle<0){target_angle+=1024;}
	}
	if(byte=='3'){
		target_speed += 2;
	}
	if(byte=='4'){
		target_speed -= 2;
	}
	if(byte=='5'){
		target_current_q += 5;
	}
	if(byte=='6'){
		target_current_q -= 5;
	}
}


int main(void) {
		
	{//init	
		
		SystemInit();
		SystemCoreClockUpdate();
		gpio_rcc_init_all();
		ticks_init();
		timer_init();
		uart_init(COM3, 115200);
		uart_interrupt_init(COM3, *Uart_listener);
		AbsEnc_init();
		led_init();
		PWM_init();	
		current_sensing_init(get_ticks());		
		
		HS_opamp_init();
		HS_opamp_enable();	//for driving
		
		//DAC_enable_init();
		//DAC_enable(ALL_DISABLE);
		
		uart_tx_blocking(COM3, "%% init done\n");
	}//init
	
	
	while(1){
		
		u32 this_ticks = get_ticks();		//1 tick = 250us
		
		/* led blinking */
			u32 static last_led_ticks = 0;
			if(this_ticks - last_led_ticks>1000){	//1000*500us = 0.5s
				led_blink(LED_1);
				last_led_ticks = this_ticks;			
			}
		
		/* start after the uart give response */
			if(start==0){start_ticks = get_ticks();	continue;}
		
		//////////////////// if start = 1 //////////////////////////////////
			
		/* Absolute Encoder update */
			u32 static last_angle_ticks = 0;
			if(this_ticks - last_angle_ticks >= 4){	//for every 1ms
				//AbsEnc update
					this_AbsEnc = AbsEnc_data();
					elec_angle = get_elec_angle(this_AbsEnc);
									
				//ticks update
					last_angle_ticks = this_ticks;
			}
			
		/* current data cummulation */
			current_1000_update();
			current_cumulationA += get_instant_current_A();
			current_cumulationB += get_instant_current_B();
			current_cumulationC += get_instant_current_C();
			++cumulating_count;
			
		/* Speed update */
			u32 static last_speed_ticks = 0;
			if(this_ticks - last_speed_ticks >= 200){	//200*250us = 50ms
				//get speed
					speed = get_velcity(this_AbsEnc, last_AbsEnc);
					speed = (speed*75)/64;	// change unit: AbsEnc/50ms => rpm 
					last_AbsEnc = this_AbsEnc;
				
				/* speed control 1: pid control 
					if(this_ticks-start_ticks>3000){
							speed_pid_control(target_speed, speed, &target_current_q);
					}
				*/ 
				
				//ticks update
					last_speed_ticks = this_ticks;
			}	
		
		/* speed control 2: target_speed => time_step & AbsEnc_step 
				if(this_ticks-start_ticks>3000){
					low_speed_control(target_speed, &target_angle, this_ticks);
				}
		*/	
			
		
		/* target_current_q update */
			u32 static last_target_current_q_update_ticks = 0;
			if (this_ticks - last_target_current_q_update_ticks >= 4){	//250us*k
					 
				//Position Control
					/* elec_angle control	
						if(this_ticks-start_ticks>3000){
							elec_angle_control(target_angle, elec_angle, &target_current_q);
						}
					*/
					/* mechinical angle control	
						if(this_ticks-start_ticks>3000){
								mech_angle_control(target_angle, this_AbsEnc, &target_current_q);
						}
					*/
				//ticks update	
					last_target_current_q_update_ticks = this_ticks;			
					
			} // target_current_q update
		
		
		/* PWM update */
			u32 static last_pwm_update_ticks = 0;
			if (this_ticks - last_pwm_update_ticks >= 1){	//250us*k
				
				//calculate current data
				{
					//get current data
						current_A = current_cumulationA / cumulating_count;
						current_B = current_cumulationB / cumulating_count;
						current_C = current_cumulationC / cumulating_count;
					//distribute the error to three current 
						current_error_mean = (current_A + current_B + current_C)/3;
						current_A -= current_error_mean;
						current_B -= current_error_mean;
						current_C -= current_error_mean;
					//frame transformation1
						abc_to_dq(elec_angle, &current_A, &current_B, &current_C, &current_d, &current_q);
					//reset current cumulating variable
						current_cumulationA = 0;
						current_cumulationB = 0;
						current_cumulationC = 0;
						cumulating_count = 0;		
				}
						
				
				//PWM Control	
					/* hard drive 
						static u32 phase_switching_count = 0;
						u16 pwm_value = 970;
						switch(phase_switching_count%3){
							case 0: pwm_A = pwm_value;	pwm_B = 1000;	pwm_C = 1000;
											break;
							case 1: pwm_A = 1000;	pwm_B = pwm_value;	pwm_C = 1000;
											break;
							case 2: pwm_A = 1000;	pwm_B = 1000;	pwm_C = pwm_value;
											break;
							default:	break;
						}
						++phase_switching_count;
					*/
					/* drive with AbsEnc	
						static u16 pwm_absenc = 970;
						switch(elec_angle/60){
							case 0: //0~59
											pwm_A = 1000;	pwm_B = pwm_absenc;	pwm_C = 1000;
											break;
							case 1: //60~119
											pwm_A = 1000;	pwm_B = pwm_absenc;	pwm_C = pwm_absenc;
											break;
							case 2: //120~179
											pwm_A = 1000;	pwm_B = 1000;	pwm_C = pwm_absenc;
											break;
							case 3: //180~239
											pwm_A = pwm_absenc;	pwm_B = 1000;	pwm_C = pwm_absenc;
											break;
							case 4:	//240~299
											pwm_A = pwm_absenc;	pwm_B = 1000;	pwm_C = 1000;
											break;
							case 5:	//300~359
											pwm_A = pwm_absenc;	pwm_B = pwm_absenc;	pwm_C = 1000;
											break;
							default: //error
											 pwm_A = 1000;	pwm_B = 1000;	pwm_C = 1000;
											 break;
						}
					*/
					/* current control (6 angle)
						
						//			phase A PID:  30 0 18
						//			phase B PID:	30 0 16
						//			phase C PID:	30 0 12
						
						static s16 target_current = 80;
						static s16 target_current_half = 40;
						switch(elec_angle/60){
							case 0: //0~59		 	 - | ++ |  -
											target_current_A = 0;											target_current_B = target_current;				target_current_C = 0;
											break;
							case 1: //60~119		-- |  + |  +
											target_current_A = 0;											target_current_B = target_current_half;		target_current_C = target_current_half;
											break;
							case 2: //120~179		 - |  - | ++
											target_current_A = 0;											target_current_B = 0;											target_current_C = target_current;
											break;
							case 3: //180~239		 + | -- |  +
											target_current_A = target_current_half;		target_current_B = 0;											target_current_C = target_current_half;
											break;
							case 4:	//240~299		++ |  - |  -
											target_current_A = target_current;				target_current_B = 0;											target_current_C = 0;
											break;	
							case 5:	//300~359		 + |  + | --
											target_current_A = target_current_half;		target_current_B = target_current_half;		target_current_C = 0;
											break;
							default: //error
											 target_current_A = 0;										target_current_B = 0;											target_current_C = 0;
											 break;
						}
						//if(this_ticks>10100){target_current_A = 30;										target_current_B = 0;											target_current_C = 0;			}
					*/
													
					/* current control (FOC on abc-frame) 
						//convect target current from dq-frame to abc-frame
							dq_to_abc(elec_angle+0, &target_current_A, &target_current_B, &target_current_C, &target_current_d, &target_current_q);
						//current error update
							d_error_current_A = (current_A-target_current_A) - error_current_A;
							d_error_current_B = (current_B-target_current_B) - error_current_B;
							d_error_current_C = (current_C-target_current_C) - error_current_C;
								error_current_A = current_A-target_current_A;
								error_current_B = current_B-target_current_B;
								error_current_C = current_C-target_current_C;
							i_error_current_A += error_current_A;
							i_error_current_B += error_current_B;
							i_error_current_C += error_current_C;
						//PWM pid control (400 000 090)
							//PWM A control
								pwm_A += (p_const_abc*error_current_A + i_const_abc*i_error_current_A + d_const_abc*d_error_current_A)/1000;//35 0 27
								if (pwm_A>1000){pwm_A = 1000;}
								if (pwm_A<0){pwm_A = 0;}
							//PWM B control
								pwm_B += (p_const_abc*error_current_B + i_const_abc*i_error_current_B + d_const_abc*d_error_current_B)/1000;//39 0 8
								if (pwm_B>1000){pwm_B =	 1000;}
								if (pwm_B<0){pwm_B = 0;}
							//PWM C control
								pwm_C += (p_const_abc*error_current_C + i_const_abc*i_error_current_C + d_const_abc*d_error_current_C)/1000;//45 0 11
								if (pwm_C>1000){pwm_C = 1000;}
								if (pwm_C<0){pwm_C = 0;}	
					*/
					
					/* current control (FOC on dq-frame)	*/
						if(this_ticks-start_ticks>3000){
							Current_control_FOC_dq_Frame(elec_angle, target_current_d, target_current_q, current_d, current_q, &pwm_A, &pwm_B, &pwm_C);
						}
						
					/* voltage control (FOC on abc-frame) 
						static u8 c_100 = 45;
						dq_to_abc(elec_angle, &target_current_A, &target_current_B, &target_current_C, &target_current_d, &target_current_q);
						if (target_current_A <= target_current_B && target_current_A <= target_current_C) {
							pwm_A = 1000;
							pwm_B = 1000 - (2*target_current_B + target_current_C)*c_100/100;
							pwm_C = 1000 - (2*target_current_C + target_current_B)*c_100/100;
						}
						else if (target_current_B <= target_current_A && target_current_B <= target_current_C) {
							pwm_A = 1000 - (2*target_current_A + target_current_C)*c_100/100;
							pwm_B = 1000;
							pwm_C = 1000 - (2*target_current_C + target_current_A)*c_100/100;
						}
						else {
							pwm_A = 1000 - (2*target_current_A + target_current_B)*c_100/100;
							pwm_B = 1000 - (2*target_current_B + target_current_A)*c_100/100;
							pwm_C = 1000;
						} 
					*/			
				
				
				
				//Update PWM
					//pwm_update(pwm_A, pwm_B, pwm_C);

				//store data
					/* store data in iterations 
						dq_to_abc(elec_angle, &target_current_A, &target_current_B, &target_current_C, &target_current_d, &target_current_q);
						if(this_ticks-start_ticks>10000){
							if(sd_down_sampling_var==0){
								buffer_current_A[buffer_count] = current_A;
								buffer_current_B[buffer_count] = current_B;
								buffer_current_C[buffer_count] = current_C;		
								buffer_angle[buffer_count] = elec_angle;
								buffer_current_D[buffer_count] = current_d;
								buffer_current_Q[buffer_count] = current_q;
								++buffer_count;			// comment this to skip the data storing
							}
							sd_down_sampling_var = (sd_down_sampling_var+1)%4;		// for every 1 iteration (0.5ms) 
						}
					*/
					
				
				//ticks update
					last_pwm_update_ticks = this_ticks;
					
			}	 // pwm update
		
		/* uart debug message */
			u32 static last_debug_ticks = 0;
			if(this_ticks - last_debug_ticks >= 200){	//200*250us = 50ms
				
					abc_to_dq(elec_angle, &current_A, &current_B, &current_C, &current_d, &current_q);
					uart_tx(COM3, "%d ", this_AbsEnc);
				 
				last_debug_ticks = this_ticks; 
			}
		
		
		/* store data with fixed period 
			//delay
				if(this_ticks-start_ticks<5000){continue;} 
			//store data
				u32 static data_storing_ticks = 0;
				if (this_ticks - data_storing_ticks >= 1){	//sample pweiod = 500us*k
					//storing data
						buffer_current_A[buffer_count] = get_instant_current_A();
						buffer_current_B[buffer_count] = get_instant_current_B();
						buffer_current_C[buffer_count] = get_instant_current_C();
						buffer_angle[buffer_count] = elec_angle/60;
						buffer_current_D[buffer_count] = 0;
						buffer_current_Q[buffer_count] = 0;
						++buffer_count;	
						
					
					data_storing_ticks = this_ticks;
				}
		*/
		
		
		/* send data */
			if(buffer_count==300){		
				buffer_count=0;
				set_PWM(1000, 1000, 1000);

				//testing function
				uart_tx_blocking(COM3, "testing([");
				for(u16 i=0; i<300; i++){
					uart_tx_blocking(COM3, "%d ", buffer_current_A[i]);
				}
				uart_tx_blocking(COM3, ";\n ");
				for(u16 i=0; i<300; i++){
					uart_tx_blocking(COM3, "%d ", buffer_current_B[i]);
				}
				uart_tx_blocking(COM3, ";\n ");
				for(u16 i=0; i<300; i++){
					uart_tx_blocking(COM3, "%d ", buffer_current_C[i]);
				}
				uart_tx_blocking(COM3, ";\n ");
				for(u16 i=0; i<300; i++){
					uart_tx_blocking(COM3, "%d ", buffer_angle[i]);
				}
				uart_tx_blocking(COM3, ";\n ");
				for(u16 i=0; i<300; i++){
					uart_tx_blocking(COM3, "%d ", buffer_current_D[i]);
				}
				uart_tx_blocking(COM3, ";\n ");
				for(u16 i=0; i<300; i++){
					uart_tx_blocking(COM3, "%d ", buffer_current_Q[i]);
				}
				uart_tx_blocking(COM3, ";], 300);\n");
				
				//dq_scatter function
				uart_tx_blocking(COM3, "dq_scatter([");
				for(u16 i=0; i<300; i++){
					uart_tx_blocking(COM3, "%d ", buffer_current_D[i]);
				}
				uart_tx_blocking(COM3, ";");
				for(u16 i=0; i<300; i++){
					uart_tx_blocking(COM3, "%d ", buffer_current_Q[i]);
				}
				uart_tx_blocking(COM3, ";]);\n");
				
				while(1);	//stop the program
			}
		
	
	}//while 1 
	uart_tx_blocking(COM3, "END of main");
}//main
