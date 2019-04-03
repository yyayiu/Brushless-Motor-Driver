#include "gpio_test.h"

static uint8_t is_tft_pin(const GPIO* gpio){
	return (gpio == &PA4 || gpio == &PA5 || gpio == &PA6 ||gpio == &PA7 ||gpio == &PA8);
}

//Start the test
void gpio_test(){
	gpio_rcc_init_all();
	
	uint8_t ok = 1;
	
	for (uint32_t curr = START_TEST_PIN; curr<END_TEST_PIN; curr++){
		if (is_tft_pin(GPIOArray[curr])) continue;
		
		gpio_output_init(GPIOArray[curr], GPIO_OType_OD, GPIO_PuPd_DOWN);
		for (uint32_t other = START_TEST_PIN; other<END_TEST_PIN; other++){
			if (is_tft_pin(GPIOArray[other])) continue;
			if (curr == other){
				continue;
			}
			gpio_input_init(GPIOArray[other], GPIO_PuPd_UP);
		}
		
		_delay_ms(5);
		
		for (uint32_t other = START_TEST_PIN; other<END_TEST_PIN; other++){
			if (is_tft_pin(GPIOArray[other])) continue;
			if (curr == other){
				continue;
			}
			tft_clear();
			tft_println("%d", get_ticks());
			tft_println("Current output: ");
			tft_println("Port: %c", (curr/15 + 65));
			tft_println("Pin:  %d", curr%15);
			tft_println("");
			tft_println("Current input: ");
			tft_println("Port: %c", (other/15 + 65));
			tft_println("Pin:  %d", other%15);
			
			uint8_t result = gpio_read_input(GPIOArray[other]);
			tft_println("Result: %d", result);
			tft_update();
			
			if (result == 0){
				while(!btn_pressed(BUTTON_1));
				while(btn_pressed(BUTTON_1));
				ok = 0;
			}
		}
	}
	
	tft_clear();
	if (ok == 1){
		tft_println("OK LA");
		tft_println("GOOD GOOD");
	}else{
		tft_println("NOT OK AH");
	}
	tft_update();
	
	while(!btn_pressed(BUTTON_1));
	while(btn_pressed(BUTTON_1));
}
