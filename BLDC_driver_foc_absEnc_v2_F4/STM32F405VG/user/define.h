#ifndef _DEFINE_H
#define _DEFINE_H

/**
	PIN_ON_TOP = 		0
	PIN_ON_LEFT = 	1
	PIN_ON_BOTTOM =	2
	PIN_ON_RIGHT = 	3
*/
#define ORIENTATION_SETTING 0

#ifndef ARM_MATH_CM4
	#define ARM_MATH_CM4
#endif

#define BUTTON_1							BOARD_BTN_1
#define BUTTON_2							BOARD_BTN_2
#define JOYSTICK_N 						JS_BTN_A
#define JOYSTICK_E 						JS_BTN_B
#define JOYSTICK_S 						JS_BTN_C
#define JOYSTICK_W 						JS_BTN_D
#define JOYSTICK_M						JS_BTN_M

#endif 
