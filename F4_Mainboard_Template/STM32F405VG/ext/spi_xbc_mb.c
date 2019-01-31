/*
To Do list for later on
//TX Display
//TX Control
*/

#include "spi_xbc_mb.h"

//Storing button states
static u32 xbc_digital = 0;
static s16 xbc_joy[XBC_JOY_COUNT] = {0};
static u16 xbc_back_buttons = 0;

//XBC connection states
static SPI_XBC_CONNECTION_MODE xbc_connection = SPI_XBC_DISCONNECTED;

static u8 my_data[32] = {0};
static u32 pointer = 0;
static u32 error_count = 0;
static u32 last_recv = 0;

static void spi_xbc_update_ssi() {
	if (GPIO_ReadInputDataBit(xbc_nss->gpio, xbc_nss->gpio_pin)) {
		SPI_NSSInternalSoftwareConfig(SPI_XBC_PORT, SPI_NSSInternalSoft_Set);
	} else {
		SPI_NSSInternalSoftwareConfig(SPI_XBC_PORT, SPI_NSSInternalSoft_Reset);
	}
}

void spi_xbc_mb_init(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	spi_init(SPI_XBC_PORT, SPI_Mode_Slave, SPI_BaudRatePrescaler_128);
	
	gpio_rcc_init(xbc_nss);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	gpio_input_init(xbc_nss, GPIO_PuPd_UP);
	
	SYSCFG_EXTILineConfig(XBC_NSS_PORTSOURCE, XBC_NSS_PINSOURCE);
	
	EXTI_InitStructure.EXTI_Line = XBC_NSS_EXTILINE;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = XBC_NSS_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	spi_rx_set_handler(SPI_XBC_PORT, &spi_xbc_rx_handler);
	spi_xbc_update_ssi();
}

SPI_XBC_CONNECTION_MODE spi_xbc_get_connection(void) {
	s32 ticks = get_ticks();
	if (ticks - last_recv > SPI_XBC_CONNECTION_TIMEOUT_MS) {
		xbc_connection = SPI_XBC_DISCONNECTED;
	}
  return xbc_connection;
}

u32 spi_xbc_get_digital(void) {
  if (spi_xbc_get_connection() != SPI_XBC_ALL_CONNECTED) return 0;
	return xbc_digital;
}

u8 spi_xbc_get_button(u8 buttonId) {
  if (spi_xbc_get_connection() != SPI_XBC_ALL_CONNECTED) return 0;
	return ((xbc_digital & XBC_BTN_MASK_TABLE[buttonId]) != 0);
}

s16 spi_xbc_get_joy_raw(XBC_JOY j) {
  if (spi_xbc_get_connection() != SPI_XBC_ALL_CONNECTED) return 0;
  return xbc_joy[j];
}

s16 spi_xbc_get_joy(XBC_JOY j) {
  if (spi_xbc_get_connection() != SPI_XBC_ALL_CONNECTED) return 0;
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

u16 spi_xbc_get_back_buttons(void) {
  if (spi_xbc_get_connection() != SPI_XBC_ALL_CONNECTED) return 0;
  return xbc_back_buttons;
}

void spi_xbc_rx_handler(uc8 data) {
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
				xbc_connection = SPI_XBC_ALL_CONNECTED;
			}else{
				xbc_connection = SPI_XBC_USB_DISCONNECTED;
			}
			
			last_recv = get_ticks();
		}else{
			//fucked
			error_count++;
		}
	}
}

void XBC_NSS_HANDLER(void) {
	if (EXTI_GetITStatus(XBC_NSS_EXTILINE) == SET) {
		spi_xbc_update_ssi();
		EXTI_ClearITPendingBit(XBC_NSS_EXTILINE);
	}
}
