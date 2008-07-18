// CodeVisionAVR C Compiler
// (C) 1998-2003 Pavel Haiduc, HP InfoTech S.R.L.
// I/O registers definitions for the ATmega8535(L)
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
sfrb PINA=0x19;
sfrb DDRA=0x1a;
sfrb PORTA=0x1b;
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
sfrb OCDR=0x31;
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
sfrb OCR0=0X3c;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-
// Interrupt vectors definitions
// Needed by the power management functions (sleep.h)
#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x40
	.EQU __sm_mask=0xB0
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0xA0
	.EQU __sm_ext_standby=0xB0
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
#endasm
// CodeVisionAVR C Compiler
// (C) 1998-2006 Pavel Haiduc, HP InfoTech S.R.L.
// Prototypes for standard I/O functions
// CodeVisionAVR C Compiler
// (C) 1998-2002 Pavel Haiduc, HP InfoTech S.R.L.
// Variable length argument list macros
typedef char *va_list;
#pragma used+
char getchar(void);
void putchar(char c);
void puts(char *str);
void putsf(char flash *str);
char *gets(char *str,unsigned int len);
void printf(char flash *fmtstr,...);
void sprintf(char *str, char flash *fmtstr,...);
void snprintf(char *str, unsigned int size, char flash *fmtstr,...);
void vprintf (char flash * fmtstr, va_list argptr);
void vsprintf (char *str, char flash * fmtstr, va_list argptr);
void vsnprintf (char *str, unsigned int size, char flash * fmtstr, va_list argptr);
signed char scanf(char flash *fmtstr,...);
signed char sscanf(char *str, char flash *fmtstr,...);
                                               #pragma used-
#pragma library stdio.lib
 //flash char fca[]="Scorpion Controls";
//flash char *fptr; 
eeprom unsigned char *eepptr;
unsigned char a,b,c;
//pragma w-
//pragma w+
void add(void)
{           
        printf("Enter the 1st digit to ADD");
        scanf("%x",&a);    
        *++eepptr = a;
        printf("Enter the 2nd digit to ADD");
        scanf("%x",&b);
        *++eepptr = b;
        c=*--eepptr + *++eepptr;                     
        *++eepptr = c;
        printf("Added Result is:%d",c);
        a=b=c=0;
}
void sub(void)
{
        printf("Enter two digit to SUB"); 
        scanf("%x%x",&a,&b);
        c=a-b;  
        printf("Subtracted Result is:%d",c);
        }
void mul(void)
{
        printf("Enter two digit to MUL");
        scanf("%x%x",&a,&b);
        c=a*b;  
        printf("Multiplied Result is:%d",c);
}
void div(void)
{
        printf("Enter two digit to DIV");
        scanf("%x%x",&a,&b);
        c=a/b;  
        printf("Divided Result is:%d",c);
}
void wreep(unsigned char);
 void (*funp[5])(void)= {add,sub,mul,div};
void main(void)
{               
        void (*fp) (void);
        unsigned char FunSel;
                UBRRL=0x0C;
        UBRRH=0x00;
        UCSRA=0x02;
        UCSRB=0x18;
        UCSRC=0x86;
                 while(1)
        {       
                printf("Select any one operation representing it's serial no:");
                printf("(1)ADD(2)SUB(3)MUL(4)DIV");
                scanf("%x",&FunSel);
                wreep(FunSel);
                        if((FunSel>0x00) && (FunSel<0x05))
                {
                        FunSel=FunSel-0x01;                         
                        fp=funp[FunSel];
                        (*fp)();   
                } 
                printf("End");
        }
        }
void wreep(unsigned char data)
{
   *eepptr = data;
}
   