/*
 * defines.h
 *
 * Created: 
 *  Author: PjV
 * Version: 1.0 
 */ 

#ifndef DEFINES_H_
#define DEFINES_H_

#define F_CPU 1000000UL

#define FALSE 	0
#define TRUE 	1

#define SETBIT(ADDRESS,BIT)		(ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT)	(ADDRESS &= ~(1<<BIT))
#define TOGGLEBIT(ADDRESS,BIT)	(ADDRESS ^= (1<<BIT))

//rgb portb-pins
//#define RED 	1
//#define GRN 	3
//#define BLU 	5

//piezo portb-pin
#define BUZZER	6

/***************************************
* Timers
***************************************/

/* Compare match default values, adjust brightness, uint8_t */
#define RED_CMP_MATCH	60
#define GRN_CMP_MATCH	42
#define BLU_CMP_MATCH	100

#define Red_ON();		TCCR1A |= (1<<PWM1A); 
#define Red_OFF();		TCCR1A &= ~(1<<PWM1A);

#define Yellow_ON();	TCCR1A |= (1<<PWM1A);/*red on*/ TCCR1A |= (1<<PWM1B);/*green on*/
#define Yellow_OFF();	TCCR1A &= ~(1<<PWM1A);/*red off*/ TCCR1A &= ~(1<<PWM1B);/*green off*/

#define Green_ON();		TCCR1A |= (1<<PWM1B); 
#define Green_OFF();	TCCR1A &= ~(1<<PWM1B);

#define Blue_ON();		TCCR1C |= (1<<PWM1D); 
#define Blue_OFF();		TCCR1C &= ~(1<<PWM1D);


#endif /* DEFINES_H_ */