#ifndef __SPI_XBC_MB_H
#define __SPI_XBC_MB_H

/*
 * SPI XBC Library for STM32F4 mainboard
 * Hong Wing PANG
 * Mar 2017
 */

/*-- Dependencies --*/

#include "stm32f4xx.h"
#include "spi_protocol.h"
#include "xbc_mb.h"
#include "gpio.h"
#include "ticks.h"
#include "lcd_main.h"

/*-- Definitions --*/

//SPI XBC MISO command list
#define SPI_REPLY		0x50
#define SPI_NO_USB	0x51

//SPI XBC Validity Check value
#define SPI_CHECKVAL 0x4E

//SPI XBC MOSI command list, reserved
#define SPI_TFT			0x70
#define SPI_CTRL		0x71

//SPI XBC receiver states
typedef enum {
	SPI_RX_XBC_CMD,	//Incoming command
	SPI_RX_XBC_DATA	//Incoming XBC button data
} SPI_RX_XBC_STATE;

//SPI connection states
typedef enum {
  SPI_XBC_DISCONNECTED,		//XBC board disconnected
  SPI_XBC_USB_DISCONNECTED,	//XBC board connected, but its USB port is disconnected
  SPI_XBC_ALL_CONNECTED		//XBC board and controller connected
} SPI_XBC_CONNECTION_MODE;

typedef enum {
	SPI_XBC_TX_DISABLE,
	SPI_XBC_TX_ENABLE
} SPI_XBC_TX_STATE;

/*-- Properties --*/

//XBC SPI Port
#define SPI_XBC_PORT SPI3

#define SPI_RX_LENGTH 15

//Min time between interrupts triggered to count as disconnection
#define SPI_XBC_CONNECTION_TIMEOUT_MS   100

#define XBC_JOY_COUNT           6
#define XBC_JOY_DEADZONE_MIN    ((s16) 5000)
#define XBC_JOY_DEADZONE_MAX    ((s16) 32000)
#define XBC_JOY_SCALE           1000  // Self-defined scale

static const GPIO * xbc_nss = &PC13;	//Port for SPI NSS line
#define XBC_NSS_IRQ EXTI15_10_IRQn
#define XBC_NSS_EXTILINE EXTI_Line13
#define XBC_NSS_PORTSOURCE EXTI_PortSourceGPIOC
#define XBC_NSS_PINSOURCE EXTI_PinSource13
#define XBC_NSS_HANDLER EXTI15_10_IRQHandler

/*-- Function headers --*/

void spi_xbc_mb_init(void);

SPI_XBC_CONNECTION_MODE spi_xbc_get_connection(void);
u32 spi_xbc_get_digital(void);
u8 spi_xbc_get_button(u8 buttonId);
s16 spi_xbc_get_joy_raw(XBC_JOY j);
s16 spi_xbc_get_joy(XBC_JOY j);
u16 spi_xbc_get_back_buttons(void);
u8 spi_xbc_get_checkval(void);

void spi_xbc_rx_handler(uc8 data);

u16 spi_temp(void);
u8 * spi_get_array(void);

#endif  /** __SPI_XBC_MB_H **/
