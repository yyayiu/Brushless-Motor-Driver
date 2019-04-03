/**
*	Laser Sensor for stm32f103
*	@Desciption This is the library for the SICK DT50 laser sensor, which mesaures the distance from 200mm to 5000mm with 3.5% error from the board
*	@Reminder 	A 220nF capacitor must be presented at the adc port in order to filter out the noise, default ADC used are ADC_1, ADC_2 and ADC_3
*	@Author 		James Mok
**/

#include "LASER_DT50.h"

static u16 ls_adc_reading[ADC_COUNT];
static u16 ls_cal_reading[ADC_COUNT];

/**
*	@brief return the range found
*	@Param	The index of the ADC data array
* @return the calculated range
**/

u16 get_ls_cal_reading(AdcID device){
	/*
	if(get_adc(device)>=min_adc[device]){
		ls_cal_reading[device] = (get_adc(device) - min_adc[device]) * (max_dis[device] - min_dis[device]) / (max_adc[device] - min_adc[device]) + min_dis[device];
		if(ls_cal_reading[device] >= max_dis[device]){
			ls_cal_reading[device] = max_dis[device];
		}
	}	
	*/
	if(get_adc(device)>=min_adc){
		ls_cal_reading[device] = (get_adc(device) - min_adc) * (max_dis - min_dis) / (max_adc - min_adc) + min_dis;
		if(ls_cal_reading[device] >= max_dis){
			ls_cal_reading[device] = max_dis;
		}
	}else{
		ls_cal_reading[device] = 0;
	}
	
	return ls_cal_reading[device];
}

/**
*	@brief return the ADC value for tuning the static const array above
*	@Param	The index of the ADC data array
* @return the ADC value of the sensors
**/

u16 get_ls_adc_reading(AdcID device){
	ls_adc_reading[device] = get_adc(device);
	return ls_adc_reading[device];
}
