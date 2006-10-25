/*CAVR********************************************
Projet : M BUS
Commentaires :
Version : 1
Date    : 16/12/2005
Heure : 15:09
Auteur  : Nicolas LE FEVRE
Compagnie: Nycos Corporation

Micro Contrôleur     : AT Tiny 26
Clock frequency       : 8,000000 MHz
*********************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "M_BUS_Modules.h"
void init(void)
{
	DDRA = 0xff;					//DB7,  DB6,  DB5,  DB4,  RS,   S2,   S1,   E
												//PA7   PA6   PA5   PA4   PA3   PA2   PA1   PA0
												// O     O     O     O     O     O     O     O
												// 1     1     1     1     1     1     1     1

	PORTA = 0xfe;					// 1     1     1     1     1     1     1     0

	DDRB = 0x0e;					//RES,  INT0, SW'S, NC,   OC1B, SCK,  MISO, MOSI
	     										//PB7   PB6   PB5   PB4   PB3   PB2   PB1   PB0
												// I     I     I     I     O     O     O     I
												// 0     0     0     0     1     1     1     0

	PORTB = 0x42;					// 0     1     0     0     0     0     1     0

	TIMSK |= 0x02;				//T0 overflow interrupt enable
	MCUCR = 0x03;				//INT0 rising edge sensitive
	GIMSK |= 0x40;				//INT0 interrupt enable
	sei();	 								 //global interrupt enable
}
