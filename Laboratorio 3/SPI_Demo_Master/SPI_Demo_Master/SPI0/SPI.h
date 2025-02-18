#ifndef SPI_H_
#define SPI_H_

#include "BitwiseManager/BitwiseManager_328PB.h"
#define MISO	D12
#define MOSI	D11
#define SS		D10
#define SCK		D13

#define True	1
#define False	0

typedef enum {
	SPI_CLOCK_DIV4   = 0b000,		// Clock frequency / 4
	SPI_CLOCK_DIV16  = 0b001,		// Clock frequency / 16
	SPI_CLOCK_DIV64  = 0b010,		// Clock frequency / 64
	SPI_CLOCK_DIV128 = 0b011,		// Clock frequency / 128
	SPI_CLOCK_DIV2   = 0b100,		// Clock frequency / 2
	SPI_CLOCK_DIV8   = 0b101,		// Clock frequency / 8
	SPI_CLOCK_DIV32  = 0b110,		// Clock frequency / 32
	SPI_CLOCK_DIV64X2 = 0b111		// Clock frequency / 64 (experimental)
} SPI_SckDiv;

typedef enum {
	SPI_Mode0 = 0b00,				// SCK idle low - sample first edge
	SPI_Mode1 = 0b01,				// SCK idle low - sample last edge
	SPI_Mode2 = 0b10,				// SCK idle high - sample first edge
	SPI_Mode3 = 0b11				// SCK idle high - sample last edge
} SPI_DataMode;

typedef enum {
	SPI_LSB_first = 1,
	SPI_MSB_first = 0
} SPI_DataOrder;

typedef enum {
	SPI_Master = 1,
	SPI_Slave = 0
} SPI_Role;

void SPI_init(SPI_SckDiv, SPI_DataMode, uint8_t spi_intenable, uint8_t spi_enable, SPI_DataOrder, SPI_Role);
uint8_t SPI_transmit(uint8_t DataToTransmit);

#endif /* SPI_H_ */