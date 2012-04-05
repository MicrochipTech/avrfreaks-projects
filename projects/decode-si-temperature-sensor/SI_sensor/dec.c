/********************************************************************************************

 Sharper Image remote weather station temp sensor decoder (433 MHz) 
 Value sent every 3.00 minutes from the example studied.
 
 Message format is: 
 
 "0" = 2ms RF on, 6 ms RF off
 "1" = 6ms RF on, 2 ms RF off

Preamble = 11 "0" bits followed by "1" bit, then 28 data bits, repeated 3 times
A pause of 23 ms RF off separates the second and third repetitions
There are only 7 "0" bits in preambles for second and third repetitions.

Ignoring the start bit:

Messages begin with nibble 0x2 = sensor ID?
10 bits for temperature (Celcius*10 + 500) and 
10 bits for something else (humidity?)
4 bit checksum (CRC4) follows


20.8 C = 0x2b10002    0xb10/4 = 0x2c4 = 708 dec (-500 = 208), CRC4=0x2
20.7 C = 0x2b0c00d    0xb0c/4 = 0x2c3 = 707 dec (-500 = 207), CRC4=0xd
16.3 C = 0x2a5c00c    0xa5c/4 = 0x297 = 663 dec

The CRC4 is calculated using polynomial X^4+X^3+X^0 
(see http://ghsi.de/CRC/index.php?Polynom=11001)

 S. James Remington sjames_remingon at yahoo.com 
 Mar. 2012

 This code is distributed and licensed according to the terms of the
 Creative Commons Attribution-Sharealike License 3.0 
 http://creativecommons.org/licenses/by-sa/3.0/

 MCU: ATtiny26 @ 1MHz
 
********************************************************************************************/

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 1000000UL
#include <util/delay.h>

const char   StartStr[]   PROGMEM = "SI V1.0 decode";
char       TextBuffer[21];  //string, 20 chars max

//function prototype for CRC-4 calculation

char CRC4(unsigned long int msg);

void Delay10ms(int count)  //long delays
{
   int i;
   for (i=0; i<count; i++) _delay_ms(10);
}

// include software uart routines

#include "suart.c"


#define RF_IN   (1<<PB6)            //RF data in  PB0
#define LED      (1<<PB3)

// The following defines are for a receiver with RF ON = active low output
// switch them if RF ON = active high
// the nonzero one of RF_OFF or RF_ON must be equal in value to RF_IN
      
#define RF_ON  0
#define RF_OFF (1<<PB6)


/*** io_init *********************************************************************************************
 *   Initialize timers, pins and software serial port
 ********************************************************************************************************/

void io_init(void) 
{
   DDRA=0;               //initially all inputs
   DDRB=0;

   TX_init(2400);         //baud rate for LCD output on PA0

   PORTA = 0xFE;         //pullups on, TX idle   

   DDRB |= LED;         //led output   
   PORTB = 0xFF;         //pullups, led on

   TCCR0 = (1<<CS01)|(1<<CS00);    //Timer0 counts at system clock/64 = 64 usec ticks
   TCCR1A = 0;                  

}

/*
** Main loop waits for RF transmissions, decodes if valid OS V 1.0 packet, 
** and displays decoded packet on serial output
*/
   int main (void)
{  
    unsigned char TimerValue=0, Ton, Toff;     // times between edges
    unsigned char ErrCode,crc;
    unsigned char BitValue=0;
    unsigned char BitCnt;       // received bits counter
    unsigned char PreambleCnt=0;   // counts preamble pulses
    unsigned int  temp;
	int tempC;

// SI message structure. A long int is equivalenced to four bytes

union shared {
uint32_t l;
uint8_t b[4];
} SI_msg;  //SI_msg.b[3] is most significant byte of SI_msg.l


// the following define expected interval timing and allowed 
// uncertainty for timer tick rate 64 uSec

#define deltaT 5
#define InRange(TVAL, TSETUP, dT) ((TVAL<=(TSETUP+dT))&&(TVAL>=(TSETUP-dT)))

#define TrfON0 31               // 2 ms RF ON time, zero bit 
#define TrfON1 94				// 6 ms RF ON time, one bit

   io_init();
   Delay10ms(20);

   cls();   //clear LCD screen
   TX_puts_f(StartStr);

   while (1)
   {
      
      PORTB &= ~LED;
      ErrCode = 0;
      PreambleCnt = 0;
	  BitValue = 0;
      TCNT0 = 0;

// look for SI preamble, 7-11 zeros, depending on position in message
      
   while(2)

   {
      
	  while ((PINB & RF_IN) == RF_ON); //RF is on, wait for RF off

      Ton = TCNT0;     // Read ON timer value
	  TCNT0 = 0;

	  while ((PINB & RF_IN) == RF_OFF); //time the off period 
      
      Toff = TCNT0;  
      TCNT0 = 0;
      if (InRange(Ton, TrfON0, deltaT) && InRange(Toff, TrfON1, deltaT)) PreambleCnt++;
	  if (InRange(Ton, TrfON1, deltaT) && InRange(Toff, TrfON0, deltaT)) BitValue=1;
	  if ((PreambleCnt > 6) && (BitValue == 1)) break;

// invalid RF on pulse length?

	  if (!InRange(Ton, TrfON0, deltaT) && !InRange(Ton, TrfON1, deltaT))
	  { PreambleCnt=0; BitValue=0; }

    } //end while (2)


    PORTB |= LED;  //LED on, preamble detected

// if OK so far, get rest of message

     BitCnt = 1;
	 SI_msg.l = 0; //ignore start bit, clear message buffer

/*
** loop through next 28 bits and build message
** Decide one/zero by checking the RF_ON length (2 or 6 ms)
*/
     while (BitCnt < 29)
     {  
         while ((PINB & RF_IN) == RF_ON); //wait for RF_OFF
		 
		 TimerValue = TCNT0;
		 TCNT0 = 0;
		 
		 BitValue = 2;  //assume invalid

		 if(InRange(TimerValue, TrfON0, deltaT)) BitValue=0;
		 if(InRange(TimerValue, TrfON1, deltaT)) BitValue=1;
 
 		 if(BitValue == 2) ErrCode |= 1; //invalid RF ON pulse width    
 
//shift in bit

		SI_msg.l <<= 1;
		if (BitValue == 1) SI_msg.l |= BitValue;

//wait for rising edge, with timeout upon valid end of message (RF_OFF >> 6 ms)

         while (((PINB & RF_IN) == RF_OFF) && ((TCNT0 & 0x80) != 0x80)); //0x80 = 8.2 ms

		 TimerValue = TCNT0;  // and check that RF off time was valid
		 TCNT0 = 0;

		 if (BitCnt < 28)  //unless it is the last bit, flag bad RF_OFF period
			{
			 if ((BitValue == 1) && !(InRange(TimerValue, TrfON0, deltaT))) ErrCode |= 2;
			 if ((BitValue == 0) && !(InRange(TimerValue, TrfON1, deltaT))) ErrCode |= 4;
			}

         BitCnt++; 

     } //end while BitCnt

// check message integrity

 	crc = CRC4(SI_msg.l>>4);  //generate CRC4 of top 24 bits: should match bottom 4 bits

	if (crc != (SI_msg.b[0] & 0x0F)) ErrCode |= 8;

// display message on LCD in hex 

	 crlf();

     TX_puts(ltoa(SI_msg.l,TextBuffer,16));
     TX_putc(' ');
	 TX_puts(itoa(crc,TextBuffer,16));
	 TX_putc(' ');
	 TX_putc(':'); 
	 TX_puts(itoa(ErrCode,TextBuffer,16));
	 TX_putc(' ');

// if message OK, display temperature in Celcius*10

	 if (ErrCode == 0) {

	   temp = SI_msg.b[2]<<8; 
	   temp = (temp + (SI_msg.b[1]&0xc0))>>6;
	   tempC = temp - 500;
	   TX_puts(itoa(tempC,TextBuffer,10));

// the second 10-bit data field, if populated, 
// should be = ((SI_msg.b[1]&0x3f)<<4 | (SI_msg.b[0]&0xf0)>>4)

	   }

// here, we could read the next two copies of message, but what the hey...
 
     PORTB &= ~LED;  //led off

     Delay10ms(200);  //wait 5 s


    } //end while(1)
 }

// ==========================================================================
// CRC Generation Unit - Linear Feedback Shift Register implementation
// (c) Kay Gorontzi, GHSi.de, distributed under the terms of LGPL
//  code generated by http://ghsi.de/CRC/ and modified by SJR
//
// This code generates the 4-bit CRC of the least significant 24 bits of 
// the 32 bit input value, in order bit23, bit22, ... bit0
//
// ==========================================================================

char CRC4(unsigned long int msg)
   {
   
   char Res, Bit;    // CRC Result, intermediate bit
   char CRC[4];  //temps
   int  i;
   char DoInvert;

   msg <<= 8;             //only bottom 3 bytes of message checked

   for (i=0; i<4; ++i)  CRC[i] = 0;                    // Init before calculation

   for (i=0; i<24; ++i)
      {
      Bit=0;
      if ( (msg&0x80000000) > 0) Bit=1;  //get top bit of message
      msg <<=1;
      DoInvert = Bit ^ CRC[3];         // XOR required?
      CRC[3] = CRC[2] ^ DoInvert;
      CRC[2] = CRC[1];
      CRC[1] = CRC[0];
      CRC[0] = DoInvert;
//      printf("%i crc: %x %x %x %x\n",i,CRC[3],CRC[2],CRC[1],CRC[0]);
      }

   Res=0;  //result is lower nibble of one byte
   for (i=0; i<4; ++i)  if (CRC[i]) Res |= 1<<i;

   return(Res);
   }
