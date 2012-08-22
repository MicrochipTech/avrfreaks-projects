//
// file: baudprecalc.h
//
// xmega demo program VG Feb,Mar,Oct 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
// Version: 1.2.1
//
// precalculatet baudrate value for the usarts of xmega cpus.
// to be used in calls o USART_Baudrate_Set() 
//
// Calculation tool used:	
// http://www.avrcalc.elektronik-projekt.de/xmega/baud_rate_calculator
//
// BSCALE  is 4 bit (twos complement)
// BSEL    is 12 bit
/*
 *  Equation for calculation of BSEL value in asynchronous normal speed mode:
 *  	If ScaleFactor >= 0
 *  		BSEL = ((I/O clock frequency)/(2^(ScaleFactor)*16*Baudrate))-1
 *  	If ScaleFactor < 0
 *  		BSEL = (1/(2^(ScaleFactor)*16))*(((I/O clock frequency)/Baudrate)-1)
*/

#ifndef _BAUDPRECALC_H_
#define _BAUDPRECALC_H_


#ifndef F_CPU
#  error "baudprecalc.h requires F_CPU to be defined"
#endif

#if (F_CPU == 14745600) // baud rate has 0.0% error 
#define BSEL_4800		191		
#define BSCALE_4800		0
#define BSEL_9600		95		
#define BSCALE_9600		0
#define BSEL_19200		47		
#define BSCALE_19200	0
#define BSEL_38400		23		
#define BSCALE_38400	0
#define BSEL_57600		15		
#define BSCALE_57600	0
#define BSEL_115200		1		
#define BSCALE_115200	2
#define BSEL_230400		1		
#define BSCALE_230400	1

#elif (F_CPU == 29491200) // doubled by PLL
#define BSEL_4800		191		
#define BSCALE_4800		1
#define BSEL_9600		95		
#define BSCALE_9600		1
#define BSEL_19200		47		
#define BSCALE_19200	1
#define BSEL_38400		23		
#define BSCALE_38400	1
#define BSEL_57600		15		
#define BSCALE_57600	1
#define BSEL_115200		7				
#define BSCALE_115200	1	
#define BSEL_230400		3			
#define BSCALE_230400	1

#elif (F_CPU == 32000000)
#define BSEL_4800		13301
#define BSCALE_4800		-5
#define BSEL_9600		13269		
#define BSCALE_9600		-6
#define BSEL_19200		13205		
#define BSCALE_19200	-7
#define BSEL_38400		6539		
#define BSCALE_38400	-7
#define BSEL_57600		2158		
#define BSCALE_57600	-6
#define BSEL_115200		1047				
#define BSCALE_115200	-6	
#define BSEL_230400		983				
#define BSCALE_230400	-7

#elif (F_CPU == 2000000)
#define BSEL_4800		3205
#define BSCALE_4800		-7		
#define BSEL_9600		1539		
#define BSCALE_9600		-7
#define BSEL_19200		705	
#define BSCALE_19200	-7
#define BSEL_38400		289		
#define BSCALE_38400	-7
#define BSEL_57600		75		
#define BSCALE_57600	-6

#else
#  error "no BSEL values for this F_CPU in baudprecalc.h"
#endif

#endif // _BAUDPRECALC_H_
 
