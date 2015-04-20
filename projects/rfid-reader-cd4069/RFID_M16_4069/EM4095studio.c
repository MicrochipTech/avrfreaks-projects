/*
	20.4.2015
	Sevc Dominik	sevc@post.sk

	This is my code for RFID read 125kHz for ATMega16 (16Mhz - but can be lower .. more than 2Mhz)
	125kHz signal is generated from PWM - signal must be exactly 125kHz generated from crystal not from internal RC oscilator or similar.
	As demodulator is used CD4069 (look to schematic)
	Signal is demodulated in real time .. very fast .. and easy.. for EM4100 standard .
	If read tag then send to UART .




*/
#define 	outb(addr, data)   addr = (data)
#define 	inb(addr)   (addr)
#define 	outw(addr, data)   addr = (data)
#define 	inw(addr)   (addr)
#define 	outp(addr, data)   addr = (data)
#define 	inp(addr)   (addr)
#define 	BV(bit)   (1<<(bit))
#define 	cbi(reg, bit)   reg &= ~(BV(bit))
#define 	sbi(reg, bit)   reg |= (BV(bit))
#define 	cli()   __asm__ __volatile__ ("cli" ::)
#define 	sei()   __asm__ __volatile__ ("sei" ::)
#define 	DDR(x)   ((x)-1)
#define 	PIN(x)   ((x)-2)
/// Typedefs //////////
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/parity.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uartio.h"


/// Define port B
#define		DATA	2 // on port D  INT0	here is signal from demodulator..
// PB3 is PWM output from MCU to RFID generator..

/************ RFID section **********/
#define	DataIn	0	// store actual data in bit from PIN port
#define	MBit	1	// Acvtual bit state 0 or 1
#define	GHeader	3	// Got Header 9x1
#define	Verp	4	// store vertical parity
#define	ActualB	5	// actual received bit
#define PrevB	6	// previouse bit


#define	VerpC	4	// How many we shift bit in RFID_temp while test parity
#define HorpC	10	// How many we calculate XOR horizontal parity

uint8_t RFID_bit;	// Here we store bit of RFID setting
uint8_t	RFID_temp;	// Here we store actual received data from demodulator
uint8_t	RFID_horp;	// Here we store horizontal parity 
uint8_t	RFID_verpC;	// Here we count vertical parity
uint8_t	RFID_horpC;	// Here we count horizontal parity
uint8_t	RFID_Zero;	// Here we check if incomming data are not 0 //

uint8_t DataX[11];	// Here we save received data from card..


void RFID_Start()
{
RFID_bit=0;
RFID_temp=0;
RFID_horp=0;
RFID_verpC=VerpC;
RFID_horpC=HorpC;
RFID_Zero=0;
}

void Send_Card_DataX(uint8_t Count,uint8_t *Buffer);

SIGNAL(INT0_vect)
{
if (PIND&(1<<DATA)) sbi(RFID_bit,PrevB);else cbi(RFID_bit,PrevB);		// this section decode manchaster coding ..
_delay_us(400);															// here wait for next bit ..
if (PIND&(1<<DATA)) sbi(RFID_bit,ActualB); else cbi(RFID_bit,ActualB);	// test it .. 
// this part is not ideal for ral time application .. here wait 400us .. if need better code then need use one timer...

if ((RFID_bit&(1<<ActualB))&&(RFID_bit&(1<<PrevB))) sbi(RFID_bit,MBit);// If detect 11 - then data are 1 
else if ((!(RFID_bit&(1<<ActualB)))&&(!(RFID_bit&(1<<PrevB))))	cbi(RFID_bit,MBit);	// If detect 00 - then data are 0 
	if (!(RFID_bit&(1<<GHeader)))
		{
		if ((RFID_temp==0xFF)&&(RFID_bit&(1<<MBit))) {RFID_Start();sbi(RFID_bit,GHeader);} // We got header .. now switch state .. receive data from card..
		}
	else
		{
		if (!RFID_verpC) 
			{
			RFID_verpC=VerpC;
			RFID_temp &=0x0F;
			if (!(RFID_horpC))
				{
				RFID_horp ^= RFID_temp;
				if ((!(RFID_horp))&(RFID_Zero))
					{
//					sbi(PORTD,6);
//					_delay_ms(100);
//					cbi(PORTD,6);

					// all is OK . RFID tag is OK .. in DataX is TAG number.. pls .. check if not all 0!!!
					// some bug ..
					uart_send_string("\r\n");
					Send_Card_DataX(10,DataX);
					uart_send_string("\r\n");

					}
				RFID_Start();
				}

			uint8_t temp_decode = 0;
			if (RFID_bit&(1<<MBit)) temp_decode=1;
			temp_decode ^= parity_even_bit(RFID_temp);
			if (!temp_decode)
					{
					RFID_verpC=VerpC;DataX[HorpC-RFID_horpC]=RFID_temp;RFID_horp ^= RFID_temp;RFID_horpC--;RFID_Zero|=RFID_temp&0x0F; // use for test if not all data are 0
					}
				else
					{
					RFID_Start();
					}
			} 
		else RFID_verpC--;

		}

	RFID_temp <<=1;	// roll 1 bit left .. 
	if (RFID_bit&(1<<MBit)) RFID_temp +=1; // add log 1 if need ..

sbi(GIFR,INTF0);	// clear interupt while waiting 400us
}

/************************** main() *******************************/
		
int main(void)
{
	outb(PORTD,0xff);	//internal pullups up
	outb(DDRD,0x00);	//internal pullups up
	sbi(DDRD,6);	// chime
	cbi(PORTD,6);	// chime
	sbi(DDRD,5);
	cbi(PORTD,5);

/********************** INIT UART 0 ******************************/
	uart_init(103); //16 57600 16Mhz	8Mhz - 9600 - 0x33 / 0x19 - 9600- 4Mhz / 103 - 9600 in 16Mhz
/*****************************************************************/



	sbi(PORTD,6);
	uart_send_string("RFID Reader Start\r\n");
	cbi(PORTD,6);

	/******* PD2 INT0 used for Data in from RFID reader */
	sbi(MCUCR,ISC00);
	sbi(GICR,INT0);

	
	/*******/

	DDRB |= 1<<PB3;	// OC0 vystup

	TCCR0 |= (0<<WGM00 | 1<<WGM01 | 1<<COM00 | 0<<COM01);  // Configure timer 1 for CTC mode
	TCCR0 |=  1<<CS00 | 0<<FOC0 ;

	OCR0   = 63;    // Set CTC compare value | 38 for ATtiny13 at 9.6 MHz | 32 for ATtiny85 at 8 MHz


	RFID_Start();

	sei();
	while(1) 
	{
//	RFID_decode();
	}
}

/// Send data from Buffer to Uart Count is number to send
void Send_Card_DataX(uint8_t Count,uint8_t *Buffer)
{
	signed char i;
	for(i=0;i<Count;i++) 
	{
	uart_send_char(B2X((Buffer[i])&0x0f));
	}
}
