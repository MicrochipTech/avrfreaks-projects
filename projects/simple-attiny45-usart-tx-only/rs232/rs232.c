//name: RS232.c	 		version : 1			function: rs232 TX 8N1 9600
//date: 14.02.07		customer: 			target  : ATTiny45 1MHZ 
//Fuse: CKSEL3, CKSEL2, CKSEL0 8Mhz PLL / CKDIV8 = 1 MHz 
#include <iot45v.h>
#include <macros.h>

#pragma interrupt_handler tim1_ovf:iv_TIMER1_OVF
#pragma interrupt_handler tim0_ovf:iv_TIMER0_OVF

#define LEDOFF PORTB&=~0x10						//LED OFF
#define LEDON  PORTB|= 0x10						//LED ON
#define LEDTOG PORTB^= 0x10						//LED TOGGLE

#define TXHIGH PORTB|= 0x08						//TX HIGH
#define TXLOW  PORTB&=~0x08						//TX LOW
//connect TX pin to 4,7kOhm and simple NPN-transistor, then to FTDI232R-USB-Chip

#define T0ON   TCNT0 =TIME;TCCR0B =(1<<CS00); // /1
#define T0OFF  TCCR0B = 0

#define T1ON   TCCR1 = (1<<CS10)|(1<<CS11)|(1<<CS12)|(1<<CS13)
#define T1OFF  TCCR1 = 0

#define TIMER1 flag.a   		   				//
#define TIMER0 flag.b   		   				//

void transmit(unsigned char);		//transmit byte
void trs(const char *);				//transmit string
void trb(unsigned char);			//transmit byte
void trh(unsigned char);			//transmit hex byte

struct {unsigned a:1,b:1;}flag;

unsigned char count;						//common counter
unsigned char bcount;						//bit counter
unsigned char TIME;							//timer0 delay 

//CONST--------------------------------------------------------------------CONST
const char m1[]="$AT45-RS232 "__DATE__" "__TIME__" ";

//TIMER0------------------------------------------------------------------TIMER0
void tim0_ovf(void)							//232 delay
{
 TCNT0 = TIME;
 TIMER0=0;
}

//TIMER1------------------------------------------------------------------TIMER1
void tim1_ovf(void)							//simple delay timer
{
 TIMER1=1;
}										//end timer

//MAIN----------------------------------------------------------------------MAIN
void main()
{
 DDRB	=0xF8;	   							//0-2 in
 PORTB	=0x07;								//in high
 TIME 	=182;								//delay time for 1Mhz / 9600 baud
 //timer 
 TIMSK =(1<<TOIE1)|(1<<TOIE0);
 T1ON;
 TXLOW;
 SEI();  	 	  			  				//start
//LOOP----------------------------------------------------------------------LOOP
 while(1)
 {
  if(TIMER1)								//timer
  {
   T1OFF;
   LEDON;
   trs(m1);trb(count);transmit('-');trh(count++);
   LEDOFF;
   T1ON;
   TIMER1 =0;
  }	 	 								//end timer 
 }								   		//end loop
} 								   		//end main

//TRANSMIT--------------------------------------------------------------TRANSMIT
void transmit(unsigned char data)				//transmit 8N1
{
 TXHIGH;				   							//start bit
 T0ON;
 TIMER0=1;while(TIMER0){;}
 for(bcount=0;bcount<8;bcount++) 				//data bit
 {
  if(data & 0x01)
  {
   TXLOW;  
  }
  else
  {
   TXHIGH;
  }
  TIMER0=1;while(TIMER0){;}
  data >>=1;
 }
 TXHIGH;					   						//stop bit
 TIMER0=1;while(TIMER0){;}
 TXLOW;
 TIMER0=1;while(TIMER0){;} 						//don't forget to wait 1 bit time 
 T0OFF; 
}

//TRS------------------------------------------------------------------------TRS
void trs(const char *ptr)
{
 while(*ptr != 0)
 {
  if(*ptr != '$')
  {
   transmit(*ptr);
  }
  else
  {
   transmit(0x0D);
   transmit(0x0A);
  }
  ptr++; 
 }
}

//TRH------------------------------------------------------------------------TRH
void trh(unsigned char hbyte)
{
 unsigned char dbyte;
 dbyte =hbyte>>4;
 if(dbyte>9)
 {
  transmit(dbyte+55);
 }
 else
 {
  transmit(dbyte+0x30);
 }
 dbyte =hbyte & 0x0F;
 if(dbyte>9)
 {
  transmit(dbyte+55);  
 }
 else
 {
  transmit(dbyte+0x30);
 }
}

//TRB------------------------------------------------------------------------TRB
void trb(unsigned char w)
{
 transmit((w/100)+0x30);
 w	=w%100;				 		   		 //transmit ##
 transmit((w/10)+0x30);
 transmit((w%10)+0x30);
}
