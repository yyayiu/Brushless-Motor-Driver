#include "adc.h"

static volatile uint16_t adc_reading[ADC_PORT_COUNT]; 


/** 
* Initalize all adc ports, and start taking readings automatically
*/
void adc_init(){
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_InitTypeDef ADC_InitStructure;
	
	//RCC init
	for (uint8_t i=0; i<ADC_COUNT; i++){
		RCC_APB2PeriphClockCmd(ADCs[i].rcc, ENABLE); 
		RCC_AHB1PeriphClockCmd(ADCs[i].dma_rcc, ENABLE);  
	}
	
	//GPIO init
	for (uint8_t i=0; i<ADC_PORT_COUNT-2; i++){
		gpio_rcc_init(ADCPorts[i].gpio);
		gpio_init(ADCPorts[i].gpio, GPIO_Mode_AN, GPIO_High_Speed, GPIO_OType_OD, GPIO_PuPd_NOPULL);
	}
	
	//ADC common init
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	//ADC init
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	DMA_InitTypeDef DMA_InitStructure; 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
	uint16_t index = 0;
	for (uint8_t i=0; i<ADC_COUNT; i++){
		uint8_t channel_count = 0;
		
		if (ADCs[i].adc == ADC1){
			ADC_TempSensorVrefintCmd(ENABLE);
			ADC_VBATCmd(ENABLE);
			
			//If Init ADC1, add two ADC channels -> Channel 16 for temperature, Channel 17 for Vref
			channel_count += 2;
			ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles);
			ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 2, ADC_SampleTime_480Cycles);
		}
		
		for (uint8_t j=0; j<ADC_PORT_COUNT-2; j++){
			if (ADCPorts[j].adc == ADCs[i].adc){
				channel_count++;
				//Channel init
				ADC_RegularChannelConfig(ADCPorts[j].adc, ADCPorts[j].channel, channel_count, ADC_SampleTime_480Cycles);
			}
		}
		
		ADC_InitStructure.ADC_NbrOfConversion = channel_count;
		ADC_Init(ADCs[i].adc, &ADC_InitStructure);
		
		DMA_InitStructure.DMA_Channel = ADCs[i].channel;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(ADCs[i].adc)->DR;
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &(adc_reading[index]);
		DMA_InitStructure.DMA_BufferSize = channel_count;
		DMA_Init(ADCs[i].stream, &DMA_InitStructure);
		DMA_Cmd(ADCs[i].stream, ENABLE);
		
		ADC_DMARequestAfterLastTransferCmd(ADCs[i].adc, ENABLE);
		ADC_DMACmd(ADCs[i].adc, ENABLE);
		ADC_Cmd(ADCs[i].adc, ENABLE);
		ADC_SoftwareStartConv(ADCs[i].adc);
		
		index += channel_count;
	}
}

/** Get the latest adc reading
** @return Unsigned numerical reading representing voltage level
*/
uint16_t get_adc(AdcID id){
	return adc_reading[id];
}


/** Get the temperature value
* @return Temperature value (x10)
*/
int16_t get_mcu_temp(){
	if (adc_reading[TEMPERATURE_ADC] == 0){
		return 0;
	}else{
		return (adc_reading[TEMPERATURE_ADC]*3300/4096-TEMP_ADC_AT_25)*100/TEMP_SLOPE + 250;
	}
}
