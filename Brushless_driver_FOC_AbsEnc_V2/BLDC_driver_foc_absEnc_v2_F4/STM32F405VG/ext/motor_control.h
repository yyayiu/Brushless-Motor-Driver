#ifndef _MOTOR_CONTROL_H
#define _MOTOR_CONTROL_H

#include <stm32f4xx.h>
#include <stdbool.h>

/**
* This is an interface for controlling motors.
*/

#define NUMBER_OF_MOTOR 3

#define USING_CAN_MOTOR
//#define USING_SPI_MOTOR

#ifdef USING_CAN_MOTOR
	#ifdef USING_SPI_MOTOR
		#error Both CAN and SPI Motor enabled!
	#endif
#else
	#ifndef USING_SPI_MOTOR
		#error Please enable either one of CAN or SPI Motor!
	#endif
#endif

typedef enum {
	MOTOR_1 = 0,
	MOTOR_2,
	MOTOR_3,
	MOTOR_4,
	MOTOR_5,
	MOTOR_6,
	MOTOR_7,
	MOTOR_8,
	MOTOR_9,
	MOTOR_10,
	MOTOR_11,
	MOTOR_12,
	MOTOR_13,
	MOTOR_14,
	MOTOR_15,
	MOTOR_16
} MotorID;

typedef enum{
	OPEN_LOOP = 0,
	CLOSE_LOOP = 1
} CloseLoopFlag;

#define get_motor_id(motor_id)	(CAN_MOTOR_BASE + (uint8_t)motor_id)

//This is the velocity that the user has set
extern int32_t curr_motor_vel[NUMBER_OF_MOTOR];
extern CloseLoopFlag curr_motor_flag[NUMBER_OF_MOTOR];
#define get_motor_vel(id) (curr_motor_vel[id])
#define get_motor_flag(id) (curr_motor_flag[id])

/**********
* TX Side
***********/

//Init CAN/SPI Motor
void motor_init(void);

/**
* @brief Set motor velocity (CAN/SPI)
* @param motor_id: MOTORx, which motor to control
* @param vel: Open loop: (-1799~1799); Close loop: (-150~150);
* @param loop: Open loop or close loop control
*/
void motor_set_vel(MotorID id, int16_t vel, CloseLoopFlag loop);

/**
* @brief Set motor position (CAN/SPI)
* @param motor_id: MOTORx, which motor to control
* @param vel (vel of close_loop is not corresponded to open_loop)
* @param pos: The position need to move to relative to current encoder value.
*/
void motor_set_pos(MotorID id, uint16_t vel, int32_t pos);

/**
* @brief Set motor acceleration (CAN/SPI)
* @param motor_id: MOTORx, which motor to control
* @param accel: acceleration parameter of motor
*/
void motor_set_accel(MotorID id, uint16_t accel);

/**
* @brief Lock and stop motor immediately (CAN/SPI)
* @param motor_id: MOTORx, which motor to control
*/
void motor_lock(MotorID id);


/**********
* RX Side
***********/

/**
* @brief Get the motor encoder value (based on CAN/SPI rx result)
* @param motor_id: MOTORx, which motor to control
*/
int32_t get_encoder_value(MotorID id);

#endif
