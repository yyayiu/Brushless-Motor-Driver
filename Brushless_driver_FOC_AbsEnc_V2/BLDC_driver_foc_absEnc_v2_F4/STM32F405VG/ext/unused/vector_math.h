#ifndef _VECTOR_MATH_H
#define _VECTOR_MATH_H

/************************************************************************
** VECTOR MATH - STM32F405VG
**
** This library is for calculation of 3-dimensional vectors
** Designed For IMU
**
** ROBOCON 2016
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
** v1.0 April 2016
**
** Our scientific power has outrun our spiritual power.
**     We have guided missiles and misguided men. ~Martin Luther King Jr.
*************************************************************************/
//Note that this library is not optimised for speed. Only experimental.

#include "stm32f4xx.h"
#include "dsp_math.h"
#include "arm_math.h"
#include "quick_math.h"

typedef float32_t f_vector[3]; //Define f_vector as a array of three floats

//Copy src vector to dst vector
void vector_copy(f_vector src, f_vector dst);

/**
	Perform dot operation. (v1 dot v2)
	@return the dot product
*/
float32_t vector_dot(f_vector v1, f_vector v2);

/**
	Perform cross operation. (v1 x v2)
	@param out_vector: allocated memory space for the output vector
	@return The pointer to the resultant array(f_vector)
*/
float32_t* vector_cross(f_vector v1, f_vector v2, f_vector out_vector);

/**
	Scale by the vector by a scalar.
	@param out_vector: allocated memory space for the output vector
	@return The pointer to the resultant array(f_vector)
*/
float32_t* vector_scale(f_vector v1, float32_t scalar, f_vector out_vector);

/**
	Add two vectors. (v1+v2)
	@param out_vector: allocated memory space for the output vector
	@return The pointer to the resultant array(f_vector)
*/
float32_t* vector_add(f_vector v1, f_vector v2, f_vector out_vector);
float32_t* vector_minus(f_vector v1, f_vector v2, f_vector out_vector);

/**
	Find the length of the vector.
	@return Vector length.
*/
float32_t vector_len(f_vector v1);

/**
	Normalise the vector to be of length 1.
	@param out_vector: allocated memory space for the output vector
	@return The pointer to the resultant array(f_vector)
*/
float32_t* vector_normalize(f_vector v1, f_vector out_vector);

#endif
