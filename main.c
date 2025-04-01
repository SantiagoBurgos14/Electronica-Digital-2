/*
 * Botones_juego_Lab_6.c
 *
 * Created: 3/25/2025 14:49:01
 * Author : jhorm
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "UART/UART.h"

void initUART9600();
void initPCINT2();


int main(void)
{
    
	
	PORTD |= (1 << PORTD2) | (1 <<PORTD3) | (1 << PORTD4) | (1 <<PORTD5) | (1 << PORTD6) | (1 <<PORTD7);			// BOtones en el puerto D
	DDRD &= ~((1 << PORTD2) | (1 <<PORTD3) | (1 << PORTD4) | (1 <<PORTD5) | (1 << PORTD6) | (1 <<PORTD7)) ;
	
	initUART9600();
	initPCINT2();
	
	send_Button("UART FUNCIONANDO\n");

	sei();

    while (1) 
    {
		
		
    }
}

void initPCINT2(void){				//INICIA LAS INTERRUPCIONES

	PCICR |= (1 << PCIE2);			//HABILITA LAS INTERRUPCION EN PCINT1
	PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);		// INDICA QUE LAS INTERRUPCIONES SE HACEN EN PC0-PC2

	
}


	
	
ISR(PCINT2_vect){
	
	 if (!(PIND & (1 << PD2))) {
		 send_Button("U");
		 }
		  else if (!(PIND & (1 << PD3))) {
		 send_Button("D");
		 } 
		 else if (!(PIND & (1 << PD4))) {
		 send_Button("L");
		 } 
		 else if (!(PIND & (1 << PD5))) {
		 send_Button("R");
		 } 
		 else if (!(PIND & (1 << PD6))) {
		 send_Button("A");
		 } 
		 else if (!(PIND & (1 << PD7))) {
		 send_Button("B");
	 }
	
}