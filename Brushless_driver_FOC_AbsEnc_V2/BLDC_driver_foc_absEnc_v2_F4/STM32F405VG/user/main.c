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
#include "led.h"

//pid constant for current control(abc) 420 000 090 /1000
s16 p_const_abc = 420;
s16 i_const_abc = 0;
s16 d_const_abc = 90;

//pid constant for current control(dq) /100
s16 p_const_d = 0;	//10
s16 i_const_d = 0;	//30
s16 d_const_d = 0;	//20
s16 p_const_q = 0;	//10
s16 i_const_q = 0;	//30
s16 d_const_q = 0;	//30

//pid constant for position control(elec angle)
s16	p_const_elec_angle = 500;
s16	i_const_elec_angle = 10;
s16	d_const_elec_angle = 0;

//pid constant for position control(mech angle)
s16	p_const_mech_angle = 1200;
s16	i_const_mech_angle = 10;
s16	d_const_mech_angle = 0;

//pid constant for speed control
s16	p_const_speed = 0;
s16	i_const_speed = 0;
s16	d_const_speed = 0;

u16 start = 0;
u32 start_ticks = 0;

s16 target_angle = 0;

static u32 zero_mean[3] = {0};
u8 pd_const_index[12] = {1, 5, 1, 2, 1, 5, 0, 0, 2, 0, 0, 0};
u8 pd_const_ptr = 0;
//uart
void Uart_listener(uint8_t byte){
	if(byte=='n'){uart_tx_blocking(COM3, "\n");}
	
	if(byte>=48 && byte<=57){
		pd_const_index[pd_const_ptr%12] = byte-48;
		pd_const_ptr++;
	}
	if(byte=='.'){
		p_const_d = pd_const_index[0]*10 + pd_const_index[1]*1;
		i_const_d = pd_const_index[2]*10 + pd_const_index[3]*1;
		d_const_d = pd_const_index[4]*10 + pd_const_index[5]*1;
		p_const_q = pd_const_index[6]*10 + pd_const_index[7]*1;
		i_const_q = pd_const_index[8]*10 + pd_const_index[9]*1;
		d_const_q = pd_const_index[10]*10 + pd_const_index[11]*1;
		uart_tx_blocking(COM3, "%% d:\np=%d;\ni=%d;\nd=%d;\n", p_const_d, i_const_d, d_const_d);
		uart_tx_blocking(COM3, "%% q:\np=%d;\ni=%d;\nd=%d;\n", p_const_q, i_const_q, d_const_q);
		start=1;
	}
	if(byte=='a'){
		set_PWM(950, 1000, 1000);
	}
	if(byte=='s'){
		set_PWM(1000, 970, 1000);
	}
	if(byte=='d'){
		set_PWM(1000, 1000, 970);
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
		
//	DAC_enable_init();
//	DAC_enable(ALL_DISABLE);
	
	PWM_init();
	
	//current sensing init 
	current_sensing_init();
	while (get_ticks() < 2000);
		
	TIM_SetCounter(TIM1,0);
	TIM_SetCounter(TIM3,0);
	set_PWM(1000, 1000, 1000);
	while (get_ticks() < 6000); 
	cal_zero_mean(zero_mean);
	}//init
	

	
	//Current Sensing
	s32 current_cumulationA = 0;
	s32 current_cumulationB = 0;
	s32 current_cumulationC = 0;
	u16 cumulating_count = 0;
	
	//buffer
	s32 buffer_current_A[300];
	s16 buffer_current_B[300];
	s16 buffer_current_C[300];
	s16 buffer_current_D[300];
	s16 buffer_current_Q[300];
	s32 buffer_angle[300];
	s16 buffer_count = 0;
	
	//data for control	
		//Angle data
		u16 last_AbsEnc = 0;
		u16 this_AbsEnc = 0;
		s16 speed = 0;
		u16 elec_angle = 0;
		
		//Current data
		s32 current_A = 0;
		s32 current_B = 0;
		s32 current_C = 0;
		s16 current_error_mean = 0;
		s32 current_d = 0;
		s32 current_q = 0;
		
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
		
		s16   error_current_d = 0;
		s32 i_error_current_d = 0;
		s16 d_error_current_d = 0;
		s16   error_current_q = 0;
		s32 i_error_current_q = 0;
		s16 d_error_current_q = 0;
		
		s16		error_angle = 0;
		s32 i_error_angle = 0;
		s16 d_error_angle = 0;
		
		s16		error_speed = 0;
		s32 i_error_speed = 0;
		s16 d_error_speed = 0;
		
		s32 change_d_1000 = 0;
		s32 change_q_1000 = 0;
		s32 change_A_1000 = 0;
		s32 change_B_1000 = 0;
		s32 change_C_1000 = 0;
			
		//PWM data
		s16 pwm_A = 1000;
		s16 pwm_B = 1000;
		s16 pwm_C = 1000;
		
		//target_data
		s32 target_current_A = 0;
		s32 target_current_B = 0;
		s32 target_current_C = 0;
		s32 target_current_d = 0;
		s32 target_current_q = 150;	//max = 270
		
		s16 target_speed = 580;	//10*rpm
		
		u8 var = 0;
		u16 static check=0;
			
	uart_tx_blocking(COM3, "%% P and D\n");

	while(1){
		u32 this_ticks = get_ticks();		//1 tick = 500us
		
		/* led blinking */
		u32 static last_led_ticks = 0;
		if(this_ticks - last_led_ticks>500){	//1000*500us = 0.5s
			led_blink(LED_1);
			last_led_ticks = this_ticks;
			
			//uart_tx_blocking(COM3, "%d\n", AbsEnc_data());
		}
		
		
		
		/* start after the uart give response */
		if(start==0){start_ticks = get_ticks();	continue;}
		
		//////////////////// if start = 1 ////////////////////	
		//Absolute Encoder update
			this_AbsEnc = AbsEnc_data();
			elec_angle = get_elec_angle(this_AbsEnc);

		//Speed update
			u32 static last_speed_ticks = 0;
			if(this_ticks - last_speed_ticks >= 300){	//10*500us = 5ms
				//get speed
					speed = get_velcity(this_AbsEnc, last_AbsEnc);	
					last_AbsEnc = this_AbsEnc;
				
				//debug 
					//uart_tx(COM3, "%d, %d, %d, %d, %d\n", target_angle, (this_AbsEnc*360)/1024, this_AbsEnc, target_current_q, current_q);
					uart_tx(COM3, "%d ", this_AbsEnc);
				
				/* speed control 1: pid control 			
				//target speed change unit
					target_speed = target_speed*5120/360000;	// mech/s => AbsEnc/10ticks
				//speed error update
					d_error_speed = (speed-target_speed) - error_speed;
						error_speed = speed-target_speed;
					i_error_speed += error_speed;
				//target q pid control
					target_current_q = -1*(p_const_speed*error_angle + i_const_speed*i_error_angle + d_const_speed*d_error_angle);	
				*/
				
				
				
				last_speed_ticks = this_ticks;
			}	
			
		//current data cummulation
			current_1000_update(zero_mean);
			current_cumulationA += get_instant_current_A();
			current_cumulationB += get_instant_current_B();
			current_cumulationC += get_instant_current_C();
			++cumulating_count;
		
		/* speed control 2: Timer control angle, target speed: mech_angle/min , max. speed: 1-mech_degree/ticks(0.5ms)=>333rpm */
			if(this_ticks-start_ticks>3000){
				//calculate time step for increasing target_mech_angle by one
					u32 time_step = 1200000/(1024*target_speed);	//min=2 => max_speed=58.6rpm,  max=117 => min_speed(1.002 rpm)
				//increase target_mech_angle for each time step
					static u32 last_target_mech_angle_increase_ticks = 0;
					if (this_ticks - last_target_mech_angle_increase_ticks >= time_step) {
						target_angle = target_angle + 1;
						if(target_angle>=1024){
							target_angle -= 1024;
						}
						if(target_angle<0){
							target_angle += 1024;
						}
						last_target_mech_angle_increase_ticks = this_ticks;
					}
				}
		
		
		u32 static last_pwm_update_ticks = 0;
		if (this_ticks - last_pwm_update_ticks >= 2){	//500us*k
			//calculate current data
				//get current data
					current_A = current_cumulationA / cumulating_count;
					current_B = current_cumulationB / cumulating_count;
					current_C = current_cumulationC / cumulating_count;
				//distribute the error to three current 
					current_error_mean = (current_A + current_B + current_C)/3;
					current_A -= current_error_mean;
					current_B -= current_error_mean;
					current_C -= current_error_mean;
				//frame transformation1=
					abc_to_dq(elec_angle, &current_A, &current_B, &current_C, &current_d, &current_q);
				//reset current cumulating variable
					current_cumulationA = 0;
					current_cumulationB = 0;
					current_cumulationC = 0;
					cumulating_count = 0;					
					
			//Position Control
				/* elec_angle control	
					if(this_ticks-start_ticks>3000){
						//elec angle error update
							d_error_angle = (elec_angle-target_angle) - error_angle;
								error_angle = elec_angle-target_angle;
									if(error_angle>180){error_angle -= 360;		d_error_angle -= 360;}
							i_error_angle += error_angle;
						//target q pid control
							target_current_q = -1*(p_const_elec_angle*error_angle + i_const_elec_angle*i_error_angle + d_const_elec_angle*d_error_angle)/100;	
						//reduce the target_q when angle reach the target
							if(error_angle>0){
								target_current_q -= 60;
							}
							if(error_angle<0){
								target_current_q += 60;
							}
					}
				*/
				/* mechinical angle control	*/	
					if(this_ticks-start_ticks>3000){
					//mechinical angle error update
							d_error_angle = (this_AbsEnc-target_angle) - error_angle;
								error_angle =  this_AbsEnc-target_angle;
									if(error_angle>512){error_angle -= 1024;		d_error_angle -= 1024;}
									if(error_angle<-512){error_angle += 1024;		d_error_angle += 1024;}
							i_error_angle += error_angle;
						//target q pid control
							target_current_q = -1*(p_const_mech_angle*error_angle + i_const_mech_angle*i_error_angle + d_const_mech_angle*d_error_angle)/100;
						//reduce the target_q when angle reach the target
							if(error_angle>0){
								target_current_q -= 60;
							}
							if(error_angle<0){
								target_current_q += 60;
							}
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
						//current error update
							d_error_current_d = (current_d-target_current_d) - error_current_d;
							d_error_current_q = (current_q-target_current_q) - error_current_q;
								error_current_d = current_d-target_current_d;
								error_current_q = current_q-target_current_q;
							i_error_current_d += error_current_d;
							i_error_current_q += error_current_q;
						//PWM pid control
							change_d_1000 = 1*(p_const_d*error_current_d + i_const_d*i_error_current_d + d_const_d*d_error_current_d)/1000;//151215
							change_q_1000 = 1*(p_const_q*error_current_q + i_const_q*i_error_current_q + d_const_q*d_error_current_q)/1000;//002000
							dq_to_abc(elec_angle, &change_A_1000, &change_B_1000, &change_C_1000, &change_d_1000, &change_q_1000);
							pwm_A = (change_A_1000);
							pwm_B = (change_B_1000);
							pwm_C = (change_C_1000);
				}
					
				/* voltage control (FOC on abc-frame) 
				static u8 c_100 = 55;
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
				} */
				
				
			
			

			
			//correct the PWM value
				/* method 1: set the max. pwm to 1000, shift the other
				if(pwm_A>=pwm_B && pwm_A>=pwm_C){
					pwm_B += (1000-pwm_A);
					pwm_C += (1000-pwm_A);
					pwm_A = 1000;
				}
				else if (pwm_B>=pwm_A && pwm_B>=pwm_C) {
					pwm_A += (1000-pwm_B);
					pwm_C += (1000-pwm_B);
					pwm_B = 1000;
				}
				else if (pwm_C>=pwm_A && pwm_C>=pwm_B){
					pwm_A += (1000-pwm_C);
					pwm_B += (1000-pwm_C);
					pwm_C = 1000;
				}
				*/
				/* mwthod 2: fix pwm A to 500 
				pwm_B = 500 + (pwm_B - pwm_A);
				pwm_C = 500 + (pwm_C - pwm_A);
				pwm_A = 500;
				*/
				/* method 3: mean pwm = 500 */
				u16 shift_pwm = 500 - (pwm_A + pwm_B + pwm_C)/3;
				pwm_A += shift_pwm;
				pwm_B += shift_pwm;
				pwm_C += shift_pwm;
				
			//set PWM
				set_PWM(pwm_A, pwm_B, pwm_C);

			//store data
				/* store data for each iteration */
				abc_to_dq(elec_angle, &current_A, &current_B, &current_C, &current_d, &current_q);
				dq_to_abc(elec_angle, &target_current_A, &target_current_B, &target_current_C, &target_current_d, &target_current_q);
				if(this_ticks-start_ticks>5000){
					if(var==0){
						buffer_current_A[buffer_count] = current_A;
						buffer_current_B[buffer_count] = current_B;
						buffer_current_C[buffer_count] = current_C;		
						buffer_angle[buffer_count] = elec_angle;
						buffer_current_D[buffer_count] = current_d;
						buffer_current_Q[buffer_count] = current_q;
						//++buffer_count;
					}
					//var = (var+1)%2;
				} 
				
			//ticks update
				last_pwm_update_ticks = this_ticks;
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
		
		
		
		//send data
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
