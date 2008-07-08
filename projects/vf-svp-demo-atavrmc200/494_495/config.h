/**
* @file config.h
*
* Copyright (c) 2004 Atmel.
*
* @brief This module provide services to define config for AT90PWM2/3
* Describes the system dependant software configuration.
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.4)
* @date 2006/03/08 17:02:04 (created on 06/04/2004)
* @author raubree (created by Jérome le denmat)

*****************************************************************************/

/*typedef unsigned char U8;
typedef unsigned int U16 ;
typedef signed int S16;
typedef signed long S32;
typedef unsigned long U32;*/

/*_____ I N C L U D E S ____________________________________________________*/

#ifndef _CONFIG_H_
#define _CONFIG_H_
#endif

/*-------------- UART LIB CONFIGURATION ---------------*/


/* baud rate register value calculation */		
#define	BRREG_VALUE	100    /* 4800 bauds */
//#define	BRREG_VALUE	25    /* 19200 bauds */
//#define	BRREG_VALUE	12    /* 38400 bauds */

/* definitions for UART control */		
#define	BAUD_RATE_LOW_REG	UBRRL
#define	UART_CONTROL_REG	UCSRB
#define	ENABLE_TRANSMITTER_BIT	TXEN
#define	ENABLE_RECEIVER_BIT	RXEN
#define	UART_STATUS_REG	UCSRA
#define	TRANSMIT_COMPLETE_BIT	TXC
#define	RECEIVE_COMPLETE_BIT	RXC
#define	UART_DATA_REG	UDR

/*----------- SOME USEFUL MACROINSTRUCTIONS -----------*/
#define Set_PC7()     PORTC |= _BV(PORTC7) 
#define Clear_PC7()   PORTC &= ~_BV(PORTC7)
#define Toggle_PC7()  PINC  |=  (1<<PINC7)

#define Set_PC3()     PORTC |=  (1<<PORTC3)
#define Clear_PC3()   PORTC &= ~(1<<PORTC3)
#define Toggle_PC3()  PINC  |=  (1<<PINC3)

#define Set_PE1()     PORTE |=  (1<<PORTE1)
#define Clear_PE1()   PORTE &= ~(1<<PORTE1)
#define Toggle_PE1()  PINE  |=  (1<<PINE1)

