/*****************************************************
Project                 : Dimmer
Version                 : 1.0
Date                    : 21/02/2008
Author                  : Ehab Anwar                            
Company                 : BDR Electronics                            
Chip type               : ATmega8L
Program type            : Application
Clock frequency         : 8.000000 MHz
Memory model            : Small
External SRAM size      : 0
Data Stack size         : 256
*****************************************************/

#include <mega8.h>
#include <delay.h>

// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x18 ;PORTB
#endasm
#include <lcd.h>

char LightIntensity=50;

////////////////////////////////////////////ok
void show_number(char a,char c,char temp) //
{                                         //
unsigned char number[3];                  //
char b;                                   //
for(b=0;b<3;b++)                          //
 {                                        //
 number[b]=temp%10;                       //
 temp/=10;                                //
 _lcd_ready();                            //
 lcd_gotoxy(a+1-b,c);                     //
 lcd_putchar(number[b]|0x30);             //
 }                                        //
}                                         //
////////////////////////////////////////////

/////////////////////////////////////////////////////////ok      
// Timer 1 output compare A interrupt service routine  //
interrupt [TIM1_COMPA] void timer1_compa_isr(void)     //
        {                                              //
        PORTD.4=0;                                     //
        }                                              //
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////not used
// Timer 1 output compare B interrupt service routine  //
interrupt [TIM1_COMPB] void timer1_compb_isr(void)     //
        {                                              // 
        }                                              //
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////ok
// Analog Comparator interrupt service routine //
interrupt [ANA_COMP] void ana_comp_isr(void)   //
        {                                      //
        PORTD.4=1;                             //
        TCNT1H=0x00;                           //
        TCNT1L=0x00;                           //
        }                                      //
/////////////////////////////////////////////////

///////////////////////////////////////////
void SetTimers(void)
        {
        OCR1A=((int)(LightIntensity)*100);
        if(LightIntensity>99)
                TCCR1B=0x00;
        else
                TCCR1B=0x02;        
        //OCR1B=((int)(200-LightIntensity)*100);
        }
///////////////////////////////////////////

// Declare your global variables here

void main(void)
{
// Declare your local variables here
//char loop;
// Input/Output Ports initialization
// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0xff;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x11;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
TCCR0=0x00;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 7.813 kHz
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: On
// Compare B Match Interrupt: On
TCCR1A=0x00;
TCCR1B=0x02;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
// OCR1AH=0x13;
// OCR1AL=0x88;
// OCR1BH=0x3a;
// OCR1BL=0x98;

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
TIMSK=0x18;

// Analog Comparator initialization
// Analog Comparator: On
// Interrupt on Output Toggle
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x08;
SFIOR=0x00;

// LCD module initialization
lcd_init(16);

// Global enable interrupts

PORTD.4=1;
SetTimers();
#asm("sei")
lcd_gotoxy(0,0);
lcd_putsf("Light Intensity");
show_number(1,1,LightIntensity);
while (1)
      {
      while(PINC.5==0)
        {
        if(LightIntensity<100)
                LightIntensity++;
        SetTimers();
        lcd_gotoxy(0,0);
        lcd_putsf("Light Intensity");
        show_number(1,1,LightIntensity);
        delay_ms(500);
        }
      while(PINC.2==0)
        {
        if(LightIntensity>1)
                LightIntensity--;
        SetTimers();
        lcd_gotoxy(0,0);
        lcd_putsf("Light Intensity");
        show_number(1,1,LightIntensity);
        delay_ms(500);
        }

      };
}
