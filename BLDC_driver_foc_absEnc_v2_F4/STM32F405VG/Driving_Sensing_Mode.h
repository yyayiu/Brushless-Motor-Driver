#ifndef DRIVING_SENSING_MODE_H
#define DRIVING_SENSING_MODE_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

enum Mode{driving = 0, sensing};

//HS_opamp function
void HS_opamp_init(void);
void HS_opamp(enum Mode mode);	

//Sensing MOSFET function
void Sensing_MOSFET_init(void);
void Sensing_MOSFET(enum Mode mode);	

//Injection_opamp init
void Injection_opamp_init(void);
void Injection_opamp(enum Mode mode); 

#endif
