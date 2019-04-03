#ifndef _LASER_DT50_H
#define _LASER_DT50_H

/**
*	Laser Sensor for stm32f103
*	@Desciption This is the library for the SICK DT50 laser sensor, which mesaures the distance from 200mm to 5000mm with 3.5% error from the board
*	@Reminder 	A 220nF capacitor must be presented at the adc port in order to filter out the noise, default ADC used are ADC_1, ADC_2 and ADC_3
*	@Author 		James Mok
**/

#include "adc.h"

//Calibration constant
static const u16 min_adc = 5;
static const u16 max_adc = 2910;
static const u16 min_dis = 200;
static const u16 max_dis = 5000;

/*
static const u16 min_adc[ADC_COUNT] = {5, 5};
static const u16 max_adc[ADC_COUNT] = {2910, 2910};
static const u16 min_dis[ADC_COUNT] = {200, 200};
static const u16 max_dis[ADC_COUNT] = {5000, 5000};
*/

/**
	Get the calculated reading from the adc value.
	@param device: The adc port to be read from
	@return measured distance in mm
*/
u16 get_ls_cal_reading(AdcID device);

/**
	Get the uncalculated raw reading.
	@param device: The adc port to be read from
	@return adc value
*/
u16 get_ls_adc_reading(AdcID device);
 
#endif	
