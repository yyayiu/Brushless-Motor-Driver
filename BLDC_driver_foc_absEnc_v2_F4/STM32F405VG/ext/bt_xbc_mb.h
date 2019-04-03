#ifndef _BT_XBC_MB_H
#define _BT_XBC_MB_H

/**
* Connection to XBOX Controller via bluetooth.
* Make for dual link with SPI.
* Rex Cheng
*/

#include "stm32f4xx.h"
#include "spi_protocol.h"
#include "xbc_mb.h"
#include "gpio.h"
#include "ticks.h"
#include "uart.h"
#include "button.h"

typedef enum {
  BT_XBC_DISCONNECTED,   /*!< XBC Board Bluetooth Disconnected ***/
  BT_XBC_BT_CONNECTED,  /*!< XBC Board Bluetooth connected, but the XBC USB on the XBC board disconnected ***/
  BT_XBC_ALL_CONNECTED   /*!< XBC Board All connected ***/
} BTConnectionStatus;

#define BT_XBC_COM COM2
#define BT_TIMEOUT_MS 100

void bt_xbc_mb_init(void);

void bt_xbc_receiver(u8 data);

u32 bt_xbc_get_digital(void);
u8 bt_xbc_get_button(u8 buttonId);
s16 bt_xbc_get_joy_raw(XBC_JOY j);
s16 bt_xbc_get_joy(XBC_JOY j);

//Get the connection status
BTConnectionStatus bt_xbc_get_connection(void);

//This only includes CRC error count.
u32 get_bt_xbc_error_count(void);

//Get last received ticks
u32 get_bt_xbc_last_recv(void);

#endif
