/**
* A library for GP2Y0A41SK ADC IR sensor
* Range: 4~30 cm, out of range reading is unreliable
*	Within range, voltage level has linear relationship with 1/distance
**/

#include "IR_GP2Y0A41SK.h"

/** This magic constant comes from a formula
* Range = 1 / (adc_reading * (1/adc_min_reading - 1/adc_max_reading) / (IR_MAX_RANGE - IR_MIN_RANGE))
* Therefore, IR_MAGIC_CONSTANT = (IR_MAX_RANGE - IR_MIN_RANGE) / (1/adc_min_reading - 1/adc_max_reading)
*/
#define IR_MAGIC_CONSTANT 145834

u16 get_gp2y_dis(AdcID adc_id){
	return IR_MAGIC_CONSTANT/adc_get(adc_id);
}

#undef IR_MAGIC_CONSTANT
