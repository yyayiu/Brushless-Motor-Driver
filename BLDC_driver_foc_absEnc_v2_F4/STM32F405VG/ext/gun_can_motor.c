#include "gun_can_motor.h"
#include "ticks.h"

#define get_command_id(motor_id)	(GUN_CAN_MOTOR_COMMAND_BASE + (uint8_t)motor_id)

static int32_t enc_value[GUN_CAN_MOTOR_COUNT] = {0};
static int32_t pwm_value[GUN_CAN_MOTOR_COUNT] = {0};
static int32_t limit_pos[GUN_CAN_MOTOR_COUNT][2] = {0};
static GunMotorState state[GUN_CAN_MOTOR_COUNT] = {NO_RESPOND};
static int32_t last_recv[GUN_CAN_MOTOR_COUNT] = {0};
static bool limit_trig[GUN_CAN_MOTOR_COUNT][2] = {0};

static OnLimitPosUpdate limit_pos_update_listener[GUN_CAN_MOTOR_COUNT] = {0};
static OnTargetReached target_reached_listener[GUN_CAN_MOTOR_COUNT] = {0};

/**
* @brief Handler for decoding motor CAN feedback message
* @param msg: the CAN msg to be decoded
*/
static void feedback_decoding(CanRxMsg* msg) {
	if (msg->StdId >= GUN_CAN_MOTOR_FEEDBACK_BASE && msg->StdId < GUN_CAN_MOTOR_FEEDBACK_BASE + GUN_CAN_MOTOR_COUNT) {
		u8 id = msg->StdId - GUN_CAN_MOTOR_FEEDBACK_BASE;
		
		switch (msg->Data[0]) {
				
			case GUN_CAN_LIMIT_LO_FEEDBACK:
			case GUN_CAN_LIMIT_HI_FEEDBACK:
				if (msg->DLC == GUN_CAN_LIMIT_FEEDBACK_LENGTH){
					s32 enc = msg->Data[1] | msg->Data[2] << 8 | msg->Data[3] << 16 | msg->Data[4] << 24;
					GunLimitID lid = (GunLimitID)(msg->Data[0] - GUN_CAN_LIMIT_LO_FEEDBACK);
					limit_pos[id][lid] = enc;
					if (limit_pos_update_listener[id]) {
						limit_pos_update_listener[id](lid);
					}
				}
			break;
			
			case GUN_CAN_ARRIVAL_PT_1_FEEDBACK:
			case GUN_CAN_ARRIVAL_PT_2_FEEDBACK:
			case GUN_CAN_ARRIVAL_PT_3_FEEDBACK:
				if (msg->DLC == GUN_CAN_ARRIVAL_FEEDBACK_LENGTH){
					if (target_reached_listener[id]) {
						target_reached_listener[id]((GunTargetReachPoint)(msg->Data[0] - GUN_CAN_ARRIVAL_PT_1_FEEDBACK));
					}
				}
			break;
				
			default:
				if (msg->DLC == GUN_CAN_ENCODER_FEEDBACK_LENGTH && (msg->Data[0] & GUN_CAN_ENCODER_FEEDBACK_ID)){
					// Range check 
					int32_t encFeedback = msg->Data[1] | msg->Data[2] << 8 | msg->Data[3] << 16 | msg->Data[4] << 24;
					int32_t pwmFeedback = msg->Data[5] | msg->Data[6] << 8| ((msg->Data[7] & 127) << 16);
					enc_value[id] = encFeedback;
					
					if (msg->Data[7] & 128){
						pwm_value[id] = (pwmFeedback | 0xFF800000);
					}else{
						pwm_value[id] = pwmFeedback;
					}
					
					last_recv[id] = get_ticks();
					state[id] = (msg->Data[0] & GUN_CAN_ENCODER_OK_MASK) ? RUNNING : ENCODER_DEAD;
					limit_trig[id][GUN_LIMIT_LO] = msg->Data[0] & GUN_CAN_ENCODER_LM_LO_MASK;
					limit_trig[id][GUN_LIMIT_HI] = msg->Data[0] & GUN_CAN_ENCODER_LM_HI_MASK;
				}
			break;
		}
	}
}

//Init Gungnir CAN Motor
void gun_can_motor_init(){
	can_rx_add_filter(GUN_CAN_MOTOR_FEEDBACK_BASE, CAN_RX_MASK_DIGIT_0_F, 0, GUN_MOTOR_CAN, feedback_decoding);
}

void gun_can_motor_set_vel(MotorID id, int32_t vel, CloseLoopFlag loop){
	CanMessage msg;

	msg.id = get_command_id(id);
	msg.length = GUN_CAN_MOTOR_VEL_LENGTH;
	msg.data[0] = GUN_CAN_MOTOR_VEL_CMD;
	msg.data[1] = vel;
	msg.data[2] = vel >> 8;
	msg.data[3] = vel >> 16;
	msg.data[4] = vel >> 24;
	msg.data[5] = loop;
	
	can_tx_enqueue(GUN_MOTOR_CAN, msg);
}

void gun_can_motor_set_pos(MotorID id, int32_t pos){
	CanMessage msg;
	
	msg.id = get_command_id(id);
	msg.length = GUN_CAN_MOTOR_POS_LENGTH;
	msg.data[0] = GUN_CAN_MOTOR_POS_CMD;

	msg.data[1] = pos;
	msg.data[2] = pos >> 8;
	msg.data[3] = pos >> 16;
	msg.data[4] = pos >> 24;
	can_tx_enqueue(GUN_MOTOR_CAN, msg);
}

void gun_can_motor_set_accel(MotorID id, uint32_t accel){
	CanMessage msg;
	
	msg.id = get_command_id(id);
	msg.length = GUN_CAN_MOTOR_ACCEL_LENGTH;
	msg.data[0] = GUN_CAN_MOTOR_ACCEL_CMD;
	msg.data[1] = accel;
	msg.data[2] = accel >> 8;
	msg.data[3] = accel >> 16;
	msg.data[4] = accel >> 24;

	can_tx_enqueue(GUN_MOTOR_CAN, msg);
}

void gun_can_motor_set_max_vel(MotorID id, uint32_t vel){
	CanMessage msg;
	
	msg.id = get_command_id(id);
	msg.length = GUN_CAN_MOTOR_MAXV_LENGTH;
	msg.data[0] = GUN_CAN_MOTOR_MAXV_CMD;
	msg.data[1] = vel;
	msg.data[2] = vel >> 8;
	msg.data[3] = vel >> 16;
	msg.data[4] = vel >> 24;

	can_tx_enqueue(GUN_MOTOR_CAN, msg);
}

void gun_can_motor_lock(MotorID id){
	CanMessage msg;
	
	msg.id = get_command_id(id);
	msg.length = GUN_CAN_MOTOR_LOCK_LENGTH;
	msg.data[0] = GUN_CAN_MOTOR_LOCK_CMD;
	
	can_tx_enqueue(GUN_MOTOR_CAN, msg);
}

void gun_can_motor_set_competition_mode(MotorID id, FunctionalState new_state){
	CanMessage msg;
	
	msg.id = get_command_id(id);
	msg.length = GUN_CAN_MOTOR_COMPETITION_MODE_LENGTH;
	msg.data[0] = GUN_CAN_MOTOR_COMPETITION_MODE_CMD;
	msg.data[1] = new_state;

	can_tx_enqueue(GUN_MOTOR_CAN, msg);
}

void gun_can_motor_stfu(MotorID id, FunctionalState new_state){
	CanMessage msg;
	
	msg.id = get_command_id(id);
	msg.length = GUN_CAN_MOTOR_STFU_MODE_LENGTH;
	msg.data[0] = GUN_CAN_MOTOR_STFU_MODE_CMD;
	msg.data[1] = new_state;

	can_tx_enqueue(GUN_MOTOR_CAN, msg);
}

/**********
* RX Side
***********/

int32_t gun_can_get_encoder(MotorID id){
	return enc_value[id];
}

int32_t gun_can_get_pwm(MotorID id){
	return pwm_value[id];
}

GunMotorState gun_can_get_state(MotorID id){
	if (get_ticks() - last_recv[id] > 128){
		return NO_RESPOND;
	}else{
		return state[id];
	}
}

bool gun_can_get_limit_trig(MotorID id, GunLimitID limit_id){
	return limit_trig[id][limit_id];
}

s32 gun_can_get_limit_pos(MotorID id, GunLimitID limit_id){
	return limit_pos[id][limit_id];
}

void gun_reg_OnLimitPosUpdate(MotorID id, OnLimitPosUpdate listener){
	limit_pos_update_listener[id] = listener;
}

void gun_reg_OnTargetReached(MotorID id, OnTargetReached listener){
	target_reached_listener[id] = listener;
}

void gun_disable_OnLimitPosUpdate(MotorID id){
	limit_pos_update_listener[id] = 0;
}

void gun_disable_OnTargetReached(MotorID id){
	target_reached_listener[id] = 0;
}

#undef get_id
