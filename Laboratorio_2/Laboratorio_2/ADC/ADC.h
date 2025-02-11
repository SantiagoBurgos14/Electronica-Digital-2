/*
 * ADC.h
 *
 * Created: 2/9/2025 12:34:35
 *  Author: jhorm
 */ 


#ifndef ADC_H_
#define ADC_H_
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

void initADC(void);

uint16_t ADC_Read(uint8_t canalADC);


void Print_V_LCD(uint16_t adc_value);
void Print_V_LCD2(uint16_t adc_value2);


#endif /* ADC_H_ */