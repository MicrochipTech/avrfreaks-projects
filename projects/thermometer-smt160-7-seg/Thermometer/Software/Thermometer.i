          /****************************************************
          ** Processor    : ATtiny2313-20PI                   **
         *** Frequncy     : 12.000000 MHz                     ***
        **** AUTHOR       : Behnam Esckandariun               ****
       ***** Project      : Thermometer with SMT160 V.1.00.5  *****
        **** Date         : 2008/02/24 11:49PM IRAN-ZANJAN    ****
         *** Compiler     : CodeVisionAVR  V 1.25.8           ***
          ** comment      : Just for educational use          **
           ****************************************************
           
                        behnam.esckandariun@gmail.com    


/***********************************************************************************
************               Include Used library's                       ************
***********************************************************************************/
// CodeVisionAVR C Compiler
// (C) 1998-2003 Pavel Haiduc, HP InfoTech S.R.L.
// I/O registers definitions for the ATtiny2313
#pragma used+
#pragma used+
sfrb DIDR=1;
sfrb UBRRH=2;
sfrb UCSRC=3;
sfrb ACSR=8;
sfrb UBRRL=9;
sfrb UCSRB=0xa;
sfrb UCSRA=0xb;
sfrb UDR=0xc;
sfrb USICR=0xd;
sfrb USISR=0xe;
sfrb USIDR=0xf;
sfrb PIND=0x10;
sfrb DDRD=0x11;
sfrb PORTD=0x12;
sfrb GPIOR0=0x13;
sfrb GPIOR1=0x14;
sfrb GPIOR2=0x15;
sfrb PINB=0x16;
sfrb DDRB=0x17;
sfrb PORTB=0x18;
sfrb PINA=0x19;
sfrb DDRA=0x1a;
sfrb PORTA=0x1b;
sfrb EECR=0x1c;
sfrb EEDR=0x1d;
sfrb EEAR=0x1e;
sfrb PCMSK=0x20;
sfrb WDTCR=0x21;
sfrb TCCR1C=0x22;
sfrb GTCCR=0x23;
sfrb ICR1L=0x24;
sfrb ICR1H=0x25;
sfrw ICR1=0x24;   // 16 bit access
sfrb CLKPR=0x26;
sfrb OCR1BL=0x28;
sfrb OCR1BH=0x29;
sfrw OCR1B=0x28;   // 16 bit access
sfrb OCR1AL=0x2a;
sfrb OCR1AH=0x2b;
sfrw OCR1A=0x2a;   // 16 bit access
sfrb TCNT1L=0x2c;
sfrb TCNT1H=0x2d;
sfrw TCNT1=0x2c;  // 16 bit access
sfrb TCCR1B=0x2e;
sfrb TCCR1A=0x2f;
sfrb TCCR0A=0x30;
sfrb OSCCAL=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0B=0x33;
sfrb MCUSR=0x34;
sfrb MCUCR=0x35;
sfrb OCR0A=0x36;
sfrb SPMCSR=0x37;
sfrb TIFR=0x38;
sfrb TIMSK=0x39;
sfrb EIFR=0x3a;
sfrb GIMSK=0x3b;
sfrb OCR0B=0x3c;
sfrb SPL=0x3d;
sfrb SREG=0x3f;
#pragma used-
// Interrupt vectors definitions
// for compatibility with the interrupt vector names from Atmel's datasheet
// Needed by the power management functions (sleep.h)
#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x20
	.EQU __sm_mask=0x50
	.EQU __sm_powerdown=0x10
	.EQU __sm_standby=0x40
	.SET power_ctrl_reg=mcucr
	#endif
#endasm
/* CodeVisionAVR C Compiler
   Prototypes for standard library functions

   (C) 1998-2003 Pavel Haiduc, HP InfoTech S.R.L.
*/
#pragma used+
#pragma used+
int atoi(char *str);
long int atol(char *str);
float atof(char *str);
void itoa(int n,char *str);
void ltoa(long int n,char *str);
void ftoa(float n,unsigned char decimals,char *str);
void ftoe(float n,unsigned char decimals,char *str);
void srand(int seed);
int rand(void);
void *malloc(unsigned int size);
void *calloc(unsigned int num, unsigned int size);
void *realloc(void *ptr, unsigned int size); 
void free(void *ptr);
#pragma used-
#pragma library stdlib.lib
// CodeVisionAVR C Compiler
// (C) 1998-2000 Pavel Haiduc, HP InfoTech S.R.L.
#pragma used+
#pragma used+
void delay_us(unsigned int n);
void delay_ms(unsigned int n);
#pragma used-
/***********************************************************************************
************                  Define Used  I/O                          ************
***********************************************************************************/
/***********************************************************************************
************              Declare function of init                      ************
***********************************************************************************/
void system_init(void);
/***********************************************************************************
************        Declare global variable's and const here            ************
***********************************************************************************/
unsigned char a[5],b;           //used for 7-segment refresh program
unsigned char i=0;              //used for 7-segment refresh program
float t1,t2,t;                  //contain on & off time of SMT160 output signal
signed int temp;                //Contain temp*10 for one poine
bit edge=0;                     //Used in musering t1 & t2 
unsigned int memory; 
//This line Save Com-Anode 7-segment Data on Flash ROM
flash char s_data[]={192,249,164,176,153,146,130,248,128,144,191,255};
/***********************************************************************************
************           INT0 Service routin Used for SMT160              ************
************                   Duty-cycle mesure                        ************
***********************************************************************************/
//This ISR just mesure on and off time of the SMT160 Out signal in t1 & t2
//Globale variable's . calculate of temp accure in main function
interrupt [2] void ext_int0_isr(void)
{
if (edge)
        {
        t2=TCNT1;
        TCNT1=0;
        //Falling Edge
        MCUCR=0x02;
        edge=0;
        }
else
        {
        t1=TCNT1;
        TCNT1=0;
        //Rising edge
        MCUCR=0x03;
        edge=1;
        }
}
/***********************************************************************************
************      Timer0 INT Service routin for 7-segment refresh       ************
***********************************************************************************/
//This ISR refresh 7-segment 22 time's per second
//temp varible showing on display
interrupt [7] void timer0_ovf_isr(void)
{
i++;     
switch (i)
        {
        case 1:
                memory = temp;
                if (temp<0)
                        {
                        a[1]=10;
                        temp *= -1;
                        }
                else
                        a[1]=11;
                temp %= 1000;
                a[2] = temp / 100;
                b = temp % 100;
                a[3] = b / 10;
                a[4] = b % 10;
                PORTD  |= 0xf8;
                PORTB = s_data[a[i]];
                PORTD.6 = 0;
                break; 
        case 2:
                PORTD  |= 0xf8;
                PORTB = s_data[a[i]];
                PORTD.5 = 0;
                break;
        case 3:
                PORTD  |= 0xf8;
                PORTB = s_data[a[i]];
                PORTB &= 0b01111111;
                PORTD.4 = 0;
                break;
        case 4:
                PORTD  |= 0xf8;
                PORTB = s_data[a[i]];
                PORTD.3 = 0;
                i = 0;
                break;
        };
temp = memory;
TCNT0=0xd7;                      //Reinitlaiz timer start value for display refresh
}
/***********************************************************************************
************               Starting main program                        ************
***********************************************************************************/
void main(void)
{
float dc;                       //Duty cycle save in this variable 
system_init();                  // System initalize port and register's value
while (1)
      {                         
      PORTD.1 =1;                //Turn SMT160 on
      //delay_ms(100);          //Waiting for mesuring temp
      //smt160=off;             //Turn SMT160 off
      dc=t1/(t1+t2);            //this line's calculate temp from duty-cycle's
      t=(dc-0.32)/0.0047;
      temp = t*10;
      delay_ms(500);            //wait for next mesurment in 5sec
      };
}
/***********************************************************************************
************          System initlaizing in this Function               ************
***********************************************************************************/
void system_init(void)
{
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#pragma optsize+
PORTB=0x00;
DDRB=0xFF;
PORTD=0x78;
DDRD=0x7A;
//Timer0: 11.71MHz
TCCR0A=0x00;
TCCR0B=0x05;
//Timer1: 12MHz
TCCR1A=0x00;
TCCR1B=0x01;
//INT0:Falling
GIMSK=0x40;
MCUCR=0x02;
EIFR=0x40;
TIMSK=0x02;
#asm("sei")
}             
/***********************************************************************************
************            End of thermometer Program                      ************
***********************************************************************************/
