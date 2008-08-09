/*****************************************************
Project : Shifter_CDI
Version : 1.5
Date    : 9/8/2007
Author  : hendyeka
Company : FreeLancer
Comments: 


Chip type           : ATmega8535
Program type        : Application
Clock frequency     : 16.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 128
*****************************************************/
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
/*
CodeVisionAVR C Compiler
Prototypes for mathematical functions

Portions (C) 1998-2001 Pavel Haiduc, HP InfoTech S.R.L.
Portions (C) 2000-2001 Yuri G. Salov
*/
#pragma used+
unsigned char cabs(signed char x);
unsigned int abs(int x);
unsigned long labs(long x);
float fabs(float x);
signed char cmax(signed char a,signed char b);
int max(int a,int b);
long lmax(long a,long b);
float fmax(float a,float b);
signed char cmin(signed char a,signed char b);
int min(int a,int b);
long lmin(long a,long b);
float fmin(float a,float b);
signed char csign(signed char x);
signed char sign(int x);
signed char lsign(long x);
signed char fsign(float x);
unsigned char isqrt(unsigned int x);
unsigned int lsqrt(unsigned long x);
float sqrt(float x);
float floor(float x);
float ceil(float x);
float fmod(float x,float y);
float modf(float x,float *ipart);
float ldexp(float x,int expon);
float frexp(float x,int *expon);
float exp(float x);
float log(float x);
float log10(float x);
float pow(float x,float y);
float sin(float x);
float cos(float x);
float tan(float x);
float sinh(float x);
float cosh(float x);
float tanh(float x);
float asin(float x);
float acos(float x);
float atan(float x);
float atan2(float y,float x);
#pragma used-
#pragma library math.lib
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
// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x1B ;PORTA
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
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned int waktu1;
unsigned char waktu2;
unsigned char waktu3;
unsigned int waktu4;
unsigned int waktu5;
char buff[50];
char keadaan_pin_up;
bit tanda_pin_up;
char keadaan_pin_down;
bit tanda_pin_down;
bit penanda_ext_interrupt_rise;
bit penanda_ukur_logika1;
bit penanda_sedang_ukur_logika1;
bit penanda_ukur_1periode;
bit penanda_sedang_ukur_1periode;
unsigned int durasi_logika1, n;
unsigned long int durasi_logika0;
unsigned long int durasi_1periode;
unsigned int jml_pulsa;
unsigned int rpm;
unsigned long int timing;
unsigned int time_delay;
signed int time_delay_step;// -20 s.d. 60
unsigned int nilai_mutlak;
signed char i;
flash unsigned int data_acuan[25]=
{
        0,
        667,
        222,
        133,
        95,
        74,
        61,
        51,
        44,
        39,
        35,
        32,
        29,
        27,
        25,
        23,
        22,
        20,
        19,
        18,
        17,
        16,
        16,
        15,
        14
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++
void cek_pin_up(void)
{
        switch(keadaan_pin_up)
        {
                case 1 : 
                        if(PINC.0 == 0)
                                keadaan_pin_up= 2;
                        else
                                keadaan_pin_up= 1 ;
                        break;
                case 2:
                        if(PINC.0 == 0) 
                        {
                                keadaan_pin_up= 3;   
                                tanda_pin_up= 1;
                        }
                        else
                                keadaan_pin_up= 1 ;
                        break;
                case 3:  
                        if(PINC.0 == 0)
                                keadaan_pin_up= 3; 
                        else
                                keadaan_pin_up= 4 ;    
                        break;
                case 4 :
                        if(PINC.0 == 0)
                                keadaan_pin_up= 3; 
                        else 
                        {
                                keadaan_pin_up= 1 ;
                                tanda_pin_up= 0;
                        }    
                        break;
        }
}
void cek_pin_down(void)
{
        switch(keadaan_pin_down)
        {
                case 1 : 
                        if(PINC.1 == 0)
                                keadaan_pin_down= 2;
                        else
                                keadaan_pin_down= 1 ;
                        break;
                case 2:
                        if(PINC.1 == 0) 
                        {
                                keadaan_pin_down= 3;   
                                tanda_pin_down= 1;
                        }
                        else
                                keadaan_pin_down= 1 ;
                        break;
                case 3:  
                        if(PINC.1 == 0)
                                keadaan_pin_down= 3; 
                        else
                                keadaan_pin_down= 4 ;    
                        break;
                case 4 :
                        if(PINC.1 == 0)
                                keadaan_pin_down= 3; 
                        else 
                        {
                                keadaan_pin_down= 1 ;
                                tanda_pin_down= 0;
                        }    
                        break;
        }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
void aktifkan_pengukur_logika1(void)
{
        TCCR1A=0x00;
        TCCR1B=0x01;
        TCNT1H=0x00;
        TCNT1L=0x00;
        ICR1H=0x00;
        ICR1L=0x00;
        OCR1AH=0x00;
        OCR1AL=0x00;
        OCR1BH=0x00;
        OCR1BL=0x00;
                TIMSK=0x02;       
}
void aktifkan_pengukur_1periode(void)
{
        TCCR1A=0x00;
        TCCR1B=0x03;
        TCNT1H=0x00;
        TCNT1L=0x00;
        ICR1H=0x00;
        ICR1L=0x00;
        OCR1AH=0x00;
        OCR1AL=0x00;
        OCR1BH=0x00;
        OCR1BL=0x00;
                TIMSK=0x02;       
}
// Timer shifter
void aktifkan_timer1_cmpA(unsigned long int nilai_compare)
{
        unsigned char presc= 0x00;
        unsigned int nilai_ocr= 0x0000;
                PORTD.7= 0;
                if(nilai_compare > 4000)
        {
                presc= 0x03;
                nilai_ocr= nilai_compare*0.25;
        }
        else
        {
                presc= 0x01;
                nilai_ocr= nilai_compare*16;
        }
                TCCR1A=0x00;
        TCCR1B= presc;
        TCNT1H=0x00;
        TCNT1L=0x00;
        ICR1H=0x00;
        ICR1L=0x00;
        OCR1A= nilai_ocr;
        OCR1BH=0x00;
        OCR1BL=0x00;
                TIMSK=0x12;
}
// Timer logika
void aktifkan_timer1_cmpB(unsigned long int nilai_compare)
{
        unsigned char presc= 0x00;
        unsigned int nilai_ocr= 0x0000;
                penanda_ext_interrupt_rise= 0;
                if(nilai_compare > 4000)
        {
                presc= 0x03;
                nilai_ocr= nilai_compare*0.25;
        }
        else
        {
                presc= 0x01;
                nilai_ocr= nilai_compare*16;
        }
                TCCR1A=0x00;
        TCCR1B= presc;
        TCNT1H=0x00;
        TCNT1L=0x00;
        ICR1H=0x00;
        ICR1L=0x00;
        OCR1AH=0x00;
        OCR1AL=0x00;
        OCR1B= nilai_ocr;
                TIMSK=0x0A;
}
void matikan_timer1(void)
{
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
                TIMSK=0x02;       
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
void hitung_timing(void)
{
        unsigned int tmp= 0;
                        if(time_delay_step > 0)
        {
                timing= (unsigned long int)time_delay*time_delay_step;
        }
        else if(time_delay_step < 0)
        {
                nilai_mutlak= abs(time_delay_step);
                tmp= time_delay*nilai_mutlak;
                if(durasi_logika1 > tmp)
                {
                        timing= (unsigned long int)durasi_logika1-tmp;
                }
                else
                {
                        timing= 0;
                }
        }
        else if(time_delay_step == 0)
                timing= 0;
}
void increment_shift(void)
{
        if(time_delay_step < 60)
        {
                ++time_delay_step;
        }
        hitung_timing();
}
void decrement_shift(void)
{
        if(time_delay_step > -20)
        {
                --time_delay_step;
        }
        hitung_timing();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
void hitung_rpm(void)
{
        /**
        Satu putaran adalah 1 gelombang.
        jml_pulsa di sini adalah banyaknya gelombang yang terjadi setiap 1 s.
        Satu menit adalah 60x (1 s).
        Sehingga pendekatan perhitungan yang dicapai, rpm= (jml_pulsa*60) rotation per minute
        **/
        rpm= jml_pulsa*60;
        jml_pulsa= 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
void tampil_lcd(void)
{        
        lcd_clear();
        lcd_putsf("td        RPM");
        sprintf(buff, "\n%2i        %5i", time_delay_step, rpm);
        lcd_puts(buff);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// External Interrupt 0 service routine
interrupt [2] void ext_int0_isr(void)
{
        if(PIND.2 == 1)
        {
                if(penanda_ukur_logika1)
                {
                        if(!penanda_sedang_ukur_logika1)
                        {
                                n= 0;
                                penanda_sedang_ukur_logika1= 1;
                        }
                }
                else if(penanda_ukur_1periode)
                {
                        if(penanda_sedang_ukur_1periode)
                        {
                                durasi_logika0= (unsigned long int)TCNT1*4;
                                matikan_timer1();
                                durasi_1periode= durasi_logika0+durasi_logika1;
                                penanda_ukur_1periode= 0;
                                penanda_sedang_ukur_1periode= 0;
                        }       
                }
                else
                {
                        if(time_delay_step == 0)
                        {
                                PORTD.7= 1;
                                aktifkan_timer1_cmpB(durasi_logika1);
                        }
                        else if(time_delay_step > 0)
                        {
                                PORTD.7= 0;
                                aktifkan_timer1_cmpA(timing);// timing shift for logic 0
                        }
                        else if(time_delay_step < 0)
                        {
                                if(timing > 0)
                                {
                                        PORTD.7= 1;
                                        aktifkan_timer1_cmpB(timing);// timing shift for logic 1
                                }
                                else
                                {
                                        PORTD.7= 0;
                                        aktifkan_timer1_cmpA(durasi_logika0);
                                }
                        }
                }
                ++jml_pulsa;
                penanda_ext_interrupt_rise= 1;
        }
}
// Timer 0 output compare interrupt service routine, base time 20 us
interrupt [20] void timer0_comp_isr(void)
{
        TCNT0= 0x00;
                if(n < 64000)
                ++n;
        if(waktu1 > 0)
                --waktu1;
        if(waktu2 > 0)
                --waktu2;
        if(waktu3 > 0)
                --waktu3;
        if(waktu4 > 0)
                --waktu4;
        if(waktu5 > 0)
                --waktu5;
}
// Timer 1 output compare A interrupt service routine, timer shifter match
interrupt [7] void timer1_compa_isr(void)
{
        PORTD.7= 1;
        matikan_timer1();
        if(time_delay_step > 0)
        {
                aktifkan_timer1_cmpB(durasi_logika1);
        }
}
// Timer 1 output compare B interrupt service routine, timer logika 1 match
interrupt [8] void timer1_compb_isr(void)
{
        PORTD.7= 0;
        matikan_timer1();
        if(time_delay_step < 0)
        {
                aktifkan_timer1_cmpA(durasi_logika0);
        }
}
void main(void)
{
        /**
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        Port A -- Port LCD
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        **/ 
        PORTA=0x00;
        DDRA=0xFF;
                /**
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        Port B -- Dummies
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        **/ 
        PORTB=0x00;
        DDRB=0x00;
                /**
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        Port C -- Kontrol Shifter
        C.0: Up
        C.1: Down
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        **/ 
        PORTC=0x03;
        DDRC=0x00;
                /**
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        Port D
        D.2: INT0
        D.7: Shifted Output Pulse
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        **/ 
        PORTD=0x04;
        DDRD=0x80;
                // Timer/Counter 0 initialization
        // Clock source: System Clock
        // Clock value: 2000.000 kHz
        // Mode: Normal top=FFh
        // OC0 output: Disconnected
        TCCR0=0x02;
        TCNT0=0x00;
        OCR0=0x28;
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
        // INT0: On
        // INT0 Mode: Rising Edge
        // INT1: Off
        // INT2: Off
        GICR|=0x40;
        MCUCR=0x03;
        MCUCSR=0x00;
        GIFR=0x40;
                // Timer(s)/Counter(s) Interrupt(s) initialization
        TIMSK=0x02;
                // Analog Comparator initialization
        // Analog Comparator: Off
        // Analog Comparator Input Capture by Timer/Counter 1: Off
        ACSR=0x80;
        SFIOR=0x00;
                // LCD module initialization
        lcd_init(16);
        lcd_clear();
                // Global enable interrupts
        #asm("sei")
                penanda_ext_interrupt_rise= 0;
        // Dapatkan data awal durasi pulsa logika 1
        while(!penanda_ext_interrupt_rise);
        #asm("cli")
        aktifkan_pengukur_logika1();
        while(PIND.2);
        durasi_logika1= TCNT1/16;
        matikan_timer1();
                // Global enable interrupts
        #asm("sei")
                penanda_ext_interrupt_rise= 0;
        // Dapatkan data awal durasi 1 periode pulsa
        while(!penanda_ext_interrupt_rise);
        #asm("cli")
        aktifkan_pengukur_1periode();
        while(PIND.2);
        while(!PIND.2);
        durasi_1periode= (unsigned long int)TCNT1*4;
        matikan_timer1();
                // Dapatkan data awal durasi pulsa logika 0
        durasi_logika0= durasi_1periode-durasi_logika1;
                // Global disable interrupts
        #asm("cli")
                // Timer scheduling
        waktu1= 5000000/20;// 3 detik
        waktu2= 2000/20;// 2 mili detik
        waktu3= 3000/20;// 3 mili detik
        waktu4= 1000000/20;// 1 detik
        waktu5= 350000/20;// 400 mili detik
                // Inisialisasi data
        PORTD.7= 0;
        penanda_ext_interrupt_rise= 0;
        penanda_ukur_logika1= 0;
        penanda_sedang_ukur_logika1= 0;
        penanda_ukur_1periode= 0;
        penanda_sedang_ukur_1periode= 0;
        n= 0;
        jml_pulsa= 0;
        rpm= 0;
        i= 1;
                // Inisialisasi pin control
        tanda_pin_up= 0;
        keadaan_pin_up= 1 ;
        tanda_pin_down= 0;
        keadaan_pin_down= 1 ;
                // Global enable interrupts
        #asm("sei")
                // Main eternal loop
        for(;;)
        {
                if(waktu1 == 0)
                {
                        waktu1= 5000000/20;
                        penanda_ukur_logika1= 1;
                }
                                // Eksekusi kalibrasi durasi logika 1
                if(penanda_ukur_logika1)
                {
                        if(penanda_sedang_ukur_logika1)
                        {
                                if(!PIND.2)
                                {
                                        durasi_logika1= (unsigned long int)n*20;
                                        aktifkan_pengukur_1periode();
                                        penanda_ukur_logika1= 0;
                                        penanda_sedang_ukur_logika1= 0;
                                        penanda_ukur_1periode= 1;
                                                                                waktu1= 5000000/20;
                                }
                        }
                }
                                // Eksekusi kalibrasi durasi 1 periode dan logika 0
                if(penanda_ukur_1periode)
                {
                        penanda_sedang_ukur_1periode= 1;
                }
                                                // Scanning tombol
                if(waktu2 == 0)
                {
                        waktu2= 2000/20;
                        cek_pin_up();
                        cek_pin_down();
                }
                                // Penentuan keadaan pin_up dan pin_down
                if(waktu3 == 0)
                {
                        waktu3= 3000/20;
                        if(tanda_pin_up)
                        {
                                tanda_pin_up= 0;
                                increment_shift();
                        }
                        if(tanda_pin_down)
                        {
                                tanda_pin_down= 0;
                                decrement_shift();
                        }
                }
                                // Hitung RPM
                if(waktu4 == 0)
                {
                        waktu4= 1000000/20;
                        hitung_rpm();
                }
                                //Penentuan timing
                if(rpm > 0 && rpm <= 250)
                        i= 1;
                else if(rpm > 250 && rpm <= 750)
                        i= 2;
                else if(rpm > 750 && rpm <= 1250)
                        i= 3;
                else if(rpm > 1250 && rpm <= 1750)
                        i= 4;
                else if(rpm > 1750 && rpm <= 2250)
                        i= 5;
                else if(rpm > 2250 && rpm <= 2750)
                        i= 6;
                else if(rpm > 2750 && rpm <= 3250)
                        i= 7;
                else if(rpm > 3250 && rpm <= 3750)
                        i= 8;
                else if(rpm > 3750 && rpm <= 4250)
                        i= 9;
                else if(rpm > 4250 && rpm <= 4750)
                        i= 10;
                else if(rpm > 4750 && rpm <= 5250)
                        i= 11;
                else if(rpm > 5250 && rpm <= 5750)
                        i= 12;
                else if(rpm > 5750 && rpm <= 6250)
                        i= 13;
                else if(rpm > 6250 && rpm <= 6750)
                        i= 14;
                else if(rpm > 6750 && rpm <= 7250)
                        i= 15;
                else if(rpm > 7250 && rpm <= 7750)
                        i= 16;
                else if(rpm > 7750 && rpm <= 8250)
                        i= 17;
                else if(rpm > 8250 && rpm <= 8750)
                        i= 18;
                else if(rpm > 8750 && rpm <= 9250)
                        i= 19;
                else if(rpm > 9250 && rpm <= 9750)
                        i= 20;
                else if(rpm > 9750 && rpm <= 10250)
                        i= 21;
                else if(rpm > 10250 && rpm <= 10750)
                        i= 22;
                else if(rpm > 10750 && rpm <= 11250)
                        i= 23;
                else if(rpm > 11250 && rpm <= 11750)
                        i= 24;
                time_delay= data_acuan[i];
                                // Hitung timing
                hitung_timing();
                                // Tampil LCD
                if(waktu5 == 0)
                {
                        waktu5= 350000/20;
                        tampil_lcd();
                }
        };
}
