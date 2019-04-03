#include "vector_math.h"

void vector_copy(f_vector src, f_vector dst){
	arm_copy_f32(src, dst, 3);
}

float32_t vector_dot(f_vector v1, f_vector v2){
	float32_t result;
	arm_dot_prod_f32(v1, v2, 3, &result);
	return result;
}

float32_t* vector_cross(f_vector v1, f_vector v2, f_vector out_vector){
  out_vector[0]= v1[1]*v2[2] - v1[2]*v2[1];
  out_vector[1]= v1[2]*v2[0] - v1[0]*v2[2];
  out_vector[2]= v1[0]*v2[1] - v1[1]*v2[0];
	return out_vector;
}

float32_t* vector_scale(f_vector v1, float32_t scalar, f_vector out_vector){
	arm_scale_f32(v1, scalar, out_vector, 3);
	return out_vector;
}

float32_t* vector_add(f_vector v1, f_vector v2, f_vector out_vector){
	arm_add_f32(v1, v2, out_vector, 3);
	return out_vector;
}

float32_t* vector_minus(f_vector v1, f_vector v2, f_vector out_vector){
	arm_sub_f32(v1, v2, out_vector, 3);
	return out_vector;
}

float32_t vector_len(f_vector v1){
	float32_t out;
	arm_sqrt_f32(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2], &out);
	return out;
}

float32_t* vector_normalize(f_vector v1, f_vector out_vector){
	float32_t length = vector_len(v1);
	out_vector[0] = v1[0]/length;
	out_vector[1] = v1[1]/length;
	out_vector[2] = v1[2]/length;
	return out_vector;
}
