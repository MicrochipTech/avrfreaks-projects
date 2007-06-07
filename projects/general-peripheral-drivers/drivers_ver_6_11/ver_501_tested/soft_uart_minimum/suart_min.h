/************************************************************************************
 Title  :   C include file for the SOFTWARE UART MINIMUM FUNCTIONS (suart_min.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      6/Jun/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

************************************************************************************/
                 
#ifndef SUART_MIN_H
#define SUART_MIN_H



volatile unsigned char suart_timeout_flag;






extern void            suart_putc(unsigned char data);
extern unsigned char   suart_getc(void);


#endif
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/


