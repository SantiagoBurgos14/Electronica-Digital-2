#include "USART0.h"

void SerialBegin(ModeUsart mode_usart, uint32_t BaudRate, SpeedMode speed_mode, uint8_t InterruptRX, uint8_t InterruptTX) {
	/*----------------------------------------------------USART0---------------------------------------------------------------------*/
		DDRD &= ~(1<<DDD0);										// D0 as input RX
		UCSR0B |= (1<<RXEN0);									// Receiver enable
		DDRD |= (1<<DDD1);										// D1 as output TX
		UCSR0B |= (1<<TXEN0);									// Transmitter enable
		UCSR0A = (UCSR0A & 0xFD) | (speed_mode);					// Transmission speed
		UCSR0B = (UCSR0B & 0x7F) | (InterruptRX<<RXCIE0);		// RX complete interrupt enable
		UCSR0B = (UCSR0B & 0xBF) | (InterruptTX<<TXCIE0);		// TX complete interrupt enable
		UCSR0C = (UCSR0C & 0x3F) | (mode_usart);					// USART mode
		UCSR0C = (UCSR0C & 0xC0) | (1<<UCSZ01 | 1<<UCSZ00);		// Frame: no parity, 1 stop bit, 8-bit size
		
		if (speed_mode == NormalSpeed) {
			if (mode_usart == Synch) {
				switch (BaudRate) {
					case 9600:
					UBRR0 = 832;
					break;
					default:
					UBRR0 = 832;
					break;
				}
			}
			else if (mode_usart == Asynch) {
				switch (BaudRate) {
					case 9600:
					UBRR0 = 103;
					break;
					default:
					UBRR0 = 103;
					break;
				}
			}
		}
		else if (speed_mode == DoubleSpeed) {
			if (mode_usart == Synch) {
				UCSR0A = (UCSR0A & 0xFD);						// Forces normal speed on synchronous operation
				switch (BaudRate) {
					case 9600:
					UBRR0 = 832;
					break;
					default:
					UBRR0 = 832;
					break;
				}
			}
			else if (mode_usart == Asynch) {
				switch (BaudRate) {
					case 9600:
					UBRR0 = 207;
					break;
					default:
					UBRR0 = 207;
					break;
				}
			}
		}
}

void SerialWriteText(char* TextToTransmit) {
	if (TextToTransmit == NULL) return;
	for (uint8_t i = 0; TextToTransmit[i] != '\0'; i++) {
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = TextToTransmit[i];
	}
}

char* IntToString(int IntToConvert) {
	int ConvertedLength = snprintf(NULL, 0, "%d", IntToConvert) + 1;
	char* IntConverted = malloc(ConvertedLength);
	if (IntConverted == NULL) return NULL;
	sprintf(IntConverted, "%d", IntToConvert);
	return IntConverted;
}

void SerialWriteFloat(float Variable, uint8_t NumDecimal) {
	int Variable_int = (int)(Variable);
	int Variable_dec = (int)((Variable - Variable_int) * pow(10, NumDecimal));
	SerialWriteText(IntToString(Variable_int));
	SerialWriteText(".");
	SerialWriteText(IntToString(Variable_dec));
}
