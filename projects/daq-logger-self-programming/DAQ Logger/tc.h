/////////////////////////////////////////////////////////////////////
//
//		RTC.H
//
//		real time clock using asysnchronous counter for AVR ATmega16
//
/////////////////////////////////////////////////////////////////////

#ifndef RTC_H
#define RTC_H
	
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

/* 		DEFINTIONS		*/

typedef struct {
 
	unsigned char second;   //enter the current time, date, month, and year
	unsigned char minute;
	unsigned char hour;                                     
	unsigned char date;       
	unsigned char month;
	unsigned int year;      
} time;


/*		FUNCTIONS		*/

void rtcInit(void); 
char notLeap(void);

#endif
