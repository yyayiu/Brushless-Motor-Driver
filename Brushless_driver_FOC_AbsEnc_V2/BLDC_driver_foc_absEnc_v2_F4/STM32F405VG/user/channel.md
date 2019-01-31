SysTick 
	- Ticks increment

Nested Vector Interrupt Controller Priority:

	- Preemption Priority 0
		1 - Encoder overflow/underflow interrupt + External interrupt line 8 & 9
		3 - CAN 1 TX complete interrupt
		4 - CAN 2 TX complete interrupt
		5 - Ticks

	- Preemption Priority 1
		1 - UART interrupt
		7 - CAN1 RX interrupt
		8 - CAN2 RX interrupt
		
	- Preemption Priority 2
		4 - Timer event
	
DMA Channel:

	- DMA 1
		Stream 3 - Channel 4 - USART3 TX
		Stream 4 - Channel 4 - USART4 TX
		Stream 6 - Channel 4 - USART2 TX
		Stream 7 - Channel 4 - UART5 TX
	- DMA 2
		Stream 7 - Channel 4 - USART1 TX
		
Timer:
	-Timer 2
		-Channel 1 - Ticks

	-Timer 3
		- Channel 1 - Encoder 2 A phase
		- Channel 2 - Encoder 2 B phase
		
	-Timer 4
		- Channel 1-4 - Servo 1-4

	-Timer 7 (Basic timer)
		-Handling timer event
		
	-Timer 11
		- Channel 1 - Servo 5
		
	-Timer 13
		-Channel 1 - Servo 6
		
		
SPI:
	- SPI 1 - TFT
	
External interrupt:
	- Line 8 - Encoder 1 A Phase (PC8)
	- Line 9 - Encoder 2 B Phase (PC9)
	
CAN:
	- CAN1
		- 0x0B0 ~ 0x0BF (CAN Motor)
		- 0x0D0 ~ 0x0DF (Gungnir CAN Motor Command)
		- 0x0E0 ~ 0x0EF (Gungnir CAN Motor Feedback)
	