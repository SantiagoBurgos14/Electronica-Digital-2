#include "SPI1.h"

void SPI_init(SPI_SckDiv spi_sckdiv, SPI_DataMode spi_datamode, uint8_t spi_intenable, uint8_t spi_enable, SPI_DataOrder spi_dataorder, SPI_Role spi_role) {

	if (spi_role == SPI_Master) {
		/* Relative to the master */
		SET_BIT(SPCR1, MSTR1);
		
		PIN_MODE(MISO, INPUT_PULLDOWN);		// MISO1 (PC0)
		PIN_MODE(MOSI, OUTPUT);				// MOSI1 (PE3)
		PIN_MODE(SS, OUTPUT);				// SS1 (PE2)
		DIGITAL_WRITE(SS, HIGH);			// Master, SS1 = HIGH
		PIN_MODE(SCK, OUTPUT);				// SCK (PC1)
		
		COPY_BIT(SPSR1, SPI2X1, (spi_sckdiv & 0b100) >> 2);
		COPY_BIT(SPCR1, SPR11, (spi_sckdiv & 0b010) >> 1);
		COPY_BIT(SPCR1, SPR10, spi_sckdiv & 0b001);
	}
	else {
		/* Relative to slave */
		CLEAR_BIT(SPCR1, MSTR1);
		
		PIN_MODE(MISO, OUTPUT);				// MISO1 (PC0)
		PIN_MODE(MOSI, INPUT_PULLDOWN);		// MOSI1 (PE3)
		PIN_MODE(SS, INPUT_PULLDOWN);		// SS1 (PE2)
		PIN_MODE(SCK, INPUT_PULLDOWN);		// SCK (PC1)
	}
	COPY_BIT(SPCR1, SPE1, spi_enable);
	COPY_BIT(SPCR1, SPIE1, spi_intenable);
	COPY_BIT(SPCR1, DORD1, spi_dataorder);
	COPY_BIT(SPCR1, CPOL1, (spi_datamode & 0b10) >> 1);
	COPY_BIT(SPCR1, CPHA1, (spi_datamode & 0b01));
}

uint8_t SPI_transmit(uint8_t DataToTransmit) {
	DIGITAL_WRITE(SS, LOW);
	SPDR1 = DataToTransmit;
	while (!(READ_BIT(SPSR1, SPIF)));
	DIGITAL_WRITE(SS, HIGH);
	return SPDR1;
}