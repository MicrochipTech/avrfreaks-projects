//name: fuse.c	 		version : 1			function: BTS digital fuse TX 8N1 9600
//date: 21.02.07		customer: GSNT		target  : ATTiny45 1MHZ 
//Fuse: CKSEL3, CKSEL2, CKSEL0 8Mhz PLL / CKDIV8 = 1 MHz 
/* Hardware:PB2 = input  ADC1 BTS640-IS
		PB3 = output TX RS232
		PB4 = input  on/off button
		PB1 = output BTS640-IN (5V = switch on)
		PB0 = input  BTS640-ST (status of BTS)		

 Info BTS640:
 The BTS640 is switched via a 5V signal to IN (HIGH = ON).
 It shows the actual status inverted at ST (LOW = ON).
 The current can be measured via resistor at IS (not very precise).
 At 5A there is a current of about 1mA through IS (see datasheet), which causes
 a voltage of 1V at 1000 Ohm.
 Although Infineon's datasheet describes that this PROFET is nearly indestructible,
 I killed a few with my inductive load. So be careful and use resistor and
 zener-diode to measure the IS voltage (see schematic). If the BTS fails, there
 are 12V at IS and your ATTiny is crying or dying.
 Nevertheless the BTS640 and his big brother BTS650 are very smart automotive
 Power-switches with overload protection, thermal shutdown, current limitation
 and last not least proportional load current sense.      
*/

#include <iot45v.h>
#include <macros.h>

#define MAXCURRENT 45	 		   //max current in 1/10A: 50 = 5A 
#define MAXTIME	   3		   	   //max time in 1/10 sec
//after switching the BTS on, it's allowed MAXTIME to exceed the MAXCURRENT =>
//so a load, which needs more current the first x/10 seconds doesn't blow the
//fuse at once 

#define FUSEON  PORTB|= 0x02			//fuse ON
#define FUSEOFF PORTB&=~0x02			//fuse OFF

#define TXHIGH PORTB|= 0x08				//TX HIGH
#define TXLOW  PORTB&=~0x08				//TX LOW
//connect TX pin to 4,7kOhm and simple NPN-transistor, then to FTDI232R-USB-Chip

#define SW1	   0x10						//switch at PB4
#define ST 	   0x01						//ST at PB0

#define T0ON   TCNT0 =TIME;TCCR0B =(1<<CS00); //timer 0 on
#define T0OFF  TCCR0B = 0		  			  //timer 0 off

#define T1ON   TCNT1 =61;TCCR1 = (1<<CS11)|(1<<CS13);//timer 1 on
#define T1OFF  TCCR1 = 0	   	 				//timer 1 off				 

#define TIMER1 flag1.a   		   				//timer1 flag
#define TIMER0 flag1.b   		   				//timer0 flag
#define BUTTON flag2.a							//button flag
#define STATUS flag2.b							//status flag
#define FUSE   flag3.a							//fuse flag

//function
void transmit(unsigned char);		//transmit byte
void trs(const char *);				//transmit string
void trb(unsigned char);			//transmit byte
void trd(unsigned char);			//transmit byte with decimal
void trh(unsigned char);			//transmit hex byte

typedef struct {unsigned a:1,b:1;}flags;
flags flag1,flag2,flag3;

unsigned char count;				//common counter
unsigned char bcount;				//bit counter
unsigned char TIME;					//timer0 delay 
unsigned char current;				//current
unsigned char timecount;			//1/10s time counter for MAXTIME

//CONST--------------------------------------------------------------------CONST
const char m1[]="$Digital Fuse "__DATE__" "__TIME__" ";
const char m2[]="$C:";
const char m3[]="$BU ON";
const char m4[]="$BU OFF";
const char m5[]="$ST ON";
const char m6[]="$ST OFF";
const char m7[]="$FU ON";
const char m8[]="$FU OFF";
const char m9[]="$AU OFF";

//TIMER0------------------------------------------------------------------TIMER0
#pragma interrupt_handler tim0_ovf:iv_TIMER0_OVF
void tim0_ovf(void)							//232 delay
{
 TCNT0 = TIME;
 TIMER0=0;	  								//set flag
}

//TIMER1------------------------------------------------------------------TIMER1
#pragma interrupt_handler tim1_ovf:iv_TIMER1_OVF
void tim1_ovf(void)							//simple delay timer 1/10sec
{
 TCNT1 =61; 
 TIMER1=1;									//set flag
}										//end timer

//MAIN----------------------------------------------------------------------MAIN
void main()
{
 PRR   =(1<<PRUSI);							//switch of USI power
 MCUCR =0;
 DDRB  =(1<<PB1)|(1<<PB3);	   				//PB1,PB3 OUT
 PORTB =(1<<PB0)|(1<<PB4);					//in high
 TIME  =182;								//delay time for 1Mhz / 9600 baud 
 //init ADC
 ADMUX =(1<<REFS2)|(1<<REFS1)|(1<<ADLAR)|(1<<MUX0);//2.56V, left adjust PB2
 ADCSRA=(1<<ADEN) |(1<<ADSC) |(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //scaler 128
 ADCSRB=0;
 DIDR0 =(1<<ADC1D);							//disable digital input for ADC input
 //init timer 
 TIMSK =(1<<TOIE1)|(1<<TOIE0);				//enable int. timer overflow 0,1
 T1ON;
 TXLOW;
 FUSEOFF;					  				//start with fuse off  
 SEI();  	 	  			  				//start
 trs(m1);									//show intro
//LOOP----------------------------------------------------------------------LOOP
 while(1)									//endless loop
 {
  if(ADCSRA & (1<<ADIF))					//ad conversion ready 
  {
   current =ADCH>>1;						//read adc as 8 bit and calc current
/*
 Don't panik, here's the formula to calculate the current:
- Ref voltage for ADC is 2,56 V
- Resolution is 8 bit (ADC high byte, filled with ADLAR)
- BTS datasheet diagram shows 1mA current through IS at 5A, with RIS=1000 Ohm
  that's: 1mA * 1000 Ohm at 5A => 1V at 5A
  So we just have to measure the voltage an multiply it with 5 to get the current!!
  Shouldn't be too difficult:
  U = (ADCH/256)*2,56V => ADCH/100
  I = 5*U => (ADCH/100)*5 => ADCH/20
  So just shift right and display the value with decimal point!! 
  */
   ADCSRA |=(1<<ADIF)|(1<<ADSC);			//reset flag and start again
  }
  if(TIMER1)								//timer 1/10 sec
  {
   if(FUSE)									//if on, show current
   {
    if(timecount < MAXTIME)					//within maxtime 				
    {
     timecount++;
    }
    else
    {
     if(current > MAXCURRENT)				//
     {
      FUSEOFF;
      FUSE =0;
      trs(m9);	  			 				//show auto off
     }
    }
    T1OFF;
    trs(m2);trd(current);					//show current
    T1ON;
   }
   TIMER1 =0;								//reset timer flag
  }	 	 								//end timer 
  
//decode button----------------------------------------------------decode button
//this button toggles the fuse. if fuse is on, actual current is shown
  if(!(PINB & SW1))							//button down
  {
   if(!BUTTON)				   				//first time down
   {
    BUTTON=1;								//set button
    trs(m3);								//show button on
    if(FUSE)								//toggle fuse
    {
     FUSE =0;
     FUSEOFF;
     trs(m8);
    }
    else
    {
     FUSE =1;
     FUSEON;
     timecount=0;							//restart MAXTIME counter
     trs(m7);
    }    
   }
  }
  else	 	   				   				//button up
  {
   if(BUTTON)				   	    		//first time up
   {
    BUTTON=0;				   				//reset switch
    trs(m4);								//show button off
   }
  }						   				//end button
//decode ST------------------------------------------------------------decode ST
//BTS-ST output shows the status of the fuse
  if(!(PINB & ST))							//ST down
  {
   if(!STATUS)				   				//first time down
   {
    STATUS=1;								//set status
    trs(m5);
   }
  }
  else	 	   				   				//ST up
  {
   if(STATUS)				   	    		//first time up
   {
    STATUS=0;				   				//reset status
    trs(m6);
   }
  }						   				//end ST
 }								   		//end loop
} 								   		//end main

//TRANSMIT--------------------------------------------------------------TRANSMIT
//transmit byte 8N1, 9600
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
//transmit const char string
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
//transmit byte as 2 digit hex val (HH)
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
//transmit byte as 3 digit val (###)
void trb(unsigned char w)
{
 transmit((w/100)+0x30);
 w	=w%100;				 		   		 //transmit ##
 transmit((w/10)+0x30);
 transmit((w%10)+0x30);
}

//TRD------------------------------------------------------------------------TRD
//transmit byte as 3 digit val with dec.point (##.#)
void trd(unsigned char w)
{
 transmit((w/100)+0x30);
 w	=w%100;				 		   		 //transmit ##
 transmit((w/10)+0x30);
 transmit('.');		   							 //transmit decimal
 transmit((w%10)+0x30);
}

