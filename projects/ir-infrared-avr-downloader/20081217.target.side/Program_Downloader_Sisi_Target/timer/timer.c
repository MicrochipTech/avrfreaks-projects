#include "timer.h" 

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
    TCCR3A=0x00;
    TCCR3B=0x00;        
    TCNT3H=0x00;
    TCNT3L=0x00;
    ICR3H=0x00;
    ICR3L=0x00;
    OCR3AH=0x00;
    OCR3AL=0x00;
    OCR3BH=0x00;
    OCR3BL=0x00; 
    
    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=0x40;
    ETIMSK=0x00;
} 