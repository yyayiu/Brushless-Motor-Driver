#include "gpio.h"
#include "spi_protocol.h"

static SpiListener * SpiRxListenerList[3] = {0}; 

u8 spi_get_id(SPI_TypeDef * port) {
	if (port == SPI1) return 0;
	if (port == SPI2) return 1;	
	if (port == SPI3) return 2;
	return 255;
}

void spi_init(SPI_TypeDef * port, u16 mode, u16 br_presc) {
	u8 id = spi_get_id(port);
	
	SPI_InitTypeDef SPI_InitStructure;
	
	if (port == SPI1)
		RCC_APB2PeriphClockCmd(spi_rcc[id], ENABLE);
	else
		RCC_APB1PeriphClockCmd(spi_rcc[id], ENABLE);
	
	if (mosi[id]) gpio_rcc_init(mosi[id]);
	if (miso[id]) gpio_rcc_init(miso[id]);
	if (sclk[id]) gpio_rcc_init(sclk[id]);
	
	if (mosi[id]) gpio_af_init(mosi[id], GPIO_OType_PP, GPIO_PuPd_NOPULL, spi_af[id]);
	if (miso[id]) gpio_af_init(miso[id], GPIO_OType_PP, GPIO_PuPd_NOPULL, spi_af[id]);
	if (sclk[id]) gpio_af_init(sclk[id], GPIO_OType_PP, GPIO_PuPd_NOPULL, spi_af[id]);
	
	SPI_I2S_DeInit(port);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = mode;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = br_presc;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Init(port, &SPI_InitStructure);
  SPI_Cmd(port, ENABLE);
  SPI_CalculateCRC(port, DISABLE);
  SPI_SSOutputCmd(port, DISABLE);
}

void spi_deinit(SPI_TypeDef * port) {
	u8 id = spi_get_id(port);
	SPI_I2S_DeInit(port);
	gpio_input_init(mosi[id], GPIO_PuPd_NOPULL);
	gpio_input_init(miso[id], GPIO_PuPd_NOPULL);
	gpio_input_init(sclk[id], GPIO_PuPd_NOPULL);
}

void spi_rx_set_handler(SPI_TypeDef * port, SpiListener * listener) {
	u8 id = spi_get_id(port);
	
	NVIC_InitTypeDef NVIC_InitStructure;	
	NVIC_InitStructure.NVIC_IRQChannel = spi_irq[id];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	SPI_I2S_ITConfig(port, SPI_I2S_IT_RXNE, ENABLE);
	
	SpiRxListenerList[id] = listener;
}

u16 spi_tx_byte(SPI_TypeDef* port, u8 data) {
	while (SPI_I2S_GetFlagStatus(port, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(port, data);
	if (SpiRxListenerList[spi_get_id(port)] == 0) {
		while (SPI_I2S_GetFlagStatus(port, SPI_I2S_FLAG_RXNE) == RESET);
		return SPI_I2S_ReceiveData(port);
	}
	return 0;
}

void SPI1_IRQHandler(void) {
	u8 data;
	if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET) {
		data = (u8)SPI_I2S_ReceiveData(SPI1);
		if (SpiRxListenerList[0])
			(*SpiRxListenerList[0])(data);
	}
}

void SPI2_IRQHandler(void) {
	u8 data;
	if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) == SET) {
		data = (u8)SPI_I2S_ReceiveData(SPI2);
		if (SpiRxListenerList[1])
			(*SpiRxListenerList[1])(data);
	}
}

void SPI3_IRQHandler(void) {
	u8 data;
	if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE) == SET) {
		data = (u8)SPI_I2S_ReceiveData(SPI3);
		
		if (SpiRxListenerList[2])
			(*SpiRxListenerList[2])(data);
	}
}
