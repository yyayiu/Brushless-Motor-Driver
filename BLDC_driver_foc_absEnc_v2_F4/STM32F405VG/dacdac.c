#include "dacdac.h"


const u16 sin1[7][64]= {{892,911,929,948,965,983,999,1014,1028,1040,1052,1061,1069,1076,1080,1083,1084,1083,1080,1076,1069,1061,1052,1040,1028,1014,999,983,965,948,929,911,892,873,855,836,819,801,785,770,756,744,732,723,715,708,704,701,700,701,704,708,715,723,732,744,756,770,785,801,819,836,855,873},{1020,1051,1082,1113,1142,1171,1198,1223,1246,1267,1286,1302,1316,1326,1334,1338,1340,1338,1334,1326,1316,1302,1286,1267,1246,1223,1198,1171,1142,1113,1082,1051,1020,989,958,927,898,869,842,817,794,773,754,738,724,714,706,702,700,702,706,714,724,738,754,773,794,817,842,869,898,927,958,989},{1148,1192,1235,1278,1319,1359,1397,1432,1465,1494,1520,1543,1562,1577,1587,1594,1596,1594,1587,1577,1562,1543,1520,1494,1465,1432,1397,1359,1319,1278,1235,1192,1148,1104,1061,1018,977,937,899,864,831,802,776,753,734,719,709,702,700,702,709,719,734,753,776,802,831,864,899,937,977,1018,1061,1104},{1276,1332,1388,1443,1496,1548,1596,1641,1683,1721,1755,1784,1808,1827,1841,1849,1852,1849,1841,1827,1808,1784,1755,1721,1683,1641,1596,1548,1496,1443,1388,1332,1276,1220,1164,1109,1056,1004,956,911,869,831,797,768,744,725,711,703,700,703,711,725,744,768,797,831,869,911,956,1004,1056,1109,1164,1220},{1404,1473,1541,1608,1673,1736,1795,1851,1902,1948,1989,2025,2054,2078,2094,2105,2108,2105,2094,2078,2054,2025,1989,1948,1902,1851,1795,1736,1673,1608,1541,1473,1404,1335,1267,1200,1135,1072,1013,957,906,860,819,783,754,730,714,703,700,703,714,730,754,783,819,860,906,957,1013,1072,1135,1200,1267,1335},{1532,1614,1694,1774,1850,1924,1994,2060,2120,2175,2224,2266,2301,2328,2348,2360,2364,2360,2348,2328,2301,2266,2224,2175,2120,2060,1994,1924,1850,1774,1694,1614,1532,1450,1370,1290,1214,1140,1070,1004,944,889,840,798,763,736,716,704,700,704,716,736,763,798,840,889,944,1004,1070,1140,1214,1290,1370,1450},{1660,1754,1847,1939,2027,2113,2193,2269,2339,2402,2458,2507,2547,2579,2602,2615,2620,2615,2602,2579,2547,2507,2458,2402,2339,2269,2193,2113,2027,1939,1847,1754,1660,1566,1473,1381,1293,1207,1127,1051,981,918,862,813,773,741,718,705,700,705,718,741,773,813,862,918,981,1051,1127,1207,1293,1381,1473,1566},};
const u16 sin2[7][32]= {{342,379,415,449,478,502,519,530,534,530,519,502,478,449,415,379,342,305,269,235,206,182,165,154,150,154,165,182,206,235,269,305},{470,532,592,648,696,736,766,784,790,784,766,736,696,648,592,532,470,408,348,292,244,204,174,156,150,156,174,204,244,292,348,408},{598,685,769,847,915,970,1012,1037,1046,1037,1012,970,915,847,769,685,598,511,427,349,281,226,184,159,150,159,184,226,281,349,427,511},{726,838,946,1046,1133,1205,1258,1291,1302,1291,1258,1205,1133,1046,946,838,726,614,506,406,319,247,194,161,150,161,194,247,319,406,506,614},{854,991,1123,1245,1352,1439,1504,1544,1558,1544,1504,1439,1352,1245,1123,991,854,717,585,463,356,269,204,164,150,164,204,269,356,463,585,717},{982,1144,1300,1444,1570,1674,1751,1798,1814,1798,1751,1674,1570,1444,1300,1144,982,820,664,520,394,290,213,166,150,166,213,290,394,520,664,820},{1110,1297,1477,1643,1789,1908,1997,2052,2070,2052,1997,1908,1789,1643,1477,1297,1110,923,743,577,431,312,223,168,150,168,223,312,431,577,743,923},};
const u16 sin3[7][16]= {{342,415,478,519,534,519,478,415,342,269,206,165,150,165,206,269},{470,592,696,766,790,766,696,592,470,348,244,174,150,174,244,348},{598,769,915,1012,1046,1012,915,769,598,427,281,184,150,184,281,427},{726,946,1133,1258,1302,1258,1133,946,726,506,319,194,150,194,319,506},{854,1123,1352,1504,1558,1504,1352,1123,854,585,356,204,150,204,356,585},{982,1300,1570,1751,1814,1751,1570,1300,982,664,394,213,150,213,394,664},{1110,1477,1789,1997,2070,1997,1789,1477,1110,743,431,223,150,223,431,743},};

const u16* const sine[21] = {sin1[0],sin1[1],sin1[2],sin1[3],sin1[4],sin1[5],sin1[6],
sin2[0],sin2[1],sin2[2],sin2[3],sin2[4],sin2[5],sin2[6],
sin3[0],sin3[1],sin3[2],sin3[3],sin3[4],sin3[5],sin3[6]

};

u16 volt[7] = {384,640,896,1152,1408,1664,1920};

u8 volt_index = 0;

u16 frequency = 0;

GPIO_InitTypeDef GPIO_InitStructure;



void DAC_TIM4_Config(u16 period)
{
        
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_DeInit(TIM4);
     
// TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = period;        //100 => 13k 20=>62.5k 30=>42.37k  10=>120k      9=>130
  TIM_TimeBaseStructure.TIM_Prescaler = 0;                                                               
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;                                                   
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;         
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);

  TIM_Cmd(TIM4, ENABLE);	
}
									
void dac_init(u16 freq,u8 level){
	
	
	frequency = freq;
	
	GPIO_InitTypeDef GPIO_InitStructure;
  DAC_InitTypeDef  DAC_InitStructure;


  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);      
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
         
	//  calculation for period and voltage for timer and 
	u16 period = 0;
	if(level>6){level=6;}
	volt_index = level;
	if(freq<50){freq=50;}
	if(freq>400){freq=400;}
	
	u8 i = 0;
	period = 1280/freq;
	while(period<10){
		i++;
		period = 1280*(0x01<<i)/freq;
	}

	//
	DAC_TIM4_Config(period);
	
	
		/* Enable DAC clock */
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	/* Enable DMA1 clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T4_TRGO;    
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;        
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;       
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);        

	DAC_SetChannel1Data(DAC_Align_12b_R, 0x0000);
	DAC_Cmd(DAC_Channel_1, ENABLE);
	//DAC_DMACmd(DAC_Channel_1, ENABLE);

	
	
	DMA_InitTypeDef DMA_InitStruct;
	
	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)(sine[i*7+level]);
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStruct.DMA_BufferSize = 64/(0x01<<i);
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
	
	
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1;
		
			/* Disable DMA */
	DMA_DeInit(DMA1_Stream5);
			
			/* Set channel used */
	DMA_InitStruct.DMA_Channel = DMA_Channel_7;
		
			/* Initialize DMA */
	DMA_Init(DMA1_Stream5, &DMA_InitStruct);
			
			/* Enable DMA Stream for DAC Channel 1 */
	DMA_Cmd(DMA1_Stream5, ENABLE);

			/* Enable DAC Channel 1 */
	DAC_Cmd(DAC_Channel_1, ENABLE);

			/* Enable DMA for DAC Channel 1 */
	DAC_DMACmd(DAC_Channel_1, ENABLE);
	
	
	TIM4->CR1 |= TIM_CR1_CEN;
}

void DAC_enable_init(void){
	// PA5,6,7

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);      
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	DAC_enable(ALL_DISABLE);
}



//0x_ _
// first_ is the resistor choice
// second_ is the DAC phase choice

// MODE_10|DAC_A

void DAC_enable(u8 channel){
		switch(channel&0xF0){
		case MODE_10:{ // 10R
				switch(channel&0x0F){
					case DAC_A:{
						GPIOC->ODR = ((GPIOC->ODR)&(0xC307))|(0x0008);
						break;
					}
					case DAC_B:{
						GPIOC->ODR = ((GPIOC->ODR)&(0xC307))|(0x0010);
						break;
					}
					case DAC_C:{
						GPIOC->ODR = ((GPIOC->ODR)&(0xC307))|(0x0020);
						break;
					}
					case DAC_DISABLE:{
						GPIOC->ODR = (GPIOC->ODR)&(0xC307);	
						break;
					}
					
				}
				break;
			}	
			case MODE_100:{ // 100R
				switch(channel&0x0F){
					case DAC_A:{
						GPIOC->ODR = ((GPIOC->ODR)&(0xC307))|(0x0040);
						break;
					}
					case DAC_B:{
						GPIOC->ODR = ((GPIOC->ODR)&(0xC307))|(0x0080);
						break;
					}
					case DAC_C:{
						GPIOC->ODR = ((GPIOC->ODR)&(0xC307))|(0x0400);				
						break;
					}
					case DAC_DISABLE:{
						GPIOC->ODR = (GPIOC->ODR)&(0xC307);					
						break;
					}
					
				}
				break;
			}
			case MODE_1K:{ //1K
				switch(channel&0x0F){
					case DAC_A:{
						GPIOC->ODR = ((GPIOC->ODR)&(0xC307))|(0x0800);
						break;
					}
					case DAC_B:{
						GPIOC->ODR = ((GPIOC->ODR)&(0xC307))|(0x1000);
						break;
					}
					case DAC_C:{
						GPIOC->ODR = ((GPIOC->ODR)&(0xC307))|(0x2000);				
						break;
					}
					case DAC_DISABLE:{
						GPIOC->ODR = (GPIOC->ODR)&(0xC307);				
						break;
					}
					
				}
				break;
			}
			default:{
				
				GPIOC->ODR = (GPIOC->ODR)&(0xC307);
				break;
			}
		}
}


bool DAC_on(void){
	if(GPIOC->ODR&0x3CF8){
		return true;
	}
	return false;
}





void FET_GPIO_init(void){
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);      
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);      
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9|GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void FET_gnd(u16 input){
	FET_GPIO_init();
	
	switch(input){
		case (FET_A):{
			GPIOA->ODR = (GPIOA->ODR&0xF9FF)|0x0400;
			GPIOC->ODR = (GPIOC->ODR&0xFDFF);
		  break;
		}
		case (FET_B):{
			GPIOA->ODR = (GPIOA->ODR&0xF9FF)|0x0200;
			GPIOC->ODR = (GPIOC->ODR&0xFDFF);
			break;
		}
		case (FET_C):{
			GPIOA->ODR = (GPIOA->ODR&0xF9FF);
			GPIOC->ODR = (GPIOC->ODR&0xFDFF)|0x0200;
			break;			
		}
		default:{
			GPIOA->ODR = GPIOA->ODR&0xF9FF;
			GPIOC->ODR = GPIOC->ODR&0xFDFF;
			break;
		}
	}
	

	
}

u16 get_freq(void){
	return frequency;
}

u32 get_voltage(void){
	return (u32)volt[volt_index];
}

u16 get_interval(void){
	return 818/frequency;
}