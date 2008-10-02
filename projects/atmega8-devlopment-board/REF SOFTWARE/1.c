//*****************************************************************************
// File Name	: main.c
// 
// Title		: Atmega8 Analog Meter Demonstration
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
// 
// Revision History:
// When			Who			Description of change
// -----------	-----------	-----------------------
// 14-Dec-2004	HsK		Created the program
//*****************************************************************************


//----- Include Files ---------------------------------------------------------
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/signal.h>		// include "signal" names (interrupt names)
#include <avr/interrupt.h>	// include interrupt support


#include "global.h"		// include our global settings
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "timer.h"		// include timer function library
#include "a2d.h"		// include A/D converter function library
#include "extint.h"		// include external interrupt library

#define IGNORE_PROX 2

#define MAF1_LENGTH 64
unsigned char initMAF1(unsigned int dPoint);
unsigned int MAF1(unsigned int dPoint);
unsigned int MAF1_Buffer[MAF1_LENGTH],MAF1_cntr,MAF1_ptr,MAF1_output;
unsigned char MAF1_var;

#define MAF2_LENGTH 64
unsigned char initMAF2(unsigned int dPoint);
unsigned int MAF2(unsigned int dPoint);
unsigned int MAF2_Buffer[MAF2_LENGTH],MAF2_cntr,MAF2_ptr,MAF2_output;
unsigned char MAF2_var;

#define MAF3_LENGTH 8
unsigned char initMAF3(unsigned int dPoint);
unsigned int MAF3(unsigned int dPoint);
unsigned int MAF3_Buffer[MAF3_LENGTH],MAF3_cntr,MAF3_ptr,MAF3_output;
unsigned char MAF3_var;

#define MAF4_LENGTH 4
unsigned char initMAF4(unsigned int dPoint);
unsigned int MAF4(unsigned int dPoint);
unsigned int MAF4_Buffer[MAF4_LENGTH],MAF4_cntr,MAF4_ptr,MAF4_output;
unsigned char MAF4_var;



// global variables
volatile u16 Int0Count;
unsigned char ga,flag,tdx,igcntr,txByte,prev_txByte,fflag;
unsigned int gx;
unsigned char tmp1,tmp2;
unsigned char opMode,waste,testRunnig;



// functions
void extintTest(void);
void myInt0Handler(void);
void rx(unsigned char r);

void delayx(unsigned long d);


void opRelay(unsigned char rnum,unsigned char state);
unsigned char lmSwitch(unsigned char swnum);

void start_machine(void);
void stop_machine(void);

//----- Begin Code ------------------------------------------------------------
int main(void)
{
unsigned int mk;
unsigned char temp1,temp2;

txByte=0;igcntr=0;
ga=0;gx=0;tdx=0;flag=0;
opMode=0;
testRunnig=0;fflag=0;

sbi(DDRB,2);sbi(DDRB,1);sbi(DDRD,5);sbi(DDRD,6);sbi(DDRD,7);
opRelay(1,0);opRelay(2,0);opRelay(3,0);opRelay(4,0);opRelay(5,0);

cbi(DDRB,0);cbi(DDRD,3);cbi(DDRD,4);
sbi(PORTB,0);sbi(PORTD,3);sbi(PORTD,4);

uartInit();uartSetBaudRate(115200);uartSetRxHandler(rx);//rprintfInit(uartSendByte);

timerInit();


extintInit();extintConfigure(EXTINT0, EXTINT_EDGE_RISING);
extintAttach(EXTINT0, myInt0Handler);sbi(GICR,INT0);		

a2dInit();DDRC = 0x00;PORTC = 0x00;
a2dSetPrescaler(ADC_PRESCALE_DIV128);a2dSetReference(ADC_REFERENCE_256V);


//while(lmSwitch(1)!=0){}
//opRelay(5,1);	//motor start
//if(lmSwitch(1)==0){opRelay(5,1);}
//	else{opRelay(5,0);}
/*
while(1)
	{
	start_machine();
	timerPause(3000);
	stop_machine();
	timerPause(3000);
	
	}
*/

//start_machine();

MAF1_ptr=0;
while(initMAF1(a2dConvert8bit(0))!=1){}

	
MAF2_ptr=0;
while(initMAF2(MAF1(a2dConvert8bit(0)))!=1){}

while(1)
	{
	mk=	MAF2(MAF1(a2dConvert8bit(0)));//data is continously acquired all the time				

	temp1=a2dConvert8bit(1);
	temp2=a2dConvert8bit(2);

	mk=255-mk;
	//if(mk>ga){ga=mk;}
	//ga=mk;uartSendByte(ga);
	//ga=(unsigned char)(mk&0x00ff);uartSendByte(ga);
	
	/*
	if(flag==1 && tdx==1)
		{
		flag=0;			// To convert tx on next interrupt from prox.
		
		uartSendByte(0x05);uartSendByte(0);	
		uartSendByte(0);//hb
		txByte=(unsigned char)(gx&0x00ff);uartSendByte(txByte);	//lb
		uartSendByte(temp1);//t1
		uartSendByte(temp2);//t2
				
		uartSendByte(0);uartSendByte(0);//reserved for future use
		}
	*/
	
	
	if(opMode=='S')	//start new test
		{
		//operate relays
		start_machine();
		igcntr=0;
		opMode='!';	//opMode is Free, previous task executed successfully.
		testRunnig=1;
		}

	
	if(opMode=='M')	//start new test with filter
		{
		//operate relays
		start_machine();
		igcntr=0;
		MAF4_var=0;MAF4_ptr=0;fflag=0;
		opMode='!';	//opMode is Free, previous task executed successfully.
		testRunnig=2;
		}


	if(opMode=='C')	//start calibration
		{
		//operate relays
		start_machine();
		igcntr=0;
		MAF4_var=0;MAF4_ptr=0;
		opMode='!';	//opMode is Free, previous task executed successfully.
		testRunnig=3;
		}


	
	if(opMode=='E')	//end test
		{
		//operate relays
		stop_machine();
		opMode='!';	//opMode is Free, previous task executed successfully.
		testRunnig=0;
		}
		
	
	if(testRunnig==1)
		{
		
		if(mk>ga){ga=mk;}
		
		if(flag==1)
			{
			flag=0;			// To convert tx on next interrupt from prox.
				
			uartSendByte(txByte);//lb
			uartSendByte(temp1);//t1
			uartSendByte(temp2);//t2
			}
		}


		
	if(testRunnig==2)
		{
		
		if(mk>ga){ga=mk;}
		
		if(flag==1)
			{
			flag=0;			// To convert tx on next interrupt from prox.
			
			if(MAF4_var==0)
				{MAF4_var=initMAF4(txByte);}
			else
				{
				txByte=MAF4(txByte);
				
				
				if(fflag==0){prev_txByte=txByte;fflag=1;}
				else
					{
					if(txByte>(prev_txByte+10) || txByte<(prev_txByte-10))
						{txByte=prev_txByte;}
					else
						{prev_txByte=txByte;}
					}
				
				
				uartSendByte(txByte);//lb
				timerPause(20);
				uartSendByte(temp1);//t1
				timerPause(20);
				uartSendByte(temp2);//t2
				
				}	
			}
		}


		
	if(testRunnig==3)
		{
		
		if(mk>ga){ga=mk;}
		
		if(flag==1)
			{
			flag=0;			// To convert tx on next interrupt from prox.
			
			if(MAF4_var==0)
				{MAF4_var=initMAF4(txByte);}
			else
				{
				txByte=MAF4(txByte);
				
				uartSendByte(txByte);//lb
				//uartSendByte(temp1);//t1
				//uartSendByte(temp2);//t2
				}	
			}
		}


		
	}
return 0;
}


void delayx(unsigned long w)
{
unsigned long e=0;
while(w!=0)
	{e++;w--;}

}

void start_machine(void)
{
opRelay(1,0);opRelay(2,0);opRelay(3,0);opRelay(4,0);opRelay(5,0);timerPause(1000);
opRelay(4,1);timerPause(1000);	//hold router

opRelay(1,1);timerPause(2000);		//shield down
opRelay(1,0);

opRelay(3,1);		//die down	
timerPause(3000);
//while(lmSwitch(1)==1){}
opRelay(5,1);	//motor start
}
void stop_machine(void)
{
opRelay(5,0);timerPause(1000);	//motor stop

opRelay(3,0);timerPause(2000);		//die up	
opRelay(4,0);timerPause(2000);		//release router

opRelay(2,1);timerPause(3000);		//shield up
opRelay(2,0);		

}


void opRelay(unsigned char rnum,unsigned char state)
{
switch(rnum)
	{
	case 1:
		{
		if(state==1){sbi(PORTB,2);}
		else{cbi(PORTB,2);}
		}
	break;

	case 2:
		{
		if(state==1){sbi(PORTB,1);}
		else{cbi(PORTB,1);}
		
		}
	break;

	case 3:
		{
		if(state==1){sbi(PORTD,5);}
		else{cbi(PORTD,5);}
		
		}
	break;

	case 4:
		{
		if(state==1){sbi(PORTD,6);}
		else{cbi(PORTD,6);}
		
		}
	break;

	case 5:
		{
		if(state==1){sbi(PORTD,7);}
		else{cbi(PORTD,7);}

		}
	break;
	
	
	}
}

unsigned char lmSwitch(unsigned char swnum)
{
switch(swnum)
	{
	case 1:
	{return(bit_is_set(PINB,0));}
	break;
	
	case 2:
	{return(bit_is_set(PIND,3));}
	break;

	case 3:
	{return(bit_is_set(PIND,4));}
	break;
	
	}
return(2);	//error	
}

void rx(unsigned char r)
{
opMode=r;

/*
if(r=='1')
{tdx=1;igcntr=0;}

if(r=='2')
{flag=0;tdx=0;}
*/

if(r=='A')
	{uartSendByte('A');}
}


void myInt0Handler(void)
{
if(igcntr<=IGNORE_PROX)
	{igcntr++;MAF3_var=0;MAF3_ptr=0;flag=0;goto DE;}

if(MAF3_var==0)
	{MAF3_var=initMAF3(ga);ga=0;flag=0;}
else
	{txByte=MAF3(ga);ga=0;flag=1;}	
DE:
waste=0;
}


//Filter Subroutines

/*
--------------------------------MAF1-------------------------------------------------------------
Uses MAF1_LENGTH Macro,unsigned int MAF1_Buffer[64], MAF1_cntr,MAF1_var,MAF1_ptr,MAF1_output
To init check output for 1(marks success), till then loop in while() and feed new value to dPoint
Set MAF1_ptr to zero before executing initMAF1
*/
unsigned char initMAF1(unsigned int dPoint)  
{
if(MAF1_ptr<=(MAF1_LENGTH-1))
	{
	MAF1_Buffer[MAF1_ptr]=dPoint;
	MAF1_ptr++;
	return(0);
	}
else
	{
	return(1);
	}	
}


unsigned int MAF1(unsigned int dPoint)
{
	for(MAF1_cntr=0;MAF1_cntr<=(MAF1_LENGTH-1)-1;MAF1_cntr++)
		{MAF1_Buffer[MAF1_cntr]=MAF1_Buffer[MAF1_cntr+1];}
		
	MAF1_Buffer[MAF1_LENGTH-1]=dPoint;

	
	MAF1_output=0;
	for(MAF1_cntr=0;MAF1_cntr<=(MAF1_LENGTH-1);MAF1_cntr++)
		{MAF1_output=MAF1_output+MAF1_Buffer[MAF1_cntr];}
	MAF1_output=MAF1_output/MAF1_LENGTH;					
return(MAF1_output);
}



/*
--------------------------------MAF2-------------------------------------------------------------
Uses MAF2_LENGTH Macro,unsigned int MAF2_Buffer[MAF2_LENGTH], MAF2_cntr,MAF2_var,MAF2_ptr,MAF2_output
To init check output for 1(marks success), till then loop in while() and feed new value to dPoint
Set MAF2_ptr to zero before executing initMAF2
*/
unsigned char initMAF2(unsigned int dPoint)  
{
if(MAF2_ptr<=(MAF2_LENGTH-1))
	{
	MAF2_Buffer[MAF2_ptr]=dPoint;
	MAF2_ptr++;
	return(0);
	}
else
	{
	return(1);
	}	
}


unsigned int MAF2(unsigned int dPoint)
{
	for(MAF2_cntr=0;MAF2_cntr<=(MAF2_LENGTH-1)-1;MAF2_cntr++)
		{MAF2_Buffer[MAF2_cntr]=MAF2_Buffer[MAF2_cntr+1];}
		
	MAF2_Buffer[MAF2_LENGTH-1]=dPoint;

	
	MAF2_output=0;
	for(MAF2_cntr=0;MAF2_cntr<=(MAF2_LENGTH-1);MAF2_cntr++)
		{MAF2_output=MAF2_output+MAF2_Buffer[MAF2_cntr];}
	MAF2_output=MAF2_output/MAF2_LENGTH;					
return(MAF2_output);
}




/*
--------------------------------MAF3-------------------------------------------------------------
Uses MAF3_LENGTH Macro,unsigned int MAF3_Buffer[MAF3_LENGTH], MAF3_cntr,MAF3_var,MAF3_ptr,MAF3_output
To init check output for 1(marks success), till then loop in while() and feed new value to dPoint
Set MAF3_ptr to zero before executing initMAF3
*/
unsigned char initMAF3(unsigned int dPoint)  
{
if(MAF3_ptr<=(MAF3_LENGTH-1))
	{
	MAF3_Buffer[MAF3_ptr]=dPoint;
	MAF3_ptr++;
	return(0);
	}
else
	{
	return(1);
	}	
}


unsigned int MAF3(unsigned int dPoint)
{
	for(MAF3_cntr=0;MAF3_cntr<=(MAF3_LENGTH-1)-1;MAF3_cntr++)
		{MAF3_Buffer[MAF3_cntr]=MAF3_Buffer[MAF3_cntr+1];}
		
	MAF3_Buffer[MAF3_LENGTH-1]=dPoint;

	
	MAF3_output=0;
	for(MAF3_cntr=0;MAF3_cntr<=(MAF3_LENGTH-1);MAF3_cntr++)
		{MAF3_output=MAF3_output+MAF3_Buffer[MAF3_cntr];}
	MAF3_output=MAF3_output/MAF3_LENGTH;					
return(MAF3_output);
}




/*
--------------------------------MAF4-------------------------------------------------------------
Uses MAF4_LENGTH Macro,unsigned int MAF4_Buffer[MAF4_LENGTH], MAF4_cntr,MAF4_var,MAF4_ptr,MAF4_output
To init check output for 1(marks success), till then loop in while() and feed new value to dPoint
Set MAF4_ptr to zero before executing initMAF4
*/
unsigned char initMAF4(unsigned int dPoint)  
{
if(MAF4_ptr<=(MAF4_LENGTH-1))
	{
	MAF4_Buffer[MAF4_ptr]=dPoint;
	MAF4_ptr++;
	return(0);
	}
else
	{
	return(1);
	}	
}


unsigned int MAF4(unsigned int dPoint)
{
	for(MAF4_cntr=0;MAF4_cntr<=(MAF4_LENGTH-1)-1;MAF4_cntr++)
		{MAF4_Buffer[MAF4_cntr]=MAF4_Buffer[MAF4_cntr+1];}
		
	MAF4_Buffer[MAF4_LENGTH-1]=dPoint;

	
	MAF4_output=0;
	for(MAF4_cntr=0;MAF4_cntr<=(MAF4_LENGTH-1);MAF4_cntr++)
		{MAF4_output=MAF4_output+MAF4_Buffer[MAF4_cntr];}
	MAF4_output=MAF4_output/MAF4_LENGTH;					
return(MAF4_output);
}


//-----------------

