//name: tea.c	 		version : 1			function: tee timer (3 Volt) 
//date: 20.02.07		customer: 			target  : ATTiny45 
//Fuse: CKSEL3, CKSEL2, CKSEL0 8Mhz PLL / CKDIV8 = 1 MHz 
#include <iot45v.h>
#include <macros.h>

#define TEESEC 300 			   			  		//teat timer 300 sec
#define TSOUND 5								//sound 5 sec
#define FREQ1  50								//freq1
#define FREQ2  25								//freq2

#pragma interrupt_handler tim1_ovf:iv_TIMER1_OVF
#pragma interrupt_handler pc_int:iv_PCINT0		//use int to wake up

#define LEDOFF PORTB&=~0x10						//LED OFF
#define LEDON  PORTB|= 0x10						//LED ON
#define LEDTOG PORTB^= 0x10						//LED TOGGLE

#define SPON   PORTB&=~0x08						//SPEAKER OFF
#define SPOFF  PORTB|= 0x08						//SPEAKER ON
#define SPTOG  PORTB^= 0x08						//SPEAKER TOGGLE

#define EON	   0x01	   	  		   				//PORTB input signal on
#define EOFF   0x04	   							//PORTB input signal off

#define TON   TCNT1 =0;TCCR1 = (1<<CS13)|(1<<CS12)|(1<<CS10); // /4096
#define TOFF  TCCR1 = 0

#define SOUND flag.a   		   					//sound on
#define PD	  flag.b   		   					//power down
#define FRE	  flag.c   		   					//freq1 or freq2

struct{unsigned a:1,b:1,c:1;}flag;

unsigned char tB,oB;				 			//input B
unsigned int teetime;							//tea time counter
unsigned char soundtime;						//sound time counter
unsigned char frequenz;						//frequenz
unsigned int count;							//common counter

//PC_INT------------------------------------------------------------------PC_INT
void pc_int (void)							//wake up int
{
 PD =0;										//reset power down
}

//TIMER1------------------------------------------------------------------TIMER1
void tim1_ovf(void)
{
 WDR();			   							//reset watchdog
 TCNT1 =12;
 LEDTOG;
 if(++teetime > TEESEC)					//end of teatime
 {
  LEDON;
  SOUND =1;
  if(FRE)  	  		   						//switch back frequenz
  {
   frequenz=FREQ1;							//select freq1
   FRE =0;
  }
  else
  {
   frequenz=FREQ2;							//select freq2
   FRE =1;
  }
  if(++soundtime > TSOUND)					//switch off soundtime
  {
   SOUND =0;
   SPOFF;
   LEDOFF;
   PD =1; 		   							//goto power down
  }
 }
}											//end timer

//MAIN----------------------------------------------------------------------MAIN
void main()
{
 DDRB	=0xF8;	   							//0-2 in
 PORTB	=0x07;								//in high
//timer 1
 TIMSK =(1<<TOIE1);
 TCNT1 =12;
 TON;  	 		   			 	 					
 SPOFF;
 PRR=(1<<PRTIM0)|(1<<PRUSI)|(1<<PRADC);	//shutdown 
 PCMSK =(1<<PCINT1)|(1<<PCINT0);			//enable PC
 GIMSK =(1<<PCIE);							//enable int
 PD =1;										//power down
 WDR();
 WDTCR|=(1<<WDCE)|(1<<WDE);				//enable watchdog change
 WDTCR = 0x09;  							//WDT 0.25s ON
 SEI();  	 	  			  				//start
//LOOP----------------------------------------------------------------------LOOP
 while(1)
 {
  WDR();
  if(SOUND)									//make sound 
  {
   WDR();						
   for(count=0;count<frequenz;count++)NOP();//
   SPTOG;
   WDR();
   for(count=0;count<frequenz;count++)NOP();//     
   LEDTOG;
  }
  tB =PINB & 0x05;					   		//read selected input 
  if(tB != oB)						   		//new?
  {
   if(!(tB & EON))					   		//button on?
   {	  	 								//start timer
    teetime =0;
    soundtime=0;
    TCNT1=12;
    PD =0;
    LEDON;	 								//LED on
   }	 		 							//end button on
   if(!(tB & EOFF))				   		//button off?
   {	  	 								//stop timer
    PD =1;									//enable power down	   
    LEDOFF;									//LED off
   }	  									//end button off
   oB =tB;									//save new input
  }   	 	 						   		//end read port
//POWER_DOWN----------------------------------------------------------POWER_DOWN
//if(0)										//test
  if(PD)							   		//power down
  {
   WDR();	   						   		//reset WDT
   MCUSR &= ~(1<<WDRF);WDTCR|=(1<<WDCE)|(1<<WDE);WDTCR = 0x00;  //stop WDT    
   TOFF;									//stop timer
   LEDOFF;									//LED OFF
   SPOFF;									//speaker off
   FRE =0;
   SOUND =0;
   MCUCR =(1<<SE)|(1<<SM1);   				//set power down mode					   
   asm("sleep");							//sleep
   asm("nop;");asm("nop;");asm("nop;");asm("nop;");asm("nop;");asm("nop;");
   MCUCR =0;								//set normal  mode
   TON;										//restart timer
   WDTCR|=(1<<WDCE)|(1<<WDE);WDTCR = 0x09;//WDT 0.25s ON
   WDR();									//start WDT,Timer
  }								   			//end power down
 }								   			//end loop
} 								   			//end main
