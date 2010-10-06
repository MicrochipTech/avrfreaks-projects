/*****************************************************
Copyright (c) 2009 by ali hooshangian all rights reserved
Compiler: CodeWizardAVR V1.25.8 Standard
This program display number: 1234 simultaneously although the main process trapped into the infinite loop.

Chip type           : ATmega8L
Program type        : Application
Clock frequency     : 1.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*****************************************************/

#include <mega8.h>
#include <delay.h>
#include <stdio.h>
//#include <string.h>

// Global Vars.
unsigned char raster=0;
unsigned char d[4]={0xFF,0xFF,0xFF,0xFF}; //blanked
char flash chrdata[12]={0x03 , 0x9F , 0x25 , 0x0D , 0x99 , 0x49 , 0x41 , 0x1B , 0x01 , 0x09};
//unsigned int strlen(char *str);
//void ltoa(unsigned int,char *ss);



// Timer 0 overflow interrupt service routine

interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
// Place your code here
PORTC&=0b11000011;
raster++;
if (raster==4)raster=0;
PORTD=d[raster];
switch (raster) {
    case 0:PORTC.2=1;break;
    case 1:PORTC.3=1;break;
    case 2:PORTC.4=1;break;
    case 3:PORTC.5=1;break;
    }
}
// Declare your global variables here


/*     Turn7SegOn7Seg Performance:     */
/*  1820 LOOP PER SECOND @ 1 MHz */
void Disp7Seg(unsigned int number){
    char ss[5];
    sprintf(ss, "%04u", number);
    //itoa(number,ss);
    d[0]=chrdata[ss[0]-48]; //lookup table
    d[1]=chrdata[ss[1]-48]; //lookup table
    d[2]=chrdata[ss[2]-48]; //lookup table
    d[3]=chrdata[ss[3]-48]; //lookup table
}
#define ADC_VREF_TYPE 0xC0

void main(void){
// Declare your local variables here
unsigned int Wcnt;
//unsigned int number;
// Input/Output Ports initialization
// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
PORTB=0x06;
DDRB=0x06;

// Port C initialization
// Func6=In Func5=Out Func4=Out Func3=Out Func2=In Func1=In Func0=In
// State6=T State5=1 State4=1 State3=1 State2=T State1=T State0=T
//PORTC=0x38;
PORTC=0b00111100;
DDRC=0x38;

// Port D initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out
// State7=1 State6=1 State5=1 State4=1 State3=1 State2=1 State1=1 State0=1
PORTD=0x03;
DDRD=0xFF;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 3.906 kHz
TCCR0=0x02;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer 1 Stopped
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
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
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x01;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// Global enable interrupts
#asm("sei")

Wcnt=1234;
Disp7Seg(Wcnt);
while (1) {
      // Place your code here
      }
}
