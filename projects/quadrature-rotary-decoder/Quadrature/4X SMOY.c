/*****************************************************
This program was produced by the
CodeWizardAVR V2.60 Standard
Automatic Program Generator
© Copyright 1998-2012 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project                  : Meter counter/Leine Linde RS501 (incremental rotary encoder)
Version                  : 120916
Date                     : 19.2.2012
Author                   : Klaus Varis
Company                  : SMOY
Chip type                : ATmega32
Program type             : Application
AVR Core Clock frequency : 20,000 MHz (Overclocked frequency +4 MHz)
Memory model             : Small
External RAM size        : 0
Data Stack size          : 512
Maximum detection speed  : 220/4 kHz (4,54 µs)
Maximun RPM              : 6600
Maximum Propagation      : 33,35 m/s or 120 km/h
*****************************************************/

#include <mega32.h>
#include <alcd.h>
#include <stdio.h>
#include <delay.h>
#define S00 PIND.2
#define S90 PIND.3
#define RESET PINB.2

// Global routine variables
long int pulses = 0;
register unsigned char input;
unsigned long int pulse_errors = 0;
unsigned char clear_lcd_once = 1;
const unsigned long int ERROR_THRESHOLD = 50;
const float calibration_pulses = 659636;
const float calibrated_meters = 100.0;

#pragma keep+
flash char LookupTable[256] = 
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x01, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x01,
    0x01, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x01,
    0x01, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x01, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
}; // Inverted lookup table for postive counting meters (0xFF = 0x01 and 0x01 = 0xFF)
#pragma keep-

void reset_pulses();
void init();

// Declare your global variables here
char lcd_buf[80];

// External Interrupt 0 service routine on both edges
interrupt [EXT_INT0] void ext_int0_isr(void)
{
    #asm        	    
        PUSH    R16
        PUSH    R17
        PUSH    R18
        PUSH    R19
        PUSH    R22
        PUSH    R24
        PUSH    R30
        IN      R30, SREG
        PUSH    R30
        PUSH    R31   
        IN      R31, 0x10
        LSR     R31
        LSR     R31
        LSR     R31
        ROL     _input
        LSR     R31
        ROL     _input             
        MOV     R30, _input
        EOR		R31, R31
	    SUBI	R30, LOW(-_LookupTable << 1)
	    SBCI	R31, HIGH(-_LookupTable << 1)
	    LPM		R22, Z
	    CPI		R22, 0
	    BREQ	Error_A
	    LDS		R16, _pulses
	    LDS		R17, _pulses+1
	    LDS		R18, _pulses+2
	    LDS		R19, _pulses+3	
	    MOV		R24, R22
	    ADD		R24, R24
	    SBC		R24, R24
	    ADD		R16, R22
	    ADC		R17, R24
	    ADC		R18, R24
	    ADC		R19, R24
	    STS		_pulses, R16
	    STS		_pulses+1, R17
	    STS		_pulses+2, R18
	    STS		_pulses+3, R19
    	JMP		Exit_A
    Error_A:
	    LDS		R16, _pulse_errors
	    LDS		R17, _pulse_errors+1
	    LDS		R18, _pulse_errors+2
	    LDS		R19, _pulse_errors+3	
	    LDI		R24, 1
	    ADD		R16, R24
	    LDI		R24, 0
	    ADC		R17, R24
	    ADC		R18, R24
	    ADC		R19, R24
	    STS		_pulse_errors, R16
	    STS		_pulse_errors+1, R17
	    STS		_pulse_errors+2, R18
	    STS		_pulse_errors+3, R19
    Exit_A:
        POP     R31
        POP     R30
        OUT     SREG, R30        
        POP     R30
        POP     R24
        POP     R22
        POP     R19
        POP     R18
        POP     R17
        POP     R16           
    #endasm    
}

// External Interrupt 1 service routine on both edges
interrupt [EXT_INT1] void ext_int1_isr(void)
{
    #asm        	      
        PUSH    R16
        PUSH    R17
        PUSH    R18
        PUSH    R19
        PUSH    R22
        PUSH    R24
        PUSH    R30
        IN      R30, SREG
        PUSH    R30
        PUSH    R31 
        IN      R31, 0x10
        LSR     R31
        LSR     R31
        LSR     R31
        ROL     _input
        LSR     R31
        ROL     _input             
        MOV     R30, _input
        EOR		R31, R31
	    SUBI	R30, LOW(-_LookupTable << 1)
	    SBCI	R31, HIGH(-_LookupTable << 1)
	    LPM		R22, Z
	    CPI		R22, 0
	    BREQ	Error_B
	    LDS		R16, _pulses
	    LDS		R17, _pulses+1
	    LDS		R18, _pulses+2
	    LDS		R19, _pulses+3	
	    MOV		R24, R22
	    ADD		R24, R24
	    SBC		R24, R24
	    ADD		R16, R22
	    ADC		R17, R24
	    ADC		R18, R24
	    ADC		R19, R24
	    STS		_pulses, R16
	    STS		_pulses+1, R17
	    STS		_pulses+2, R18
	    STS		_pulses+3, R19
    	JMP		Exit_B
    Error_B:
	    LDS		R16, _pulse_errors
	    LDS		R17, _pulse_errors+1
	    LDS		R18, _pulse_errors+2
	    LDS		R19, _pulse_errors+3	
	    LDI		R24, 1
	    ADD		R16, R24
	    LDI		R24, 0
	    ADC		R17, R24
	    ADC		R18, R24
	    ADC		R19, R24
	    STS		_pulse_errors, R16
	    STS		_pulse_errors+1, R17
	    STS		_pulse_errors+2, R18
	    STS		_pulse_errors+3, R19
    Exit_B:        
        POP     R31
        POP     R30
        OUT     SREG, R30        
        POP     R30
        POP     R24
        POP     R22
        POP     R19
        POP     R18
        POP     R17
        POP     R16                  
    #endasm   
}

// External Interrupt 2 service routine for reset switch (falling edge)
interrupt [EXT_INT2] void ext_int2_isr(void)
{
    // Place your code here
    unsigned int cnt = 0;
    GICR &= 0b11011111; // Disable EXT_INT2
    lcd_init(20);
    lcd_putsf("Resetting...\n\nDo not move\nthe encoder!");    
    while (RESET == 0)
    {
        delay_ms(1);
        cnt++;
        if (cnt > 2000)
        {            
            lcd_clear();            
            lcd_putsf("\nHardware reset!\nPlease wait...");
            while (RESET == 0) delay_ms(1);
            lcd_clear();
            delay_ms(1000);
            GIFR |= 0b00100000; // Clear pending EXT_INT2 interrupts caused by key debounce 
            WDTCR = 0x18;       // Sophisticated reset with Watch Dog Timer (WDT).
            WDTCR = 0x08;       // In some situations an ordinary JMP 0x0000 may not be enough.
            while(1);           // Wait until WDT resets the chip.
        }
    }                  
    #asm("sei");
    do
    {        
        reset_pulses();
        for (cnt = 0; cnt < 3500; cnt++)
        {
            lcd_gotoxy(19,3);
            if (pulses != 0)
            {
                lcd_putchar(255);
                delay_us(50);
            }
            else
            {
                lcd_putchar(' ');
                delay_us(50);
            }            
        }       
    }
    while(pulses != 0); 
    reset_pulses();
    lcd_init(20);
    lcd_clear();
    for (cnt = 0; cnt < 80; cnt++) lcd_buf[cnt] = 0; // Fixes random lcd_clear issues with undesired character messy. Maybe enhanced lcd_clear function shoud be created.
    GIFR |= 0b00100000; // Clear pending EXT_INT2 interrupts caused by key debounce 
    GICR |= 0b00100000; // Re-enable EXT_INT2       
}

void main(void)
{
    unsigned int cnt = 0;
    init();
    while ((RESET == 0) & (cnt != 2000))
    {
        cnt++;
        delay_ms(1);
    }
    if (cnt >= 2000)
    {
        sprintf(lcd_buf, "  Calibration data\nMeters  : %10.3f\nPulses  : %10.0f\nMaximum : %2.2f m/s",calibrated_meters, calibration_pulses, (calibrated_meters/calibration_pulses)*220000);
        lcd_puts(lcd_buf);
        while (1);
    }     
    if (RESET == 1)
    {                
        lcd_putsf("\n      Drillcon\n        SMOY");
        do
        {
            delay_ms(1);
            cnt++;
            if (RESET == 0) goto SKIP;
        }
        while (cnt != 3750);
        cnt = 0;
        lcd_clear();        
        lcd_putsf("\n    Software ver.\n       120916"); 
        do
        {
            delay_ms(1);
            cnt++;
            if (RESET == 0) goto SKIP;
        }
        while (cnt != 3750);
        cnt = 0;
        lcd_clear();
        lcd_putsf("Precise reading of\nQuadrature incremen.\nrotary encoder like\nLeine & Linde RS-501");
        do
        {
            delay_ms(1);
            cnt++;
            if (RESET == 0) goto SKIP;
        }
        while (cnt != 3750);        
        cnt = 0;
    }
    SKIP:
    while(RESET == 0) delay_ms(1);                                                   
    lcd_clear();
    GIFR |= 0b00100000; // Clear pending EXT_INT2 interrupts     
    #asm("sei") // Global enable interrupts
    reset_pulses();
    while (1)
    {
        // Place your code here;   
        if (pulse_errors > ERROR_THRESHOLD)
        {
            if (clear_lcd_once == 1)
            {
                clear_lcd_once = 0;
                GICR &= 0b11011111; // Disable EXT_INT2
                lcd_clear();
                GICR |= 0b00100000; // Re-enable EXT_INT2
            }               
            sprintf(lcd_buf, "  Depth in meters\n     [%8.3f]\n\n    Errors: %lu", (calibrated_meters/calibration_pulses)*pulses, pulse_errors);
            //else sprintf(lcd_buf, "Pulses %li\some erros", pulses);
        }
        else sprintf(lcd_buf, "\n  Depth in meters\n     [%8.3f]", (calibrated_meters/calibration_pulses)*pulses);
        //else sprintf(lcd_buf, "Pulses %li", pulses);
        GICR &= 0b11011111; // Disable EXT_INT2, fixes random lcd_clear issues with undesired character messy
        lcd_gotoxy(0, 0);        
        lcd_puts(lcd_buf);
        GICR |= 0b00100000; // Re-enable EXT_INT2
    }
}
  
void reset_pulses()
{
    // Pulse sensor initialization
    if ((S00 == 0) && (S90 == 0)) input = 0x78;
    if ((S00 == 1) && (S90 == 0)) input = 0x1E;
    if ((S00 == 1) && (S90 == 1)) input = 0x87;
    if ((S00 == 0) && (S90 == 1)) input = 0xE1;
    pulse_errors = 0;
    pulses = 0;
    clear_lcd_once = 1;
}

void init()
{
    unsigned char temp;
    // Input/Output Ports initialization
    // Port A initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTA=0x00;
    DDRA=0x00;

    // Port B initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=P State1=T State0=T 
    PORTB=0x04;
    DDRB=0x00;

    // Port C initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTC=0x00;
    DDRC=0x00;

    // Port D initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=P State2=P State1=T State0=T 
    PORTD=0x0C;
    DDRD=0x00;

    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: Timer 0 Stopped
    // Mode: Normal top=0xFF
    // OC0 output: Disconnected
    TCCR0=0x00;
    TCNT0=0x00;
    OCR0=0x00;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: Timer1 Stopped
    // Mode: Normal top=0xFFFF
    // OC1A output: Discon.
    // OC1B output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer1 Overflow Interrupt: Off
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
    // Clock value: Timer2 Stopped
    // Mode: Normal top=0xFF
    // OC2 output: Disconnected
    ASSR=0x00;
    TCCR2=0x00;
    TCNT2=0x00;
    OCR2=0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=0x00;

    // USART initialization
    // USART disabled
    UCSRB=0x00;

    // Analog Comparator initialization
    // Analog Comparator: Off
    // Analog Comparator Input Capture by Timer/Counter 1: Off
    ACSR=0x80;
    SFIOR=0x00;

    // ADC initialization
    // ADC disabled
    ADCSRA=0x00;

    // SPI initialization
    // SPI disabled
    SPCR=0x00;

    // TWI initialization
    // TWI disabled
    TWCR=0x00;

    // Alphanumeric LCD initialization
    // Connections are specified in the
    // Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
    // RS - PORTA Bit 0
    // RD - PORTA Bit 1
    // EN - PORTA Bit 2
    // D4 - PORTA Bit 4
    // D5 - PORTA Bit 5
    // D6 - PORTA Bit 6
    // D7 - PORTA Bit 7
    // Characters/line: 20
    lcd_init(20);
    for (temp = 0; temp < 80; temp++) lcd_buf[temp] = 0;

    // External Interrupt(s) initialization
    // INT0: On
    // INT0 Mode: Any change
    // INT1: On
    // INT1 Mode: Any change
    // INT2: On
    // INT2 Mode: Falling Edge
    GICR|=0xE0;
    MCUCR=0x05;
    MCUCSR=0x00;
    GIFR=0xE0;
}