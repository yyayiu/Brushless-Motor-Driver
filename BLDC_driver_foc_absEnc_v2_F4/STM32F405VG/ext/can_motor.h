#ifndef _CAN_MOTOR_H
#define _CAN_MOTOR_H

#include "can_protocol.h"
#include "can_motor.h"
#include "motor_control.h"
#include <stdbool.h>

#define MOTOR_CAN CAN_1

#define CAN_MOTOR_COUNT								16
#define	CAN_MOTOR_BASE								0x0B0

/*** TX ***/
#define	CAN_MOTOR_VEL_LENGTH					6
#define CAN_MOTOR_VEL_CMD							0xAA

#define CAN_MOTOR_POS_LENGTH 					7
#define CAN_MOTOR_POS_CMD 						0xBB

#define CAN_MOTOR_PARAMETER_LENGTH		3
#define CAN_MOTOR_PARAMETER_CMD				0x44

#define CAN_MOTOR_LOCK_LENGTH	 		  	1
#define CAN_MOTOR_LOCK_CMD						0xEE	

/*** RX ***/
#define CAN_ENCODER_FEEDBACK_LENGTH		5
#define CAN_ENCODER_FEEDBACK					0x22


/**********
* TX Side
***********/


//Init CAN Motor
void can_motor_init(void);

/**
* @brief Set motor velocity (CAN)
* @param id: MOTORx, which motor to control
* @param vel: Open loop: (-1799~1799); Close loop: (-150~150);
* @param loop: Open loop or close loop control
*/
void can_motor_set_vel(MotorID id, int32_t vel, CloseLoopFlag loop);

/**
* @brief Set motor position (CAN)
* @param id: MOTORx, which motor to control
* @param vel (vel of close_loop is not corresponded to open_loop)
* @param pos: The position need to move to relative to current encoder value.
*/
void can_motor_set_pos(MotorID id, uint16_t vel, int32_t pos);

/**
* @brief Set motor acceleration (CAN)
* @param id: MOTORx, which motor to control
* @param accel: acceleration parameter of motor
*/
void can_motor_set_accel(MotorID id, uint16_t accel);

/**
* @brief Lock and stop motor immediately (CAN)
* @param id: MOTORx, which motor to control
*/
void can_motor_lock(MotorID id);



/**********
* RX Side
***********/


/**
* @brief Get the motor encoder value (based on CAN rx result)
* @param id: MOTORx, which motor to control
*/
int32_t can_get_encoder_value(MotorID id);

#endif
