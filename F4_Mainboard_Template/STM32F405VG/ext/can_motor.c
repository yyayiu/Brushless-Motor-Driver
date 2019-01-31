#include "can_motor.h"

#define get_id(motor_id)	(CAN_MOTOR_BASE + (uint8_t)motor_id)

static int32_t encValue[CAN_MOTOR_COUNT] = {0};

/**
* @brief Handler for decoding motor CAN feedback message
* @param msg: the CAN msg to be decoded
*/
static void feedback_decoding(CanRxMsg* msg) {
	switch (msg->Data[0]) {
		case CAN_ENCODER_FEEDBACK:
			if (msg->DLC == CAN_ENCODER_FEEDBACK_LENGTH) {
				// Range check 
				if (msg->StdId >= CAN_MOTOR_BASE && msg->StdId < CAN_MOTOR_BASE + CAN_MOTOR_COUNT) {
					int32_t feedback = n_bytes_to_one(&msg->Data[1], 4);
					encValue[msg->StdId - CAN_MOTOR_BASE] = feedback;
				}
			}
		break;
	}
}

//Init CAN Motor
void can_motor_init(){
	can_rx_add_filter(CAN_MOTOR_BASE, CAN_RX_MASK_DIGIT_0_F, 0, MOTOR_CAN, feedback_decoding);
}


/**
* @brief Set motor velocity (CAN)
* @param id: MOTORx, which motor to control
* @param vel: Open loop: (-1799~1799); Close loop: (-150~150);
* @param loop: Open loop or close loop control
*/
void can_motor_set_vel(MotorID id, int32_t vel, CloseLoopFlag loop){
	CanMessage msg;
	
	assert_param((uint8_t)motor_id < CAN_MOTOR_COUNT);

	msg.id = get_id(id);
	msg.length = CAN_MOTOR_VEL_LENGTH;
	msg.data[0] = CAN_MOTOR_VEL_CMD;
	msg.data[1] = vel;
	msg.data[2] = vel >> 8;
	msg.data[3] = vel >> 16;
	msg.data[4] = vel >> 24;
	msg.data[5] = loop;
	
	can_tx_enqueue(MOTOR_CAN, msg);
}

/**
* @brief Set motor position (CAN)
* @param id: MOTORx, which motor to control
* @param vel (vel of close_loop is not corresponded to open_loop)
* @param pos: The position need to move to relative to current encoder value.
*/
void can_motor_set_pos(MotorID id, uint16_t vel, int32_t pos){
	CanMessage msg;
	
	assert_param((uint8_t)motor_id < CAN_MOTOR_COUNT);
	
	msg.id = get_id(id);
	msg.length = CAN_MOTOR_POS_LENGTH;
	msg.data[0] = CAN_MOTOR_POS_CMD;
	msg.data[1] = vel;
	msg.data[2] = vel >> 8;
	msg.data[3] = pos;
	msg.data[4] = pos >> 8;
	msg.data[5] = pos >> 16;
	msg.data[6] = pos >> 24;

	can_tx_enqueue(MOTOR_CAN, msg);
}

/**
* @brief Set motor acceleration (CAN)
* @param id: MOTORx, which motor to control
* @param accel: acceleration parameter of motor
*/
void can_motor_set_accel(MotorID id, uint16_t accel){
	CanMessage msg;
	
	assert_param((uint8_t)motor_id < CAN_MOTOR_COUNT);
	
	msg.id = get_id(id);
	msg.length = CAN_MOTOR_PARAMETER_LENGTH;
	msg.data[0] = CAN_MOTOR_PARAMETER_CMD;
	msg.data[1] = accel;
	msg.data[2] = accel >> 8;

	can_tx_enqueue(MOTOR_CAN, msg);
}

/**
* @brief Lock and stop motor immediately (CAN)
* @param id: MOTORx, which motor to control
*/
void can_motor_lock(MotorID id){
	CanMessage msg;
	
	assert_param((uint8_t)motor_id < CAN_MOTOR_COUNT);
	
	msg.id = get_id(id);
	msg.length = CAN_MOTOR_LOCK_LENGTH;
	msg.data[0] = CAN_MOTOR_LOCK_CMD;

	can_tx_enqueue(MOTOR_CAN, msg);
}


/*** RX ***/

/**
* @brief Get the motor encoder value (based on CAN rx result)
* @param id: MOTORx, which motor to control
*/
int32_t can_get_encoder_value(MotorID id){
	return encValue[id];
}

#undef get_id
