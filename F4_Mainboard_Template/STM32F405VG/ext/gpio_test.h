#ifndef	_GPIO_TEST_H
#define	_GPIO_TEST_H

/** This is a helper file to test GPIO.
* It will config all but one pins as input pull up, that particular pin will do Open-drain
* It will do it iterately for all pins.
* Do not use this function in your real program, for testing purpose only.
* Rex Cheng
*/

#include "stm32f4xx_gpio.h"
#include "gpio.h"
#include "lcd_main.h"
#include "button.h"
#include "define.h"

#define START_TEST_PIN (16*4 + 0)
#define END_TEST_PIN (16*5 + 0)

//Start the test
void gpio_test(void);

#endif
