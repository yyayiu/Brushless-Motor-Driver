#ifndef __LCD_MAIN_H
#define __LCD_MAIN_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_rcc.h"
#include "ticks.h"
#include "spi_protocol.h"
#include "spi_xbc_mb.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum {
	PIN_ON_TOP,
	PIN_ON_LEFT,
	PIN_ON_BOTTOM,
	PIN_ON_RIGHT
}TFT_ORIENTATION;

/*
typedef enum {
	DISPLAY_ON_BOARD,
	DISPLAY_REMOTE
}DISPLAY_MODE;
*/

/* GPIO settings */

static const GPIO * tft_rst = &PA8;

static GPIO * tft_dc;
static GPIO * tft_cs;
static SPI_TypeDef * tft_spi_port;

#define ON_BOARD_PORT	SPI1
#define ON_BOARD_CS		&PA4
#define ON_BOARD_DC		&PA6

#define REMOTE_PORT	SPI3
#define REMOTE_CS		&PC13
#define REMOTE_DC		&PB4

/* Colors */

#define	RGB888TO565(RGB888)  (((RGB888 >> 8) & 0xF800) |((RGB888 >> 5) & 0x07E0) | ((RGB888 >> 3) & 0x001F))

#define WHITE				(RGB888TO565(0xFFFFFF))
#define BLACK				(RGB888TO565(0x000000))
#define DARK_GREY		(RGB888TO565(0x555555))
#define GREY				(RGB888TO565(0xAAAAAA))
#define RED					(RGB888TO565(0xFF0000))
#define DARK_RED		(RGB888TO565(0x800000))
#define ORANGE			(RGB888TO565(0xFF9900))
#define YELLOW			(RGB888TO565(0xFFFF00))
#define GREEN				(RGB888TO565(0x00FF00))
#define DARK_GREEN	(RGB888TO565(0x00CC00))
#define BLUE				(RGB888TO565(0x0000FF))
#define BLUE2				(RGB888TO565(0x202060))
#define SKY_BLUE		(RGB888TO565(0x11CFFF))
#define CYAN				(RGB888TO565(0x8888FF))
#define PURPLE			(RGB888TO565(0x00AAAA))
#define PINK				(RGB888TO565(0xC71585))

/* Pixels */

#define MAX_WIDTH			128
#define MAX_HEIGHT		160
#define CHAR_WIDTH		8
#define CHAR_HEIGHT		16

#define CHAR_MAX_X_VERTICAL		16
#define CHAR_MAX_Y_VERTICAL		10

#define CHAR_MAX_X_HORIZONTAL	20
#define CHAR_MAX_Y_HORIZONTAL	8

extern u8 tft_orientation;
extern u16 curr_bg_color;
extern u16 curr_text_color;
extern u16 curr_text_color_sp;

extern char text_buf[2][160];
extern u16 text_color_buf[2][160];
extern u16 bg_color_buf[2][160];

// garuda
extern u8 char_max_x, char_max_y;

/* TFT communication control */
void tft_spi_init(void);
void tft_init(TFT_ORIENTATION orientation, u16 in_bg_color, u16 in_text_color, u16 in_text_color_sp); //, DISPLAY_MODE mode);
//void tft_deinit(void);
//void tft_toggle_display(void);
void tft_enable(void);
void tft_disable(void);

/* Display properties configuration */
void tft_set_bg_color(u16 in_bg_color);
void tft_set_text_color(u16 in_text_color);
void tft_set_special_color(u16 text_color_sp);
u8 tft_get_orientation(void);

/* Pixel printing */
void tft_put_pixel(u8 x, u8 y, u16 color);
void tft_fill_color(u16 color);
void tft_force_clear(void);

/* Text buffer printing */
void tft_clear_line(u8 line);
void tft_clear(void);
char tft_get_buffer_char(u8 x, u8 y);
u16 tft_get_buffer_color(u8 x, u8 y);
void tft_prints(u8 x, u8 y, const char * pstr, ...);
void tft_println(const char * pstr, ...);
void tft_update(void);

void tft_put_logo(u8 x, u8 y);

#endif		/* __LCD_MAIN_H */
