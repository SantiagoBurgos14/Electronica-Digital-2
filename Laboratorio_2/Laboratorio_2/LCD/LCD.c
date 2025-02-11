/*
 * CFile1.c
 *
 * Created: 2/4/2025 10:54:27
 *  Author: jhorm
 */ 

#include "LCD.h"

void initLCD8bits(void){
	
	DDRD |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7);		// PORT D COMO SALIDA
	PORTD = 0;					
	DDRB |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);			// Salidas del Puerto B
	PORTB = 0;
	
	// PORT D= RS(D2), E(D3), D0(D4), D1(D5), D2(D6), D3(D7), D4(D8), D5(D9), D6(D10), D7(D11)
		
	
	_delay_ms(5);
	LCD_CMD(0x38);				// modo 8 bits/ 2 lineas
	
	_delay_ms(5);
	LCD_CMD(0x0C);				// Enciende el display, apaga el cursor
	
	_delay_ms(10);
	LCD_CMD(0x06);				// Incrementa el cursos
	
	_delay_ms(10);
	LCD_CMD(0x01);				// Limpia el diplay
	
	_delay_ms(10);
	
		
	
}

 void LCD_CMD(char a) {
	 
	 // RS = 0; // => RS = 0 // Dato en el puerto lo va a interpretar como comando
	 PORTD &= ~(1 << PORTD2);
	 LCD_Port(a);

	 // EN = 1; // => E = 1
	 PORTD|= (1 << PORTD3);
	 _delay_ms(4);

	 // EN = 0; // => E = 0
	 PORTD &= ~(1 << PORTD3);
 }
 
 
 // Funcion para colocar en el puerto un valor
 void LCD_Port(char a) {
	 if (a & (1 << 0)) 
	 PORTD |= (1 << PORTD4);		 // D0 = 1;
	 else
	 PORTD &= ~(1 << PORTD4);		// D0 = 0;

	 if (a & (1 << 1))
	 PORTD |= (1 << PORTD5);		// D1 = 1;
	 else
	 PORTD &= ~(1 << PORTD5);		// D1 = 0;
	 
	 if (a & (1 << 2))
	 PORTD |= (1 << PORTD6);		// D2 = 1;
	 else
	 PORTD &= ~(1 << PORTD6);		// D2 = 0;
	 
	 if (a & (1 << 3))
	 PORTD |= (1 << PORTD7);		// D3 = 1;
	 else
	 PORTD &= ~(1 << PORTD7);		// D3= 0;
	 
	 if(a & (1 << 4))
	 PORTB |= (1 << PORTB0);		 // D4= 1;
	 else
	 PORTB &= ~(1 << PORTB0);		 // D4 = 0;
	 
	  if(a & (1 << 5))
	 PORTB |= (1 << PORTB1);		// D5 = 1;
	 else
	 PORTB &= ~(1 << PORTB1);		// D5 = 1;
	 
	 if(a & (1 << 6))
	 PORTB |= (1 << PORTB2);		 // D6 = 1;
	 else	 
	 PORTB &= ~(1 << PORTB2);		// D6 = 0;
	 
	 if(a & (1 << 7))
	 PORTB |= (1 << PORTB3);		// D7 = 1;
	 else
	 PORTB &= ~(1 << PORTB3);		 // D7 = 0;
	 
 }
  
 void LCD_Write_Char( char c){
	
	
	PORTD |= (1 << PORTD2);			// RS = 1 modo de datos
	LCD_Port(c);					// Envia el caracter completo
	
	PORTD |= (1 << PORTD3);			// E = 1
	_delay_ms(4);
	PORTD &= ~(1 << PORTD3);		// E = 0
	
	
	 
 }
 
 //void LCD_Write_String(char *c){
	//int i;
	//for (i = 0; a[i] !='\0'; i++){
		
	//LCD_Write_Char(a[1]);
	
	//} 
 //} 
 
 void LCD_Write_String(char *str){
	 while (*str)
	 {
		 LCD_Write_Char(*str);
		 str++;
	 }
 }
  
  void LCD_Shift_Right(void){
	  LCD_CMD(0x01);
	  LCD_CMD(0x0C);
	  
  }
  
  void LCD_Shift_Left(void){
	  
	  LCD_CMD(0x01);
	  LCD_CMD(0x08);
	  
  }
  
  void LCD_Set_Cursor(char f, char c){
	  
	  //char temp = 0x80;			// Valor por defecto para evitar errores
	  char temp;			// Valor por defecto para evitar errores

	  if (f ==1 ){
		  temp = 0x80 + ( c- 1);	//FIla 1
		   }
		  else if (f == 2){
			
			temp = 0xC0 + ( c - 1);		// FIla 2  
		  }else{
			  return;
		  }
		  LCD_CMD(temp);
	  
  }