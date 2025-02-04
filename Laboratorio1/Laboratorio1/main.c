/*
 *Universidad del Valle de Guatemala
 * Laboratorio1.c
 *
 * Created: 1/28/2025 16:43:12
 * Author : Jhorman Santiago Burgos Gonzalez
 */ 


#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>




// VARIABLES

uint8_t p1 = 0;
uint8_t p2 = 0;
uint8_t inicio = 0;
uint8_t fin = 0;


//uint8_t display[16] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};


void initPCINT1(void);
void initRace(void);
void p1Wins(void);
void p2Wins(void);


int main(void)
{
	cli();
	
	DDRB |= (1 << PORTB0) | (1 <<PORTB1);			//PORTB COMO SALIDA
	PORTB |= (1 << PORTB0) | (1 <<PORTB1) ;
	
	
	
	DDRD = 0xFF;			//PORTD COMO SALIDA
	PORTD = 0;
	
	UCSR0B = 0;				// DESACTIVO RX Y TX
	
	
	PORTC|= (1 << PINC0) | (1 <<PINC1) | (1 <<PINC2);
	DDRC &= ~(1 << PINC0) | ~(1 <<PINC1) | ~(1 <<PINC2);			//PUSHBUTTONS

	initPCINT1();
	
	
	sei();


	while (1)
	{
		
		if (inicio == 1){
			PCMSK1 &= ~((1 << PCINT9) | (1 << PCINT10));// DESHABILITA LOS BOTONES DE LOS JUGADORES
			initRace();					// Empieza el contador de inicio de 5-0
			inicio = 0;					// Hace que el contador se quede en 0
			PORTD = 0x00;				// Apaga los leds.
			if (inicio == 0){			
				PCMSK1 |= (1 << PCINT9) | (1 << PCINT10);// HABILITA LOS BOTONES PARA JUGAR
			}
		}
		if (p1 > 0){
			PORTB |= (1 << PB1);
			PORTB &= ~(1 << PB0);		//CUANDO P1 PRESIONA EL BOTON SE APAGA EL DISPLAY
		}
		if (p2 > 0){
			PORTB |= (1 << PB1);
			PORTB &= ~(1 << PB0);		//CUANDO P2 PRESIONA EL BOTON SE APAGA EL DISPLAY
		}
		//if (PORTD == (1 << PD3)){
		if (p1 >= 4){
			PORTD &= ~((1<<PD4) | (1<<PD5) | (1<<PD6) |(1<<PD7));		//AL GANAR APAGA LOS LEDS DE P2
			PCMSK1 &=~((1<< PINC2) | (1<< PINC1)); 
			PORTB |= (1 << PB0);
			p1Wins();
			fin = 1;
			
			
			
			
			}if (p2 >= 4){
			PORTD &= ~((1<<PD0) | (1<<PD1) | (1<<PD2) |(1<<PD3));				//AL GANAR APAGA LOS LEDS DE P1
			PCMSK1 &=~((1<< PINC1) | (1<< PINC2)); 
			PORTB |= (1 << PB0);
			p2Wins();
			fin = 1;

		}
	}
}



void initPCINT1(void){				//INICIA LAS INTERRUPCIONES

	PCICR |= (1 << PCIE1);			//HABILITA LAS INTERRUPCION EN PCINT1
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10);		// INDICA QUE LAS INTERRUPCIONES SE HACEN EN PC0-PC2

}


ISR(PCINT1_vect)
{
	_delay_ms(50);					//ANTIRREBOTE
	
	
	if (!(PINC & (1 << PINC0))){	//SI SE PRESIONA PC EMPIEZA EL CONTADOR
		inicio = 1;
		} else {
		inicio = 0;
		} 

	if (PORTD != (1 << PD3)) {
		if (!(PINC & (1 << PINC1))){	// EMPEIZA EL CONTADOR DE P1
			if (p1 <= 3){
				PORTD &= ~(1 << (p1 - 1));	// APAGA EL LED ANTERIOR
				p1++;
				PORTD |= (1 << (p1 - 1));	// ENCIENDE EL LED EN LA POSICION ACTUAL
				
			}
		}
	}

	if (PORTD != (1 << PD7)){
		if (!(PINC & (1 << PINC2))){	// EMPIEZA EL CONTADOR DE P2
			if (p2 <= 3){
				PORTD &= ~(1 << (p2 + 3));		// APAGA EL LED ANTERIOR
				p2++;
				PORTD |= (1 << (p2 + 3));		// ENCIEDNE EL LED EN LA POSICION ACTUAL
			}
			
		}
	}
	
	/*void restart(void){
		
		PORTD = 0x00;  // Apaga todos los LEDs
		p1 = 0;
		p2 = 0;
		inicio = 0;
		
		PORTB &= ~(1 << PB0); // Apaga el display
		PCMSK1 |= (1 << PCINT8);
	}8*/
	
}



//FINAL DEL DOCUMENTO

