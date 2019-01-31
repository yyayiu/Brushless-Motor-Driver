#ifndef _TRIGON_MATH_H
#define _TRIGON_MATH_H

/************************************************************************************************************
** TRIGON MATH - STM32F4
**
** A math library using CMSIS DSP math library plus custom functions that use 
** lookup tables and linear interpolation to estimate value of trigonometric functions.
**
** ROBOCON 2016
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
**
** v1.0 July 2016
**
** Geometry is not true, it is advantageous.  ~Henri Poincare
*************************************************************************************************************/

#include "stm32f4xx.h"

#ifndef ARM_MATH_CM4
	#define ARM_MATH_CM4
#endif
#include "arm_math.h"

#include <math.h>
#include <stdbool.h>
#include <string.h>

/** Approximate a sine value.
	@param angle: 0 ~ 35999 (Scaled by 100)
	@return sin(angle) scaled by 32768 (-32768 ~ 32767)
*/
int16_t app_sin(int32_t angle);

/** Approximate a cosine value.
	@param angle: 0 ~ 35999 (Scaled by 100)
	@return cos(angle) scaled by 32768 (-32768 ~ 32767)
*/
int16_t app_cos(int32_t angle);

/** Approximate a tan value.
	Be careful value around multiple of 90 deg will be very inaccurate.
	@param angle: 0 ~ 35999 (Scaled by 100)
	@return tan(angle) scaled by 16384
*/
int32_t app_tan(int32_t in);

/** Approximate an asin value.
	@param in: scaled by 32768, +ve or -ve
	@return asin(in), angle scaled by 100, +ve or -ve
*/
int32_t app_asin(int32_t in);

/** Approximate an acos value.
	@param in: scaled by 32768, +ve or -ve
	@return acos(in), angle scaled by 100, +ve or -ve
*/
int32_t app_acos(int32_t in);

/** Approximate an atan value.
	@param in: scaled by 16384, +ve or -ve
	@return atan(in), angle scaled by 100, +ve or -ve
*/
int32_t app_atan(int32_t in);

/** Approximate an atan2 value.
	@param y: scaled by 16384, +ve or -ve
	@param x: scaled by 16384, +ve or -ve
	@return atan2(y, x), angle scaled by 100, +ve or -ve
*/
int32_t app_atan2(int32_t y, int32_t x);

#endif
