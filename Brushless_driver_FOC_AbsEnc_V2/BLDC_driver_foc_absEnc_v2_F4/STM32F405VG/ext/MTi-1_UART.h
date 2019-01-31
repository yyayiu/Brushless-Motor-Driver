#ifndef MTI_1_UART__H
#define	MTI_1_UART__H

/**
** MTi-1_UART Library for STM32F4 Eco robot
** Device: MTi-3-8A7G6-DK
** @Author James Mok, Simon Tam
** @Note   We think no body will read the code so its kinda messy but nevermind :)
**/

#include "stm32f4xx.h"
#include "uart.h"
#include "stm32f4xx_gpio.h"
#include "ticks.h"

//UART setting
#define MTi_1_UART								USART1
#define MTi_1_COM									COM1
#define MTi_1_UART_Tx_Pin					GPIO_Pin_9
#define	MTi_1_UART_Rx_Pin					GPIO_Pin_10
#define MTi_1_UART_RTS_Pin				GPIO_Pin_12
#define MTi_1_UART_GPIO						GPIOA
#define MTi_1_UART_RCC						RCC_APB2Periph_USART1
#define MTi_1_UART_GPIO_RCC				RCC_AHB1Periph_GPIOA
#define MTi_1_default_BR					(uint32_t)115200

extern float MTi_acc[3];

typedef enum
{
	MTi_Data_Ready 			= 1,
	MTi_Data_Not_Ready 	= 0
}MTi_Data_Statue;
	
typedef enum
{
	rx_NA 			= 0,
	rx_Preamble = 1,
	rx_BusID 		= 2,
	rx_MsgID 		= 3,
	rx_Length 	= 4,
	rx_Payload 	= 5,
	rx_Checksum = 6
}MTi_Rx_Statue;

/**
	Initialize the MTi-3.
	Notice: If there are no readings, probably the init function is called before the MTi is running.
	Add delay at start up if needed.
*/
void MTi_1_UART_init(void);

/**
	Reset the MTi-3, including orientation.
*/
void MTi_1_reset(void);

//Return the acceleration. 0=x 1=y 2=z
float get_MTi_acc(uint8_t index);

//Return the euler angle. 0=yaw 1=pitch 2=roll
float get_MTi_ang(uint8_t index);
#endif
