#ifndef PWM_H
#define PWM_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

void PWM_init(void);

void PWM_AL(u16 pwm);
void PWM_NAH(u16 pwm);
void PWM_BL(u16 pwm);
void PWM_NBH(u16 pwm);
void PWM_CL(u16 pwm);
void PWM_NCH(u16 pwm);

void PWM_A(u16 pwm);
void PWM_B(u16 pwm);
void PWM_C(u16 pwm);

void set_PWM(s16 A, s16 B, s16 C);


#endif
