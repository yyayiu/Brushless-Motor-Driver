#include "pneumatic.h"

/**
	Initialize all pneu ports.
*/
void pneu_init(){
	for (uint8_t i=0; i<PNEU_COUNT; i++){
		gpio_rcc_init(Pneus[i]);
		gpio_output_init(Pneus[i], GPIO_OType_PP, GPIO_PuPd_DOWN);
	}
}

/**
	Control pneu valve.
	@param id: the pneu to be controlled
	@param state: Bit_RESET/Bit_SET, where Bit_RESET refers to original(uncontrolled) state 
*/
void pneu_control(PneuID id, BitAction state){
	gpio_write(Pneus[id], state);
}

/**
	Toggle the pneu valve.
	@param id: the pneu to be controlled
*/
void pneu_toggle(PneuID id){
	gpio_toggle(Pneus[id]);
}
