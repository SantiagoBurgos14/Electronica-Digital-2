/*
 * Laboratorio_2.c
 *
 * Created: 2/4/2025 10:51:41
 * Author : jhorm
 */ 


#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "LCD/LCD.h"
#include "ADC/ADC.h"
#include "UART/UART.h"




int main(void)
{		
	
	initLCD8bits();			//  Inicializa la pantalla
	_delay_ms(20);
	
	initADC();				// Inicializa el ADC
	initUART9600();
	    
    while (1) 
    {
		uint16_t adc_value = ADC_Read(0);		// Leer ADC en PC0
		if (adc_value > 1023) {
			adc_value = 1023;				// Limita el valor máximo
		}
		LCD_Set_Cursor(1,1);
		LCD_Write_String("V0L1:");
		LCD_Set_Cursor(1, 6);				// Indica donde aparecera el valor del voltaje en la LCD
		Print_V_LCD(adc_value);				 // MUestra Volt1
		
		_delay_ms(250); // Actualiza cada 250ms
		
		
		uint16_t adc_value2 = ADC_Read(1);			// Leer ADC en PC1
		if (adc_value2 > 1023) {
			adc_value2 = 1023;						// Limita el valor máximo
		}
		
		
		LCD_Set_Cursor(2,1);
		LCD_Write_String("V0L2:");
		LCD_Set_Cursor(2, 6);				// Muestra el valor VOl2 en la linea de abajo
		Print_V_LCD2(adc_value2);			 // Convierte y muestra el voltaje
		
		_delay_ms(250); 
		
		//UART;
		LCD_Set_Cursor(1,12);
		LCD_Write_String("Cont:");
		LCD_Set_Cursor(2,12);
		LCD_Write_String("    ");
		LCD_Set_Cursor(2,12);
		Print_UART_LCD();
		_delay_ms(250); 
    }
}

