#include "bt_xbc_mb.h"

static u32 xbc_digital = 0;
static s16 xbc_joy[XBC_JOY_COUNT] = {0};
//static u16 xbc_back_buttons = 0;
static BTConnectionStatus xbc_connection = BT_XBC_DISCONNECTED;

void bt_xbc_mb_init(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	CRC_ResetDR();
	
	uart_init(BT_XBC_COM, 115200);
	uart_interrupt_init(BT_XBC_COM, bt_xbc_receiver);
}

u32 bt_xbc_get_digital(){
  if (bt_xbc_get_connection() == BT_XBC_DISCONNECTED) {
    return 0;
  }
  return xbc_digital;
}


u8 bt_xbc_get_button(u8 buttonId) {
	return ((xbc_digital & XBC_BTN_MASK_TABLE[buttonId]) != 0);
}

s16 bt_xbc_get_joy_raw(XBC_JOY j) {
  return xbc_joy[j];
}

s16 bt_xbc_get_joy(XBC_JOY j) {
  switch (j) {
    case XBC_JOY_LT:
    case XBC_JOY_RT:
      return xbc_joy[j];

    case XBC_JOY_LX:
    case XBC_JOY_LY:
    case XBC_JOY_RX:
    case XBC_JOY_RY:
      if (xbc_joy[j] >= -XBC_JOY_DEADZONE_MIN && xbc_joy[j] <= XBC_JOY_DEADZONE_MIN) {
        return 0;
      } else if (xbc_joy[j] < -XBC_JOY_DEADZONE_MAX) {
        return -XBC_JOY_SCALE;
      } else if (xbc_joy[j] > XBC_JOY_DEADZONE_MAX) {
        return XBC_JOY_SCALE;
      } else {
        if (xbc_joy[j] > 0) {
          return (xbc_joy[j] - XBC_JOY_DEADZONE_MIN) * XBC_JOY_SCALE / (XBC_JOY_DEADZONE_MAX - XBC_JOY_DEADZONE_MIN);
        } else {
          return (xbc_joy[j] - -XBC_JOY_DEADZONE_MIN) * XBC_JOY_SCALE / (XBC_JOY_DEADZONE_MAX - XBC_JOY_DEADZONE_MIN);
        }
      }
  }
  return 0;
}

static u8 my_data[32] = {0};
static u32 pointer = 0;
static u32 error_count = 0;
static u32 last_recv = 0;
//UART interrupt receiver
void bt_xbc_receiver(u8 data){
	if (data == 0){
		pointer = 0;
	}
	
	my_data[pointer] = data;
	pointer++;
	
	if (pointer == 32){
		pointer = 0;
		
		u32 their_crc = (my_data[28]<<24) | (my_data[29]<<16) | (my_data[30]<<8) | (my_data[31]);
		my_data[28] = 28;
		my_data[29] = 29;
		my_data[30] = 30;
		my_data[31] = 31;
		
		CRC_ResetDR();
		u32 crc = CRC_CalcBlockCRC((u32*)my_data, 8);
		
		my_data[28] = crc >> 24;
		if (my_data[28] == 0) my_data[28] = 1;
		my_data[29] = crc >> 16;
		if (my_data[29] == 0) my_data[29] = 1;
		my_data[30] = crc >> 8;
		if (my_data[30] == 0) my_data[30] = 1;
		my_data[31] = crc;
		if (my_data[31] == 0) my_data[31] = 1;
		
		u32 my_crc = (my_data[28]<<24) | (my_data[29]<<16) | (my_data[30]<<8) | (my_data[31]);
		
		if (their_crc == my_crc){
			//ok
			
			xbc_digital = (my_data[1]&0xF)|((my_data[2]&0xF)<<4)|((my_data[3]&0xF)<<8)|((my_data[4]&0xF)<<12);
			
			xbc_joy[XBC_JOY_LT] = (my_data[5]&0xF)|((my_data[6]&0xF)<<4);
			xbc_joy[XBC_JOY_RT] = (my_data[7]&0xF)|((my_data[8]&0xF)<<4);
			
			xbc_joy[XBC_JOY_LX] = (my_data[9]&0xF)|((my_data[10]&0xF)<<4)|((my_data[11]&0xF)<<8)|((my_data[12]&0xF)<<12);
			xbc_joy[XBC_JOY_LY] = (my_data[13]&0xF)|((my_data[14]&0xF)<<4)|((my_data[15]&0xF)<<8)|((my_data[16]&0xF)<<12);
			xbc_joy[XBC_JOY_RX] = (my_data[17]&0xF)|((my_data[18]&0xF)<<4)|((my_data[19]&0xF)<<8)|((my_data[20]&0xF)<<12);
			xbc_joy[XBC_JOY_RY] = (my_data[21]&0xF)|((my_data[22]&0xF)<<4)|((my_data[23]&0xF)<<8)|((my_data[24]&0xF)<<12);
			//xbc_back_buttons = my_data[27]&0xF;
			
			if (my_data[25] == 25 && my_data[26] == 26){
				xbc_connection = BT_XBC_ALL_CONNECTED;
			}else{
				xbc_connection = BT_XBC_BT_CONNECTED;
			}
			
			last_recv = get_ticks();
		}else{
			//fucked
			error_count++;
		}
	}
}

BTConnectionStatus bt_xbc_get_connection(){
  if (get_ticks() - last_recv > BT_TIMEOUT_MS) {
    xbc_connection = BT_XBC_DISCONNECTED;
  }
  return xbc_connection;
}

//This only includes CRC error count.
u32 get_bt_xbc_error_count(){
	return error_count;
}

//Get last received ticks
u32 get_bt_xbc_last_recv(){
	return last_recv;
}
