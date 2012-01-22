/*
**
** Demo program to read LaCrosse TX6U, TX3-TH wireless remote temp sensors 
** AVR-GCC using ATtiny26 @ 1 MHz, internal calibrated RC oscillator
**
** Output on PA1 for >= 16x2 character serial LCD display (TTL RS-232, not inverted)
**
** Useful documentation:
** frame protocol 	http://www.f6fbb.org/domo/sensors/tx3_th.php
** RF signals: 		http://www.f6fbb.org/domo/sensors/tx_signals.php
**
** TX6U sends signals at about 1 minute intervals
**
** The TX-29 uses a similar protocol, documented at 
** http://fredboboss.free.fr/tx29/tx29_1.php
**
** S. James Remington    sjames_remington at yahoo.com
** Jan. 2012
*/


#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 1000000UL
#include <util/delay.h>

#define RF_IN   (1<<PB6)            //RF data in  PB0, ACTIVE LOW receiver
#define LED      (1<<PB3)

// The following defines are for a receiver with RF ON = active low output
// switch them if RF ON = active high
// also, definitions must be consistent with definition for RF_IN port bit
      
#define RF_ON  0
#define RF_OFF (1<<PB6)

#define BUFSZ 12   
char buf[BUFSZ+1];   //packet character buffer, count includes possible 0 terminator

const char   StartStr[]   PROGMEM = "TX6U-decoder";
char       TextBuffer[21];         //character buffer for PROGMEM strings 

void Delay10ms(int count)
{
   int i;
   for (i=0; i<count; i++) _delay_ms(10);
}

// software uart routines

#include "suart.c"

/*** io_init *********************************************************************************************
 *   Initialize timer, pins and software serial port
 ********************************************************************************************************/
void io_init(void) 
{
   DDRA=0;               //initially all inputs
   DDRB=0;

   TX_init(2400);         //baud rate for LCD output on PA0

   PORTA = 0xFE;         //pullups on, TX idle   

   DDRB |= LED;         //led output   
   PORTB = 0xFF;         //pullups, led on

   TCCR0 = 1<<CS01;      //Timer0 prescaler = CPU clock/8 = 8 usec ticks      
}


int main(void) {

   unsigned char Bit, BitCnt, ErrCnt, ErrCode, BufPtr;
   unsigned char c, idx;
   unsigned char BitTime, sensor_id;
   signed int tempc,tempf;

   io_init();

   cls();   //clear LCD screen and display welcome message
   TX_puts_f(StartStr);

   PORTB &= ~LED; //LED off

//
// main loop
//
   while (1)
   {

   ErrCode = 0;

//
//  loop: wait for possible transmissions.
//


   while (1) 
      {
      while ((PINB & RF_IN) == RF_OFF);     //wait for RF on

      TCNT0=0;                     			//zero 8 usec counter
      while ((PINB & RF_IN) == RF_ON);		//wait for RF off, time the on pulse

      if (TCNT0 > 162) break;          		//got long RF pulse (expect ~ 172 ticks)
      }

/*
      if (TCNT0 > 182) bail_out();			//probably not LaCrosse protocol

Kudos to avr-gcc developers! The compiler produces astoundingly efficient code:

      while ((PINB & RF_IN) == RF_OFF);     //wait for RF on
 148:	b6 99       	sbic	0x16, 6	; 22
 14a:	fe cf       	rjmp	.-4      	; 0x148 <main+0x44>

*/

	ErrCnt=0;
	PORTB |= LED;

//  possible start of packet, RF is OFF. Check for rest of preamble (3 more zero bits)

   for (idx=0; idx<3; idx++)
      {

      TCNT0 = 0;
      while ((PINB & RF_IN) == RF_OFF);  //time RF off period, expect 1 ms = 125 ticks
      BitTime = TCNT0;
   
      if ((BitTime < 115) || (BitTime > 135)) ErrCnt++;

      TCNT0 = 0;
      while ( (PINB & RF_IN) == RF_ON);  //time the RF on period, expect ~1.5 ms ~ 172 ticks
      BitTime = TCNT0;

      if ((BitTime < 162) || (BitTime > 182)) ErrCnt++;

      }

   if(ErrCnt > 0) ErrCode |= 1;  //glitchy preamble, or not LaCrosse protocol

// attempt to read and decode packet

   BitCnt = 0;
   BufPtr = 0;
   c = 0;                  
   while (BitCnt<40)                 //0 to 39
      {

      PORTB |= LED;               //led on

      while ((PINB & RF_IN) == RF_OFF);  //wait for on, expect 1 ms = 125 ticks
      
      TCNT0 = 0;                   		//time the RF pulse 
      while ((PINB & RF_IN) == RF_ON);  //wait for off, expect 0.6 ms = 0 or 1.5 ms = 1
      BitTime = TCNT0;

      Bit=0;
      if (BitTime < 125) Bit=1;        // ~ bisect 0/1 timing (measured 72/172 ticks) 

      c = (c<<1)|Bit;               //get bit, LSB first

      BitCnt++;

      if (BitCnt % 4 == 0) //remainder modulo 4
         {
         buf[BufPtr++]=c; //got 4 bits, store nibble in buf[]
         c=0;
         }
      
      }   //end while BitCnt

      BufPtr--;            //point to last character in buffer

//   check message integrity 
//  Lacrosse TX6U, TX3, version 1: 10 nibbles

      if (buf[0] != 10) ErrCode |= 2;      //message length not ten nibbles
      if (buf[1] !=  0) ErrCode |= 2;      //not a temperature sensor (humidity?)

//  check parity

      c=0;
      for (idx=0; idx < BufPtr; idx++) c += buf[idx];   // add chars in first 9 positions

      if ( (c & 0x0F) != buf[BufPtr]) ErrCode |= 4 ;   //parity nibble doesn't match

// check that first two digits of temperature are repeated

      if ( (buf[4] != buf[7]) || (buf[5] != buf[8]) ) ErrCode |= 8; // don't repeat

// done decoding
   
   if ( (ErrCode & 0x0E) == 0 ) // OK except for possibly forgiveable glitches in preamble
      {
      
      PINB &= ~LED;		//LED off

      sensor_id = ((buf[2]<<4) | buf[3])>>1;

      tempc=100*((int) buf[4]) + 10*((int) buf[5]) + (int) buf[6]; //celcius*10
      tempc = tempc-500;                //subtract offset applied by sensor
      tempf = (tempc*90+25)/50 + 320;   //deg F*10 rounded up for positive temperatures

//   convert entire message to hex for display

      for( idx=0;idx<=BufPtr;idx++)
         {
         c = buf[idx];            
         if (c<10) c += '0';   else c += 'a'-10;
         TextBuffer[idx]=c;
         } //end for idx

      TextBuffer[BufPtr+1] = 0;        	//end the string
      cls();
      TX_puts(TextBuffer);		       	//display received string in hex 
      TX_putc(' ');
      TX_puts(itoa(ErrCode,buf,16)); 	//and append error code
      TX_putc('h');
      crlf();

      itoa(tempf, buf, 10);				//display temp in degrees F
      idx=strlen(buf);
      c=buf[idx-1]; buf[idx-1]='.'; buf[idx]=c; buf[++idx]=0;   //place decimal point   
      TX_puts(buf);      				//temp
	  TX_putc('F');
      TX_putc(' '); TX_putc(' ');
      TX_puts(itoa(sensor_id,buf,16)); //Display sensor ID
      TX_putc('h');

      } //end if ErrCode
   
   } //end while(1)
}
