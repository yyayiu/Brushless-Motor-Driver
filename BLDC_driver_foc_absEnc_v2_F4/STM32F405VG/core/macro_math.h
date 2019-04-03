#ifndef _MACRO_MATH
#define _MACRO_MATH

//Find the value a, after lower bounded by b and upper bounded by c
#define CAP(a, b, c) ((a)<(b)?(b):((a)>(c)?(c):(a)))

//Scale value a from a range from 0~b to 0~c
#define SCALE(a, b, c) ((a)*(c)/(b))

#define ABS(a) ((a)<0?-(a):(a))

#endif
