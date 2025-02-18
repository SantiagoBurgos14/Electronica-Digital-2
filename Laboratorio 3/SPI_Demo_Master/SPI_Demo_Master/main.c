/*
Santiago Burgos 22517
Pablo Garcia	22679

*/

#include <avr/io.h>
#include "SPI0/SPI.h"
#include "USART0/USART0.h"
#define F_CPU 16000000UL
#include <util/delay.h>

#define PREPARE_SENSOR1	0xA1	// 0b1010_0001
#define PREPARE_SENSOR2	0xA2	// 0b1010_0010
#define WAIT_CHAR		0xA3	// 0b1010_0011

uint8_t Sensor1_data;
uint8_t Sensor2_data;

uint8_t received_dummy = 0;
uint8_t process_char = 0;

volatile char character_rx;
volatile uint8_t refresh_count = 0;

ISR(USART0_RX_vect);
ISR(TIMER3_COMPA_vect);

void setup(void);
void SeeData(uint8_t Data);

int main(void)
{	setup();
    while (1) 
    {
		if (refresh_count >= 100) {
			refresh_count = 0;
			received_dummy = SPI_transmit(PREPARE_SENSOR1);	// Nothing
			_delay_ms(1);
			Sensor1_data = SPI_transmit(0xFF);				// Sensor 1 data
			_delay_ms(1);
			received_dummy = SPI_transmit(PREPARE_SENSOR2);	// Nothing
			_delay_ms(1);
			Sensor2_data = SPI_transmit(0xFF);				// Sensor 2 data
		
			float voltage_sensor1 = ((Sensor1_data * 5.0)/255);
			float voltage_sensor2 = ((Sensor2_data * 5.0)/255);
		
			SerialWriteText("Sensor 1: ");
			SerialWriteFloat(voltage_sensor1, 2);
			SerialWriteText(" V     ");
			SerialWriteText("Sensor 2: ");
			SerialWriteFloat(voltage_sensor2, 2);
			SerialWriteText(" V\n");
		}

		if (process_char) {
			process_char = 0;
			received_dummy = SPI_transmit(WAIT_CHAR);			// Nothing
			_delay_ms(1);
			received_dummy = SPI_transmit(character_rx);		// Nothing
			SeeData(character_rx);
		}
    }
}
void setup(void) {
	SPI_init(SPI_CLOCK_DIV16, SPI_Mode0, False, True, SPI_MSB_first, SPI_Master);
	SerialBegin(Asynch, 9600, NormalSpeed, True, False);
	sei();
	TCCR3B |= (1<<WGM32);
	TCCR3B |= (1<<CS32 | 1<<CS30);
	TIMSK3 = (1 << OCIE3A);
	OCR3A = 78;
	PIN_MODE(A0, OUTPUT);
	PIN_MODE(A1, OUTPUT);
	PIN_MODE(A2, OUTPUT);
	PIN_MODE(A3, OUTPUT);
	PIN_MODE(A4, OUTPUT);
	PIN_MODE(A5, OUTPUT);
	PIN_MODE(A6, OUTPUT);
	PIN_MODE(A7, OUTPUT);
	SerialWriteText(" System configuration completed...\n");
}

void SeeData(uint8_t Data) {
	DIGITAL_WRITE(A0, READ_BIT(Data, 7));
	DIGITAL_WRITE(A1, READ_BIT(Data, 6));
	DIGITAL_WRITE(A2, READ_BIT(Data, 5));
	DIGITAL_WRITE(A3, READ_BIT(Data, 4));
	DIGITAL_WRITE(A4, READ_BIT(Data, 3));
	DIGITAL_WRITE(A5, READ_BIT(Data, 2));
	DIGITAL_WRITE(A6, READ_BIT(Data, 1));
	DIGITAL_WRITE(A7, READ_BIT(Data, 0));
}

ISR(USART0_RX_vect) {
	process_char = 1;
	char validation_char = UDR0;
	if (validation_char != '\n' && validation_char != '\r') character_rx = validation_char;
}

ISR(TIMER3_COMPA_vect) {
	refresh_count++;
};