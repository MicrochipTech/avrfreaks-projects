/*****************************************************
This program was produced by the
CodeWizardAVR V2.03.4 Standard
Automatic Program Generator
© Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 2/11/2009
Author  : 
Company : 
Comments: 


Chip type           : ATmega32
Program type        : Application
Clock frequency     : 8.000000 MHz
Memory model        : Small
External RAM size   : 0
Data Stack size     : 512
*****************************************************/
//#define DEBUG
#define DEBUG2

#include <mega32.h>
#include <delay.h>

// Standard Input/Output functions
#include <stdio.h>

unsigned char run=3;
float time=0;
void getDist();
void sendData();

// Timer 1 input capture interrupt service routine
interrupt [TIM1_CAPT] void timer1_capt_isr(void)
{
    #ifdef DEBUG
       printf("\n\rICP");
    #endif
    if(run == 0)
    {
        #ifdef DEBUG
            printf("\n\rStart");
        #endif
        TCCR1B |= 0x02; //Start Timer 1
        TCCR1B &= 0xBF; //Falling edge 
        run = 1; 
        #ifdef DEBUG  
            printf("\n\r1Run = %d",run);  
        #endif   
    }
    else if(run == 1)
    {   
        time = TCNT1;                
        TCCR1B &= 0xF8; // Disable Timer 1
        TCNT1 = 0x0000; //Clear Timer 1          
        TIMSK &= 0xDF; // Disable ICP   
        DDRD |= 0x40; //Set PIND.6 as out
        TCCR1B |= 0x40; //Rising edge         
        run = 3;   
        #ifdef DEBUG        
            printf("\n\rEND TCNT = %f 2Run = %d",time,run);  
        #endif      
        #ifdef DEBUG2
            sendData();
        #endif                
    }
    else
    {
        #ifdef DEBUG
            printf("\n\rrun = %d",run); 
        #endif 
        run = 0;
        #ifdef DEBUG
            printf("\n\rrun = %d",run);
        #endif
    }
}

// Declare your global variables here

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=Out 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=0 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x40;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=FFh
// OC0 output: Disconnected
TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 1000 kHz
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Rising Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: On
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x40; //Initially disable TMR1
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer 2 Stopped
// Mode: Normal top=FFh
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=0x00;
MCUCSR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=0x00;
UCSRB=0x18;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x33;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// Global enable interrupts
#asm("sei")
printf("Welcome");

while (1)
      {
        delay_ms(500);
        getDist();   
        #ifdef DEBUG
            printf("\n\r3Run = %d",run);
        #endif             
      };
}

void getDist()
{
    #ifdef DEBUG2
        printf("\n\rGET");
    #endif
    // Output Burst signal  
    TIMSK &= 0xDF; // Disable ICP           //Problem2--
    DDRD |= 0x40; //Set PIND.6 as out       //--Problem2 
    PORTD |= 0x40;
    delay_us(5);      
    PORTD.6 &= 0xBF; 
    DDRD &= 0xBF; // Set PORTD.6 as in        
    PORTD &= 0xBF; //Tri-state                       
    run = 2;  
    #ifdef DEBUG
        printf("\n\r4Run = %d",run);
    #endif
    TIMSK |= 0x20; // Enable ICP  //Problem1   
}

void sendData()
{
    printf("\n\rTCNT1 = %u -- Distance = %f",TCNT1,(float)(time*340.0/2000000.0));
}