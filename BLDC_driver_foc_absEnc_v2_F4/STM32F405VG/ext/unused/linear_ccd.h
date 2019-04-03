#ifndef _LINEAR_CCD_H
#define _LINEAR_CCD_H

#include "adc.h"
#include "stm32f4xx.h"
#include "ticks.h"
#include "gpio.h"

extern u32 linear_ccd_buffer[128];

#define SI_PORT &PB0
#define CLK_PORT &PA9

#define ADC_CHANNEL 2

void linear_ccd_init(void);
void linear_ccd_read(void);

#endif
