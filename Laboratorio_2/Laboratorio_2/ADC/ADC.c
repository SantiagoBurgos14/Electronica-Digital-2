/*
 * ADC.c
 *
 * Created: 2/9/2025 12:34:25
 *  Author: jhorm
 */ 

#include <avr/interrupt.h>
#include "../LCD/LCD.h"
#include "ADC.h"


void initADC(void) {
	
	//ADMUX |= 0x00;		// Seleciono el ADC que se va a usar 
	
	//Vref = AVcc = 5Vs<
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1 << REFS1);
	
	// justifica hacia la izquierda
	//ADMUX |= (1 << ADLAR);
	
	ADMUX &= ~(1 << ADLAR); // Justificación a la derecha (valores de 10 bits en ADC)
	
	ADCSRA = 0;				// Limpia el regristro 
	
	// encendiendo ADC
	ADCSRA |= (1<<ADEN);
	
	// habilita isr (interrupcion ) ADC
	
	ADCSRA |= (1<<ADIE);
	
	// prescaler de 128 > 16M / 128 = 125khz
	
	ADCSRA |= (1<<ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	DIDR0|= (1 << ADC0D) | (1 << ADC1D);			// Deshabilita la entrada digital de PC0 
	
	sei();
}


uint16_t ADC_Read(uint8_t canalADC) 
	{
	ADMUX = (ADMUX & 0xF8) | (canalADC & 0x07); 	// Asegura que estás leyendo el canal correcto
	ADCSRA |= (1 << ADSC);				// Inicia la conversión
	while (ADCSRA & (1 << ADSC));		// Espera a que termine
	return ADC;								// Devuelve el valor de 10 bits
}


void Print_V_LCD(uint16_t adc_value)
	{
		float volt1 = (adc_value * 5.00)/ 1023.0;		// COnvertir el valor del ADC a V
		int entero = (int)volt1;
		int decimal = (int)((volt1 - entero) * 100);
				
		// Convertir cada dígito a char y enviarlo a la LCD
		LCD_Write_Char(entero + '0'); // Convierte el entero a carácter y lo imprime
		LCD_Write_Char('.'); // Imprime el punto decimal
		LCD_Write_Char((decimal / 10) + '0'); // Dígito de las decenas
		LCD_Write_Char((decimal % 10) + '0'); // Dígito de las unidades
		LCD_Write_Char('V'); // Imprime 'V' para indicar voltios
		
	}
	
void Print_V_LCD2(uint16_t adc_value2)
	{
		float volt2 = (adc_value2 * 5.00)/ 1023.0;		// COnvertir el valor del ADC a V
		int entero2 = (int)volt2;
		int decimal2 = (int)((volt2 - entero2) * 100);
		
		// Borra el texto anterior escribiendo "     "
		//LCD_Write_String("     ");
		//LCD_Set_Cursor(1, 7); // Volver a la posición correcta
		
		// Convertir cada dígito a char y enviarlo a la LCD
		LCD_Write_Char(entero2 + '0'); // Convierte el entero a carácter y lo imprime
		LCD_Write_Char('.'); // Imprime el punto decimal
		LCD_Write_Char((decimal2 / 10) + '0'); // Dígito de las decenas
		LCD_Write_Char((decimal2 % 10) + '0'); // Dígito de las unidades
		LCD_Write_Char('V'); // Imprime 'V' para indicar voltios
		
	}
	
ISR (ADC_vect){
	
	//ADCSRA |= (1<<ADIF);
}
