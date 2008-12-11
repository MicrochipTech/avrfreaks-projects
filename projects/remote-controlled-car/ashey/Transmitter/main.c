// This is the Code For Transmission .....

//   _____________ 
/*1  |            | 28
  2  | A        P | 27
  3  | T        I | 26 (Switch 3)
  4  | M        N | 25 (Switch 2)
  5  | E          | 24 (Switch 1)
  6  | G        C | 23 (Switch 0)
  7  | A        O | 22 (GND)
  8  | 8        N | 21 
  9  |          F | 20
  10 |          I | 19 
  11 |          G | 18
  12 |            | 17
  13 |            | 16 (IR Leds)
  14 |____________| 15        
   
	
	
*/	


#include<avr/io.h>
#include<avr/interrupt.h>
#include<avr/signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "main.h"
#include "test.c"
#include "init.c"

int array3[5]= {1,1,1,1,1}; // on receiver it is 15
int array2[5]= {1,1,0,1,0}; // on receiver it is 5
int array1[5]= {1,0,1,0,0}; // on receiver it is 2
int array0[5]= {1,0,0,1,1}; // on receiver it is 12
int main(void)
{
	unsigned char i = 0 ;
	
	//unsigned int  a=1;
    initDDRs();
	DDRC = 0 ; // As inputs ...
	PORTD  = PORTD | _BV(PD0) | _BV(PD1) ; // Setting all of them to high 
	PORTC  = PORTC | _BV(PC3) | _BV(PC2) | _BV(PC1) | _BV(PC0); // Setting all of them to high
	sei();
	for(;;)
   { 
      while(S3)                      // For When the 3rd Switch is Pressed 
      {                               //Right Switch transmitting 15      
	    StartTransmit(array3);        // start Transmitting 
		for( i = 0 ; i < 10 ; i++ ) // delay of 2 Bytes 
		{
	    	setupTimer2();
		    delay(30,90,105);
		}	
	  }	
	  while(S2)                      // For When the 2nd Switch is Pressed
	  {                               // Switch for Moving Forward  transmitting 5 
	    StartTransmit(array2);        // start Transmitting 
		for( i = 0 ; i < 10 ; i++ ) // delay of 2 Bytes 
		{
	    	setupTimer2();
		    delay(30,90,105);
		}	
	  }
	  while(S1)                      //  For When the 1st Switch is Pressed
	  {                               // For Backward Transmitting 2
	    StartTransmit(array1);        // start Transmitting 
		for( i = 0 ; i < 10 ; i++ ) // delay of 2 Bytes 
		{
	    	setupTimer2();
		    delay(30,90,105);
		}	
	  } 
	  while(S0)                     // For When the 0th Switch is Pressed
	  {                               // For Left Transmitting 12  
	    StartTransmit(array0);        // start Transmitting 
		for( i = 0 ; i < 10 ; i++ ) // delay of 2 Bytes 
		{
	    	setupTimer2();
		    delay(30,90,105);
		}	
	  }
	} 
}
void StartTransmit( int *array )
{  
    unsigned int i = 0;
    for(i=0;i<5;i++)
	{
	  if(array[i])
	   {
	    setupTimer1();
		SetIR(30,90,105);
		PORTB = PORTB & ~(_BV(IR));
	   }
	  else 
	   {
        setupTimer2();  
	    delay(30,90,105);
		PORTB = PORTB & ~(_BV(IR));
	   }
		//transmit = transmit/2;
	}
	   //setupTimer1();
	   //SetIR(30,70,105); 
}
