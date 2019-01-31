#ifndef _PNEUMATIC_H
#define _PNEUMATIC_H

/**
* This library is for controlling pneumatic devices (and in fact all kinds of on/off devices controlled with GPIO)
*
* Rex Cheng
*/

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "gpio.h"

#define PNEU_TABLE \
X(PNEU_1, PA15)			\
X(PNEU_2, PB0)			\
X(PNEU_3, PB1)			\
X(PNEU_4, PB3)			\
X(PNEU_5, PF9)			\
X(PNEU_6, PF10)			\
X(PNEU_7, PF11)			\
X(PNEU_8, PF12)			\
X(PNEU_9, PF13)			\
X(PNEU_10, PF14)			\
X(PNEU_11, PD8)			\
X(PNEU_12, PD9)			\
X(PNEU_13, PD10)			\
X(PNEU_14, PD11)			

#define X(a, b) a,
typedef enum{
	PNEU_TABLE
} PneuID;
#undef X

#define X(a, b) &b,
static const GPIO* Pneus[] = {PNEU_TABLE};
#undef X

#define PNEU_COUNT (sizeof(Pneus)/sizeof(GPIO*))

/**
	Initialize all pneu ports.
*/
void pneu_init(void);

/**
	Control pneu valve.
	@param id: the pneu to be controlled
	@param state: Bit_RESET/Bit_SET, where Bit_RESET refers to original(uncontrolled) state 
*/
void pneu_control(PneuID id, BitAction state);

/**
	Toggle the pneu valve.
	@param id: the pneu to be controlled
*/
void pneu_toggle(PneuID id);

#endif
