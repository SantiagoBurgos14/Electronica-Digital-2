#ifndef USART0_H_
#define USART0_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define False			0
#define True			1

typedef enum {
	Asynch	=	0,
	Synch	=	(1<<UMSEL00),
	MasterSPI = (1<<UMSEL01 | 1<<UMSEL00)
} ModeUsart;

typedef enum {
	NormalSpeed	= 0,
	DoubleSpeed	= (1<<U2X0)
} SpeedMode;

void SerialBegin(ModeUsart, uint32_t BaudRate, SpeedMode, uint8_t InterruptRX, uint8_t InterruptTX);
void SerialWriteText(char* TextToTransmit);
char* IntToString(int IntToConvert);
void SerialWriteFloat(float Variable, uint8_t NumDecimal);

#endif /* USART0_H_ */