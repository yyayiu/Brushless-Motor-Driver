#ifndef	_BUTTON_H
#define	_BUTTON_H

/***************************************************************************************************************************************
** BUTTON - STM32F4
**
** This library provides functions for reading from buttons (or switches)
** It provides btn_pressed function to read input from buttons, adapted with the respective pull-up/pull-down
** It also provides some listener functions, which helps to respond to input easily
** Feel free to add more "buttons" down there in the BTN_TABLE, it works for all GPIO switches, like limit switches or IR switches
**
** ROBOCON 2017
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
**
** v1.0 December 2016
**
** "You press the button, we do the rest." ~ Slogan of Kodak Camera
****************************************************************************************************************************************/

#include "stm32f4xx.h"
#include <stdbool.h>
#include "gpio.h"

//BUTTON       PIN  Pull-up/Pull-down
#define BTN_TABLE \
X(BOARD_BTN_1, PD6, GPIO_PuPd_UP) \
X(BOARD_BTN_2, PD7, GPIO_PuPd_UP) \
X(JS_BTN_A, PD0, GPIO_PuPd_UP) \
X(JS_BTN_B, PD1, GPIO_PuPd_UP) \
X(JS_BTN_C, PD3, GPIO_PuPd_UP) \
X(JS_BTN_D, PD4, GPIO_PuPd_UP) \
X(JS_BTN_M, PD5, GPIO_PuPd_UP)

#define X(a, b, c) a, 
typedef enum{
	BTN_TABLE
}ButtonID;
#undef X

typedef struct{
	const GPIO* gpio;
	const GPIOPuPd_TypeDef PuPd;
}ButtonStruct;

#define X(a, b, c) {&b, c},
static const ButtonStruct BUTTONS[] = {BTN_TABLE};
#undef X

#define BTN_COUNT (sizeof(BUTTONS)/sizeof(ButtonStruct))

typedef void(*OnClickListener)(void);
typedef void(*OnReleaseListener)(void);
typedef void(*OnHoldListener)(void);

typedef struct{
	OnHoldListener holdListener;
	s16 holdThreshold;
	s16 trigFrequency;
}HoldListenerStruct;

//Init buttons
void btn_init(void);

//To be called at a regular time interval. Button event will be triggered in this function
void btn_update(void);

//These functions are for registering listener for buttons
void btn_reg_OnClickListener(ButtonID button_id, OnClickListener listener); //Called when the button is first pressed
void btn_reg_OnReleaseListener(ButtonID button_id, OnReleaseListener listener); //Called when the button is released 
//Called every "frequency" after "threshold" have been reached. Actual time depends on frequency of @button_update()
void btn_reg_OnHoldListener(ButtonID button_id, uint16_t threshold, uint16_t frequency, OnHoldListener listener); 

//These functions are to disable registered listener
void btn_disable_OnClickListener(ButtonID button_id);
void btn_disable_OnReleaseListener(ButtonID button_id);
void btn_disable_OnHoldListener(ButtonID button_id);

//Return the state of the selected button, 1 if pressed
bool btn_pressed(ButtonID button_id);

#endif
