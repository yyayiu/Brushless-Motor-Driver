/**
  ******************************************************************************
  * @file    can_protocol.c
  * @author  Kenneth Au (v1.0.0)
	* @modify	 Ding & Simon (v1.1.0)
	* @modify  Rex Cheng (v1.2.x)
  * @version V1.2.1
  * @date    Mar 2016
  * @brief   This file provides all the CAN basic protocol functions, including
	* 				 initialization, CAN transmission and receive handlers.
  ******************************************************************************
	
	Only use Mailbox 1 to transmit and 2 FIFO to receive.
	
	Performace: Stable until ~100KB/s, more than that, some packets would be lost.
	If the network is longer/larger, use lower rate.
	
	CAN1 is used for motor control.
	CAN2 is used for other communication.
**/
	
#include <can_protocol.h>

static CanMessage CAN1_tx_queue_items[CAN1_TX_QUEUE_MAX_SIZE];
static CanMessage CAN2_tx_queue_items[CAN2_TX_QUEUE_MAX_SIZE];

static void can1_tx_dequeue(void);
static void can2_tx_dequeue(void);
static void (*can_tx_dequeue[2])(void) = {can1_tx_dequeue, can2_tx_dequeue};

CanQueue CAN_tx_queue[2] = {{0, 0, CAN1_tx_queue_items}, {0, 0, CAN2_tx_queue_items}};

static uint8_t CAN_filter_count[2] = {0};
static uint8_t CAN_max_filter[2] = {CAN1_FILTER_LIMIT, CAN2_FILTER_LIMIT};
static uint32_t CAN_queue_max_size[2] = {CAN1_TX_QUEUE_MAX_SIZE, CAN2_TX_QUEUE_MAX_SIZE};

// Array storing all the handler functions for CAN RX (element id equals to filter id)
CanRxHandler CAN_Rx_Handlers[CAN_RX_FILTER_LIMIT] = {0};

//Init both CAN1 and CAN2
void can_init(){
	//Enable RCC
	RCC_APB1PeriphClockCmd(CAN1_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(CAN2_RCC, ENABLE);
	gpio_rcc_init(&CAN1_RX_GPIO);
	gpio_rcc_init(&CAN2_RX_GPIO);
	gpio_rcc_init(&CAN1_TX_GPIO);
	gpio_rcc_init(&CAN2_TX_GPIO);

	//Init GPIO AF
	gpio_af_init(&CAN1_RX_GPIO, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF_CAN1);
	gpio_af_init(&CAN1_TX_GPIO, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF_CAN1);
	gpio_af_init(&CAN2_RX_GPIO, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF_CAN2);
	gpio_af_init(&CAN2_TX_GPIO, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF_CAN2);

	CAN_InitTypeDef CAN_InitStructure;
	//CAN1 init
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;	//ENABLE = DISABLE auto resend
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE; //ENABLE = FIFO Mailbox
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	
	//Config to 1Mbps
	//Baud Rate = Clock / ( (BS1+BS2+1) *Prescaler)
	//Sample time = 83.3%
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;
	CAN_InitStructure.CAN_Prescaler = 6;
	while (CAN_Init(CAN1, &CAN_InitStructure) != CAN_InitStatus_Success);
	
	//CAN2 init
	CAN_DeInit(CAN2);
	CAN_StructInit(&CAN_InitStructure);
	
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;	//ENABLE = DISABLE auto resend
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE; //ENABLE = FIFO Mailbox
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	
	//Config to 1Mbps
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;
	CAN_InitStructure.CAN_Prescaler = 6;
	while (CAN_Init(CAN2, &CAN_InitStructure) != CAN_InitStatus_Success);
	
	//Enable mailbox empty interrupt
	CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
	CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);
	
	//NVIC init for CAN1 TX
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel= CAN1_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//NVIC init for CAN2 TX
	NVIC_InitStructure.NVIC_IRQChannel= CAN2_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/** Return the queue size of CAN1/CAN2
* @param id: which CAN queue to look at
* @return size of the specified queue
*/
uint16_t get_can_queue_size(CanID id){
	return(CAN_tx_queue[id].tail - CAN_tx_queue[id].head + CAN_queue_max_size[id]) % CAN_queue_max_size[id];
}

/** Process one CAN message in the queue for CAN1.
* Called automatically.
*/
static void can1_tx_dequeue(){
	//Transmit when the first mailbox is empty
	if ((get_can_queue_size(CAN_1)!=0) && ((CAN1->TSR & CAN_TSR_TME0) == CAN_TSR_TME0)) {
		#define msg CAN_tx_queue[CAN_1].queue[CAN_tx_queue[CAN_1].head]

		/* Set up the Id */
		CAN1->sTxMailBox[0].TIR &= (uint32_t)0x00000001;
		CAN1->sTxMailBox[0].TIR |= ((msg.id << 21) | CAN_RTR_DATA);
		
		CAN1->sTxMailBox[0].TDTR &= (uint32_t)0xFFFFFFF0;
		CAN1->sTxMailBox[0].TDTR |= msg.length;

		/* Set up the data field */
		CAN1->sTxMailBox[0].TDLR = (((uint32_t)msg.data[3] << 24) | 
																						 ((uint32_t)msg.data[2] << 16) |
																						 ((uint32_t)msg.data[1] << 8) | 
																						 ((uint32_t)msg.data[0]));
		CAN1->sTxMailBox[0].TDHR = (((uint32_t)msg.data[7] << 24) | 
																						 ((uint32_t)msg.data[6] << 16) |
																						 ((uint32_t)msg.data[5] << 8) |
																						 ((uint32_t)msg.data[4]));
		/* Request transmission */
		CAN1->sTxMailBox[0].TIR |= (uint32_t)0x00000001;
		
		CAN_tx_queue[CAN_1].head = (CAN_tx_queue[CAN_1].head + 1) % CAN1_TX_QUEUE_MAX_SIZE;
		#undef msg
	}
}

/** Process one CAN message in the queue for CAN1.
* Called automatically.
*/
static void can2_tx_dequeue(){
	//Transmit when the first mailbox is empty
	if ((get_can_queue_size(CAN_2)!=0) && ((CAN2->TSR & CAN_TSR_TME0) == CAN_TSR_TME0)) {
		#define msg CAN_tx_queue[CAN_2].queue[CAN_tx_queue[CAN_2].head]

		/* Set up the Id */
		CAN2->sTxMailBox[0].TIR &= (uint32_t)0x00000001;
		CAN2->sTxMailBox[0].TIR |= ((msg.id << 21) | CAN_RTR_DATA);
		
		CAN2->sTxMailBox[0].TDTR &= (uint32_t)0xFFFFFFF0;
		CAN2->sTxMailBox[0].TDTR |= msg.length;

		/* Set up the data field */
		CAN2->sTxMailBox[0].TDLR = (((uint32_t)msg.data[3] << 24) | 
																						 ((uint32_t)msg.data[2] << 16) |
																						 ((uint32_t)msg.data[1] << 8) | 
																						 ((uint32_t)msg.data[0]));
		CAN2->sTxMailBox[0].TDHR = (((uint32_t)msg.data[7] << 24) | 
																						 ((uint32_t)msg.data[6] << 16) |
																						 ((uint32_t)msg.data[5] << 8) |
																						 ((uint32_t)msg.data[4]));
		/* Request transmission */
		CAN2->sTxMailBox[0].TIR |= (uint32_t)0x00000001;
		
		CAN_tx_queue[CAN_2].head = (CAN_tx_queue[CAN_2].head + 1) % CAN2_TX_QUEUE_MAX_SIZE;
		#undef msg
	}
}

/** Put a CAN message into the CAN queue for transmission
* @param id: which CAN to use
* @param msg: The message to be sent
* @return true if successful
*/
bool can_tx_enqueue(CanID id, CanMessage msg){
	if (get_can_queue_size(id) == CAN_queue_max_size[id]-1){
		return false;
	}

	CAN_tx_queue[id].queue[CAN_tx_queue[id].tail] = msg;
	CAN_tx_queue[id].tail = (CAN_tx_queue[id].tail + 1) % CAN_queue_max_size[id];

	can_tx_dequeue[id]();
	return true;
}

// Reset the entire CAN TX message queue
void can_tx_queue_clear(CanID id){
	CAN_tx_queue[id].head = CAN_tx_queue[id].tail = 0;
}


//TX complete handler for CAN1
void CAN1_TX_IRQHandler(void){
	if (CAN_GetITStatus(CAN1, CAN_IT_TME) != RESET){
		// If all the mailboxes are empty
		CAN_ClearITPendingBit(CAN1, CAN_IT_TME);
		can_tx_dequeue[0]();
	}
}

//TX complete handler for CAN2
void CAN2_TX_IRQHandler(){
	if (CAN_GetITStatus(CAN2, CAN_IT_TME) != RESET){
		// If all the mailboxes are empty
		CAN_ClearITPendingBit(CAN2, CAN_IT_TME);
		can_tx_dequeue[1]();
	}
}

// Initialize CAN RX
void can_rx_init(){
	//CAN interrupt FIFO interrupt
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN1, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);

	//NVIC init
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	//Init both FIFO interrupt for CAN1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	//Init both FIFO interrupt for CAN2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	//Init filter bank
	CAN_SlaveStartBank(CAN1_FILTER_LIMIT);
}

/** Add a mask filter to receive some messages
* @warning Cannot exceed filter size limit
* @param id: 11-bit ID (0x000 to 0x7FF)
* @param mask: 11-bit mask, corresponding to the 11-bit ID (0x000 to 0x7FF)
* @param FIFO_num: 0 or 1, to select which FIFO will receive the message, each CAN has 2 FIFO
* @param CANx: which CAN to use
* @param handler: Function to handle the received message
* @example Please read the mask exmaple in the header file
*/
void can_rx_add_filter(uint16_t id, uint16_t mask, uint8_t FIFO_num, CanID CANx, CanRxHandler handler){
	if (CAN_filter_count[CANx] >= CAN_max_filter[CANx]){
		//Error
		while(1);
	}
	
	uint8_t filter_id = CAN_filter_count[CANx];
	if (CANx == CAN_2){
		filter_id += CAN1_FILTER_LIMIT;
	}
	
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	
	CAN_FilterInitStructure.CAN_FilterNumber = filter_id;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	
	CAN_FilterInitStructure.CAN_FilterIdHigh = id << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = mask << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = FIFO_num;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);	
	
	CAN_Rx_Handlers[filter_id] = handler;
	
	CAN_filter_count[CANx]++;
}

//Interrupt functions for CAN1/CAN2, FIFO1/FIFO2

void CAN1_RX0_IRQHandler(){
	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET) {
		CanRxMsg RxMessage;
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);

		if(RxMessage.IDE == CAN_ID_STD) {
			if (CAN_Rx_Handlers[RxMessage.FMI] != 0 && RxMessage.FMI < CAN_filter_count[CAN_1]) {
				CAN_Rx_Handlers[RxMessage.FMI](&RxMessage);
			}
		}
	}
}

void CAN1_RX1_IRQHandler(){
	if (CAN_GetITStatus(CAN1, CAN_IT_FMP1) != RESET) {
		CanRxMsg RxMessage;
		CAN_Receive(CAN1, CAN_FIFO1, &RxMessage);
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP1);

		if(RxMessage.IDE == CAN_ID_STD) {
			if (CAN_Rx_Handlers[RxMessage.FMI] != 0 && RxMessage.FMI < CAN_filter_count[CAN_1]) {
				CAN_Rx_Handlers[RxMessage.FMI](&RxMessage);
			}
		}
	}
}

void CAN2_RX0_IRQHandler(){
	if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET) {
		CanRxMsg RxMessage;
		CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);

		if(RxMessage.IDE == CAN_ID_STD) {
			if (CAN_Rx_Handlers[RxMessage.FMI + CAN1_FILTER_LIMIT] != 0 && RxMessage.FMI < (CAN_filter_count[CAN_2] + CAN1_FILTER_LIMIT)) {
				CAN_Rx_Handlers[RxMessage.FMI + CAN1_FILTER_LIMIT](&RxMessage);
			}
		}
	}
}

void CAN2_RX1_IRQHandler(){
	if (CAN_GetITStatus(CAN2, CAN_IT_FMP1) != RESET) {
		CanRxMsg RxMessage;
		CAN_Receive(CAN2, CAN_FIFO1, &RxMessage);
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);

		if(RxMessage.IDE == CAN_ID_STD) {
			if (CAN_Rx_Handlers[RxMessage.FMI + CAN1_FILTER_LIMIT] != 0 && RxMessage.FMI < (CAN_filter_count[CAN_2] + CAN1_FILTER_LIMIT)) {
				CAN_Rx_Handlers[RxMessage.FMI + CAN1_FILTER_LIMIT](&RxMessage);
			}
		}
	}
}

/** Get the receive error count
* @param id: Which CAN to look at
* @return The number of receive error occured
*/
uint8_t get_can_rx_err_cnt(CanID id){
	if (id == CAN_1){
		return CAN_GetReceiveErrorCounter(CAN1);
	}else{
		return CAN_GetReceiveErrorCounter(CAN2);
	}
}

/** Get the transmit error count
* @param id: Which CAN to look at
* @return The number of transmit error occured
*/
uint8_t get_can_tx_err_cnt(CanID id){
	if (id == CAN_1){
		return CAN_GetLSBTransmitErrorCounter(CAN1);
	}else{
		return CAN_GetLSBTransmitErrorCounter(CAN2);
	}
}

/*** Protocol Encoding / Decoding function ***/
/**
	* @brief Convert one n-byte variable to n one-byte variable (ENCODE)
	* @param n: the nth byte 
	* @param num: the nth byte number (can be unsigned)
	* @retval The nth byte variable
	*/
uint8_t one_to_n_bytes(int32_t num, uint8_t n){
	assert_param(n >= 0 && n <= 3);
	return (n == 0) ? (num & 0xFF) : (one_to_n_bytes(num >> 8, n-1));
}

/**
	* @brief Convert n one-byte variable to an array of n bytes (DECODE)
	* @param n: the number of bytes
	* @param array: the array of n bytes
	*/
int32_t n_bytes_to_one(uint8_t* array, uint8_t n){
	assert_param(n >= 1 && n <= 4);
	return (n == 0) ? (array[0] & 0xFF) : ((array[0] & 0xFF) + (n_bytes_to_one(&array[1], n-1) << 8));
}
