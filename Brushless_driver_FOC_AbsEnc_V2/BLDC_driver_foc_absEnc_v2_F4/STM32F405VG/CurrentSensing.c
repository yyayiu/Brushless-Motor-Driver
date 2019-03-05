#include "CurrentSensing.h"
#include "uart.h"

static volatile uint16_t adc_current_reading[3] = {0};						//0		~	4095
static volatile uint16_t last_adc_current_reading[3] = {0};				//0		~	4095

u32 zero_mean[3] = {0};
void current_sensing_init(u32 init_ticks){
	
	//RCC init
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE); 
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
	
	//GPIO init
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
		GPIO_InitTypeDef GPIO_initStructure;
	
		GPIO_initStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_initStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_initStructure.GPIO_Speed = GPIO_High_Speed;
		GPIO_initStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_initStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_initStructure);
	
	//ADC common init
		ADC_CommonInitTypeDef ADC_CommonInitStruct;
	
		ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
		ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
		ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
		ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
		ADC_CommonInit(&ADC_CommonInitStruct);
		
	//ADC init
		ADC_InitTypeDef ADC_InitStructure;
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
		
		ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
		ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 2, ADC_SampleTime_3Cycles);
		ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 3, ADC_SampleTime_3Cycles);
		ADC_InitStructure.ADC_NbrOfConversion = 3;
		ADC_Init(ADC3, &ADC_InitStructure);
		
		DMA_InitStructure.DMA_Channel = DMA_Channel_2;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(ADC3)->DR;
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &(adc_current_reading[0]);
		DMA_InitStructure.DMA_BufferSize = 3;
		DMA_Init(DMA2_Stream1, &DMA_InitStructure);
		DMA_Cmd(DMA2_Stream1, ENABLE);
		
		ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
		ADC_DMACmd(ADC3, ENABLE);
		ADC_Cmd(ADC3, ENABLE);
		ADC_SoftwareStartConv(ADC3);
		
		while (get_ticks() - init_ticks < 5000);
		cal_zero_mean(zero_mean);
		
}
void cal_zero_mean(u32* zeromean){
	u16 count = 0;
	for(s16 i=-4000;i<8000;){
		if(last_adc_current_reading[0]!= adc_current_reading[0] && last_adc_current_reading[1]!= adc_current_reading[1] && last_adc_current_reading[2]!= adc_current_reading[2]){
				last_adc_current_reading[0] = adc_current_reading[0];
				last_adc_current_reading[1] = adc_current_reading[1];
				last_adc_current_reading[2] = adc_current_reading[2];
			if(i>=0){
				zeromean[0] += adc_current_reading[0];
				zeromean[1] += adc_current_reading[1];
				zeromean[2] += adc_current_reading[2];
				++count;
			}
			++i;
		}
	}
		zeromean[0] /= 8000;
		zeromean[1] /= 8000;
		zeromean[2] /= 8000;
	
	print_zero_mean(zeromean);
		
}

static s16 current_1000[3] = {0};								//-20000 ~ 20000	//store the current data *1000
void current_1000_update(){
	//Phase A
		current_1000[0] = adc_current_reading[0] - (s16)(zero_mean[0]);//*18.493;

	//Phase B
		current_1000[1] = adc_current_reading[1] - (s16)(zero_mean[1]);//*18.493;
		
	//Phase C
		current_1000[2] = adc_current_reading[2] - (s16)(zero_mean[2]);//*18.493;
	
}

s16 get_instant_current_A(void){
	return current_1000[0];
}

s16 get_instant_current_B(void){
	return current_1000[1];
}

s16 get_instant_current_C(void){
	return current_1000[2];
}

void print_zero_mean(u32* zeromean){
	uart_tx_blocking(COM3, "%% %d %d %d\n\n", zeromean[0], zeromean[1], zeromean[2]);
}

void abc_to_dq(u16 elec_angle, s16* a, s16* b, s16* c, s16* d, s16* q){
	s32 cos = app_cos(elec_angle*100);		//scaled by 32768
	s32 sin = app_sin(elec_angle*100);		//scaled by 32768
	
	s32 cos500 = (500)*cos/32768;					//calculated cos, 0.5*cos() , scaled by 1000
	s32 cos866 = (866)*cos/32768;					//calculated cos, (3)^0.5*0.5*cos() , scaled by 1000
	s32 sin500 = (500)*sin/32768;					//calculated sin, 0.5*sin() , scaled by 1000
	s32 sin866 = (866)*sin/32768;					//calculated sin, (3)^0.5*0.5*sin() , scaled by 1000
	
	*d =  cos*(*a)/32768 + (-cos500+sin866)*(*b)/1000 + (-cos500-sin866)*(*c)/1000;
	*q = -sin*(*a)/32768 + ( sin500+cos866)*(*b)/1000 + ( sin500-cos866)*(*c)/1000;	
	
//	uart_tx_blocking(COM3, "%d\n", elec_angle);
//	uart_tx_blocking(COM3, "%d %d %d\n",  cos, (-cos500+sin866), (-cos500-sin866));
//	uart_tx_blocking(COM3, "%d %d %d\n", -sin, ( sin500+sin866), ( sin500-sin866));
	
}

void dq_to_abc(u16 elec_angle, s16* a, s16* b, s16* c, s16* d, s16* q){
	s32 cos = app_cos(elec_angle*100);		//scaled by 32768
	s32 sin = app_sin(elec_angle*100);		//scaled by 32768
	
	s32 cos667 = (667)*cos/32768;					//calculated cos, (2/3)*cos() , scaled by 1000
	s32 cos333 = (333)*cos/32768;					//calculated cos, (1/3)*cos() , scaled by 1000
	s32 cos577 = (577)*cos/32768;					//calculated cos, (3^0.5/3)*cos() , scaled by 1000
	s32 sin667 = (667)*sin/32768;					//calculated cos, (2/3)*sin() , scaled by 1000
	s32 sin333 = (333)*sin/32768;					//calculated cos, (1/3)*sin() , scaled by 1000
	s32 sin577 = (577)*sin/32768;					//calculated cos, (3^0.5/3)*sin() , scaled by 1000
	
	*a = ( (				cos667)*(*d) + (-sin667				)*(*q) )/1000; 
	*b = ( ( sin577-cos333)*(*d) + ( sin333+cos577)*(*q) )/1000;
	*c = ( (-sin577-cos333)*(*d) + ( sin333-cos577)*(*q) )/1000;
	
}



