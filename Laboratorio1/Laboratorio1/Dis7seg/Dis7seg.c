/*
 * Dis7seg.c
 *
 * Created: 1/30/2025 18:24:03
 *  Author: jhorm
 */ 


#include "Dis7seg.h"


uint8_t display[16] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};
	
void initRace(void){
	
	PORTB |= (1 << PB0);
	PORTB &= ~(1 << PB1);
	
	PORTD = display[5];
	_delay_ms(1000);
	PORTD = display[4];
	_delay_ms(1000);
	PORTD = display[3];
	_delay_ms(1000);
	PORTD = display[2];
	_delay_ms(1000);
	PORTD = display[1];
	_delay_ms(1000);
	PORTD = display[0];
	//_delay_ms(1000);
	
}

void p1Wins(void){
	PORTB |= (1 << PB1);
	PORTB &= ~(1 << PB0);
	PORTD |= ((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3));
	_delay_ms(5);
	
	PORTB |= (1 << PB0);
	PORTB &= ~(1 << PB1);
	PORTD = display[1];
	_delay_ms(5);
}

void p2Wins(void){
	PORTB |= (1 << PB1);
	PORTB &= ~(1 << PB0);
	PORTD |= ((1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7));
	_delay_ms(5);
	
	PORTB |= (1 << PB0);
	PORTB &= ~(1 << PB1);
	PORTD = display[2];
	_delay_ms(5);
}

