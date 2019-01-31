#ifndef _GUN_CAN_MOTOR_H
#define _GUN_CAN_MOTOR_H

/**************************************************************************************************
** Motor driver - Project Gungnir
**
** This is a generic motor controller program.
** Please read the files in MotorDriver_Gungnir/wiki for more information.
**
** ROBOCON 2017
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
**
** v1.1 Feburary 2017 (Path generation and PID)
** v1.2 March 2017 (Smoother path generation and message filtering)
** v2.0 March 2017 (Improved performance with repesct to MOSFET and H-bridge property
**									, auto-recovery and encoder failure feedback, uart improvement and documentations)
** v2.1 April 2017 (Changed a lot of 64 bit variables to 32 bit.)
** v2.2 April 2017 (bugs fix)
** v2.3 April 2017 (Change CAN ID, separate command and feedback ID, added competition mode)
** v2.4 April 2017 (Added STFU mode under the pressure of my fellows T_T)
** v3.0 June 2017 (Added limit switches and AT mode)
** v3.1 July 2017 (Added real time limit switch monitoring and open loop limit switch handling)
**
** “Óðinn á yðr alla!” (“Odin owns all of you!”)
**
** >> "I take it from this that you are an Accelerationist?
** >> "No, " said Sam, "simply an inquirer. I am curious, that's all, as to the reasons."
** >> ~Lord of Light
**
****************************************************************************************************/

/**
* ATTENTION!
* You don't really need compile debug mode version debug.
* You can flash a normal program and connect a jumper to:
- For 3.2: UART3 (middle two, GND and TX)
- For 5.1: SWD (Rightmost two, GND and IO)
* The program will run at DEBUG_MODE. 
* Defining the DEBUG_MODE below force the program to enter debug mode.
*/

// Compile with DEBUG_MODE defined to generate debug mode hex file.
/** To QQ Hardware:
* 1. Move the cursor in front of the line "#define DEBUG_MODE"
* 2. Locate the "/" character on your screen!
* 3. Delete those two little slashes mercilessly.
* 4. Press F7 to compile
* 5. Kindly ask your motor controler to enter boot mode
* 6. Connect your computer to UART1
* 7. Press F8. You should be able to see a lot of [OK][OK][OK]...
*/ 

#include "can_protocol.h"
#include "can_motor.h"
#include "motor_control.h"
#include <stdbool.h>

#ifndef s64
#define s64 int64_t
#endif

#define GUN_MOTOR_CAN CAN_1

#define GUN_CAN_MOTOR_COUNT								16

#define	GUN_CAN_MOTOR_COMMAND_BASE				0x0D0
#define	GUN_CAN_MOTOR_FEEDBACK_BASE				0x0E0

/*** TX ***/
#define	GUN_CAN_MOTOR_VEL_LENGTH					6
#define GUN_CAN_MOTOR_VEL_CMD							0xAA

#define GUN_CAN_MOTOR_POS_LENGTH 					5
#define GUN_CAN_MOTOR_POS_CMD 						0xBC

#define GUN_CAN_MOTOR_ACCEL_LENGTH				5
#define GUN_CAN_MOTOR_ACCEL_CMD						0x45

#define GUN_CAN_MOTOR_MAXV_LENGTH					5
#define GUN_CAN_MOTOR_MAXV_CMD						0x49

#define GUN_CAN_MOTOR_LOCK_LENGTH	 		  	1
#define GUN_CAN_MOTOR_LOCK_CMD						0xEE

#define GUN_CAN_MOTOR_COMPETITION_MODE_LENGTH	 		  	2
#define GUN_CAN_MOTOR_COMPETITION_MODE_CMD						0xDD

#define GUN_CAN_MOTOR_STFU_MODE_LENGTH	 		  	2
#define GUN_CAN_MOTOR_STFU_MODE_CMD						0xCC

/*** RX ***/
#define GUN_CAN_ENCODER_FEEDBACK_LENGTH		8
#define GUN_CAN_ENCODER_FEEDBACK_ID				0x22
#define GUN_CAN_ENCODER_OK_MASK						0x01
#define GUN_CAN_ENCODER_LM_LO_MASK				0x04
#define GUN_CAN_ENCODER_LM_HI_MASK				0x08

#define GUN_CAN_LIMIT_FEEDBACK_LENGTH 		5
#define GUN_CAN_LIMIT_LO_FEEDBACK			 		0x30
#define GUN_CAN_LIMIT_HI_FEEDBACK			 		0x31

#define GUN_CAN_ARRIVAL_FEEDBACK_LENGTH		1
#define GUN_CAN_ARRIVAL_PT_1_FEEDBACK			0x40 //End of acceleration
#define GUN_CAN_ARRIVAL_PT_2_FEEDBACK			0x41 //Beginning of deceleration
#define GUN_CAN_ARRIVAL_PT_3_FEEDBACK			0x42 //Entire path finished

typedef enum{
	NO_RESPOND,
	RUNNING, 
	ENCODER_DEAD,
}GunMotorState;

typedef enum{
	GUN_LIMIT_LO,
	GUN_LIMIT_HI
}GunLimitID;

typedef enum{
	GUN_ACC_DONE,
	GUN_DEC_START,
	GUN_DEC_DONE
}GunTargetReachPoint;

typedef void(*OnLimitPosUpdate)(GunLimitID id);
typedef void(*OnTargetReached)(GunTargetReachPoint pt);

/**********
* TX Side
***********/

//Init Gungnir CAN Motor
void gun_can_motor_init(void);

/**
* Set motor velocity
* @param id: MOTOR_x, the motor to be controlled
* @param vel: Open loop: (-7999~7999); Close loop: (-max vel ~ max vel, default 150000);
* @param loop: Open loop or close loop control
*/
void gun_can_motor_set_vel(MotorID id, int32_t vel, CloseLoopFlag loop);

/**
* Set motor position
* @param id: MOTOR_x, the motor to be controlled
* @param pos: The target encoder position (Absolute)
*/
void gun_can_motor_set_pos(MotorID id, int32_t pos);

/**
* Set motor acceleration
* @param id: MOTOR_x, the motor to be controlled
* @param accel: acceleration parameter of motor, default: 900000
*/
void gun_can_motor_set_accel(MotorID id, uint32_t accel);

/**
* Set max. velocity
* @param id: MOTOR_x, the motor to be controlled
* @param vel: The velocity to be set, default: 150000
*/
void gun_can_motor_set_max_vel(MotorID id, uint32_t vel);

/**
* Lock the motor immediately with close loop.
* @param id: MOTOR_x, the motor to be controlled
*/
void gun_can_motor_lock(MotorID id);

/**
* Enter or leave competition mode.
* In competition mode, encoder malfunction would not lead to zero-pwm. It would map close-loop velocity to open-loop velocity instead.
* @param id: MOTOR_x, the motor to be controlled
* @param state: ENABLE/DISABLE
*/
void gun_can_motor_set_competition_mode(MotorID id, FunctionalState new_state);

/**
* Enter or leave silence mode.
* Double the motor pwm frequency so that it will be beyond human audible range.
* Personally I think the original frequency is better for hardware and helps debugging but well just stfu.
* @param id: MOTOR_x, the motor to be controlled
* @param state: ENABLE/DISABLE
*/
void gun_can_motor_stfu(MotorID id, FunctionalState new_state);



/**********
* RX Side
***********/


/**
* Get the motor encoder value (based on CAN rx result)
* @param id: MOTORx, which motor to look at
*/
int32_t gun_can_get_encoder(MotorID id);

/**
* Get the motor pwm value (based on CAN rx result)
* @param id: MOTORx, which motor to look at
*/
int32_t gun_can_get_pwm(MotorID id);

/**
* Get the motor state (NO_RESPOND, RUNNING, or ENCODER_DEAD)
* @param id: MOTORx, which motor to look at
*/
GunMotorState gun_can_get_state(MotorID id);

/**
* Get the limit switch triggered state
* @param id: MOTORx, which motor to look at
*/
bool gun_can_get_limit_trig(MotorID id, GunLimitID limit_id);

/**
* Get the limit switch position
* @param id: MOTORx, which motor to look at
*/
s32 gun_can_get_limit_pos(MotorID id, GunLimitID limit_id);

/**
* Register a limit pos update listener. It will be called when a new limit pos has been set.
* The limit pos will be set at the moment that the limit switch changes from untriggered state to triggered state.
* It should represent the boundary in which the limit switch is triggered.
* The limit switch id will be the parameter of the listener function.
*/
void gun_reg_OnLimitPosUpdate(MotorID id, OnLimitPosUpdate listener);

/**
* Regiser a target reached listener. It will be called when the motor driver has reached a certain point in the drawn graph.
* There are three such points: 
* 1. When the driver has finished acceleration
* 2. When the driver has started deceleration, aka end of constant phase
* 3. When the driver has finished deceleration, aka end of path
* The point id will be the parameter of the listener function.
*/
void gun_reg_OnTargetReached(MotorID id, OnTargetReached listener);

//Disabling the listener
void gun_disable_OnLimitPosUpdate(MotorID id);
void gun_disable_OnTargetReached(MotorID id);

#endif
