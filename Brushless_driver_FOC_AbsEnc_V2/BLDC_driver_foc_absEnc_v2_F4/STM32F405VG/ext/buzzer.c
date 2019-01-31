#include "buzzer.h"

//Init the buzzer
void buzzer_init(){
	gpio_rcc_init(&BUZZER_GPIO);
	gpio_output_init(&BUZZER_GPIO, GPIO_OType_PP, GPIO_PuPd_DOWN);
}

//Turn on the buzzer
void buzzer_on(){
	gpio_write(&BUZZER_GPIO, Bit_SET);
}

//Turn off the buzzer
void buzzer_off(){
	gpio_write(&BUZZER_GPIO, Bit_RESET);
}
