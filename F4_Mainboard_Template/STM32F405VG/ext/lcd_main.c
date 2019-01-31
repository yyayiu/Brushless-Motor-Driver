#include "lcd_main.h"
#include "lcd_font.h"

//private data
u16 curr_bg_color = BLACK;
u16 curr_text_color = BLACK;
u16 curr_text_color_sp = BLACK;

static u8 col_shift;
static u8 row_shift;
	
u8 tft_orientation = 0, tft_enabled = 1;
u8 tft_width = 0, tft_height = 0;
u8 tft_y_index = 0;
u8 char_max_x, char_max_y;

u8 cur_screen, next_screen;
u8 pointer_to_curr_screen = 0;
char text_buf[2][160];
u16 text_color_buf[2][160];
u16 bg_color_buf[2][160];
u16 print_pos = 0;

//static DISPLAY_MODE cur_display_mode = DISPLAY_REMOTE;

/**
  * @brief  Initialization of SPI for TFT
  * @param  None
  * @retval None
  */
void tft_spi_init(void) {
	spi_init(tft_spi_port, SPI_Mode_Master, SPI_BaudRatePrescaler_4);
	
	gpio_rcc_init(tft_rst);
	gpio_rcc_init(tft_dc);
	gpio_rcc_init(tft_cs);
	
	gpio_output_init(tft_rst, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	gpio_output_init(tft_dc, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	gpio_output_init(tft_cs, GPIO_OType_PP, GPIO_PuPd_NOPULL);
}

/**
  * @brief  Sending a command
  * @param  command: one byte command to be sent
  * @retval None
  */
static inline void tft_write_command(u8 command) {
	GPIO_ResetBits(tft_cs->gpio, tft_cs->gpio_pin);
	GPIO_ResetBits(tft_dc->gpio, tft_dc->gpio_pin);
	spi_tx_byte(tft_spi_port, command);
	GPIO_SetBits(tft_cs->gpio, tft_cs->gpio_pin);
}

/**
  * @brief  Sending a data
  * @param  data: one byte data to be sent
  * @retval None
  */
static inline void tft_write_data(u8 data) {
	GPIO_ResetBits(tft_cs->gpio, tft_cs->gpio_pin);
	GPIO_SetBits(tft_dc->gpio, tft_dc->gpio_pin);
	spi_tx_byte(tft_spi_port, data);
	GPIO_SetBits(tft_cs->gpio, tft_cs->gpio_pin);
}

/**
  * @brief  Initialization of TFT
  * @param  orientation: default orientation
  * @param  in_bg_color: default background color
  * @param  in_text_color: default text color
  * @param  in_text_color_sp: default special text color
  * @retval None
  */
void tft_init(TFT_ORIENTATION orientation, u16 in_bg_color, u16 in_text_color, u16 in_text_color_sp) { //, DISPLAY_MODE mode) {
	//cur_display_mode = mode;
	
	/*
	tft_spi_port = ((cur_display_mode == DISPLAY_ON_BOARD) ? ON_BOARD_PORT : REMOTE_PORT);
	tft_cs = (GPIO *)((cur_display_mode == DISPLAY_ON_BOARD) ? ON_BOARD_CS : REMOTE_CS);
	tft_dc = (GPIO *)((cur_display_mode == DISPLAY_ON_BOARD) ? ON_BOARD_DC : REMOTE_DC);
	*/
	
	tft_spi_port = ON_BOARD_PORT;
	tft_cs = (GPIO *)ON_BOARD_CS;
	tft_dc = (GPIO *)ON_BOARD_DC;
	
	tft_y_index = 0;
	cur_screen = 0;
	next_screen = 1;
	tft_orientation = orientation;

	col_shift = (tft_orientation % 2) ? 1 : 2;
	row_shift = (tft_orientation % 2) ? 2 : 1;
	
	tft_spi_init();
	
	//if (cur_display_mode == DISPLAY_ON_BOARD) {
	//Hardware reset
	GPIO_ResetBits(tft_rst->gpio, tft_rst->gpio_pin);
	_delay_ms(100);
	GPIO_SetBits(tft_rst->gpio, tft_rst->gpio_pin);
	_delay_ms(100);
	
	//LCD Init For 1.44Inch LCD Panel with ST7735R.
	tft_write_command(0x11);//Sleep exit
	_delay_ms(120);
		
	//ST7735R Frame Rate
	tft_write_command(0xB1);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);

	tft_write_command(0xB2); 
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);

	tft_write_command(0xB3);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);
	tft_write_data(0x01);
	tft_write_data(0x2C);
	tft_write_data(0x2D);
	
	tft_write_command(0xB4); //Column inversion
	tft_write_data(0x07); 
	
	//ST7735R Power Sequence
	tft_write_command(0xC0);
	tft_write_data(0xA2);
	tft_write_data(0x02);
	tft_write_data(0x84);
	tft_write_command(0xC1);
	tft_write_data(0xC5);

	tft_write_command(0xC2);
	tft_write_data(0x0A);
	tft_write_data(0x00);

	tft_write_command(0xC3);
	tft_write_data(0x8A);
	tft_write_data(0x2A);
	tft_write_command(0xC4);
	tft_write_data(0x8A);
	tft_write_data(0xEE);
	
	tft_write_command(0xC5); //VCOM
	tft_write_data(0x0E);

	tft_write_command(0x36); //MX, MY, RGB mode
	switch (tft_orientation) {
		case 0: tft_write_data(0xC0); break;
		case 1: tft_write_data(0xA0); break;
		case 2: tft_write_data(0x00); break;
		case 3: tft_write_data(0x60); break;
	}
	
	//ST7735R Gamma Sequence
	tft_write_command(0xe0);
	tft_write_data(0x0f);
	tft_write_data(0x1a);
	tft_write_data(0x0f);
	tft_write_data(0x18);
	tft_write_data(0x2f);
	tft_write_data(0x28);
	tft_write_data(0x20);
	tft_write_data(0x22);
	tft_write_data(0x1f);
	tft_write_data(0x1b);
	tft_write_data(0x23);
	tft_write_data(0x37);
	tft_write_data(0x00);
	tft_write_data(0x07);
	tft_write_data(0x02);
	tft_write_data(0x10);

	tft_write_command(0xe1);
	tft_write_data(0x0f);
	tft_write_data(0x1b);
	tft_write_data(0x0f);
	tft_write_data(0x17);
	tft_write_data(0x33);
	tft_write_data(0x2c);
	tft_write_data(0x29);
	tft_write_data(0x2e);
	tft_write_data(0x30);
	tft_write_data(0x30);
	tft_write_data(0x39);
	tft_write_data(0x3f);
	tft_write_data(0x00);
	tft_write_data(0x07);
	tft_write_data(0x03);
	tft_write_data(0x10);
	
	tft_write_command(0x2a);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x7f);

	tft_write_command(0x2b);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x00);
	tft_write_data(0x9f);

	tft_write_command(0xF0); //Enable test command
	tft_write_data(0x01);
	tft_write_command(0xF6); //Disable ram power save mode
	tft_write_data(0x00);
	
	tft_write_command(0x3A); //65k mode
	tft_write_data(0x05);
	
	tft_write_command(0x29);//Display on
	
	tft_write_command(0x2C);
	//}
	
	tft_set_bg_color(in_bg_color);
	tft_set_text_color(in_text_color);
	tft_set_special_color(in_text_color_sp);
	
	//if (cur_display_mode == DISPLAY_ON_BOARD)
	tft_fill_color(in_bg_color);
	
	char_max_x = (orientation % 2) ? CHAR_MAX_X_HORIZONTAL : CHAR_MAX_X_VERTICAL;
	char_max_y = (orientation % 2) ? CHAR_MAX_Y_HORIZONTAL : CHAR_MAX_Y_VERTICAL;

	tft_clear();
	tft_update();
}

/*
void tft_deinit(void) {
	spi_deinit(tft_spi_port);
	gpio_input_init(tft_cs, GPIO_PuPd_NOPULL);
	gpio_input_init(tft_dc, GPIO_PuPd_NOPULL);
}

void tft_toggle_display(void) {
	tft_deinit();
	cur_display_mode = (cur_display_mode + 1) % 2;
	tft_init(tft_orientation, curr_bg_color, curr_text_color, curr_text_color_sp, cur_display_mode);
}
*/

/**
  * @brief  Enable using TFT
  * @param  None
  * @retval None
  */
void tft_enable(void) {
	tft_enabled = 1;
}

/**
  * @brief  Disable using TFT
  * @param  None
  * @retval None
  */
void tft_disable(void) {
	tft_enabled = 0;
}

/**
  * @brief  Set the current background color
  * @param  None
  * @retval None
  */
void tft_set_bg_color(u16 in_bg_color) {
	curr_bg_color = in_bg_color;
}

/**
  * @brief  Set the current text color
  * @param  None
  * @retval None
  */
void tft_set_text_color(u16 in_text_color) {
	curr_text_color = in_text_color;
}

/**
  * @brief  Set the current special text color
  * @param  None	
  * @retval None
  */
void tft_set_special_color(u16 text_color_sp) {
	curr_text_color_sp = text_color_sp;
}

u8 tft_get_orientation() {
	return tft_orientation;
}

static inline void tft_set_region(u16 x, u16 y, u16 w, u16 h) {
	//tft_write_data(0x00); //Dummy
	tft_write_command(0x2a);
	tft_write_data(0x00);
	tft_write_data(x+col_shift);
	tft_write_data(0x00);
	tft_write_data(x+w+col_shift);

	tft_write_command(0x2b);
	tft_write_data(0x00);
	tft_write_data(y+row_shift);
	tft_write_data(0x00);
	tft_write_data(y+h+row_shift);
	
	tft_write_command(0x2c);
}

/**
  * @brief  Clear one line in the text buffer
  * @param  line: the line to be cleared
  * @retval None
  */
void tft_clear_line(u8 line){
	for (u8 x = 0; x < char_max_x; x++) {
		text_buf[cur_screen][line * char_max_x + x] = ' ';
		text_color_buf[cur_screen][line * char_max_x + x] = curr_text_color;
		bg_color_buf[cur_screen][line * char_max_x + x] = curr_bg_color;
	}
}

/**
  * @brief  Clear the whole text buffer
  * @param  None
  * @retval None
  */
void tft_clear(void){
	tft_y_index = 0;
	for(u8 y = 0; y < char_max_y; y++){
		tft_clear_line(y);
	}
}

/**
  * @brief  Print a single pixel on screen
  * @param  x: x-coordinate
  * @param  y: y-coordinate
  * @param  color: color of the pixel
  * @retval None
  */
void tft_put_pixel(u8 x, u8 y, u16 color) {
	tft_set_region(x, y, 1, 1);
	tft_write_data(color >> 8);
	tft_write_data(color);
}

/**
  * @brief  Fill the whole screen with a color
  * @param  color: color to be filled with
  * @retval None
  */
void tft_fill_color(u16 color) {
	u16 i;
	if (tft_orientation % 2)
		tft_set_region(0, 0, MAX_HEIGHT, MAX_WIDTH);
	else
		tft_set_region(0, 0, MAX_WIDTH, MAX_HEIGHT);
	
	for (i = 0; i < MAX_WIDTH*MAX_HEIGHT; i++) {
		tft_write_data(color >> 8);
		tft_write_data(color);
	}
}

/**
  * @brief  Clear every pixel on screen
  * @param  None
  * @retval None
  */
void tft_force_clear(void) {
	for (u8 i = 0; i < char_max_x * char_max_y; i++) {
		text_buf[0][i] = ' ';
		text_buf[1][i] = ' ';
	}
	tft_fill_color(curr_bg_color);
}

static void tft_set_buffer(u8 x, u8 y, u8 * fp) {
	u8 i = y * char_max_x + x;
	u8 is_special = 0;
	
	while (*fp && i < char_max_x * char_max_y) {
		switch(*fp) {
			case '\r':
			case '\n':
				i = (i / char_max_x + 1) * char_max_x + x;
				break;
			case '[':
			case ']':
				is_special = (*fp == '[');
				break;
			default:
				text_buf[cur_screen][i] = *fp;
				text_color_buf[cur_screen][i] = is_special ? curr_text_color_sp : curr_text_color;
				bg_color_buf[cur_screen][i++] = curr_bg_color;
				break;
		}
		fp++;
	}
}

char tft_get_buffer_char(u8 x, u8 y) {
	return text_buf[cur_screen][y * char_max_x + x];
}

u16 tft_get_buffer_color(u8 x, u8 y) {
	return text_color_buf[cur_screen][y * char_max_x + x];
}

/**
  * @brief  Print a string at certain position, use [] to indicate special words
  * @param  x: starting x-coordinate
  * @param  y: starting y-coordinate
  * @param  pstr: string to be printed
  */
void tft_prints(u8 x, u8 y, const char * pstr, ...){
	if (x >= char_max_x || y >= char_max_y)
		return;
	
	u8 buf[256];
	va_list arglist;
	va_start(arglist, pstr);
	vsprintf((char*)buf, (const char*)pstr, arglist);
	va_end(arglist);
	
	tft_set_buffer(x, y, buf);
}

/**
	* Append a line to the tft screen. Not affected by tft_prints
	* @param pstr: Stuff to be printed
	*/
void tft_println(const char * pstr, ...){
	u8 buf[256];
	va_list arglist;
	va_start(arglist, pstr);
	vsprintf((char*)buf, (const char*)pstr, arglist);
	va_end(arglist);
	
	tft_set_buffer(0, tft_y_index, buf);
	
	tft_y_index++;
	if (tft_y_index >= char_max_y) tft_y_index = 0;
}

static inline bool tft_char_is_changed(u8 i){
	return !((text_buf[cur_screen][i] == text_buf[next_screen][i])
						&& (text_color_buf[cur_screen][i] == text_color_buf[next_screen][i])
						&& (bg_color_buf[cur_screen][i] == bg_color_buf[next_screen][i]));
}

/**
  * @brief  Refresh the whole screen
  * @param  None
  * @retval None
  */
void tft_update(void) {
	u8 * char_ptr;
	s16 adj_cnt = 0;
	u16 color;
	u8 char_cnt = 0;
	u8 y = 0;
	
	if (!tft_enabled)
		return;
	
	for (int i = 0; i < char_max_x * char_max_y; i++) {
		if (tft_char_is_changed(i)) {
			adj_cnt = 1;
			while (((i % char_max_x) + adj_cnt < char_max_x) && tft_char_is_changed(i + (adj_cnt++))); //Search for diff char
			tft_set_region((i % char_max_x)*CHAR_WIDTH, (i / char_max_x)*CHAR_HEIGHT, adj_cnt*CHAR_WIDTH-1, CHAR_HEIGHT-1);
			
			y = 0;
			while (y < CHAR_HEIGHT) {
				char_ptr = CHAR_PTR(text_buf[cur_screen][i + char_cnt]);
				for (u8 x = CHAR_WIDTH; x > 0; x--) {
					color = (char_ptr[y] >> (x - 1) & 0x01) ? text_color_buf[cur_screen][i + char_cnt] : bg_color_buf[cur_screen][i + char_cnt];
					tft_write_data(color >> 8);
					tft_write_data(color);
				}
				if (++char_cnt == adj_cnt) {
					char_cnt = 0;
					y++;
				}
			}
			i += adj_cnt-1;
		}
	}
	
	// Swap pointers
	cur_screen = (cur_screen == 0);
	next_screen = (next_screen == 0);
}

/**
* Place an awesome logo at x, y
* Like, very awesome
**/
void tft_put_logo(u8 x, u8 y){
	//Place black background
	for (u8 loopX = 0; loopX<47; loopX++){
		for (u8 loopY = 0; loopY<39; loopY++){
			tft_put_pixel(x+loopX, y+loopY, BLACK);
		}
	}

	//Place DARK_RED parallelogram
	for (u8 loopX = 15; loopX<45; loopX++){
		tft_put_pixel(x+loopX, y+1, DARK_RED);
	}
	for (u8 loopX = 14; loopX<44; loopX++){
		tft_put_pixel(x+loopX, y+2, DARK_RED);
		tft_put_pixel(x+loopX, y+3, DARK_RED);
	}
	for (u8 loopX = 13; loopX<43; loopX++){
		tft_put_pixel(x+loopX, y+4, DARK_RED);
		tft_put_pixel(x+loopX, y+5, DARK_RED);
		tft_put_pixel(x+loopX, y+6, DARK_RED);
	}
	for (u8 loopX = 12; loopX<42; loopX++){
		tft_put_pixel(x+loopX, y+7, DARK_RED);
		tft_put_pixel(x+loopX, y+8, DARK_RED);
		tft_put_pixel(x+loopX, y+9, DARK_RED);
	}
	for (u8 loopX = 11; loopX<41; loopX++){
		tft_put_pixel(x+loopX, y+10, DARK_RED);
		tft_put_pixel(x+loopX, y+11, DARK_RED);
	}
	for (u8 loopX = 10; loopX<40; loopX++){
		tft_put_pixel(x+loopX, y+12, DARK_RED);
		tft_put_pixel(x+loopX, y+13, DARK_RED);
		tft_put_pixel(x+loopX, y+14, DARK_RED);
		tft_put_pixel(x+loopX, y+15, DARK_RED);
	}
	for (u8 loopX = 9; loopX<39; loopX++){
		tft_put_pixel(x+loopX, y+16, DARK_RED);
		tft_put_pixel(x+loopX, y+17, DARK_RED);
		tft_put_pixel(x+loopX, y+18, DARK_RED);
	}
	for (u8 loopX = 8; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+19, DARK_RED);
		tft_put_pixel(x+loopX, y+20, DARK_RED);
	}
	for (u8 loopX = 7; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+21, DARK_RED);
		tft_put_pixel(x+loopX, y+22, DARK_RED);
		tft_put_pixel(x+loopX, y+23, DARK_RED);
		tft_put_pixel(x+loopX, y+24, DARK_RED);
	}

	//Put the R letter
	tft_put_pixel(x+16, y+6, GREY);
	for (u8 loopX = 17; loopX<36; loopX++){
		tft_put_pixel(x+loopX, y+6, WHITE);
	}

	for (u8 loopX = 15; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+7, WHITE);
	}

	tft_put_pixel(x+14, y+8, DARK_GREY);
	for (u8 loopX = 15; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+8, WHITE);
	}
	tft_put_pixel(x+37, y+8, GREY);

	for (u8 loopX = 14; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+9, WHITE);
	}

	for (u8 loopX = 14; loopX<20; loopX++){
		tft_put_pixel(x+loopX, y+10, WHITE);
	}
	tft_put_pixel(x+20, y+10, GREY);
	tft_put_pixel(x+28, y+10, GREY);
	for (u8 loopX = 31; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+10, WHITE);
	}

	for (u8 loopX = 14; loopX<20; loopX++){
		tft_put_pixel(x+loopX, y+11, WHITE);
	}
	tft_put_pixel(x+28, y+11, GREY);
	tft_put_pixel(x+29, y+11, DARK_GREY);
	for (u8 loopX = 32; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+11, WHITE);
	}

	tft_put_pixel(x+13, y+12, GREY);
	for (u8 loopX = 14; loopX<19; loopX++){
		tft_put_pixel(x+loopX, y+12, WHITE);
	}
	tft_put_pixel(x+19, y+12, DARK_GREY);
	tft_put_pixel(x+21, y+12, WHITE);
	for (u8 loopX = 30; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+12, WHITE);
	}

	for (u8 loopX = 13; loopX<19; loopX++){
		tft_put_pixel(x+loopX, y+13, WHITE);
	}
	tft_put_pixel(x+19, y+13, GREY);
	tft_put_pixel(x+28, y+13, WHITE);
	tft_put_pixel(x+29, y+13, WHITE);
	for (u8 loopX = 32; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+13, WHITE);
	}

	for (u8 loopX = 13; loopX<19; loopX++){
		tft_put_pixel(x+loopX, y+14, WHITE);
	}
	tft_put_pixel(x+19, y+14, DARK_GREY);
	tft_put_pixel(x+20, y+14, DARK_GREY);
	tft_put_pixel(x+28, y+14, GREY);
	tft_put_pixel(x+31, y+14, GREY);
	for (u8 loopX = 32; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+14, WHITE);
	}

	//LEFT LEG
	for (u8 loopX = 12; loopX<18; loopX++){
		tft_put_pixel(x+loopX, y+15, WHITE);
		tft_put_pixel(x+loopX, y+16, WHITE);
		tft_put_pixel(x+loopX, y+17, WHITE);
	}
	for (u8 loopX = 11; loopX<17; loopX++){
		tft_put_pixel(x+loopX, y+18, WHITE);
		tft_put_pixel(x+loopX, y+19, WHITE);
		tft_put_pixel(x+loopX, y+20, WHITE);
	}
	for (u8 loopX = 10; loopX<16; loopX++){
		tft_put_pixel(x+loopX, y+21, WHITE);
		tft_put_pixel(x+loopX, y+22, WHITE);
	}
	for (u8 loopX = 10; loopX<15; loopX++){
		tft_put_pixel(x+loopX, y+23, WHITE);
	}
	for (u8 loopX = 9; loopX<15; loopX++){
		tft_put_pixel(x+loopX, y+24, WHITE);
	}
	for (u8 loopX = 9; loopX<14; loopX++){
		tft_put_pixel(x+loopX, y+25, WHITE);
		tft_put_pixel(x+loopX, y+26, WHITE);
	}

	//RIGHT LEG
	for (u8 loopX = 21; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+15, WHITE);
	}
	for (u8 loopX = 20; loopX<38; loopX++){
		tft_put_pixel(x+loopX, y+16, WHITE);
	}
	for (u8 loopX = 20; loopX<37; loopX++){
		tft_put_pixel(x+loopX, y+17, WHITE);
	}
	for (u8 loopX = 21; loopX<36; loopX++){
		tft_put_pixel(x+loopX, y+18, WHITE);
	}
	for (u8 loopX = 21; loopX<29; loopX++){
		tft_put_pixel(x+loopX, y+19, WHITE);
	}
	for (u8 loopX = 21; loopX<30; loopX++){
		tft_put_pixel(x+loopX, y+20, WHITE);
		tft_put_pixel(x+loopX+1, y+21, WHITE);
		tft_put_pixel(x+loopX+2, y+22, WHITE);
		tft_put_pixel(x+loopX+3, y+23, WHITE);
	}
	for (u8 loopX = 25; loopX<33; loopX++){
		tft_put_pixel(x+loopX, y+24, WHITE);
	}
	for (u8 loopX = 26; loopX<35; loopX++){
		tft_put_pixel(x+loopX, y+25, WHITE);
	}
	for (u8 loopX = 27; loopX<35; loopX++){
		tft_put_pixel(x+loopX, y+26, WHITE);
	}

	//Misc in R letter
	tft_put_pixel(x+11, y+17, DARK_GREY);
	tft_put_pixel(x+21, y+17, GREY);
	tft_put_pixel(x+17, y+18, DARK_GREY);
	tft_put_pixel(x+36, y+18, GREY);
	for (u8 loopX = 29; loopX<34; loopX++){
		tft_put_pixel(x+loopX, y+19, GREY);
	}
	tft_put_pixel(x+34, y+19, DARK_GREY);
	tft_put_pixel(x+10, y+20, GREY);
	tft_put_pixel(x+18, y+20, WHITE);
	tft_put_pixel(x+16, y+21, GREY);
	tft_put_pixel(x+20, y+21, WHITE);
	tft_put_pixel(x+18, y+22, WHITE);
	tft_put_pixel(x+18, y+22, GREY);
	tft_put_pixel(x+9, y+23, DARK_GREY);
	tft_put_pixel(x+33, y+24, GREY);
	tft_put_pixel(x+14, y+25, DARK_GREY);
	tft_put_pixel(x+8, y+26, GREY);
	tft_put_pixel(x+35, y+26, DARK_GREY);

	//Bright Red in red region
	tft_put_pixel(x+15, y+3, RED);
	tft_put_pixel(x+17, y+4, RED);
	tft_put_pixel(x+16, y+5, RED);
	tft_put_pixel(x+35, y+5, RED);
	tft_put_pixel(x+38, y+6, RED);
	tft_put_pixel(x+49, y+6, RED);
	tft_put_pixel(x+39, y+7, RED);
	tft_put_pixel(x+11, y+14, RED);
	tft_put_pixel(x+20, y+13, RED);
	tft_put_pixel(x+39, y+14, RED);
	tft_put_pixel(x+39, y+15, RED);
	tft_put_pixel(x+20, y+18, RED);
	tft_put_pixel(x+37, y+18, RED);
	tft_put_pixel(x+38, y+19, RED);
	tft_put_pixel(x+34, y+20, RED);
	tft_put_pixel(x+36, y+20, RED);
	tft_put_pixel(x+33, y+22, RED);
	tft_put_pixel(x+8, y+23, RED);
	tft_put_pixel(x+20, y+23, RED);
	tft_put_pixel(x+21, y+23, RED);
	tft_put_pixel(x+33, y+23, RED);
	tft_put_pixel(x+8, y+24, RED);
	tft_put_pixel(x+17, y+24, RED);
	tft_put_pixel(x+22, y+24, RED);
	tft_put_pixel(x+36, y+24, RED);
	tft_put_pixel(x+7, y+25, RED);
	tft_put_pixel(x+24, y+25, RED);
	tft_put_pixel(x+7, y+26, RED);
	tft_put_pixel(x+15, y+26, RED);
	tft_put_pixel(x+18, y+26, RED);
	tft_put_pixel(x+19, y+26, RED);
	tft_put_pixel(x+22, y+26, RED);
	tft_put_pixel(x+23, y+26, RED);
	tft_put_pixel(x+24, y+26, RED);

	//Put the HKUST word
	tft_put_pixel(x+6, y+28, WHITE);
	tft_put_pixel(x+10, y+28, WHITE);
	tft_put_pixel(x+11, y+28, WHITE);
	tft_put_pixel(x+13, y+28, WHITE);
	tft_put_pixel(x+17, y+28, WHITE);
	tft_put_pixel(x+20, y+28, WHITE);
	tft_put_pixel(x+23, y+28, WHITE);
	tft_put_pixel(x+24, y+28, WHITE);
	for (u8 loopX=27;loopX<36;loopX++){
		tft_put_pixel(x+loopX, y+28, WHITE);
	}

	tft_put_pixel(x+5, y+29, WHITE);
	tft_put_pixel(x+6, y+29, WHITE);
	tft_put_pixel(x+9, y+29, WHITE);
	tft_put_pixel(x+12, y+29, WHITE);
	tft_put_pixel(x+13, y+29, WHITE);
	tft_put_pixel(x+16, y+29, WHITE);
	tft_put_pixel(x+17, y+29, WHITE);
	tft_put_pixel(x+19, y+29, WHITE);
	tft_put_pixel(x+20, y+29, WHITE);
	tft_put_pixel(x+22, y+29, WHITE);
	tft_put_pixel(x+23, y+29, WHITE);
	tft_put_pixel(x+24, y+29, WHITE);
	for (u8 loopX=26;loopX<29;loopX++){
		tft_put_pixel(x+loopX, y+29, WHITE);
	}
	for (u8 loopX=29;loopX<35;loopX++){
		tft_put_pixel(x+loopX, y+29, WHITE);
	}

	tft_put_pixel(x+5, y+30, WHITE);
	tft_put_pixel(x+6, y+30, WHITE);
	tft_put_pixel(x+8, y+30, WHITE);
	tft_put_pixel(x+9, y+30, WHITE);
	tft_put_pixel(x+12, y+30, WHITE);
	tft_put_pixel(x+13, y+30, WHITE);
	tft_put_pixel(x+15, y+30, WHITE);
	tft_put_pixel(x+16, y+30, WHITE);
	tft_put_pixel(x+19, y+30, WHITE);
	tft_put_pixel(x+20, y+30, WHITE);
	tft_put_pixel(x+22, y+30, WHITE);
	tft_put_pixel(x+23, y+30, WHITE);
	tft_put_pixel(x+26, y+30, WHITE);
	tft_put_pixel(x+27, y+30, WHITE);
	tft_put_pixel(x+32, y+30, WHITE);

	for (u8 loopX=5;loopX<10;loopX++){
		tft_put_pixel(x+loopX, y+31, WHITE);
	}
	for (u8 loopX=11;loopX<16;loopX++){
		tft_put_pixel(x+loopX, y+31, WHITE);
	}
	tft_put_pixel(x+19, y+31, WHITE);
	tft_put_pixel(x+20, y+31, WHITE);
	tft_put_pixel(x+22, y+31, WHITE);
	tft_put_pixel(x+23, y+31, WHITE);
	tft_put_pixel(x+25, y+31, WHITE);
	tft_put_pixel(x+26, y+31, WHITE);
	tft_put_pixel(x+27, y+31, WHITE);
	tft_put_pixel(x+31, y+31, WHITE);
	tft_put_pixel(x+32, y+31, WHITE);

	for (u8 loopX=4;loopX<10;loopX++){
		tft_put_pixel(x+loopX, y+32, WHITE);
	}
	tft_put_pixel(x+11, y+32, WHITE);
	tft_put_pixel(x+12, y+32, WHITE);
	tft_put_pixel(x+14, y+32, WHITE);
	tft_put_pixel(x+15, y+32, WHITE);
	tft_put_pixel(x+18, y+32, WHITE);
	tft_put_pixel(x+19, y+32, WHITE);
	tft_put_pixel(x+21, y+32, WHITE);
	tft_put_pixel(x+22, y+32, WHITE);
	tft_put_pixel(x+26, y+32, WHITE);
	tft_put_pixel(x+27, y+32, WHITE);
	tft_put_pixel(x+31, y+32, WHITE);
	tft_put_pixel(x+32, y+32, WHITE);

	tft_put_pixel(x+4, y+33, WHITE);
	tft_put_pixel(x+5, y+33, WHITE);
	tft_put_pixel(x+7, y+33, WHITE);
	tft_put_pixel(x+8, y+33, WHITE);
	tft_put_pixel(x+11, y+33, WHITE);
	tft_put_pixel(x+12, y+33, WHITE);
	tft_put_pixel(x+14, y+33, WHITE);
	tft_put_pixel(x+15, y+33, WHITE);
	tft_put_pixel(x+18, y+33, WHITE);
	tft_put_pixel(x+19, y+33, WHITE);
	tft_put_pixel(x+21, y+33, WHITE);
	tft_put_pixel(x+22, y+33, WHITE);
	tft_put_pixel(x+25, y+33, WHITE);
	tft_put_pixel(x+26, y+33, WHITE);
	tft_put_pixel(x+27, y+33, WHITE);
	tft_put_pixel(x+30, y+33, WHITE);
	tft_put_pixel(x+31, y+33, WHITE);

	tft_put_pixel(x+3, y+34, WHITE);
	tft_put_pixel(x+4, y+34, WHITE);
	tft_put_pixel(x+5, y+34, WHITE);
	tft_put_pixel(x+7, y+34, WHITE);
	tft_put_pixel(x+8, y+34, WHITE);
	tft_put_pixel(x+10, y+34, WHITE);
	tft_put_pixel(x+11, y+34, WHITE);
	tft_put_pixel(x+14, y+34, WHITE);
	tft_put_pixel(x+15, y+34, WHITE);
	tft_put_pixel(x+18, y+34, WHITE);
	tft_put_pixel(x+19, y+34, WHITE);
	tft_put_pixel(x+20, y+34, WHITE);
	tft_put_pixel(x+21, y+34, WHITE);
	tft_put_pixel(x+24, y+34, WHITE);
	tft_put_pixel(x+25, y+34, WHITE);
	tft_put_pixel(x+26, y+34, WHITE);
	tft_put_pixel(x+30, y+34, WHITE);
	tft_put_pixel(x+31, y+34, WHITE);

	tft_put_pixel(x+3, y+35, WHITE);
	tft_put_pixel(x+4, y+35, WHITE);
	tft_put_pixel(x+7, y+35, WHITE);
	tft_put_pixel(x+19, y+35, WHITE);
	tft_put_pixel(x+20, y+35, WHITE);
	tft_put_pixel(x+24, y+35, WHITE);

	//Place minor detail of HKUST word
	tft_put_pixel(x+5, y+28, GREY);
	tft_put_pixel(x+12, y+28, GREY);
	tft_put_pixel(x+16, y+28, GREY);
	tft_put_pixel(x+8, y+29, GREY);
	tft_put_pixel(x+31, y+30, GREY);
	tft_put_pixel(x+4, y+31, GREY);
	tft_put_pixel(x+23, y+32, GREY);
	tft_put_pixel(x+10, y+35, GREY);
	tft_put_pixel(x+11, y+35, GREY);
	tft_put_pixel(x+14, y+35, GREY);
	tft_put_pixel(x+15, y+35, GREY);
	tft_put_pixel(x+18, y+35, GREY);
	tft_put_pixel(x+25, y+35, GREY);
	tft_put_pixel(x+30, y+35, GREY);
	tft_put_pixel(x+31, y+35, GREY);

	tft_put_pixel(x+7, y+28, DARK_GREY);
	tft_put_pixel(x+25, y+30, DARK_GREY);
	tft_put_pixel(x+13, y+32, DARK_GREY);
	tft_put_pixel(x+25, y+32, DARK_GREY);
	tft_put_pixel(x+24, y+33, DARK_GREY);
	tft_put_pixel(x+6, y+35, DARK_GREY);
	tft_put_pixel(x+12, y+34, DARK_GREY);
	tft_put_pixel(x+16, y+34, DARK_GREY);
	tft_put_pixel(x+16, y+35, DARK_GREY);
}
