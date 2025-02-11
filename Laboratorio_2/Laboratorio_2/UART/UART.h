/*
 * IncFile1.h
 *
 * Created: 2/9/2025 21:02:51
 *  Author: jhorm
 */ 


#ifndef UART_H_
#define UART_H_

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>


void initUART9600(void);

void wUART(char singleChar);

void send_ADC(const char *str);


void Print_UART_LCD(void);

#endif /* UART_H_ */