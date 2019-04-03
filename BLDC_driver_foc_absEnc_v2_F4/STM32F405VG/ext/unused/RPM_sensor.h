#ifndef __RPM_SENSOR_H__
#define	__RPM_SENSOR_H__

#include "stm32f10x_tim.h"
#include "encoder.h"
#include "servo.h"
#include "approx_math.h"
#include "ticks.h"

#define BURSHLESS_S

#define RPMs_Count_TIM	TIM6
#define RPMs_In_TIM			TIM5
#define RPMs_PID_TIM		TIM9
#define RPMs_servo			SERVO3
#define diffScale				1000000
#define PIDScale				1000
#define	CountPerRo			6

#ifdef	BRUSHLESS_R
#define RPMs_TIM_Period	SystemCoreClock / 100000 - 1
#define diffScale				10000

#define RPMs_UpLim			150
#define	RPMs_DownLim		30
#define PWM_UpLim				1000
#define	PWM_DownLim			600
#define PWM_OFF					450
#define RoMulti					1
#endif

#ifdef  BURSHLESS_S
#define RPMs_TIM_Period	SystemCoreClock / 1000000 - 1
#define diffScale				1000000

#define RPMs_UpLim			1500
#define	RPMs_DownLim		200
#define PWM_UpLim				1000
#define	PWM_DownLim			600
#define PWM_OFF					450
#define RoMulti					2
#endif

enum PID_state{
	PID_OFF = 0,
	PID_ON};

void RPMs_init(void);
void RPMs_update(s32, s32, s32);
void set_tar_val(u16);
void set_PID_FLAG(u8);

u32 get_pulse(void);
u32 get_RPS(void);
u32 get_diff(void);
u8	get_PID_FLAG(void);

#endif
