#include "MTi-1_UART.h"

//Op_code
#define ProtocolInfo							(uint8_t)0x01
#define ConfigureProtocol					(uint8_t)0x02
#define ControlPipe								(uint8_t)0x03
#define PipeStatus								(uint8_t)0x04
#define NotificationPipe					(uint8_t)0x05
#define MeasurementPipe 					(uint8_t)0x06

#define	MTi_1_Preamble						(uint8_t)0xFA
#define MTi_1_MasterDevice				(uint8_t)0xFF

//MID
#define	WakeUp										(uint8_t)0x3E
#define WakeUpAck									(uint8_t)0x3F
#define	Reset											(uint8_t)0x40
#define InitMT										(uint8_t)0x02
#define InitMTResults 						(uint8_t)0x03
#define	GoToConfig								(uint8_t)0x30
#define	GoToMeasurement						(uint8_t)0x10
#define ReqDataLength							(uint8_t)0x0A
#define	DataLength								(uint8_t)0x0B
#define Error											(uint8_t)0x42
#define ReqOutputMode							(uint8_t)0xD0
#define	SetOutputMode							(uint8_t)0xD0
#define	ReqOutputSettings					(uint8_t)0xD2
#define SetOutputSettings					(uint8_t)0xD2
#define	ReqData										(uint8_t)0x34
#define MTData										(uint8_t)0x32
#define MTData2										(uint8_t)0x36
#define ReqConfiguration					(uint8_t)0x0C
#define SetOptionFlags 						(uint8_t)0x48
#define ResetOrientation					(uint8_t)0xA4

void send_MTi_1_UART_msg(uint8_t *data, uint8_t MID, uint16_t data_length);
void MTi_1_UART_Rx(uint8_t data);
uint8_t get_ebuffer(uint8_t index);
void clear_buffer(void);
float flt_cal(uint8_t data[4]);

float MTi_ang[3] = {0, 0, 0};
float MTi_acc[3] = {0, 0, 0};

uint8_t MTi_msg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t raw_buffer[4];

/**
**@descript Init function
**/
void MTi_1_UART_init(void)
{
	uart_init(MTi_1_COM, MTi_1_default_BR);
	uart_interrupt_init(MTi_1_COM, MTi_1_UART_Rx);
	RCC_AHB2PeriphClockCmd(MTi_1_UART_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(MTi_1_UART_GPIO_RCC, ENABLE);
	
//	GPIO_InitTypeDef GPIOInitStructure;
//	GPIOInitStructure.GPIO_Pin = MTi_1_UART_RTS_Pin;
//	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//	GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIOInitStructure.GPIO_Speed = GPIO_Fast_Speed;
//	GPIO_Init(MTi_1_UART_GPIO, &GPIOInitStructure);
	
	clear_buffer();
	send_MTi_1_UART_msg(NULL, GoToMeasurement, 0);
	//Setup the x,y,z axis
	MTi_msg[1] = 3;
	send_MTi_1_UART_msg(&MTi_msg[0], ResetOrientation,2);
	MTi_msg[1] = 1;
	send_MTi_1_UART_msg(&MTi_msg[0], ResetOrientation,2);
	MTi_msg[1] = 0;
}

void MTi_1_reset(){
	clear_buffer();
	send_MTi_1_UART_msg(NULL, GoToMeasurement, 0);
	//Setup the x,y,z axis
	MTi_msg[1] = 3;
	send_MTi_1_UART_msg(&MTi_msg[0], ResetOrientation,2);
	MTi_msg[1] = 1;
	send_MTi_1_UART_msg(&MTi_msg[0], ResetOrientation,2);
	MTi_msg[1] = 0;
}

/**
**	@descript Construct the message to the IMU. See MT Datasheet for details
**	@para 		*data: the data going the send
**  	@para			*MID: the message header
**	@para			*data_length: no of bytes of the data
**	
**/
void send_MTi_1_UART_msg(uint8_t *data, uint8_t mid, uint16_t data_length)
{
	while(GPIO_ReadInputDataBit(MTi_1_UART_GPIO, MTi_1_UART_RTS_Pin) != Bit_RESET);
		
	uint8_t temp[data_length + 5];
	uint8_t checksum = 0;
	
	temp[0] = MTi_1_Preamble;
	temp[1] = MTi_1_MasterDevice;
	temp[2] = mid;
	temp[3] = data_length;
	
	for(uint8_t i=4; i<4+data_length; i++)
	{
		temp[i] = *(data + i - 4);
	}
	
	for(uint8_t i=1; i<data_length + 4; i++)
		checksum += temp[i];
	
	checksum = 0xFF - checksum + 0x01;
	temp[data_length + 4] = checksum;
	
	for(uint8_t k=0; k<(data_length + 5); k++)
	{
		uart_tx_byte_blocking(MTi_1_COM, temp[k]);
	}
}


/**
**@descript the UART listener used for the interrupt
**@para 		*data: the Rx received data
**/
void MTi_1_UART_Rx(uint8_t data)
{
	static uint16_t data_length 	= 5;	
	static uint8_t  ANG_COUNT 		= 0;	//counter for receiving Pdata
	static uint8_t  ACC_COUNT 		= 0;
	static uint8_t  MID = 0;
	static uint16_t header_count = 0;	//count the header
  static uint16_t rx_count 		= 0;	//incremented by one when RX_interrupt occoured
	rx_count++;	
	
	if(data == MTi_1_Preamble && rx_count == 1) //The 1st byte == MTi_1_Preamble
		header_count++;
	else if(data == MTi_1_MasterDevice && rx_count == 2) //The 2nd byte == MTi_1_MasterDevice
		header_count++;
	else if(rx_count == 3 && header_count == 2) //The 3rd byte == MID
	{
		header_count++;
		MID = data;
	}
	else if(header_count == 3 && MID == MTData2)	//The receive msg is data
	{
		if(rx_count == 4) //The 4th byte = data length
			data_length = data;
		
		if(rx_count == 7 && data == 0x0C)
		{
			ANG_COUNT = 12;
		}
		
		if(ANG_COUNT && rx_count >= 8) //The 8-19th byte = x,y,z angle
		{
			if(ANG_COUNT > 8)
			{
				raw_buffer[(12-ANG_COUNT)%4] = data; 
				ANG_COUNT--;
				if(ANG_COUNT == 8)
				{
					MTi_ang[0] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
			else if(ANG_COUNT > 4 && ANG_COUNT <= 8)
			{
				raw_buffer[(12-ANG_COUNT)%4] = data;
				ANG_COUNT--;
				if(ANG_COUNT == 4)
				{
					MTi_ang[1] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
			else if(ANG_COUNT>0 && ANG_COUNT<=4)
			{
				raw_buffer[(12-ANG_COUNT)%4] = data;
				ANG_COUNT--;
				if(!ANG_COUNT)
				{
					ACC_COUNT = 12;
					MTi_ang[2] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
		}
		if(rx_count >= 23 && ACC_COUNT) //The 23-34th byte = x,y,z acceleration
		{
			if(ACC_COUNT > 8)
			{
				raw_buffer[(12-ACC_COUNT)%4] = data;
				ACC_COUNT--;
				if(ACC_COUNT == 8)
				{
					MTi_acc[0] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
			else if(ACC_COUNT > 4 && ACC_COUNT <= 8)
			{
				raw_buffer[(12-ACC_COUNT)%4] = data;
				ACC_COUNT--;
				if(ACC_COUNT == 4)
				{
					MTi_acc[1] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
			else if(ACC_COUNT > 0 && ACC_COUNT <= 4)
			{
				raw_buffer[(12-ACC_COUNT)%4] = data;
				ACC_COUNT--;
				if(!ACC_COUNT)
				{
					MTi_acc[2] = flt_cal(raw_buffer);
					clear_buffer();
				}
			}
		}
	}
	else
	{
		rx_count = 0;
		header_count = 0;
	}
	
	if(rx_count == data_length + 4)
	{
		rx_count = 0;
		header_count = 0;
	}
	
}	
	
/**
**@descript Reset the buffer to zero
**/
void clear_buffer(void){
	raw_buffer[0] = 0;
	raw_buffer[1] = 0;
	raw_buffer[2] = 0;
	raw_buffer[3] = 0;
}

/**
**@descript Translate a 4-byte array to a floating point number 
**@para  		*data[4]: the 4-byte array
**@retval 	float value of the array according to the IEEE-745 standard
**/
float flt_cal(uint8_t data[4]){
	uint8_t e;
	int temp;
	int i;
	float result;
	e = (data[0]<<1)|(data[1]>>7);
	temp = (0x800000|data[1]<<16)|(data[2]<<8)|data[3];
	result = (float)temp;
	temp = 127-e+23;
	if(temp>0)for(i=0;i<temp;i++)result=result/2.0f;
	else for(i=0;i>temp;i--)result = result * 2.0f;
	return data[0]&0x80?-result:result;
}


float get_MTi_ang(uint8_t index)
{
	return MTi_ang[index];
}

float get_MTi_acc(uint8_t index)
{
	return MTi_acc[index];
}

#undef ProtocolInfo						
#undef ConfigureProtocol			
#undef ControlPipe							
#undef PipeStatus					
#undef NotificationPipe			
#undef MeasurementPipe 			
#undef	MTi_1_Preamble		
#undef MTi_1_MasterDevice	
#undef	WakeUp									
#undef WakeUpAck					
#undef	Reset								
#undef InitMT						
#undef InitMTResults 		
#undef	GoToConfig				
#undef	GoToMeasurement					
#undef ReqDataLength					
#undef	DataLength					
#undef Error								
#undef ReqOutputMode					
#undef	SetOutputMode					
#undef	ReqOutputSettings			
#undef SetOutputSettings			
#undef	ReqData								
#undef MTData			
#undef MTData2				
#undef ReqConfiguration	
#undef SetOptionFlags 
#undef ResetOrientation	
