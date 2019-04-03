#ifndef	_LED_H
#define	_LED_H

/**
* This library is for controlling LEDs (and in fact all kinds of on/off devices controlled with GPIO)
*
* Rex Cheng
*/

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "gpio.h"

#define LED_TABLE \
X(LED_1, PB15)			

#define X(a, b) a,
typedef enum{
	LED_TABLE
} LedID;
#undef X

#define X(a, b) &b,
static const GPIO* LEDs[] = {LED_TABLE};
#undef X

#define LED_COUNT (sizeof(LEDs)/sizeof(GPIO*))

/**
	Initialize all LEDs.
*/
void led_init(void);

/**
	Control LED on/off
	@param id: the led to be controlled
	@param state: Bit_RESET: Off / Bit_SET: On
*/
void led_control(LedID id, BitAction state);

/**
	Make the led blink
	@param id: the led to be controlled
*/
void led_blink(LedID id);

#endif
