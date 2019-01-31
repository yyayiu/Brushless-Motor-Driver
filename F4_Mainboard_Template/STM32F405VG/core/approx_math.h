#ifndef __APPROX_MATH_H
#define __APPROX_MATH_H

#include "stm32f4xx.h"
#include <stdlib.h>

extern int16_t cos_val[91];



/** @brief  Approximation of sin function
  * @param  a: angle in degree scaled by 10 (0-3600)
  * @retval sin(a) scaled by 10000 (0-10000)
  */
int32_t int_sin(int32_t a);

/** @brief  Approximation of cos function
  * @param  a: angle in degree scaled by 10 (0-3600)
  * @retval cos(a) scaled by 10000 (0-10000)
  */
int32_t int_cos(int32_t a);

/** @brief  Approximation of tan function
  * @param  a: angle in degree WITHOUT scaling
  * @retval tan(a) scaled by 100
  */
int16_t int_tan(int16_t a);

/** @brief  Quick asin function approximation with 
  * @param  sin_val: input scaled by 10000 (-10000-10000)
  * @retval asin(sin_val) in degree WITHOUT scaling (-90-90)
  */
int16_t int_arc_sin(int16_t sin_val);

/** @brief  Approximation of acos function
  * @param  cos_val: input scaled by 10000 (0-10000)
  * @retval acos(cos_val) in degree WITHOUT scaling
  */
int16_t int_arc_cos(int16_t cos_val);

/** @brief  Approximation of atan function
  * @param  tan_val: input scaled by 100
  * @retval atan(y/x) in degree from -89 to 90 (ignore quarter)
  */
int16_t int_arc_tan(int32_t tan_val);

/** @brief  Approximation of atan2 function
  * @param  y: input y
  * @param  x: input x
  * @retval atan(y/x) in degree from 0 to 359 WITHOUT scaling (correct quarter)
  */
int16_t int_arc_tan2(int32_t y, int32_t x);

/** @brief  Rapid sqrt approximation with maximum 0.297944% deviation at sqrt(2) and average 0.0184811% deviation
  * @param  v:	Input limited to 2^31 by variable type
  * @retval Scaled value of 1024*sqrt(v)
  */
u32 Sqrt(int32_t v);

#endif		/*  __APPROX_MATH_H */
