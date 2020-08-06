//ICC-AVR application builder : 7/27/2017 9:52:14 AM
// Target : M8
// Crystal: 1.0000Mhz
// NOTE: WHILE WE STRIVE TO GENERATE CORRECT CODE FOR ALL SUPPORTED DEVICES,
//   THE APPBUILDER MAY GENERATE INCORRECT CODE AT TIMES. IF YOU FIND ANY
//   ISSUES. PLEASE EMAIL support@imagecraft.com. THANK YOU
// The SPI module seems to have issues. We are looking into it.

#include <iccioavr.h>
#include <avrdef.h>
#include "init.h"

void port_init(void)
{
    PORTB = 0x00;
    DDRB  = 0x02; //PB1 PWM output
    PORTC = 0x00;
    DDRC  = 0x00;
    PORTD = 0x01; // RXD pull up on
    DDRD  = 0x02; // TXD output

    PORTD = 0x02; // TXD idle high
}

//TIMER1 initialize - prescale:1
// WGM: 7 PWM 10bit fast, TOP=0x03FF
// desired value: 1KHz
// actual value:  0.977KHz (2.4%)
void timer1_init(void)
{
    TCCR1B = 0x00; //stop
    TCNT1H = 0xFC; //setup
    TCNT1L = 0x01;
    OCR1AH = 0x03;
    OCR1AL = 0xFF;
    OCR1BH = 0x03;
    OCR1BL = 0xFF;
    ICR1H  = 0x03;
    ICR1L  = 0xFF;
    TCCR1A = 0x83;
    TCCR1B = 0x09; //start Timer
}

//UART0 initialize
// desired baud rate: 4800
// actual: baud rate:4808 (0.2%)
void uart0_init(void)
{
    UCSRB = 0x00; //disable while setting baud rate
    UCSRA = 0x00;
    UCSRC = BIT(URSEL) | 0x06;  //8N1 default
    UBRRL = 0x0C; //set baud rate lo
    UBRRH = 0x00; //set baud rate hi
    UCSRB = BIT(TXEN);
}

//ADC initialize
// Conversion time: 104uS
void adc_init(void)
{
    ADCSRA = 0x00; //disable adc
    ADMUX = (1<<REFS0); //AREF internal, ADC(0)
    ACSR  = (1<<ACD); //AC disable
    ADCSRA |= (1<<ADEN)|(3<<ADPS0); // set ADC prescaler to , 1MHz / 8 = 125kHz
}

//call this routine to initialize all peripherals
void init_devices(void)
{
//stop errant interrupts until set up
    INTR_OFF();
    port_init();
    timer1_init();
    uart0_init();
    adc_init();

    MCUCR = 0x00;
    GICR  = 0x00;
    TIMSK = 0x00; //timer interrupt sources
    INTR_ON();  //enable global interrupts
//all peripherals are now initialized
}

