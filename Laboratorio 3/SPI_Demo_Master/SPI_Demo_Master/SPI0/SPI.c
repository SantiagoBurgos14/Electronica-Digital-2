#include "SPI.h"

void SPI_init(SPI_SckDiv spi_sckdiv, SPI_DataMode spi_datamode, uint8_t spi_intenable, uint8_t spi_enable, SPI_DataOrder spi_dataorder, SPI_Role spi_role) {
	
	if (spi_role == SPI_Master) {
		/* Relative to the master */
		SET_BIT(SPCR0, MSTR);
		
		PIN_MODE(MISO, INPUT_PULLDOWN);		// MISO (PB4)
		PIN_MODE(MOSI, OUTPUT);				// MOSI	(PB3)
		PIN_MODE(SS, OUTPUT);				// SS0 (PB2)
		DIGITAL_WRITE(SS, HIGH);			// Master, SS0 = HIGH
		PIN_MODE(SCK, OUTPUT);				// SCK (PB5)
		
		COPY_BIT(SPSR0, SPI2X, (spi_sckdiv & 0b100) >> 2);
		COPY_BIT(SPCR0, SPR1, (spi_sckdiv & 0b010) >> 1);
		COPY_BIT(SPCR0, SPR0, spi_sckdiv & 0b001);
	}
	else {
		/* Relative to slave */
		CLEAR_BIT(SPCR0, MSTR);
		
		PIN_MODE(MISO, OUTPUT);				// MISO (PB4)		
		PIN_MODE(MOSI, INPUT_PULLDOWN);		// MOSI	(PB3)
		PIN_MODE(SS, INPUT_PULLDOWN);		// SS0 (PB2)
		PIN_MODE(SCK, INPUT_PULLDOWN);		// SCK (PB5)
	}
	COPY_BIT(SPCR0, SPE, spi_enable);
	COPY_BIT(SPCR0, SPIE, spi_intenable);
	COPY_BIT(SPCR0, DORD, spi_dataorder);
	COPY_BIT(SPCR0, CPOL, (spi_datamode & 0b10) >> 1);
	COPY_BIT(SPCR0, CPHA, (spi_datamode & 0b01));
}

uint8_t SPI_transmit(uint8_t DataToTransmit) {
	DIGITAL_WRITE(SS, LOW);
	SPDR0 = DataToTransmit;
	while (!(READ_BIT(SPSR0, SPIF)));
	DIGITAL_WRITE(SS, HIGH);
	return SPDR0;
}