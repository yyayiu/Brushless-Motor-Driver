#ifndef _UART_H
#define _UART_H

/************************************************************************************************************
** UART - STM32F4
**
** This library provides both blocking and non-blocking functions for sending and receiving uart data.
** Non-blocking transmission is implementated with a queue, dequeue occurs in interrupt
** Non-blocking reception should be implementated with message handler (provided by the user)
** The non-blocking transmission is stored in the buffer array, and its size can be defined below.
** If the buffer is full, oldest data will be overwritten and not a single f will be given.
**
** ROBOCON 2017
** H K U S T
**
** Author:	Rex Cheng
** Contact:	hkchengad@connect.ust.hk
**
** v1.0 Jan 2017
** v1.1 Mar 2017
** v1.2 May 2017
**
** "Do not answer! Do not answer! Do not answer!
** There are tens of millions of stars in your direction. As long as you do not answer, 
** this world will not be able to ascertain the source of your transmission.
** But if you do answer, the source will be located right away. Your planet will be invaded. Your world will be conquered!
** Do not answer! Do not answer!! Do not answer!!!"
** 		~Three Body
*************************************************************************************************************/

#include "stm32f4xx_usart.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "gpio.h"

//COM   UART    TX   RX    RCC                 RCC line     AF              Interrupt 
#define UART_TABLE \
X(COM1, USART1, PA9, PA10, RCC_APB2Periph_USART1, 2, GPIO_AF_USART1, USART1_IRQn) \
X(COM2, USART2, PA2, PA3, RCC_APB1Periph_USART2, 1, GPIO_AF_USART2, USART2_IRQn) \
X(COM3, USART3, PB10, PB11, RCC_APB1Periph_USART3, 1, GPIO_AF_USART3, USART3_IRQn) \
X(COM4, UART4, PA0, PA1, RCC_APB1Periph_UART4, 1, GPIO_AF_UART4, UART4_IRQn) \
X(COM5, UART5, PC12, PD2, RCC_APB1Periph_UART5, 1, GPIO_AF_UART5, UART5_IRQn) \
X(COM6, USART6, PG14, PG9, RCC_APB2Periph_USART6, 2, GPIO_AF_USART6, USART6_IRQn)

#define X(a, b, c, d, e, f, g, h) a,
typedef enum {
	UART_TABLE
} SerialPort;
#undef X

typedef struct{
	USART_TypeDef* uart;
	const GPIO* tx_port;
	const GPIO* rx_port;
	const uint32_t rcc;
	const uint8_t rcc_line;
	const uint8_t af;
	const uint16_t irq;
} UARTStruct;

#define X(a, b, c, d, e, f, g, h) {b, &c, &d, e, f, g, h},
static const UARTStruct UARTPorts[] = {UART_TABLE};
#undef X

#define COM_COUNT (sizeof(UARTPorts)/sizeof(UARTStruct))

#define UART1_TX_BUFFER_MAX 64
#define UART2_TX_BUFFER_MAX 768
#define UART3_TX_BUFFER_MAX 64
#define UART4_TX_BUFFER_MAX 64
#define UART5_TX_BUFFER_MAX 64
#define UART6_TX_BUFFER_MAX 64
typedef struct{
	volatile uint16_t head;
	volatile uint16_t tail;
	uint8_t* queue;
}UartQueue;

typedef void (*OnRxListener)(const uint8_t byte);

/** Init a UART port.
*		@param COM: Which port to initialize
*		@param baud_rate: The baud rate to be used.
*/
void uart_init(SerialPort COM, uint32_t baud_rate);

/** Register a listener for UART receive interrupt.
*		@param COM: Which port to use
*		@param listener: A function pointer of void return type and single uint8_t param
*/
void uart_interrupt_init(SerialPort COM, OnRxListener listener);


/**
* The followings are blocking TX functions.
*/


/** Send a single byte to the target port. Blocking.
*		@param COM: Which port to use
*		@param data: The content to be sent
*/
void uart_tx_byte_blocking(SerialPort COM, uint8_t data);

/** Send multiple bytes to the target port. Blocking.
*		@param COM: Which port to use
*		@param data: The content to be sent
*/
void uart_tx_blocking(SerialPort COM, const char * data, ...);

/** Send an array of data to the target port. Blocking.
*		@param COM: Which port to use
*		@param data: The pointer to the first element
*		@param len: Length of the array (in bytes)
*/
void uart_tx_array_blocking(SerialPort COM, const char * data, uint16_t len);


/**
* The followings are NON-blocking TX functions.
*/


/** Send a single byte to the target port. Non-Blocking.
*		@param COM: Which port to use
*		@param data: The content to be sent
*/
void uart_tx_byte(SerialPort COM, uint8_t data);

/** Send multiple bytes to the target port. Non-Blocking.
*		@param COM: Which port to use
*		@param data: The content to be sent
*/
void uart_tx(SerialPort COM, const char * data, ...);

/** Send an array of data to the target port. Non-Blocking.
*		@param COM: Which port to use
*		@param data: The pointer to the first element
*		@param len: Length of the array (in bytes)
*/
void uart_tx_array(SerialPort COM, const uint8_t * data, uint16_t len);

/** Get the current size of the TX buffer
* @param COM: Which port to use
* @return the size
*/
uint16_t get_uart_queue_size(SerialPort COM);


/**
* The followings are RX functions.
*/


/** Block the program until one byte is received.
*	@param COM: Which port to use
*	@return The received byte
*/
uint8_t uart_rx_byte(SerialPort COM);

#endif
