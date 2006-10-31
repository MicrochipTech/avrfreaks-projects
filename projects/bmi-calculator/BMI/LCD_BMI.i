/*****************************************************
This program was produced by the
CodeWizardAVR V1.24.8d Professional
Automatic Program Generator
© Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 12-10-2006
Author  : F4CG                            
Company : F4CG                            
Comments: 


Chip type           : ATmega128
Program type        : Application
Clock frequency     : 4,000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 1024
*****************************************************/
// CodeVisionAVR C Compiler
// (C) 1998-2004 Pavel Haiduc, HP InfoTech S.R.L.
// I/O registers definitions for the ATmega128
#pragma used+
sfrb PINF=0;
sfrb PINE=1;
sfrb DDRE=2;
sfrb PORTE=3;
sfrb ADCL=4;
sfrb ADCH=5;
sfrw ADCW=4;      // 16 bit access
sfrb ADCSRA=6;
sfrb ADMUX=7;
sfrb ACSR=8;
sfrb UBRR0L=9;
sfrb UCSR0B=0xa;
sfrb UCSR0A=0xb;
sfrb UDR0=0xc;
sfrb SPCR=0xd;
sfrb SPSR=0xe;
sfrb SPDR=0xf;
sfrb PIND=0x10;
sfrb DDRD=0x11;
sfrb PORTD=0x12;
sfrb PINC=0x13;
sfrb DDRC=0x14;
sfrb PORTC=0x15;
sfrb PINB=0x16;
sfrb DDRB=0x17;
sfrb PORTB=0x18;
sfrb PINA=0x19;
sfrb DDRA=0x1a;
sfrb PORTA=0x1b;
sfrb EECR=0x1c;
sfrb EEDR=0x1d;
sfrb EEARL=0x1e;
sfrb EEARH=0x1f;
sfrw EEAR=0x1e;   // 16 bit access
sfrb SFIOR=0x20;
sfrb WDTCR=0x21;
sfrb OCDR=0x22;
sfrb OCR2=0x23;
sfrb TCNT2=0x24;
sfrb TCCR2=0x25;
sfrb ICR1L=0x26;
sfrb ICR1H=0x27;
sfrw ICR1=0x26;   // 16 bit access
sfrb OCR1BL=0x28;
sfrb OCR1BH=0x29;
sfrw OCR1B=0x28;  // 16 bit access
sfrb OCR1AL=0x2a;
sfrb OCR1AH=0x2b;
sfrw OCR1A=0x2a;  // 16 bit access
sfrb TCNT1L=0x2c;
sfrb TCNT1H=0x2d;
sfrw TCNT1=0x2c;  // 16 bit access
sfrb TCCR1B=0x2e;
sfrb TCCR1A=0x2f;
sfrb ASSR=0x30;
sfrb OCR0=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0=0x33;
sfrb MCUCSR=0x34;
sfrb MCUCR=0x35;
sfrb TIFR=0x36;
sfrb TIMSK=0x37;
sfrb EIFR=0x38;
sfrb EIMSK=0x39;
sfrb EICRB=0x3a;
sfrb RAMPZ=0x3b;
sfrb XDIV=0x3c;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-
// Interrupt vectors definitions
// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x15 ;PORTC
#endasm
/* LCD driver routines

  CodeVisionAVR C Compiler
  (C) 1998-2003 Pavel Haiduc, HP InfoTech S.R.L.

  BEFORE #include -ING THIS FILE YOU
  MUST DECLARE THE I/O ADDRESS OF THE
  DATA REGISTER OF THE PORT AT WHICH
  THE LCD IS CONNECTED!

  EXAMPLE FOR PORTB:

    #asm
        .equ __lcd_port=0x18
    #endasm
    #include <lcd.h>

*/
#pragma used+
void _lcd_ready(void);
void _lcd_write_data(unsigned char data);
// write a byte to the LCD character generator or display RAM
void lcd_write_byte(unsigned char addr, unsigned char data);
// read a byte from the LCD character generator or display RAM
unsigned char lcd_read_byte(unsigned char addr);
// set the LCD display position  x=0..39 y=0..3
void lcd_gotoxy(unsigned char x, unsigned char y);
// clear the LCD
void lcd_clear(void);
void lcd_putchar(char c);
// write the string str located in SRAM to the LCD
void lcd_puts(char *str);
// write the string str located in FLASH to the LCD
void lcd_putsf(char flash *str);
// initialize the LCD controller
unsigned char lcd_init(unsigned char lcd_columns);
#pragma used-
#pragma library lcd.lib
// CodeVisionAVR C Compiler
// (C) 1998-2000 Pavel Haiduc, HP InfoTech S.R.L.
#pragma used+
void delay_us(unsigned int n);
void delay_ms(unsigned int n);
#pragma used-
// Declare your global variables here
 int hoejde=150;
int vaegt=700;
void SplitInto1_2Digitsh(unsigned int Split)
{
unsigned char digit[3];
//Split=(Split*10)/3;
//digit[0]=(Split/1000);
digit[0]=(Split/100);		// 100'ere
digit[1]=(Split/10)%10;		// 10'ere
digit[2]=Split%10;		// 1'ere
lcd_putchar(digit[0]+48);
lcd_putsf(".");
lcd_putchar(digit[1]+48);
lcd_putchar(digit[2]+48);
lcd_putsf(" meter");
Split=0;
}        
void SplitInto1_2Digitsv(unsigned int Split)
{
unsigned char digit[3];
//Split=(Split*10)/3;
//digit[0]=(Split/1000);
digit[0]=(Split/100);		// 100'ere
digit[1]=(Split/10)%10;		// 10'ere
digit[2]=Split%10;		// 1'ere
lcd_putchar(digit[0]+48);
lcd_putchar(digit[1]+48);
lcd_putsf(".");
lcd_putchar(digit[2]+48);
lcd_putsf(" kilo");
Split=0;
}
          void SplitInto4Digits(unsigned int Split)
{
	unsigned int digit[4];
	digit[0]=(Split/1000);		// 100'ere
	digit[1]=(Split/100)%10;		// 10'ere
	digit[2]=(Split/10)%10;		// 10'ere
	digit[3]=Split%10;		// 1'ere	
	lcd_putchar(digit[0]+48);
	lcd_putchar(digit[1]+48);
	lcd_putsf(".");
	lcd_putchar(digit[2]+48);
        lcd_putchar(digit[3]+48);
}
void SplitInto10Digits(float Split)
{
	float digit[5];
	digit[0]=(Split/100000)%10;		//      100.000'ere
	digit[1]=(Split/10000)%10;		//       10.000'ere
	digit[2]=(Split/1000)%10;		//         1000'ere
	digit[3]=(Split/100)%10;		//          100'ere
	digit[4]=(Split/10)%10;		        //           10'ere
	digit[5]=Split%10;		        //            1'ere	
	lcd_putchar(digit[0]+48);
	lcd_putchar(digit[1]+48);
	lcd_putchar(digit[2]+48);
        lcd_putchar(digit[3]+48);
        lcd_putchar(digit[4]+48);
        lcd_putchar(digit[5]+48);
        }
void udregnet_bmi(float bmi)
        {
        lcd_gotoxy(7,1);
        bmi=vaegt/((hoejde/100.0)*(hoejde/100.0))*10.0;
        SplitInto4Digits(bmi);
        }  
void main(void)
{
// Declare your local variables here
// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=Out Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=0 State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x40;
// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
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
DDRD=0x00;
// Port E initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTE=0x00;
DDRE=0x00;
// Port F initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
(*(unsigned char *) 0x62)=0x00;
(*(unsigned char *) 0x61)=0x00;
// Port G initialization
// Func4=In Func3=In Func2=In Func1=In Func0=In 
// State4=T State3=T State2=T State1=T State0=T 
(*(unsigned char *) 0x65)=0x00;
(*(unsigned char *) 0x64)=0x00;
// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=FFh
// OC0 output: Disconnected
ASSR=0x00;
TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;
// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer 1 Stopped
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// OC1C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
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
(*(unsigned char *) 0x79)=0x00;
(*(unsigned char *) 0x78)=0x00;
// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer 2 Stopped
// Mode: Normal top=FFh
// OC2 output: Disconnected
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;
// Timer/Counter 3 initialization
// Clock source: System Clock
// Clock value: Timer 3 Stopped
// Mode: Normal top=FFFFh
// Noise Canceler: Off
// Input Capture on Falling Edge
// OC3A output: Discon.
// OC3B output: Discon.
// OC3C output: Discon.
// Timer 3 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
(*(unsigned char *) 0x8b)=0x00;
(*(unsigned char *) 0x8a)=0x00;
(*(unsigned char *) 0x89)=0x00;
(*(unsigned char *) 0x88)=0x00;
(*(unsigned char *) 0x81)=0x00;
(*(unsigned char *) 0x80)=0x00;
(*(unsigned char *) 0x87)=0x00;
(*(unsigned char *) 0x86)=0x00;
(*(unsigned char *) 0x85)=0x00;
(*(unsigned char *) 0x84)=0x00;
(*(unsigned char *) 0x83)=0x00;
(*(unsigned char *) 0x82)=0x00;
// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
// INT3: Off
// INT4: Off
// INT5: Off
// INT6: Off
// INT7: Off
(*(unsigned char *) 0x6a)=0x00;
EICRB=0x00;
EIMSK=0x00;
// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;
(*(unsigned char *) 0x7d)=0x00;
// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;   
// LCD module initialization
lcd_init(16);
while (1)
      {
                         while (PINA.3==0)
         {
         } 
                //forste raekke
        lcd_gotoxy(2,0);
        lcd_putsf("indtast hoejde");
                //anden reakke
        while (PINA.3==1)  //!        
        {
        if (PINA.0==0)     //!
        {
	hoejde++;
	if (hoejde>250)
        hoejde = 150;
        }
        else if (PINA.4==0)  //!
	{
	hoejde--;
	if (hoejde<100)
        hoejde = 150;
        }
        lcd_gotoxy(5,1);
        SplitInto1_2Digitsh(hoejde);
        delay_ms(500);
        }                       
                         while (PINA.3==0)
         {
         lcd_clear();
         } 
                while (PINA.3==1)  //!        
        {
                //forste raekke
        lcd_gotoxy(1,0);
        lcd_putsf("indtast vaegt");
                //anden reakke
        if (PINA.0==0)     //!
        {
	vaegt++;
        }
        else if (PINA.4==0)  //!
	{
	vaegt--;
        }
        lcd_gotoxy(5,1);
        SplitInto1_2Digitsv(vaegt);
        delay_ms(500);
        }  
                //forste raekke
                while (PINA.3==0)
        {
         lcd_clear();               
        }
                while (PINA.3==1)
        {
        lcd_gotoxy(5,0);
        lcd_putsf("Din BMI");
        //anden raekke
                udregnet_bmi(0); 
        }
                  };
}
