#ifndef __XBC_MB_H
#define __XBC_MB_H

#include "stm32f4xx.h"
#include "button.h" //For listener typedefs

#define XBC_JOY_COUNT           6
#define XBC_JOY_DEADZONE_MIN    ((s16) 5000)
#define XBC_JOY_DEADZONE_MAX    ((s16) 32000)
#define XBC_JOY_SCALE           1000  // Self-defined scale

typedef enum {
  XBC_JOY_LT,         // 1-byte
  XBC_JOY_RT,         // 1-byte
  XBC_JOY_LX,         // 2-byte
  XBC_JOY_LY,         // 2-byte
  XBC_JOY_RX,         // 2-byte
  XBC_JOY_RY          // 2-byte
} XBC_JOY;

#define XBC_BTN_TABLE \
X(XBC_UP, 0x0001) \
X(XBC_DOWN, 0x0002) \
X(XBC_LEFT, 0x0004) \
X(XBC_RIGHT, 0x0008) \
X(XBC_START, 0x0010) \
X(XBC_BACK, 0x0020) \
X(XBC_L_JOY, 0x0040) \
X(XBC_R_JOY, 0x0080) \
X(XBC_LB, 0x0100) \
X(XBC_RB, 0x0200) \
X(XBC_XBOX, 0x0400) \
X(XBC_A, 0x1000) \
X(XBC_B, 0x2000) \
X(XBC_X, 0x4000) \
X(XBC_Y, 0x8000)
#define X(a, b) a,

typedef enum {
	XBC_BTN_TABLE
} XBCButtonID;
#undef X

#define X(a, b) b,
static const u16 XBC_BTN_MASK_TABLE[] = { XBC_BTN_TABLE };
#undef X

#define XBC_BTN_COUNT (sizeof(XBC_BTN_MASK_TABLE)/sizeof(u16))

typedef enum {
  XBC_DISCONNECTED,
  XBC_CAN_CONNECTED,
  XBC_SPI_CONNECTED,
	XBC_BT_CONNECTED
} XBC_CONNECTION;

typedef enum {
  XBC_CAN_FIRST,
  XBC_SPI_FIRST,
	XBC_BT_FIRST
} XBC_PRIORITY;

typedef struct {
  u16 color, bg_color;
  char text;
  
} XBC_LCD_DATA;

void xbc_mb_init(XBC_PRIORITY priority);
XBC_CONNECTION xbc_get_connection(void);
XBC_PRIORITY xbc_get_priority(void);
u32 xbc_get_digital(void);
s16 xbc_get_joy_raw(XBC_JOY j);
s16 xbc_get_joy(XBC_JOY j);
void xbc_button_update(void);
u8 xbc_get_button(u8 buttonId);
void xbc_reg_OnClickListener(XBCButtonID button, OnClickListener listener);
void xbc_reg_OnReleaseListener(XBCButtonID button, OnClickListener listener);
void xbc_reg_OnHoldListener(u8 button_id, uint16_t threshold, uint16_t frequency, OnHoldListener listener);
void xbc_drop_OnClickListener(XBCButtonID button);
void xbc_drop_OnReleaseListener(XBCButtonID button);
void xbc_drop_OnHoldListener(u8 button_id);

#endif  /** __XBC_MB_H **/
