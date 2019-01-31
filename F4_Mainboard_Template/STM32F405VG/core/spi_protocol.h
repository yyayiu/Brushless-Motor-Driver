#ifndef SPI_PROTOCOL_H
#define SPI_PROTOCOL_H

#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "gpio.h"

typedef void SpiListener(uc8 byte);

static const GPIO * sclk[3] = {&PA5, &PB13, &PC11};
static const GPIO * miso[3] = {0, &PB14, &PB5};
static const GPIO * mosi[3] = {&PA7, &PB15, &PC10};

static u32 spi_rcc[3] = {RCC_APB2Periph_SPI1, RCC_APB1Periph_SPI2, RCC_APB1Periph_SPI3};
static u8 spi_af[3] = {GPIO_AF_SPI1, GPIO_AF_SPI2, GPIO_AF_SPI3};
static u8 spi_irq[3] = {SPI1_IRQn, SPI2_IRQn, SPI3_IRQn};

void spi_init(SPI_TypeDef * port, u16 mode, u16 br_presc);
void spi_deinit(SPI_TypeDef * port);
void spi_rx_set_handler(SPI_TypeDef * port, SpiListener * listener);
u16 spi_tx_byte(SPI_TypeDef* port, u8 data);

#endif
