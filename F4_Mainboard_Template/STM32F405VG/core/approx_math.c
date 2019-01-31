#include "approx_math.h"

int16_t tan_val[91] = {		//scale 100
0,2,3,5,7,9,11,12,14,16,18,19,21,23,25,27,29,31,32,34,36,38,40,42,
45,47,49,51,53,55,58,60,62,65,67,70,73,75,78,81,84,87,90,93,97,100,104,107,
111,115,119,123,128,133,138,143,148,154,160,166,173,180,188,196,205,214,225,236,248,261,275,290,
308,327,349,373,401,433,470,514,567,631,712,814,951,1143,1430,1908,2864,5729};

int16_t cos_val[91] = {
10000,9998,9994,9986,9976,9962,9945,9925,9903,9877,9848,9816,9781,9744,9703,9659,9613,9563,9511,
9455,9397,9336,9272,9205,9135,9063,8988,8910,8829,8746,8660,8572,8480,8387,8290,8192,8090,7986,
7880,7771,7660,7547,7431,7314,7193,7071,6947,6820,6691,6561,6428,6293,6157,6018,5878,5736,5592,
5446,5299,5150,5000,4848,4695,4540,4384,4226,4067,3907,3746,3584,3420,3256,3090,2924,2756,2588,
2419,2250,2079,1908,1736,1564,1392,1219,1045,872,698,523,349,175,0};

/**
  * @brief  Approximation of sin function
  * @param  a: angle in degree scaled by 10 (0-3600)
  * @retval sin(a) scaled by 10000 (0-10000)
  */
int32_t int_sin(int32_t a)
{
	int16_t ta;
	while(a < 0)
		a += 3600;
	a = a % 3600;
	ta = a;
	if (a > 2700) {
		a = 3600-a;
	} else if (a > 1800) {
		a = a-1800;
	} else if (a > 900) {
		a = 1800-a;
	}
	a = cos_val[90-(a/10)] + (((a%10)*cos_val[a/10]+286)/573);
	return ta > 1800 ? -a : a;
}

/**
  * @brief  Approximation of cos function
  * @param  a: angle in degree scaled by 10 (0-3600)
  * @retval cos(a) scaled by 10000 (0-10000)
  */
int32_t int_cos(int32_t a)
{
	return int_sin(900-a);
}

/**
  * @brief  Approximation of tan function
  * @param  a: angle in degree WITHOUT scaling
  * @retval tan(a) scaled by 100
  */
int16_t int_tan(int16_t a)
{
	uint8_t neg = 0;
	if (a < 0) {
		neg = 1;
		a = -a;
	}
	while (a < 0)
		a += 180;
	a = a % 180;
	if (a > 90) {
		return neg ? tan_val[180-a] : -tan_val[180-a];
	} else if (a < 90) {
		return neg ? -tan_val[a] : tan_val[a];
	} else {
		return -1;
	}
}

/**
  * @brief  Quick asin function approximation with 
  * @param  sin_val: input scaled by 10000 (-10000-10000)
  * @retval asin(sin_val) in degree WITHOUT scaling (-90-90)
  */
int16_t int_arc_sin(int16_t sin_val)
{
	int32_t search_angle = 0;

	int16_t lower_bound = 0, upper_bound = 900;
	int16_t interval = 450, index = 0;
	uint8_t neg_val = 0;


	if (sin_val < 0)	neg_val = 1;
	sin_val = abs(sin_val);

	while (1)
	{
		index = lower_bound + interval;
		search_angle = int_sin(index);

		if (interval == 0 || sin_val == search_angle)
		{
			index = (index+4)/10;
			break;
		}
		else if (sin_val < search_angle)
			upper_bound = index;
		else//						(sin_val > search_angle)
			lower_bound = index;
		interval = (upper_bound - lower_bound) / 2;

	}

	if (neg_val)	return -index;
	else			return index;
}


/**
  * @brief  Approximation of acos function
  * @param  cos_val: input scaled by 10000 (0-10000)
  * @retval acos(cos_val) in degree WITHOUT scaling
  */
int16_t int_arc_cos(int16_t cos_val)
{
	return 90-int_arc_sin(cos_val);
}

/**
  * @brief  Approximation of atan function
  * @param  tan_val: input scaled by 100
  * @retval atan(y/x) in degree from -89 to 90 (ignore quarter)
  */
int16_t int_arc_tan(int32_t tan_val)
{
	int16_t angle = 0;
	int16_t pre_tan = 0;
	int16_t cur_tan = 0;
	if (tan_val >= 0) {
		while (angle < 90) {
			pre_tan = cur_tan;
			cur_tan = int_tan(angle);
			if (tan_val <= cur_tan && tan_val >= pre_tan) break;
			angle++;
		}
	} else {
		while (angle >= -89) {
			pre_tan = cur_tan;
			cur_tan = int_tan(angle);
			if (tan_val >= cur_tan && tan_val <= pre_tan) break;
			angle--;
		}
	}
	return angle;
}

/**
  * @brief  Approximation of atan2 function
  * @param  y: input y
  * @param  x: input x
  * @retval atan(y/x) in degree from 0 to 359 WITHOUT scaling (correct quarter)
  */
int16_t int_arc_tan2(int32_t y, int32_t x)
{
	if (x == 0) {
		if (y < 0)
			return -90;
		else if (y == 0)
			return 0;
		else
			return 90;
	} else if (y == 0) {
		return x < 0 ? 180 : 0;
	} else if (x < 0) {
		return 180+int_arc_tan((int32_t)(100*y+x/2)/x);
	} else {
		return int_arc_tan((int32_t)(100*y+x/2)/x);
	}
}

/**
  * @brief  Enhanced rapid sqrt approximation tested in 2^31 range (cmath sqrt function bugs on > 2^31)
  *					with maximum 0.101663% deviation at sqrt(11) minimum -0.000000000045339% at sqrt(24813001) and 
  *					average 0.000000559339% deviation
  *					Larger input generally results in smaller output error
  * @param  v:	Input limited to 2^31 by variable type
  * @retval Scaled value of 1024*sqrt(v)
	*	@attention		To CS members: 		Don't bother changing things not understandable unless O(f{n}) < O(1)
	*								To CPEG members:	Float is used here only because it dramatically shortens sqrt 
	*																	time compared to other implementations in Int, try proving the
	*																	magic number 0x233b4000 by relating to bits meaning of Float. 
	*																	Final result is accurate due to refinment by Newton's Iteration.
	*								To EE members:		This is much faster
  */
uint32_t Sqrt(int32_t v)
{
/*
	float y = v; 						//Fast inverse square root
	long i = 0x62b759df - (*(long*)&y>>1);
	i = (long)(*(float*)&i);
	return (200<<21) / (i * ((3<<14) - v *i*i));
*/
	union
	{
		uint32_t tmp;
		float f;
	} u;

	v = v < 0 ? -v : v;
	u.f = v;
	u.tmp = (uint32_t)(0x233b4000 + (u.tmp >> 1));
	u.tmp = (uint32_t)u.f;
	u.tmp = (u.tmp + (uint64_t)v*16384/u.tmp + 1)/2;
	u.tmp = (u.tmp + (uint64_t)v*16384/u.tmp + 1)/2;
	return u.tmp * 8;
}


