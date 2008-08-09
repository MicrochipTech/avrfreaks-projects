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

#include <mega8535.h>
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
#include <math.h>
#include <stdio.h>

// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x1B ;PORTA
#endasm
#include <lcd.h>

//+++++++++++++++++++++++++++++++++++++++++++++++++++

#define pin_data                PIND.2
#define port_output             PORTD.7
#define pin_up                  PINC.0
#define pin_down                PINC.1
#define pin_timer_up            PINC.2
#define pin_timer_down          PINC.3

#define tdk_diaktifkan          1
#define mgkn_diaktifkan         2
#define diaktifkan              3
#define mgkn_tdk_diaktifkan     4

//+++++++++++++++++++++++++++++++++++++++++++++++++++

#define t1                      5000000/20
#define t2                      1000/20
#define t3                      4000/20
#define t4                      1000000/20
#define t5                      750000/20

//+++++++++++++++++++++++++++++++++++++++++++++++++++

#define jml_timer               7

//+++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int waktu1;
unsigned char waktu2;
unsigned char waktu3;
unsigned int waktu4;
unsigned int waktu5;

char buff[33];

char keadaan_pin_up;
bit tanda_pin_up;

char keadaan_pin_down;
bit tanda_pin_down;

char keadaan_pin_timer_up;
bit tanda_pin_timer_up;

char keadaan_pin_timer_down;
bit tanda_pin_timer_down;

bit penanda_ukur_logika1;
bit penanda_ukur_logika0;
bit penanda_selesai_ukur_logika;
unsigned int durasi_logika1;
unsigned long int durasi_logika0;
unsigned long int durasi_1periode;

unsigned int rpm;
unsigned long int timing;
unsigned int time_delay;
signed int time_delay_step;// -60 s.d. 60
unsigned int nilai_mutlak;

signed char i;

flash unsigned int data_acuan_lag_us[jml_timer]=
{
        0,
        15,
        20,
        25,
        75,
        125,
        250
};

flash unsigned int data_acuan_lead_us[jml_timer]=
{
        0,
        250,
        125,
        75,
        25,
        20,
        15
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++

void cek_pin_up(void)
{
        switch(keadaan_pin_up)
        {
                case tdk_diaktifkan:
                        if(pin_up == 0)
                                keadaan_pin_up= mgkn_diaktifkan;
                        else
                                keadaan_pin_up= tdk_diaktifkan;
                        break;
                case mgkn_diaktifkan:
                        if(pin_up == 0)
                        {
                                keadaan_pin_up= diaktifkan;
                                tanda_pin_up= 1;
                        }
                        else
                                keadaan_pin_up= tdk_diaktifkan;
                        break;
                case diaktifkan:
                        if(pin_up == 0)
                                keadaan_pin_up= diaktifkan;
                        else
                                keadaan_pin_up= mgkn_tdk_diaktifkan;
                        break;
                case mgkn_tdk_diaktifkan:
                        if(pin_up == 0)
                                keadaan_pin_up= diaktifkan;
                        else
                        {
                                keadaan_pin_up= tdk_diaktifkan;
                                tanda_pin_up= 0;
                        }
                        break;
        }
}

void cek_pin_down(void)
{
        switch(keadaan_pin_down)
        {
                case tdk_diaktifkan:
                        if(pin_down == 0)
                                keadaan_pin_down= mgkn_diaktifkan;
                        else
                                keadaan_pin_down= tdk_diaktifkan;
                        break;
                case mgkn_diaktifkan:
                        if(pin_down == 0)
                        {
                                keadaan_pin_down= diaktifkan;
                                tanda_pin_down= 1;
                        }
                        else
                                keadaan_pin_down= tdk_diaktifkan;
                        break;
                case diaktifkan:
                        if(pin_down == 0)
                                keadaan_pin_down= diaktifkan;
                        else
                                keadaan_pin_down= mgkn_tdk_diaktifkan;
                        break;
                case mgkn_tdk_diaktifkan:
                        if(pin_down == 0)
                                keadaan_pin_down= diaktifkan;
                        else
                        {
                                keadaan_pin_down= tdk_diaktifkan;
                                tanda_pin_down= 0;
                        }
                        break;
        }
}

void cek_pin_timer_up(void)
{
        switch(keadaan_pin_timer_up)
        {
                case tdk_diaktifkan:
                        if(pin_timer_up == 0)
                                keadaan_pin_timer_up= mgkn_diaktifkan;
                        else
                                keadaan_pin_timer_up= tdk_diaktifkan;
                        break;
                case mgkn_diaktifkan:
                        if(pin_timer_up == 0)
                        {
                                keadaan_pin_timer_up= diaktifkan;
                                tanda_pin_timer_up= 1;
                        }
                        else
                                keadaan_pin_timer_up= tdk_diaktifkan;
                        break;
                case diaktifkan:
                        if(pin_timer_up == 0)
                                keadaan_pin_timer_up= diaktifkan;
                        else
                                keadaan_pin_timer_up= mgkn_tdk_diaktifkan;
                        break;
                case mgkn_tdk_diaktifkan:
                        if(pin_timer_up == 0)
                                keadaan_pin_timer_up= diaktifkan;
                        else
                        {
                                keadaan_pin_timer_up= tdk_diaktifkan;
                                tanda_pin_timer_up= 0;
                        }
                        break;
        }
}

void cek_pin_timer_down(void)
{
        switch(keadaan_pin_timer_down)
        {
                case tdk_diaktifkan:
                        if(pin_timer_down == 0)
                                keadaan_pin_timer_down= mgkn_diaktifkan;
                        else
                                keadaan_pin_timer_down= tdk_diaktifkan;
                        break;
                case mgkn_diaktifkan:
                        if(pin_timer_down == 0)
                        {
                                keadaan_pin_timer_down= diaktifkan;
                                tanda_pin_timer_down= 1;
                        }
                        else
                                keadaan_pin_timer_down= tdk_diaktifkan;
                        break;
                case diaktifkan:
                        if(pin_timer_down == 0)
                                keadaan_pin_timer_down= diaktifkan;
                        else
                                keadaan_pin_timer_down= mgkn_tdk_diaktifkan;
                        break;
                case mgkn_tdk_diaktifkan:
                        if(pin_timer_down == 0)
                                keadaan_pin_timer_down= diaktifkan;
                        else
                        {
                                keadaan_pin_timer_down= tdk_diaktifkan;
                                tanda_pin_timer_down= 0;
                        }
                        break;
        }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++

// Timer untuk mengukur logika
void aktifkan_pengukur_logika(void)
{
        TCCR1A=0x00;
        TCCR1B= 0x03;
        TCNT1H=0x00;
        TCNT1L=0x00;
        OCR1AH=0x00;
        OCR1AL=0x00;
        OCR1BH=0x00;
        OCR1BL=0x00;

        TIMSK= 0x02;
}

// Timer shifter
void aktifkan_timer1_cmpA(unsigned long int nilai_compare)
{
        unsigned int nilai_ocr= 0x0000;

        port_output= 0;

        if(time_delay_step < 0)
                nilai_ocr= ((nilai_compare-8)*2);// Penyesuaian sinyal lead
        else
                nilai_ocr= ((nilai_compare-12)*2);// Penyesuaian sinyal lag

        TCCR1A=0x00;
        TCCR1B= 0x02;
        TCNT1H=0x00;
        TCNT1L=0x00;
        OCR1A= nilai_ocr;
        OCR1BH=0x00;
        OCR1BL=0x00;

        TIMSK=0x12;
}

// Timer logika
void aktifkan_timer1_cmpB(unsigned long int nilai_compare)
{
        unsigned int nilai_ocr= 0x0000;

        nilai_ocr= (nilai_compare*2);

        TCCR1A=0x00;
        TCCR1B= 0x02;
        TCNT1H=0x00;
        TCNT1L=0x00;
        OCR1AH=0x00;
        OCR1AL=0x00;
        OCR1B= nilai_ocr;

        TIMSK=0x0A;
}

// Matikan timer
void matikan_timer1(void)
{
        TIMSK=0x02;

        TCCR1A=0x00;
        TCCR1B=0x00;
        TCNT1H=0x00;
        TCNT1L=0x00;
        OCR1AH=0x00;
        OCR1AL=0x00;
        OCR1BH=0x00;
        OCR1BL=0x00;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++

void hitung_timing(void)
{
        if(time_delay_step > 0)
        {
                timing= (unsigned long int)time_delay*time_delay_step;
        }
        else if(time_delay_step < 0)
        {
                nilai_mutlak= abs(time_delay_step);
                timing= (unsigned long int)time_delay*nilai_mutlak;
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
        if(time_delay_step > -60)
        {
                --time_delay_step;
        }
        hitung_timing();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++

void hitung_rpm(void)
{
        if(durasi_1periode != 0)
                rpm= 60000000/durasi_1periode;
        else
                rpm= 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++

void tampil_lcd(void)
{
        lcd_clear();
        lcd_putsf("td  d     RPM");
        sprintf(buff, "\n%2i %3i   %5u", time_delay_step, time_delay, rpm);
        lcd_puts(buff);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++

// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
        if(pin_data == 1)
        {
                // Ganti ke interrupt fall
                MCUCR&= ~(1<<0);

                if(penanda_ukur_logika1)
                {
                        aktifkan_pengukur_logika();
                }
                else if(penanda_ukur_logika0)
                {
                        // Ambil data durasi 1 periode
                        TCCR1B= 0x00;
                        durasi_1periode= (unsigned long int)TCNT1*4;
                        matikan_timer1();
                        penanda_ukur_logika0= 0;
                        penanda_selesai_ukur_logika= 1;
                }
                else
                {
                        if(time_delay_step == 0)
                        {
                                port_output= 1;
                                aktifkan_timer1_cmpB(durasi_logika1);
                        }
                        else if(time_delay_step > 0)
                        {
                                port_output= 0;
                                aktifkan_timer1_cmpA(timing);// timing shift for logic 0
                        }
                }
        }
        else if(pin_data == 0)
        {
                // Ganti ke interrupt rise
                MCUCR|= 1<<0;

                if(penanda_ukur_logika1)
                {
                        // Ambil data durasi logika 1
                        durasi_logika1= TCNT1*4;
                        penanda_ukur_logika1= 0;
                        penanda_ukur_logika0= 1;
                }
                else
                {
                        if(time_delay_step < 0)
                        {
                                port_output= 0;
                                aktifkan_timer1_cmpA(durasi_logika0-timing);// timing shift for logic 0
                        }
                }
        }
}

// Timer 0 output compare interrupt service routine, base time 20 us
interrupt [TIM0_COMP] void timer0_comp_isr(void)
{
        TCNT0= 0x00;

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
interrupt [TIM1_COMPA] void timer1_compa_isr(void)
{
        port_output= 1;
        aktifkan_timer1_cmpB(durasi_logika1);
}

// Timer 1 output compare B interrupt service routine, timer logika 1 match
interrupt [TIM1_COMPB] void timer1_compb_isr(void)
{
        port_output= 0;
}

void inisialisasi_durasi_pulsa(void)
{
        // Global enable interrupts
        #asm("sei")

        penanda_ukur_logika1= 1;
        while(!penanda_selesai_ukur_logika);
        durasi_logika0= durasi_1periode-durasi_logika1;
        penanda_selesai_ukur_logika= 0;

        // Global disable interrupts
        #asm("cli")
}

void inisialisasi(void)
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
        C.2: Timer Up
        C.3: Timer Down
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        **/
        PORTC=0x0F;
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

        // Inisialisasi durasi pulsa
        inisialisasi_durasi_pulsa();

        // Timer scheduling
        waktu1= t1;// 3 detik
        waktu2= t2;// 2 mili detik
        waktu3= t3;// 3 mili detik
        waktu4= t4;// 1 detik
        waktu5= t5;// 400 mili detik

        // Inisialisasi data
        port_output= 0;
        penanda_ukur_logika1= 0;
        penanda_ukur_logika0= 0;
        penanda_selesai_ukur_logika= 0;
        rpm= 0;
        i= 0;

        // Inisialisasi pin control
        tanda_pin_up= 0;
        keadaan_pin_up= tdk_diaktifkan;
        tanda_pin_down= 0;
        keadaan_pin_down= tdk_diaktifkan;

        // Inisialisasi pin timer
        tanda_pin_timer_up= 0;
        keadaan_pin_timer_up= tdk_diaktifkan;
        tanda_pin_timer_down= 0;
        keadaan_pin_timer_down= tdk_diaktifkan;

        // Global enable interrupts
        #asm("sei")
}

void main(void)
{
        // Inisialisasi program
        inisialisasi();

        // Main eternal loop
        for(;;)
        {
                // Selesai eksekusi kalibrasi logika
                if(penanda_selesai_ukur_logika)
                {
                        durasi_logika0= durasi_1periode-durasi_logika1;
                        penanda_selesai_ukur_logika= 0;
                }

                // Eksekusi kalibrasi logika
                if(waktu1 == 0)
                {
                        waktu1= t1;
                        penanda_ukur_logika1= 1;
                }

                // Scanning tombol
                if(waktu2 == 0)
                {
                        waktu2= t2;
                        cek_pin_up();
                        cek_pin_down();
                        cek_pin_timer_up();
                        cek_pin_timer_down();
                }

                // Penentuan keadaan pin_up, pin_down, pin_timer_up dan pin_timer_down
                if(waktu3 == 0)
                {
                        waktu3= t3;
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

                        if(tanda_pin_timer_up)
                        {
                                tanda_pin_timer_up= 0;
                                if(i < (jml_timer-1))
                                        i++;
                        }
                        if(tanda_pin_timer_down)
                        {
                                tanda_pin_timer_down= 0;
                                if(i > 0)
                                        i--;
                        }
                }

                //Penentuan timing
                if(time_delay_step < 0)
                        time_delay= data_acuan_lead_us[i];
                else
                        time_delay= data_acuan_lag_us[i];

                // Hitung timing
                hitung_timing();

                // Hitung RPM
                if(waktu4 == 0)
                {
                        waktu4= t4;
                        hitung_rpm();
                }

                // Tampil LCD
                if(waktu5 == 0)
                {
                        waktu5= t5;
                        tampil_lcd();
                }
        };
}
