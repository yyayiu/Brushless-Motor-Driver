#ifndef	_GPIO_H
#define	_GPIO_H

/**
	A gpio library for easier usage of GPIO ports
	Refreshed in F4 by Rex Cheng
*/

#include "stm32f4xx_gpio.h"
#include <stdbool.h>

typedef struct {
	GPIO_TypeDef *gpio;
	uint16_t gpio_pin;
} GPIO;

extern const GPIO 
	PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
	PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
	PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
	PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
	PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
	PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15,
	PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG7, PG8, PG9, PG10, PG11, PG12, PG13, PG14, PG15;


extern const GPIO* GPIOArray[];
#define GPIO_SIZE (7*16)

/** Get the PinSourse from gpio
* @param A pointer to a GPIO const
* @return GPIO_PinSourceX
*/
uint16_t getPinSource(const GPIO* gpio);

/**
	Complete GPIO Pin initailizer
	@param gpio: A pointer to a gpio port, like &PE0
	
	@param mode: Operating mode with type @ref GPIOMode_TypeDef
		@ref GPIO_Mode_IN   		= 0x00 		< GPIO Input Mode 
		@ref GPIO_Mode_OUT  		= 0x01 		< GPIO Output Mode 
		@ref GPIO_Mode_AF   		= 0x02 		< GPIO Alternate function Mode 
		@ref GPIO_Mode_AN   		= 0x03 		< GPIO Analog Mode 
			
	@param speed: GPIO speed with type @ref GPIOSpeed_TypeDef
		@ref GPIO_Low_Speed     	= 0x00 		< legacy = GPIO_Speed_2MHz
		@ref GPIO_Medium_Speed  	= 0x01 		< legacy = GPIO_Speed_25MHz
		@ref GPIO_Fast_Speed    	= 0x02 		< legacy = GPIO_Speed_50MHz
		@ref GPIO_High_Speed    	= 0x03 		< legacy = GPIO_Speed_100MHz
			
	@param output_type: Operating output type with type @ref GPIOOType_TypeDef
		@ref GPIO_OType_PP 		= 0x00		< Push-Pull
		@ref GPIO_OType_OD 		= 0x01		< Open-Drain
			
	@param pp_type: Specify the operating pull-up/pull-down with type @ref GPIOPuPd_TypeDef
		@ref GPIO_PuPd_NOPULL 	= 0x00		< Nothing
		@ref GPIO_PuPd_UP     	= 0x01		< Pull up
		@ref GPIO_PuPd_DOWN   	= 0x02		< Pull down
*/
void gpio_init(const GPIO* gpio, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pp_type);

/** General input GPIO initailizer
	Usage: @ref gpio_init
	Output type and speed does not matter to input gpio
*/
void gpio_input_init(const GPIO* gpio, GPIOPuPd_TypeDef pp_type);

/** General output GPIO initailizer
	Usage: @ref gpio_init
	Speed is fixed to GPIO_Medium_Speed which should be sufficient.
*/
void gpio_output_init(const GPIO* gpio, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pp_type);

/**
	Alternate function GPIO initailizer
	Usage: @ref gpio_init
*/
void gpio_af_init(const GPIO* gpio, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pp_type, uint8_t GPIO_AF);

/**
	Initilize RCC clock for all GPIO ports
*/
void gpio_rcc_init_all(void);

/** @brief GPIO Real-time Clock Initialization
	* @param GPIO pointer
	*/
void gpio_rcc_init(const GPIO* gpio);

/** @brief Read GPIO input value
	* @param GPIO pointer
	* @retval The GPIO Pin input value
	*/
uint8_t gpio_read_input(const GPIO* gpio);

/** @brief Read GPIO output value
	* @param GPIO pointer
	* @retval The GPIO Pin output value
	*/
uint8_t gpio_read_output(const GPIO* gpio);

/** @brief Write GPIO value
	* @param GPIO pointer
	*/
void gpio_write(const GPIO* gpio, BitAction BitVal);

/** @brief Toggle GPIO
	* @param GPIO pointer
	*/
void gpio_toggle(const GPIO* gpio);

#endif
