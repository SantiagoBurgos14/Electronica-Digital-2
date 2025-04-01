/*
 * UART.c
 *
 * Created: 3/25/2025 15:01:09
 *  Author: jhorm
 */ 

#include <avr/interrupt.h>
#include <stdio.h>

#include "UART.h"


volatile char bufferRX;		//Volatil porque cambia
uint16_t cont = 0;  // contador
void initUART9600(void){
	//Configurar RXy TX
	
	DDRD &= ~(1 << DDD0);		//Rx entrada
	DDRD |= (1 << DDD1);		//TX salida
	
	UCSR0A = 0;
	UCSR0A |= (1 << U2X0);		// Fast Mode de comunicacion
	
	
	UCSR0B = 0;			//configuracion de registros
	UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); //activa interrupcion para rx y TX
	//rxcie0 habilita la interrupcion de recepcion
	// txen0 habilidta la transmision uart
	// rxeno el micro recibe los datos
	// Activa la reecepcion y transmision de datos
	
	
	UCSR0C = 0;			// configuracion de registros
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);		// tamaño del caracter un bit y sin pariedad
	
	UBRR0 = 207;		// 9600 Baud rate
	
	sei();

	
}

void wUART(char singleChar){
	
	while(!(UCSR0A & (1 << UDRE0)));  // si el bufer esta lleno no envia ni recibe
	UDR0 = singleChar;  //Lee y guarda
	
}

ISR (USART_RX_vect){
	

}


void send_Button (const char *str){
	while (*str){
		wUART(*str++);
	}
}