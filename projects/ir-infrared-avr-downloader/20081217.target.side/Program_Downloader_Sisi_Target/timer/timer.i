// CodeVisionAVR C Compiler
// (C) 1998-2004 Pavel Haiduc, HP InfoTech S.R.L.
// I/O registers definitions for the ATmega162
#pragma used+
sfrb UBRR1L=0;
sfrb UCSR1B=1;
sfrb UCSR1A=2;
sfrb UDR1=3;
sfrb OSCCAL=4;
sfrb OCDR=4;
sfrb PINE=5;
sfrb DDRE=6;
sfrb PORTE=7;
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
sfrb UBRR0H=0x20;
sfrb UCSR0C=0x20;
sfrb WDTCR=0x21;
sfrb OCR2=0x22;
sfrb TCNT2=0x23;
sfrb ICR1L=0x24;
sfrb ICR1H=0x25;
sfrb ASSR=0x26;
sfrb TCCR2=0x27;
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
sfrb OCR0=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0=0x33;
sfrb MCUCSR=0x34;
sfrb MCUCR=0x35;
sfrb EMCUCR=0x36;
sfrb SPMCR=0x37;
sfrb TIFR=0x38;
sfrb TIMSK=0x39;
sfrb GIFR=0x3a;
sfrb GICR=0x3b;
sfrb UBRR1H=0x3c;
sfrb UCSR1C=0x3c;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-
 // Interrupt vectors definitions
// Needed by the power management functions (sleep.h)
#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x20
	.SET power_ctrl_reg=mcucr
	#endif
#endasm
void inisialisasiTimer(void);
void inisialisasiTimer(void)
{  
    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: 11059,200 kHz
    // Mode: CTC top=OCR0
    // OC0 output: Toggle on compare match
    TCCR0=0x19;
    TCNT0=0x00;
    OCR0=0x04;  
    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 1382,400 kHz
    // Mode: Normal top=FFFFh
    // OC1A output: Discon.
    // OC1B output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer 1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: On
    // Compare B Match Interrupt: Off
    TCCR1A=0x00;
    TCCR1B=0x02;
    TCNT1H=0x00;
    TCNT1L=0x00;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x6B;
    OCR1AL=0xFF;
    OCR1BH=0x00;
    OCR1BL=0x00; 
     // Timer/Counter 2 initialization
    // Clock source: System Clock
    // Clock value: 11059,200 kHz
    // Mode: CTC top=OCR2
    // OC2 output: Toggle on compare match
    ASSR=0x00;
    TCCR2=0x19;
    TCNT2=0x00;
    OCR2=0x91;
        // Timer/Counter 3 initialization
    // Clock value: Timer 3 Stopped
    // Mode: Normal top=FFFFh
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // OC3A output: Discon.
    // OC3B output: Discon.
    // Timer 3 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
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
        // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=0x40;
    (*(unsigned char *) 0x7d)=0x00;
} 
