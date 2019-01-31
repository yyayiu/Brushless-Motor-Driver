/**
	A gpio library for easier usage of GPIO ports
	Refreshed in F4 by Rex Cheng
*/

#include "gpio.h"

const GPIO 	
			/*** GPIOA ***/
			PA0 = {GPIOA, GPIO_Pin_0},
			PA1 = {GPIOA, GPIO_Pin_1},
			PA2 = {GPIOA, GPIO_Pin_2},
			PA3 = {GPIOA, GPIO_Pin_3},
			PA4 = {GPIOA, GPIO_Pin_4},
			PA5 = {GPIOA, GPIO_Pin_5},
			PA6 = {GPIOA, GPIO_Pin_6},
			PA7 = {GPIOA, GPIO_Pin_7},
			PA8 = {GPIOA, GPIO_Pin_8},
			PA9 = {GPIOA, GPIO_Pin_9},
			PA10 = {GPIOA, GPIO_Pin_10},
			PA11 = {GPIOA, GPIO_Pin_11},
			PA12 = {GPIOA, GPIO_Pin_12},
			PA13 = {GPIOA, GPIO_Pin_13},
			PA14 = {GPIOA, GPIO_Pin_14},
			PA15 = {GPIOA, GPIO_Pin_15},

			/*** GPIOB ***/
			PB0 = {GPIOB, GPIO_Pin_0},
			PB1 = {GPIOB, GPIO_Pin_1},
			PB2 = {GPIOB, GPIO_Pin_2},
			PB3 = {GPIOB, GPIO_Pin_3},
			PB4 = {GPIOB, GPIO_Pin_4},
			PB5 = {GPIOB, GPIO_Pin_5},
			PB6 = {GPIOB, GPIO_Pin_6},
			PB7 = {GPIOB, GPIO_Pin_7},
			PB8 = {GPIOB, GPIO_Pin_8},
			PB9 = {GPIOB, GPIO_Pin_9},
			PB10 = {GPIOB, GPIO_Pin_10},
			PB11 = {GPIOB, GPIO_Pin_11},
			PB12 = {GPIOB, GPIO_Pin_12},
			PB13 = {GPIOB, GPIO_Pin_13},
			PB14 = {GPIOB, GPIO_Pin_14},
			PB15 = {GPIOB, GPIO_Pin_15},

			/*** GPIOC ***/
			PC0 = {GPIOC, GPIO_Pin_0},
			PC1 = {GPIOC, GPIO_Pin_1},
			PC2 = {GPIOC, GPIO_Pin_2},
			PC3 = {GPIOC, GPIO_Pin_3},
			PC4 = {GPIOC, GPIO_Pin_4},
			PC5 = {GPIOC, GPIO_Pin_5},
			PC6 = {GPIOC, GPIO_Pin_6},
			PC7 = {GPIOC, GPIO_Pin_7},
			PC8 = {GPIOC, GPIO_Pin_8},
			PC9 = {GPIOC, GPIO_Pin_9},
			PC10 = {GPIOC, GPIO_Pin_10},
			PC11 = {GPIOC, GPIO_Pin_11},
			PC12 = {GPIOC, GPIO_Pin_12},
			PC13 = {GPIOC, GPIO_Pin_13},
			PC14 = {GPIOC, GPIO_Pin_14},
			PC15 = {GPIOC, GPIO_Pin_15},
			
			/*** GPIOD ***/
			PD0 = {GPIOD, GPIO_Pin_0},
			PD1 = {GPIOD, GPIO_Pin_1},
			PD2 = {GPIOD, GPIO_Pin_2},
			PD3 = {GPIOD, GPIO_Pin_3},
			PD4 = {GPIOD, GPIO_Pin_4},
			PD5 = {GPIOD, GPIO_Pin_5},
			PD6 = {GPIOD, GPIO_Pin_6},
			PD7 = {GPIOD, GPIO_Pin_7},
			PD8 = {GPIOD, GPIO_Pin_8},
			PD9 = {GPIOD, GPIO_Pin_9},
			PD10 = {GPIOD, GPIO_Pin_10},
			PD11 = {GPIOD, GPIO_Pin_11},
			PD12 = {GPIOD, GPIO_Pin_12},
			PD13 = {GPIOD, GPIO_Pin_13},
			PD14 = {GPIOD, GPIO_Pin_14},
			PD15 = {GPIOD, GPIO_Pin_15},

			/*** GPIOE ***/
			PE0 = {GPIOE, GPIO_Pin_0},
			PE1 = {GPIOE, GPIO_Pin_1},
			PE2 = {GPIOE, GPIO_Pin_2},
			PE3 = {GPIOE, GPIO_Pin_3},
			PE4 = {GPIOE, GPIO_Pin_4},
			PE5 = {GPIOE, GPIO_Pin_5},
			PE6 = {GPIOE, GPIO_Pin_6},
			PE7 = {GPIOE, GPIO_Pin_7},
			PE8 = {GPIOE, GPIO_Pin_8},
			PE9 = {GPIOE, GPIO_Pin_9},
			PE10 = {GPIOE, GPIO_Pin_10},
			PE11 = {GPIOE, GPIO_Pin_11},
			PE12 = {GPIOE, GPIO_Pin_12},
			PE13 = {GPIOE, GPIO_Pin_13},
			PE14 = {GPIOE, GPIO_Pin_14},
			PE15 = {GPIOE, GPIO_Pin_15},

			/*** GPIOF ***/
			PF0 = {GPIOF, GPIO_Pin_0},
			PF1 = {GPIOF, GPIO_Pin_1},
			PF2 = {GPIOF, GPIO_Pin_2},
			PF3 = {GPIOF, GPIO_Pin_3},
			PF4 = {GPIOF, GPIO_Pin_4},
			PF5 = {GPIOF, GPIO_Pin_5},
			PF6 = {GPIOF, GPIO_Pin_6},
			PF7 = {GPIOF, GPIO_Pin_7},
			PF8 = {GPIOF, GPIO_Pin_8},
			PF9 = {GPIOF, GPIO_Pin_9},
			PF10 = {GPIOF, GPIO_Pin_10},
			PF11 = {GPIOF, GPIO_Pin_11},
			PF12 = {GPIOF, GPIO_Pin_12},
			PF13 = {GPIOF, GPIO_Pin_13},
			PF14 = {GPIOF, GPIO_Pin_14},
			PF15 = {GPIOF, GPIO_Pin_15},

			/*** GPIOG ***/
			PG0 = {GPIOG, GPIO_Pin_0},
			PG1 = {GPIOG, GPIO_Pin_1},
			PG2 = {GPIOG, GPIO_Pin_2},
			PG3 = {GPIOG, GPIO_Pin_3},
			PG4 = {GPIOG, GPIO_Pin_4},
			PG5 = {GPIOG, GPIO_Pin_5},
			PG6 = {GPIOG, GPIO_Pin_6},
			PG7 = {GPIOG, GPIO_Pin_7},
			PG8 = {GPIOG, GPIO_Pin_8},
			PG9 = {GPIOG, GPIO_Pin_9},
			PG10 = {GPIOG, GPIO_Pin_10},
			PG11 = {GPIOG, GPIO_Pin_11},
			PG12 = {GPIOG, GPIO_Pin_12},
			PG13 = {GPIOG, GPIO_Pin_13},
			PG14 = {GPIOG, GPIO_Pin_14},
			PG15 = {GPIOG, GPIO_Pin_15}
			
			;

const GPIO* GPIOArray[] = {
	&PA0, &PA1, &PA2, &PA3, &PA4, &PA5, &PA6, &PA7, &PA8, &PA9, &PA10, &PA11, &PA12, &PA13, &PA14, &PA15,
	&PB0, &PB1, &PB2, &PB3, &PB4, &PB5, &PB6, &PB7, &PB8, &PB9, &PB10, &PB11, &PB12, &PB13, &PB14, &PB15,
	&PC0, &PC1, &PC2, &PC3, &PC4, &PC5, &PC6, &PC7, &PC8, &PC9, &PC10, &PC11, &PC12, &PC13, &PC14, &PC15,
	&PD0, &PD1, &PD2, &PD3, &PD4, &PD5, &PD6, &PD7, &PD8, &PD9, &PD10, &PD11, &PD12, &PD13, &PD14, &PD15,
	&PE0, &PE1, &PE2, &PE3, &PE4, &PE5, &PE6, &PE7, &PE8, &PE9, &PE10, &PE11, &PE12, &PE13, &PE14, &PE15,
	&PF0, &PF1, &PF2, &PF3, &PF4, &PF5, &PF6, &PF7, &PF8, &PF9, &PF10, &PF11, &PF12, &PF13, &PF14, &PF15,
	&PG0, &PG1, &PG2, &PG3, &PG4, &PG5, &PG6, &PG7, &PG8, &PG9, &PG10, &PG11, &PG12, &PG13, &PG14, &PG15};
			
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
void gpio_init(const GPIO* gpio, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pp_type){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = gpio->gpio_pin;
	GPIO_InitStructure.GPIO_Speed = speed;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_OType = output_type;
	GPIO_InitStructure.GPIO_PuPd = pp_type;
	
	GPIO_Init(gpio->gpio, &GPIO_InitStructure);
}

/**
	General input GPIO initailizer
	Usage: @ref gpio_init
	Output type and speed does not matter to input gpio
*/
void gpio_input_init(const GPIO* gpio, GPIOPuPd_TypeDef pp_type){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = gpio->gpio_pin;
	GPIO_InitStructure.GPIO_PuPd = pp_type;
	
	GPIO_Init(gpio->gpio, &GPIO_InitStructure);
}

/**
	General output GPIO initailizer
	Usage: @ref gpio_init
	Speed is fixed to GPIO_Medium_Speed which should be sufficient.
*/
void gpio_output_init(const GPIO* gpio, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pp_type){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; 
	GPIO_InitStructure.GPIO_Pin = gpio->gpio_pin;
	GPIO_InitStructure.GPIO_OType = output_type;
	GPIO_InitStructure.GPIO_PuPd = pp_type;
	
	GPIO_Init(gpio->gpio, &GPIO_InitStructure);
}

/**
	Alternate function GPIO initailizer
	Usage: @ref gpio_init
*/
void gpio_af_init(const GPIO* gpio, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pp_type, uint8_t GPIO_AF){
	assert_param(IS_GPIO_AF(GPIO_AF));
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = gpio->gpio_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = output_type;
	GPIO_InitStructure.GPIO_PuPd = pp_type;
	
	GPIO_PinAFConfig(gpio->gpio, getPinSource(gpio), GPIO_AF);
	GPIO_Init(gpio->gpio, &GPIO_InitStructure);
}

/**
	Initilize RCC clock for all GPIO ports
*/
void gpio_rcc_init_all(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
}

/**
	* @brief GPIO Real-time Clock Initialization
	* @param GPIO pointer
	*/
void gpio_rcc_init(const GPIO* gpio){
	switch ((uint32_t) gpio->gpio) {
		
		case ((uint32_t)GPIOA):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		break;
		
		case ((uint32_t)GPIOB):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		break;
		
		case ((uint32_t)GPIOC):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		break;
		
		case ((uint32_t)GPIOD):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		break;
		
		case ((uint32_t)GPIOE):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		break;
		
		case ((uint32_t)GPIOF):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		break;
		
		case ((uint32_t)GPIOG):
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
		break;
	}
}

/**
	* @brief Read GPIO input value
	* @param GPIO pointer
	* @retval The GPIO Pin input value
	*/
uint8_t gpio_read_input(const GPIO* gpio){
	return GPIO_ReadInputDataBit(gpio->gpio, gpio->gpio_pin);
}

/**
	* @brief Read GPIO output value
	* @param GPIO pointer
	* @retval The GPIO Pin output value
	*/
uint8_t gpio_read_output(const GPIO* gpio){
	return GPIO_ReadOutputDataBit(gpio->gpio, gpio->gpio_pin);
}

/**
	* @brief Write GPIO value
	* @param GPIO pointer
	*/
void gpio_write(const GPIO* gpio, BitAction BitVal){
	GPIO_WriteBit(gpio->gpio, gpio->gpio_pin, BitVal);
}

/**
	* @brief Toggle GPIO
	* @param GPIO pointer
	*/
void gpio_toggle(const GPIO* gpio) {
	GPIO_WriteBit(gpio->gpio, gpio->gpio_pin, (BitAction) !GPIO_ReadOutputDataBit(gpio->gpio, gpio->gpio_pin));
}

uint16_t getPinSource(const GPIO* gpio){
	switch(gpio->gpio_pin){
		case GPIO_Pin_0:
			return GPIO_PinSource0;
		case GPIO_Pin_1:
			return GPIO_PinSource1;
		case GPIO_Pin_2:
			return GPIO_PinSource2;
		case GPIO_Pin_3:
			return GPIO_PinSource3;
		case GPIO_Pin_4:
			return GPIO_PinSource4;
		case GPIO_Pin_5:
			return GPIO_PinSource5;
		case GPIO_Pin_6:
			return GPIO_PinSource6;
		case GPIO_Pin_7:
			return GPIO_PinSource7;
		case GPIO_Pin_8:
			return GPIO_PinSource8;
		case GPIO_Pin_9:
			return GPIO_PinSource9;
		case GPIO_Pin_10:
			return GPIO_PinSource10;
		case GPIO_Pin_11:
			return GPIO_PinSource11;
		case GPIO_Pin_12:
			return GPIO_PinSource12;
		case GPIO_Pin_13:
			return GPIO_PinSource13;
		case GPIO_Pin_14:
			return GPIO_PinSource14;
		default:
			return GPIO_PinSource15;
	}
}
