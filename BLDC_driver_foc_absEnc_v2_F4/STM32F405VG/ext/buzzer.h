#ifndef	_BUZZER_H
#define	_BUZZER_H

/** A buzzer library, just for ON and OFF.
* You can produce music note by changing the timer frequency, but PF2 is not connected to any timer
* Be careful not to consume too many MCU-time to compute the music
* This library can be used to produce sounds easily with the timer library
* Rex Cheng
*/

#include "stm32f4xx_gpio.h"
#include "gpio.h"

#define BUZZER_GPIO PF2

//Init the buzzer
void buzzer_init(void);

//Turn on the buzzer
void buzzer_on(void);

//Turn off the buzzer
void buzzer_off(void);


#endif
