/*
 * Dis7seg.h
 *
 * Created: 1/30/2025 18:24:25
 *  Author: jhorm
 */ 


#define F_CPU 16000000

#ifndef DIS7SEG_H_
#define DIS7SEG_H_
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>



void initRace(void);
void p1Wins(void);
void p2Wins(void);
void restart(void);





#endif /* DIS7SEG_H_ */