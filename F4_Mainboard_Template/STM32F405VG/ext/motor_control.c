#include "motor_control.h"
#include "can_motor.h"

int32_t curr_motor_vel[NUMBER_OF_MOTOR] = {0};
CloseLoopFlag curr_motor_flag[NUMBER_OF_MOTOR] = {OPEN_LOOP};

//Init CAN/SPI Motor
void motor_init(){
	#ifdef USING_CAN_MOTOR
		can_motor_init();
	#else
		spi_motor_init();
	#endif
}

/**
* @brief Set motor velocity (CAN/SPI)
* @param motor_id: MOTORx, which motor to control
* @param vel: Open loop: (-1799~1799); Close loop: (-150~150);
* @param loop: Open loop or close loop control
*/
void motor_set_vel(MotorID id, int16_t vel, CloseLoopFlag loop){
	curr_motor_vel[id] = vel;
	curr_motor_flag[id] = loop;
	#ifdef USING_CAN_MOTOR
		can_motor_set_vel(id, vel, loop);
	#else
		spi_motor_set_vel(id, vel, loop);
	#endif
}

/**
* @brief Set motor position (CAN/SPI)
* @param motor_id: MOTORx, which motor to control
* @param vel (vel of close_loop is not corresponded to open_loop)
* @param pos: The position need to move to relative to current encoder value.
*/
void motor_set_pos(MotorID id, uint16_t vel, int32_t pos){
	#ifdef USING_CAN_MOTOR
		can_motor_set_pos(id, vel, pos);
	#else
		spi_motor_set_pos(id, vel, pos);
	#endif
}

/**
* @brief Set motor acceleration (CAN/SPI)
* @param motor_id: MOTORx, which motor to control
* @param accel: acceleration parameter of motor
*/
void motor_set_accel(MotorID id, uint16_t accel){
	#ifdef USING_CAN_MOTOR
		can_motor_set_accel(id, accel);
	#else
		spi_motor_set_accel(id, accel);
	#endif
}

/**
* @brief Lock and stop motor immediately (CAN/SPI)
* @param motor_id: MOTORx, which motor to control
*/
void motor_lock(MotorID id){
	#ifdef USING_CAN_MOTOR
		can_motor_lock(id);
	#else
		spi_motor_lock(id);
	#endif
}


/**********
* RX Side
***********/

/**
* @brief Get the motor encoder value (based on CAN/SPI rx result)
* @param motor_id: MOTORx, which motor to control
*/
int32_t get_encoder_value(MotorID id){
	#ifdef USING_CAN_MOTOR
		return can_get_encoder_value(id);
	#else
		return spi_get_encoder_value(id);
	#endif
}
