/*****************************************************
This program was produced by the
CodeWizardAVR V2.60 Standard
Automatic Program Generator
© Copyright 1998-2012 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Author                   : Klaus Varis
Chip type                : ATmega32
Program type             : Application
AVR Core Clock frequency : 20,000 MHz (Overclocked frequency +4 MHz)
Memory model             : Small
External RAM size        : 0
Data Stack size          : 512
Maximum detection speed  : 170/4 kHz (5,88 µs)
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
register char output;
unsigned long int pulse_errors = 0;

flash const char LookupTable[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0xFF, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0xFF, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

void reset_pulses();
void init();

// Declare your global variables here
char lcd_buf[80];

// External Interrupt 0 service routine on both edges
interrupt [EXT_INT0] void ext_int0_isr(void)
{    
    input = (input << 1) | S00;
    input = (input << 1) | S90;
    output = LookupTable[input];
    pulses += output; 
    if (output == 0) pulse_errors++;
}

// External Interrupt 1 service routine on both edges
interrupt [EXT_INT1] void ext_int1_isr(void)
{
    input = (input << 1) | S00;
    input = (input << 1) | S90;
    output = LookupTable[input];
    pulses += output; 
    if (output == 0) pulse_errors++;
}

// External Interrupt 2 service routine for reset switch (falling edge)
interrupt [EXT_INT2] void ext_int2_isr(void)
{
    // Place your code here
    unsigned int cnt = 0;
    GICR &= 0b11011111; // Disable EXT_INT2             
    while ((RESET == 0) & (cnt != 1000))
    {
        cnt++;
        delay_ms(1);
    } 
    if (cnt > 900)
    {
        lcd_init(20);   
        lcd_putsf("\nHardware reset!\nPlease wait...");
        delay_ms(1000);
        WDTCR = 0x18;   // Sophisticated reset with Watch Dog Timer (WDT).
        WDTCR = 0x08;   // In some situations an ordinary JMP 0x0000 may not be enough.
        while(1);       // Wait until WDT resets the chip.
    }
    // Moving detection during reset
    lcd_init(20);
    lcd_clear();
    lcd_putsf("Resetting...\n\nDo not move\nthe encoder!");
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
    lcd_clear();
    GIFR |= 0b00100000; // Clear pending EXT_INT2 interrupts caused by key debounce 
    GICR |= 0b00100000; // Re-enable EXT_INT2       
}

void main(void)
{
    init();
    lcd_putsf("ROTARY DECODER\nMethod: Lookup 256\nResolution: 4x\nInterrupts: 2");
    delay_ms(3000);
    lcd_clear();    
    lcd_putsf("   Response up to\n      170/4 kHz\n       5.88 uS\nwith error detection");       
    delay_ms(3000);
    lcd_clear();
    // Global enable interrupts
    #asm("sei")
    reset_pulses();
    while (1)
    {
        // Place your code here;   
        sprintf(lcd_buf, "Pins   : S00:%d S90:%d\nErrors : %-11lu\nInput  : %03Xh\nPulses : %-11li", (input >> 1) & 1, (input >> 0) & 1, pulse_errors, input, pulses);  
        GICR &= 0b11011111; // Disable EXT_INT2
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
}

void init()
{
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