//**************************************************************************
// This Module has ported from Atmel applivation note 314 for atmega8
// I have modifed that app note a bit to remove
// free runnig osilation between two medulated signal.
// behzad khazama 2007/06/04 SABZEVAR-IRAN
// www.khazama.com
//**************************************************************************
// CodeVisionAVR C Compiler
// (C) 1998-2005 Pavel Haiduc, HP InfoTech S.R.L.
// Prototypes for string functions
#pragma used+
char *strcat(char *str1,char *str2);
char *strcatf(char *str1,char flash *str2);
char *strchr(char *str,char c);
signed char strcmp(char *str1,char *str2);
signed char strcmpf(char *str1,char flash *str2);
char *strcpy(char *dest,char *src);
char *strcpyf(char *dest,char flash *src);
unsigned char strcspn(char *str,char *set);
unsigned char strcspnf(char *str,char flash *set);
unsigned int strlenf(char flash *str);
char *strncat(char *str1,char *str2,unsigned char n);
char *strncatf(char *str1,char flash *str2,unsigned char n);
signed char strncmp(char *str1,char *str2,unsigned char n);
signed char strncmpf(char *str1,char flash *str2,unsigned char n);
char *strncpy(char *dest,char *src,unsigned char n);
char *strncpyf(char *dest,char flash *src,unsigned char n);
char *strpbrk(char *str,char *set);
char *strpbrkf(char *str,char flash *set);
signed char strpos(char *str,char c);
char *strrchr(char *str,char c);
char *strrpbrk(char *str,char *set);
char *strrpbrkf(char *str,char flash *set);
signed char strrpos(char *str,char c);
char *strstr(char *str1,char *str2);
char *strstrf(char *str1,char flash *str2);
unsigned char strspn(char *str,char *set);
unsigned char strspnf(char *str,char flash *set);
char *strtok(char *str1,char flash *str2);
 unsigned int strlen(char *str);
void *memccpy(void *dest,void *src,char c,unsigned n);
void *memchr(void *buf,unsigned char c,unsigned n);
signed char memcmp(void *buf1,void *buf2,unsigned n);
signed char memcmpf(void *buf1,void flash *buf2,unsigned n);
void *memcpy(void *dest,void *src,unsigned n);
void *memcpyf(void *dest,void flash *src,unsigned n);
void *memmove(void *dest,void *src,unsigned n);
void *memset(void *buf,unsigned char c,unsigned n);
#pragma used-
#pragma library string.lib
// CodeVisionAVR C Compiler
// (C) 1998-2004 Pavel Haiduc, HP InfoTech S.R.L.
// I/O registers definitions for the ATmega8
#pragma used+
sfrb TWBR=0;
sfrb TWSR=1;
sfrb TWAR=2;
sfrb TWDR=3;
sfrb ADCL=4;
sfrb ADCH=5;
sfrw ADCW=4;      // 16 bit access
sfrb ADCSRA=6;
sfrb ADMUX=7;
sfrb ACSR=8;
sfrb UBRRL=9;
sfrb UCSRB=0xa;
sfrb UCSRA=0xb;
sfrb UDR=0xc;
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
sfrb EECR=0x1c;
sfrb EEDR=0x1d;
sfrb EEARL=0x1e;
sfrb EEARH=0x1f;
sfrw EEAR=0x1e;   // 16 bit access
sfrb UBRRH=0x20;
sfrb UCSRC=0X20;
sfrb WDTCR=0x21;
sfrb ASSR=0x22;
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
sfrb SFIOR=0x30;
sfrb OSCCAL=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0=0x33;
sfrb MCUCSR=0x34;
sfrb MCUCR=0x35;
sfrb TWCR=0x36;
sfrb SPMCR=0x37;
sfrb TIFR=0x38;
sfrb TIMSK=0x39;
sfrb GIFR=0x3a;
sfrb GICR=0x3b;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-
// Interrupt vectors definitions
// Needed by the power management functions (sleep.h)
#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
#endasm
// CodeVisionAVR C Compiler
// (C) 1998-2000 Pavel Haiduc, HP InfoTech S.R.L.
#pragma used+
void delay_us(unsigned int n);
void delay_ms(unsigned int n);
#pragma used-
//************************** SIN TABLE *************************************
// Samples table : one period sampled on 128 samples and
// quantized on 7 bit
//**************************************************************************
flash unsigned char auc_SinParam [128] = {
64,67,
70,73,
76,79,
82,85,
88,91,
94,96,
99,102,
104,106,
109,111,
113,115,
117,118,
120,121,
123,124,
125,126,
126,127,
127,127,
127,127,
127,127,
126,126,
125,124,
123,121,
120,118,
117,115,
113,111,
109,106,
104,102,
99,96,
94,91,
88,85,
82,79,
76,73,
70,67,
64,60,
57,54,
51,48,
45,42,
39,36,
33,31,
28,25,
23,21,
18,16,
14,12,
10,9,
7,6,
4,3,
2,1,
1,0,
0,0,
0,0,
0,0,
1,1,
2,3,
4,6,
7,9,
10,12,
14,16,
18,21,
23,25,
28,31,
33,36,
39,42,
45,48,
51,54,
57,60};
//***************************  x_SW  ***************************************
//Table of x_SW (excess 8): x_SW = ROUND(8*N_samples*f*510/Fck)
//**************************************************************************
//high frequency (coloun)
//1209hz  ---> x_SW = 79
//1336hz  ---> x_SW = 87
//1477hz  ---> x_SW = 96
//1633hz  ---> x_SW = 107
const unsigned char auc_frequencyH [4] = {
107,96,
87,79};
//low frequency (row)
//697hz  ---> x_SW = 46
//770hz  ---> x_SW = 50
//852hz  ---> x_SW = 56
//941hz  ---> x_SW = 61
const unsigned char auc_frequencyL [4] = {
61,56,
50,46};
//**************************  global variables  ****************************
unsigned char x_SWa = 0x00;               // step width of high frequency
unsigned char x_SWb = 0x00;               // step width of low frequency
unsigned int  i_CurSinValA = 0;           // position freq. A in LUT (extended format)
unsigned int  i_CurSinValB = 0;           // position freq. B in LUT (extended format)
unsigned int  i_TmpSinValA;               // position freq. A in LUT (actual position)
unsigned int  i_TmpSinValB;               // position freq. B in LUT (actual position)
void stop_dtmf(void){
 x_SWb=0;
 x_SWb=0;
 TCCR1A &= 127;
 }
void start_dtmf(void){
 TCCR1A |= 128 ;
}
//**************************************************************************
// Timer overflow interrupt service routine
//**************************************************************************
interrupt [9] void timer1_ovf_isr(void)
{ 
  // move Pointer about step width aheaed
  i_CurSinValA += x_SWa;       
  i_CurSinValB += x_SWb;
  // normalize Temp-Pointer
  i_TmpSinValA  =  (char)(((i_CurSinValA+4) >> 3)&(0x007F)); 
  i_TmpSinValB  =  (char)(((i_CurSinValB+4) >> 3)&(0x007F));
  // calculate PWM value: high frequency value + 3/4 low frequency value
  OCR1A = (auc_SinParam[i_TmpSinValA] + (auc_SinParam[i_TmpSinValB]-(auc_SinParam[i_TmpSinValB]>>2)));
}
//**************************************************************************
// Initialization
//**************************************************************************
void dtmf_init (void)
{
    TIMSK  = 0x04;                     // Int T1 Overflow enabled
    TCCR1A=0x81;
    TCCR1B=0x01;
    DDRB   = 0x02 ;                    // PORTB.1 (OC1A) as output    
    stop_dtmf();
    #asm("sei");                       // Interrupts enabled
}
void Send_DTMF (char * number)
{       
  char ch=0,i=0;        
  for(i=0;i<strlen(number);i++){
    ch=number[i];
        if(ch=='1'){
                x_SWb = auc_frequencyL[3];  
                x_SWa = auc_frequencyH[3];
        }
        else if(ch=='2'){
                x_SWb = auc_frequencyL[3];  
                x_SWa = auc_frequencyH[2];
        }
        else if(ch=='3'){
                x_SWb = auc_frequencyL[3];  
                x_SWa = auc_frequencyH[1];
        }
        else if(ch=='4'){
                x_SWb = auc_frequencyL[2];  
                x_SWa = auc_frequencyH[3];
        }
        else if(ch=='5'){
                x_SWb = auc_frequencyL[2];  
                x_SWa = auc_frequencyH[2];
        }
        else if(ch=='6'){
                x_SWb = auc_frequencyL[2];  
                x_SWa = auc_frequencyH[1];
        }
        else if(ch=='7'){
                x_SWb = auc_frequencyL[1];  
                x_SWa = auc_frequencyH[3];
        }
        else if(ch=='8'){
                x_SWb = auc_frequencyL[1];  
                x_SWa = auc_frequencyH[2];
        }
        else if(ch=='9'){
                x_SWb = auc_frequencyL[1];  
                x_SWa = auc_frequencyH[1];
        }
        else if(ch=='0'){
                x_SWb = auc_frequencyL[0];  
                x_SWa = auc_frequencyH[2];
        }
        else if(ch=='*'){
                x_SWb = auc_frequencyL[0];  
                x_SWa = auc_frequencyH[3];
        }
        else if(ch=='#'){
                x_SWb = auc_frequencyL[0];  
                x_SWa = auc_frequencyH[1];
        }
        else if(ch=='A'){
                x_SWb = auc_frequencyL[3];  
                x_SWa = auc_frequencyH[0];
        }
        else if(ch=='B'){
                x_SWb = auc_frequencyL[2];  
                x_SWa = auc_frequencyH[0];
        }
        else if(ch=='C'){
                x_SWb = auc_frequencyL[1];  
                x_SWa = auc_frequencyH[0];
        }
        else if(ch=='D'){
                x_SWb = auc_frequencyL[0];  
                x_SWa = auc_frequencyH[0];
        }    
        start_dtmf();
        delay_ms(150);   
        stop_dtmf();
        delay_ms(150);   
    }; // end of for    
 stop_dtmf();
}
