/*********************************************
Date    : 3/27/2001                                                          
Author  : Jorge Jaar                      
Company : Tohoku University  Sendai, Japan                   
Comments: This progm is designed to control a DC motor by means of PWM
with feedback from a led/photodiode and serial interface to a computer.

Chip type           : AT90S2313
Clock frequency     : 4.000000 MHz
Memory model        : Tiny
Internal SRAM size  : 128
External SRAM size  : 0
Data Stack size     : 32
*********************************************/

#include <90s2313.h>
#include <delay.h>
#include <stdio.h>
#include <lcd.h> 
#include <stdlib.h>

#asm
   .equ __lcd_port=0x18
#endasm

void main(void)
{
//Local Variables
char 	buf[15],
	srpmi[3],
	srpms[3];

int 	rpms=0,
	rpmsn=0,
	x,
	rpmi=750;

// Input/Output Ports initialization
// Port B
DDRB=0x08;	//00001000
PORTB=0x08;    	//00001000

// Port D
DDRD=0x00;	//00000000
PORTD=0x08;     //00001000  

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 3.906 kHz
// Mode: Output Compare
// OC0 output: Disconnected
TCCR0=0x05;	//00000101
TCNT0=0x00;     //00000000

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 15.625 kHz
// Mode: 8 bit Pulse Width Modulation
// OC1 output: non inv
// Noise Canceler: Off
// Input Capture on Falling Edge
TCCR1A=0x81;	//10000001
TCCR1B=0x04;   	//00000100
TCNT1H=0x00;   	//00000000
TCNT1L=0x00;	//00000000
OCR1H=0x00;     //00000000
OCR1L=0x00;     //00000000

// UART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// UART Receiver: On
// UART Transmitter: On
// UART Baud rate: 9600
UCR=0x18;    	//00011000
UBRR=0x19;	//00011001

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;	//10000000

//intro: LCD module initialization and motor startup
lcd_init(16);
lcd_putsf("  motor driver");
delay_ms(2000);
OCR1L=050;	

while (1)
      {   
          
//get the intended rpms if key pressed POLL UART
        if(USR.7==1)
        {           
       		gets(srpmi,3);          	       		
       		rpmi=atol(srpmi);
		lcd_gotoxy(0,1);
		lcd_puts(srpmi);
	
	};                              


     
//get frequency (rpms)
	rpmsn=0;
      	do{
      		if(PIND.3==0)
      		{            
      			
      			do
      			{}while(PIND.3==0);
      			TCNT0=0;
      			do
      			{}while(PIND.3==1);
      			x=TCNT0;
      			rpmsn=(14000/x);
      		};
      	}while(rpmsn==0);
      
         
//update lcd if data changed 
      	if ((int)(rpms)!=(int)(rpmsn))
      	{ 
      		sprintf(buf, "speed=%urpm",rpmsn);
      		lcd_gotoxy(0,0);
       	       	lcd_puts(buf);
       		rpms=rpmsn;
       		//putchar("r");
       		itoa(rpms, srpms);
       		putchar(srpms); 
       		//make srpmi an int
       	
       			
     	}; 
       
     		OCR1L=OCR1L+((rpmi-rpms)/30);

      	if (OCR1L>250)
       		OCR1L=250;
       	if (OCR1L<10)
       		OCR1L=10;  
       	delay_ms(100);

     		 
};

}
