#ifndef __GYRO_H
#define __GYRO_H

#include "uart.h"
#include "ticks.h"
#include "approx_math.h"

#define GYRO_UART	COM5

/*** Command List ***/
#define GYRO_WAKEUP				0x01

#define GYRO_UPDATE				    0x10
#define GYRO_CAL				      0x20
#define GYRO_POS_SET			    0x30
#define GYRO_AUTO_UPDATE	    0x40

/*** Reply Command List ***/
#define GYRO_REPLY				    0x50
#define GYRO_UPDATED			    0x80

#define GYRO_COMMAND_LENGTH		2

#define GYRO_FLAG_SET_POS		  0x01
#define GYRO_FLAG_CAL			    0x02

typedef struct {
	int16_t x, y, angle;
} POSITION;


#define	X_FLIP						1
#define	Y_FLIP						-1
/** Varies along robots (depends on the encoder position) **/

//Scaling factors - depends on ground (decrease to lengthen actual dist)
#ifdef RED_FIELD
	#define X_SCALING			1
	#define Y_SCALING			1
#else
	#define X_SCALING			1
	#define Y_SCALING			1
#endif

extern volatile uint8_t gyro_available;

void plus_x(void);
void minus_x(void);
void plus_y(void);
void minus_y(void);
int32_t gyro_get_shift_x(void);
int32_t gyro_get_shift_y(void);

const POSITION* get_pos(void);	// Get the position ({x, y, angle})
const POSITION* get_pos_raw(void);

void gyro_init(void);
void gyro_rx_handler(uint8_t rx_data);
void gyro_pos_update(void);		//update gyro values only when auto-update function is disabled in gyro
uint8_t gyro_cal(void);	//callibrate gyro
uint8_t gyro_pos_set(int16_t x, int16_t y, int16_t a);	//set position of gyro

int16_t get_X(void);
int16_t get_Y(void);
int16_t get_angle(void);


#endif		/* __GYRO_H */
