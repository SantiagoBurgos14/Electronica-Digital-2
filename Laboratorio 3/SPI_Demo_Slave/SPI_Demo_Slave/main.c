/*
Santiago Burgos 22517
Pablo Garcia	22679
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "SPI1/SPI1.h"
#include "ADC/ADC.h"

#define PREPARE_SENSOR1	0xA1	// 0b1010_0001
#define PREPARE_SENSOR2	0xA2	// 0b1010_0010
#define WAIT_CHAR		0xA3	// 0b1010_0011

volatile uint8_t reaction[2] = {0, 0};
volatile uint8_t reaction_index = 0;

volatile uint8_t received = 0;
volatile uint8_t prepare = 0;

ISR(SPI1_STC_vect);

void setup(void);
void SeeData(uint8_t Data);

int main(void)
{	setup();
	while (1)
	{
		
		if (prepare) {
			prepare = 0;
			switch (reaction[0]) {
				case PREPARE_SENSOR1:
				SPDR1 = AnalogRead(ChannelADC5);
				break;
				
				case PREPARE_SENSOR2:
				SPDR1 = AnalogRead(ChannelADC2);
				break;
				
				case WAIT_CHAR:
				SPDR1 = 0xFF;
				break;
				
				default:
				SPDR1 = 0xFF;
				break;
			}
		}
		if (received) {
			received = 0;
			switch (reaction[0]) {
				case PREPARE_SENSOR1:
				
				break;
				
				case PREPARE_SENSOR2:
				
				break;
				
				case WAIT_CHAR:
				SeeData(reaction[1]);
				break;
			}
		}
	}
}

void setup(void) {
	SPI_init(SPI_CLOCK_DIV16, SPI_Mode0, True, True, SPI_MSB_first, SPI_Slave);
	SettingADC(AVcc, LeftJustified, ADC_Prescaler128, FreeRunning);
	sei();
	PIN_MODE(D2, OUTPUT);
	PIN_MODE(D3, OUTPUT);
	PIN_MODE(D4, OUTPUT);
	PIN_MODE(D5, OUTPUT);
	PIN_MODE(D6, OUTPUT);
	PIN_MODE(D7, OUTPUT);
	PIN_MODE(D8, OUTPUT);
	PIN_MODE(D9, OUTPUT);
}

void SeeData(uint8_t Data) {
	DIGITAL_WRITE(D2, READ_BIT(Data, 7));
	DIGITAL_WRITE(D3, READ_BIT(Data, 6));
	DIGITAL_WRITE(D4, READ_BIT(Data, 5));
	DIGITAL_WRITE(D5, READ_BIT(Data, 4));
	DIGITAL_WRITE(D6, READ_BIT(Data, 3));
	DIGITAL_WRITE(D7, READ_BIT(Data, 2));
	DIGITAL_WRITE(D8, READ_BIT(Data, 1));
	DIGITAL_WRITE(D9, READ_BIT(Data, 0));
}

ISR(SPI1_STC_vect) {
	reaction[reaction_index] = SPDR1;
	if (reaction_index == 0) {
		reaction_index++;
		prepare = 1;
	}
	else {
		reaction_index = 0;
		received = 1;
	}
}
