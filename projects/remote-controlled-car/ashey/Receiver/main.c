// This is the Receiver Code
// Motor Inputs:  ---
/*
    ATmega8                           L293D
    Motor 1 :                      Pins :
	    PD0 and PD3                   PIN2 and PIN7 
    Motor 2 :                      Pins : 
        PD2 and PB6                   PIN15 and PIN10

*/
#include<stdio.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include<avr/signal.h>
#include"definitions.h"
#include"functions.c"

int check[8] = { 2 , 4 , 8 , 16 , 32 , 128 ,64 ,32 };
unsigned char Time=0;
void FindTransmitted1(int receive);
void FindTransmitted2(int receive);
void TurnMotors(int count);
int  FindFirst(void);
void InitDDRs(void);
void delay(unsigned char value , unsigned char OCR_value);
void SetupTimer(void)
{        
    TCCR1B = TCCR1B | _BV(WGM12) | _BV(CS10) ; 
	TIMSK  = TIMSK  | _BV(OCIE1A) ;

}
int main(void)
{
   unsigned char i=0;
   int receive = 0 ;
    InitDDRs();
	sei();
	PORTB = PORTB & ~(_BV(PB1));
    for(;;)
    {
	    //PORTC  =  PORTC | _BV(PC0); 
		if(FindFirst())
		{
		    receive = FindFirst();
			if(receive <= 5 )
			{
			    //PORTB = PORTB | _BV(PB1) ;
				if(!(PINC & check[receive-1])) // Checking for the startBit
					{
					 // PORTB = PORTB | _BV(PB1) ;
					  SetupTimer();
					  delay(3,105);
				//	  PORTB = PORTB & ~(_BV(PB1));
					  FindTransmitted1(receive); 
					}
			}
            else 
            {
                if(!(PIND & check[receive-1])) // Checking for the startBit
					{
					  SetupTimer();
					  delay(3,105);
					//  PORTB = PORTB & ~(_BV(PB1));
					  FindTransmitted2(receive); 
				    }
			}
			for(i=0;i<9;i++)
			{
			  SetupTimer();  
			  delay(117,105);
			}		
		}	
		else 
		{
			PORTD = PORTD & ~(_BV(PD0)) & ~(_BV(PB2)) & ~(_BV(PD3));  
			PORTB = PORTB & ~(_BV(PB6)) & ~(_BV(PB1));
		}	
    } 
}

void FindTransmitted1(int receive)
{
   
    unsigned char i=0,count=0;
	PORTD = PORTD & ~(_BV(PD0)) & ~(_BV(PB2)) & ~(_BV(PD3));  
	PORTB = PORTB & ~(_BV(PB6));
	SetupTimer();
    delay(138,105);
	for(i=0;i<4;i++)
	{
	    //PORTB = PORTB | _BV(PB1);
		SetupTimer();
		delay(2,105);
		//PORTB = PORTB | _BV(PB1) ;
		if(!(PINC & check[receive-1]))
		{count = count | 1 << i ;} 
		//PORTB = PORTB & ~(_BV(PB1));  
		SetupTimer();
		delay(118,105); 
	}
	TurnMotors(count);
	count = 0;	
}
void FindTransmitted2(int receive)
{
    unsigned char i=0,count=0;
	PORTD = PORTD & ~(_BV(PD0)) & ~(_BV(PB2)) & ~(_BV(PD3));  
	PORTB = PORTB & ~(_BV(PB6));
	SetupTimer();
    delay(138,105);
	for(i=0;i<4;i++)
	{
	    //PORTB = PORTB | _BV(PB1);
		SetupTimer();
		delay(2,105);
		//PORTB = PORTB | _BV(PB1) ;
		if(!(PIND & check[receive-1]))
		{count = count | 1 << i ;} 
	//	PORTB = PORTB & ~(_BV(PB1));  
		SetupTimer();
		delay(118,105); 
	}
	TurnMotors(count);
	count = 0;

}
void delay(unsigned char value , unsigned char OCR_value)
{   
    OCR1A  = OCR_value;
    Time   = value;
	while(TCCR1B!=0);
}
INTERRUPT(SIG_OUTPUT_COMPARE1A)
{
    unsigned  static  char cycles = 0; 
	if(cycles == Time)
	{
	   cycles =  0;
	   TCCR1B =  0;
	}
    else cycles ++;   
}


