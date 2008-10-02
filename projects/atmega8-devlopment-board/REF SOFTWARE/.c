//*****************************************************************************
// File Name	: extinttest.c
// 
// Title		: example usage of external interrupt library functions
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
// 
// Revision History:
// When			Who			Description of change
// -----------	-----------	-----------------------
// 26-NOV-2007	HsK		Created the program
//*****************************************************************************


//----- Include Files ---------------------------------------------------------
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/signal.h>		// include "signal" names (interrupt names)
#include <avr/interrupt.h>	// include interrupt support


#include "global.h"		// include our global settings
#include "timer.h"		// include timer function library
#include "a2d.h"		// include A/D converter function library
#include "extint.h"		// include external interrupt library


//PRESCALER ADJUSTMENT - DIV1 FOR TIMER0 8 BIT
// CLKSEL 1110 FOR 1 MHz INTERNAL CRYSTAL

// global variables
volatile u16 Int0Count;
volatile u16 Tmr0OverFlowCount;


unsigned int WdtDelay,TotalTime;
unsigned char relayBit; 
// functions

//void rx(unsigned char r);// to be used in uartSetRxHandler(rx);

void extintTest(void);
void myInt0Handler(void);
void myOverflow0Function(void);


void delayx(unsigned long d);


//----- Begin Code ------------------------------------------------------------
int main(void)
{

unsigned int mi=0,mj=0,adata[48];


Int0Count=0;
Tmr0OverFlowCount=0;

WdtDelay=0;
TotalTime=1000;
relayBit=0;

cbi(DDRD,2);sbi(PORTD,2);
sbi(DDRD,4);sbi(PORTD,4);

//DDRB=0xff;PORTB=0xf0;



extintInit();extintConfigure(EXTINT0, EXTINT_EDGE_ANY);
extintAttach(EXTINT0, myInt0Handler);sbi(GICR,INT0);		

a2dInit();DDRC = 0x00;PORTC = 0x00;
a2dSetPrescaler(ADC_PRESCALE_DIV128);a2dSetReference(ADC_REFERENCE_AVCC);

timerInit();timerAttach(TIMER0OVERFLOW_INT,myOverflow0Function);


//MAF initilization  	mi,mj,adata[48] variables 
for(mi=0;mi<=47;mi++)
	{adata[mi]=a2dConvert10bit(0);}

for(mi=0;mi<=46;mi++)
	{adata[mi]=adata[mi+1];}
	adata[47]=a2dConvert10bit(0);
mj=0;
for(mi=0;mi<=47;mi++)
		{mj=mj+adata[mi];}
	mj=mj/48;	
//------------------

while(1)
	{

	/*
	// ADC output with filter	
	for(mi=0;mi<=46;mi++)
		{adata[mi]=adata[mi+1];}
	adata[47]=a2dConvert10bit(0);
	
	mj=0;
	for(mi=0;mi<=47;mi++)
		{mj=mj+adata[mi];}
	mj=mj/48;					
	//mj holds the output
	*/
	
	mj=a2dConvert10bit(0);
	WdtDelay=mj*3;
	
/*
	timerPause(mj*5);
	sbi(PORTD,4);
	timerPause(mj*5);
	cbi(PORTD,4);
*/	
	//if(bit_is_set(PIND,2)==0){}


	}
return 0;
}


void delayx(unsigned long w)
{
unsigned long e=0;
while(w!=0)
	{e++;w--;}
}


void myInt0Handler(void)
{
Int0Count++;

Tmr0OverFlowCount=0;
cbi(PORTD,4);//turn off relay
}


void myOverflow0Function(void)
{
if(Tmr0OverFlowCount>=WdtDelay)
	{
	sbi(PORTD,4);	//turn on relay
	
	Tmr0OverFlowCount=0;
	
	//if(relayBit==1){relayBit=0;sbi(PORTD,4);}
	//else{relayBit=1;cbi(PORTD,4);}
	}
else
	{
	Tmr0OverFlowCount++;	
	}
	
}



//void rx(unsigned char r){}
