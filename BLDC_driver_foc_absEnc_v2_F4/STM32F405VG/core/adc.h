#ifndef _ADC_H
#define _ADC_H

#include "stm32f4xx_adc.h"
#include "gpio.h"

//The adc must be in sequence, i.e. ADC1->ADC2->...
#define ADC_TABLE \
X(ADC1, RCC_APB2Periph_ADC1, DMA2, DMA2_Stream0, DMA_Channel_0, RCC_AHB1Periph_DMA2)\
X(ADC3, RCC_APB2Periph_ADC3, DMA2, DMA2_Stream1, DMA_Channel_2, RCC_AHB1Periph_DMA2) 

#define ADC_PORT_TABLE \
X(ADC_PORT_5, PC0, ADC1, ADC_Channel_10) \
X(ADC_PORT_6, PC1, ADC1, ADC_Channel_11) \
X(ADC_PORT_7, PC2, ADC1, ADC_Channel_12) \
X(ADC_PORT_8, PC3, ADC1, ADC_Channel_13) \
X(ADC_PORT_9, PC4, ADC1, ADC_Channel_14) \
X(ADC_PORT_10, PC5, ADC1, ADC_Channel_15) \
X(ADC_PORT_1, PF3, ADC3, ADC_Channel_9) \
X(ADC_PORT_2, PF4, ADC3, ADC_Channel_14) \
X(ADC_PORT_3, PF5, ADC3, ADC_Channel_15) \
X(ADC_PORT_4, PF6, ADC3, ADC_Channel_4) 

#define X(a, b, c, d) a, 
typedef enum{
	TEMPERATURE_ADC,
	VOLTAGE_ADC,
	ADC_PORT_TABLE
} AdcID;
#undef X

typedef struct{
	const GPIO* gpio;
	ADC_TypeDef* adc;
	uint8_t channel;
}AdcPortStruct;

typedef struct{
	ADC_TypeDef* adc;
	uint32_t rcc;
	DMA_TypeDef* dma;
	DMA_Stream_TypeDef* stream;
	uint32_t channel;
	uint32_t dma_rcc;
}AdcStruct;

#define X(a, b, c, d) {&b, c, d}, 
static const AdcPortStruct ADCPorts[] = {ADC_PORT_TABLE};
#undef X

#define X(a, b, c, d, e, f) {a, b, c, d, e, f}, 
static const AdcStruct ADCs[] = {ADC_TABLE};
#undef X

#define ADC_PORT_COUNT (sizeof(ADCPorts)/sizeof(AdcPortStruct) + 2) //+2 because temperature and voltage sensor
#define ADC_COUNT (sizeof(ADCs)/sizeof(AdcStruct))

/** 
* Initalize all adc ports, and start taking readings automatically
*/
void adc_init(void);

/** Get the latest adc reading
** @return Unsigned numerical reading representing voltage level
*/
uint16_t get_adc(AdcID id);

/**
* The following functions can monitor the MCU
*/

#define TEMP_ADC_AT_25 760 //mV
#define TEMP_SLOPE 250 // mV/C, *100

/** Get the temperature value
* @return Temperature value (x10)
*/
int16_t get_mcu_temp(void);
#endif
