/*****************************************************
This program was produced by the
CodeWizardAVR V1.24.6 Professional
Automatic Program Generator
© Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com
e-mail:office@hpinfotech.com

Project : TelPad
Version : 0.1
Date    : 7/14/2008
Author  : Srinivasan                     
Company : Scorpion Controls                         
Comments: 
TelPad is a project to use the Look Up Table(LUT) in the realtime environment......
Connect the RS232 cable to the PC and our Kit. Burn the hex file into AtMega8515 IC.
LCD and Matrix Key pad no need. After Programming remove the ISP Cable. You can see the output or HMI on the terminal software
It will scann all the 483 matrix keys by asking you Y/N. If you press 'Y' in terminal
then it assumes that particular key is pressed and it gives the corresponding Character on the Display.


Chip type           : ATmega8515
Program type        : Application
Clock frequency     : 1.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 128
*****************************************************/

#include <mega8515.h>

// Standard Input/Output functions
#include <stdio.h>                      

#define TRUE 1
#define FALSE 0

// Declare your global variables here  
char enter=0x0D; 
const char TelPad_LUT[4][3]={'1','2','3',
                             '4','5','6',
                             '7','8','9',
                             '*','0','#'};  
char getkeycode(char *row,char *col);

void main(void)
{
// Declare your local variables here

char row,col;
char i; 
// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTC=0x00;
DDRC=0xFF;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Port E initialization
// Func2=In Func1=In Func0=In 
// State2=T State1=T State0=T 
PORTE=0x00;
DDRE=0x00;

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

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=0x00;
EMCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud rate: 9600 (Double Speed Mode)
UCSRA=0x02;
UCSRB=0x18;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x0C;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;

while (1)
      { 
      // Place your code here 
      row=0;
      col=0;  
      i= getkeycode(&row,&col); 
      printf("%c",enter); 
      if (i==TRUE)
      {
      printf("%c",enter);
      printf("Key Pressed Is:%c",TelPad_LUT[row][col]);
      }
      else
      {
      printf("%c",enter);
      printf("No key is Pressed");
      }
      

      };
}

char getkeycode(char *row,char *col)
{       
        char i=FALSE; 
        while(*row != 4)  
        {       
                for (*col=0;*col<3;(*col)++)
                {
                        printf("%c",enter);
                        printf("Is the key[%d][%d] pressed?(Y/N)",*row,*col);
                        scanf("%c",&i); 
                        if (i=='Y' || i=='y')
                        { 
                                i=TRUE;
                                return i;
                        }
                        else
                        {   
                                i=FALSE;
                        }
                 }
                 *row=(*row)+1;   
        }
        return i;
        
}

