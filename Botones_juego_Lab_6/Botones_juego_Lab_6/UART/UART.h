/*
 * UART.h
 *
 * Created: 3/25/2025 15:01:34
 *  Author: jhorm
 */ 


#ifndef UART_H_
#define UART_H_

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>


void initUART9600(void);

void wUART(char singleChar);

void send_Button(const char *str);




#endif /* UART_H_ */