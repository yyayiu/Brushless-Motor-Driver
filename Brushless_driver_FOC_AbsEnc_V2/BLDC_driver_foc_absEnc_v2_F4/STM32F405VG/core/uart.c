#include "uart.h"

static uint8_t tx_buf_item_1[UART1_TX_BUFFER_MAX] = {0};
static uint8_t tx_buf_item_2[UART2_TX_BUFFER_MAX] = {0};
static uint8_t tx_buf_item_3[UART3_TX_BUFFER_MAX] = {0};
static uint8_t tx_buf_item_4[UART4_TX_BUFFER_MAX] = {0};
static uint8_t tx_buf_item_5[UART5_TX_BUFFER_MAX] = {0};
static uint8_t tx_buf_item_6[UART6_TX_BUFFER_MAX] = {0};

static UartQueue tx_queue[COM_COUNT] = {{0, 0, tx_buf_item_1}, 
																				{0, 0, tx_buf_item_2},
																				{0, 0, tx_buf_item_3},
																				{0, 0, tx_buf_item_4},
																				{0, 0, tx_buf_item_5},
																				{0, 0, tx_buf_item_6}};
static uint32_t tx_buf_max_size[COM_COUNT] = {UART1_TX_BUFFER_MAX, UART2_TX_BUFFER_MAX, UART3_TX_BUFFER_MAX,
																				UART4_TX_BUFFER_MAX, UART5_TX_BUFFER_MAX, UART6_TX_BUFFER_MAX};
																				
static OnRxListener rxListeners[COM_COUNT] = {0};

/** Init a UART port.
*		@param COM: Which port to initialize
*		@param baud_rate: The baud rate to be used.
*/
void uart_init(SerialPort COM, uint32_t br){
	USART_InitTypeDef USART_InitStructure;
	
	const UARTStruct* uart = &UARTPorts[COM];
	
	gpio_rcc_init(uart->tx_port);
	gpio_rcc_init(uart->rx_port);
	
	if (uart->rcc_line == 1){
		RCC_APB1PeriphClockCmd(uart->rcc, ENABLE);
		
	}else if(uart->rcc_line == 2){
		RCC_APB2PeriphClockCmd(uart->rcc, ENABLE);
		
	}else{
		//Error
		while(1);
	}
	
	gpio_af_init(uart->tx_port, GPIO_OType_PP, GPIO_PuPd_UP, uart->af);
	gpio_af_init(uart->rx_port, GPIO_OType_PP, GPIO_PuPd_UP, uart->af);
	
	/* USART configuration */
	USART_InitStructure.USART_BaudRate = br;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(UARTPorts[COM].uart, &USART_InitStructure);
	USART_Cmd(UARTPorts[COM].uart, ENABLE);
	
	//Setup NVIC for uart interrupt
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = UARTPorts[COM].irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//Init TC and RXNE interrupt
	USART_ITConfig(UARTPorts[COM].uart, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UARTPorts[COM].uart, USART_IT_TC, ENABLE);
}

/** Register a listener for UART receive interrupt.
*		@param COM: Which port to use
*		@param listener: A function pointer of void return type and single uint8_t param
*/
void uart_interrupt_init(SerialPort COM, OnRxListener listener){
	rxListeners[COM] = listener;
}


/** Send a single byte to the target port. Blocking.
*		@param COM: Which port to use
*		@param data: The content to be sent
*/
__INLINE void uart_tx_byte_blocking(SerialPort COM, uint8_t data){
	while (USART_GetFlagStatus(UARTPorts[COM].uart, USART_FLAG_TXE) == RESET);
	UARTPorts[COM].uart->DR = data;
}

/** Send multiple bytes to the target port. Blocking.
*		@param COM: Which port to use
*		@param data: The content to be sent
*/
void uart_tx_blocking(SerialPort COM, const char * data, ...){
	va_list arglist;
	uint8_t buf[255], *fp;
	
	va_start(arglist, data);
	vsprintf((char*)buf, (const char*)data, arglist);
	va_end(arglist);
	
	fp = buf;
	while (*fp){
		uart_tx_byte_blocking(COM, *fp++);
	}
}

/** Send an array of data to the target port. Blocking.
*		@param COM: Which port to use
*		@param data: The pointer to the first element
*		@param len: Length of the array (in bytes)
*/
void uart_tx_array_blocking(SerialPort COM, const char * data, u16 len){
	while(len--){
		uart_tx_byte_blocking(COM, *data);
		data++;
	}
}

/** Get the current size of the TX buffer
* @param COM: Which port to use
* @return the size
*/
uint16_t get_uart_queue_size(SerialPort COM){
	return (tx_queue[COM].tail - tx_queue[COM].head + tx_buf_max_size[COM]) % tx_buf_max_size[COM];
}

/** Try to dequeue and transmit a btye
*		@param COM: Which port to use
*/
__INLINE static void uart_tx_dequeue(SerialPort COM){
	if (USART_GetFlagStatus(UARTPorts[COM].uart, USART_FLAG_TXE) == SET){
		if (get_uart_queue_size(COM) > 0){
			//Put into the data register
			UARTPorts[COM].uart->DR = tx_queue[COM].queue[tx_queue[COM].head];
			
			//Dequeue
			tx_queue[COM].head = (tx_queue[COM].head + 1) % tx_buf_max_size[COM];
		}
	}
}

/** Enqueue a byte, ready to be transmitted
*		@param COM: Which port to use
*/
__INLINE static void uart_tx_enqueue(SerialPort COM, uint8_t data){
	tx_queue[COM].queue[tx_queue[COM].tail] = data;
	tx_queue[COM].tail = (tx_queue[COM].tail + 1) % tx_buf_max_size[COM];

	uart_tx_dequeue(COM);
}


/** Send a single byte to the target port. Non-Blocking.
*		@param COM: Which port to use
*		@param data: The content to be sent
*/
__INLINE void uart_tx_byte(SerialPort COM, uint8_t data){
	if (USART_GetFlagStatus(UARTPorts[COM].uart, USART_FLAG_TXE) == RESET && get_uart_queue_size(COM) == 0){
		uart_tx_byte_blocking(COM, data);
	}else{
		uart_tx_enqueue(COM, data);
	}
}

/** Send multiple bytes to the target port. Non-Blocking.
*		@param COM: Which port to use
*		@param data: The content to be sent
*/
void uart_tx(SerialPort COM, const char * data, ...){
	va_list arglist;
	uint8_t buf[255], *fp;
	
	va_start(arglist, data);
	vsprintf((char*)buf, data, arglist);
	va_end(arglist);
	
	fp = buf;
	while (*fp){
		uart_tx_enqueue(COM, *fp++);
	}
}

/** Send an array of data to the target port. Non-Blocking.
*		@param COM: Which port to use
*		@param data: The pointer to the first element
*		@param len: Length of the array (in bytes)
*/
void uart_tx_array(SerialPort COM, const uint8_t * data, u16 len){
	while(len--){
		uart_tx_enqueue(COM, *data++);
	}
}

/** Block the program until one byte is received.
*	@param COM: Which port to use
*	@return The received byte
*/
uint8_t uart_rx_byte(SerialPort COM){
	while (USART_GetFlagStatus(UARTPorts[COM].uart, USART_FLAG_RXNE) == RESET); 
	return (uint8_t)USART_ReceiveData(UARTPorts[COM].uart); 
}

//Implementing all those IRQ handlers here

#define COM_PORT COM1
void USART1_IRQHandler(void){
	if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_RXNE) == SET){
		//Handle receive interrupt
		if (rxListeners[COM_PORT] != 0){
			(rxListeners[COM_PORT])(USART_ReceiveData(UARTPorts[COM_PORT].uart));
		}
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_RXNE);
		
	}else if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_TC) == SET){
		//Handle transmit interrupt
		uart_tx_dequeue(COM_PORT);
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_TC);
	}
	UARTPorts[COM_PORT].uart->SR;
	UARTPorts[COM_PORT].uart->DR;
}
#undef COM_PORT

#define COM_PORT COM2
void USART2_IRQHandler(void){
	if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_RXNE) == SET){
		//Handle receive interrupt
		if (rxListeners[COM_PORT] != 0){
			(rxListeners[COM_PORT])(USART_ReceiveData(UARTPorts[COM_PORT].uart));
		}
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_RXNE);
		
	}else if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_TC) == SET){
		//Handle transmit interrupt
		uart_tx_dequeue(COM_PORT);
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_TC);
	}
	UARTPorts[COM_PORT].uart->SR;
	UARTPorts[COM_PORT].uart->DR;
}
#undef COM_PORT

#define COM_PORT COM3
void USART3_IRQHandler(void){
	if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_RXNE) == SET){
		//Handle receive interrupt
		if (rxListeners[COM_PORT] != 0){
			(rxListeners[COM_PORT])(USART_ReceiveData(UARTPorts[COM_PORT].uart));
		}
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_RXNE);
		
	}else if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_TC) == SET){
		//Handle transmit interrupt
		uart_tx_dequeue(COM_PORT);
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_TC);
	}
	UARTPorts[COM_PORT].uart->SR;
	UARTPorts[COM_PORT].uart->DR;
}
#undef COM_PORT

#define COM_PORT COM4
void UART4_IRQHandler(void){
	if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_RXNE) == SET){
		//Handle receive interrupt
		if (rxListeners[COM_PORT] != 0){
			(rxListeners[COM_PORT])(USART_ReceiveData(UARTPorts[COM_PORT].uart));
		}
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_RXNE);
		
	}else if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_TC) == SET){
		//Handle transmit interrupt
		uart_tx_dequeue(COM_PORT);
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_TC);
	}
	UARTPorts[COM_PORT].uart->SR;
	UARTPorts[COM_PORT].uart->DR;
}
#undef COM_PORT

#define COM_PORT COM5
void UART5_IRQHandler(void){
	if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_RXNE) == SET){
		//Handle receive interrupt
		if (rxListeners[COM_PORT] != 0){
			(rxListeners[COM_PORT])(USART_ReceiveData(UARTPorts[COM_PORT].uart));
		}
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_RXNE);
		
	}else if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_TC) == SET){
		//Handle transmit interrupt
		uart_tx_dequeue(COM_PORT);
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_TC);
	}
	UARTPorts[COM_PORT].uart->SR;
	UARTPorts[COM_PORT].uart->DR;
}
#undef COM_PORT

#define COM_PORT COM6
void USART6_IRQHandler(void){
	if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_RXNE) == SET){
		//Handle receive interrupt
		if (rxListeners[COM_PORT] != 0){
			(rxListeners[COM_PORT])(USART_ReceiveData(UARTPorts[COM_PORT].uart));
		}
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_RXNE);
		
	}else if(USART_GetITStatus(UARTPorts[COM_PORT].uart, USART_IT_TC) == SET){
		//Handle transmit interrupt
		uart_tx_dequeue(COM_PORT);
		USART_ClearITPendingBit(UARTPorts[COM_PORT].uart, USART_IT_TC);
	}
	UARTPorts[COM_PORT].uart->SR;
	UARTPorts[COM_PORT].uart->DR;
}
#undef COM_PORT

