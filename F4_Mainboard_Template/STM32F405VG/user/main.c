/**
** H K U S T
** Robocon 2018
**
** F4 Library
** Please do not edit this project directly, copy first.
**
*/

#include "main.h"

int main(void) {
	SystemInit();
	SystemCoreClockUpdate();
	gpio_rcc_init_all();
	
	ticks_init();
	adc_init();

	tft_init((TFT_ORIENTATION)ORIENTATION_SETTING, BLACK, WHITE, RED);
	
	led_init();
	buzzer_init();
	btn_init();
	timer_init();
	//encoder_1_init();
	encoder_2_init();
	servo_init();
	can_init();
	can_rx_init();
	motor_init();
	gun_can_motor_init();
	pneu_init();
	gyro_init();
	xbc_mb_init(XBC_SPI_FIRST);
	uart_init(COM1, 115200);
	
	do_after(buzzer_on, 100);
	do_after(buzzer_off, 400);
	do_after(buzzer_on, 700);
	do_after(buzzer_off, 1000);
	
	while (get_ticks() < 100);
	
	//Cystal fucker detection
	if (IsClockFucked){
		while(1){
			tft_clear();
			tft_println("CLOCK FUCKED");
			tft_println("FIX CRYSTAL");
			tft_update();
		}
	}
	
	u32 last_ticks = 0;
	while(1) {
		//Main Program
		u32 this_ticks = get_ticks();
		if (this_ticks == last_ticks) continue;
		
		static u32 last_tft_ticks = 0;
		if (this_ticks - last_tft_ticks >= 25){
			btn_update();
			
			tft_clear();
			tft_println("%d", this_ticks);
			tft_println("%d", SystemCoreClock);
			tft_update();
			last_tft_ticks = this_ticks;
		}
		
		static u32 last_led_ticks = 0;
		if (this_ticks - last_led_ticks >= 200){
			led_blink(LED_1);
			last_led_ticks = this_ticks;
		}
	}
}
