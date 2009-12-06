/*************************************************************************
Title:    testing output to a HD44780 based LCD display.
Author:   Peter Fleury  <pfleury@gmx.ch>  http://jump.to/fleury
File:     $Id: test_lcd.c,v 1.6 2004/12/10 13:53:59 peter Exp $
Software: AVR-GCC 3.3
Hardware: HD44780 compatible LCD text display
          ATS90S8515/ATmega if memory-mapped LCD interface is used
          any AVR with 7 free I/O pins if 4-bit IO port mode is used
**************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd.h"
#include <avr/eeprom.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

/*
** constant definitions
*/
static const PROGMEM unsigned char copyRightChar[] =
{
	0x07, 0x08, 0x13, 0x14, 0x14, 0x13, 0x08, 0x07,
	0x00, 0x10, 0x08, 0x08, 0x08, 0x08, 0x10, 0x00
};

#if defined(__AVR_ATmega8__)
#  define OC1 PB1
#  define DDROC DDRB
#  define OCR OCR1A
#  define PWM10 WGM10
#  define PWM11 WGM11
#else
#  error "Don't know what kind of MCU you are compiling for"
#endif

#if defined(COM11)
#  define XCOM11 COM11
#elif defined(COM1A1)
#  define XCOM11 COM1A1
#else
#  error "need either COM1A1 or COM11"
#endif

enum { UP, DOWN };

volatile uint16_t pwm; /* Note [1] */
volatile uint8_t direction;
volatile char buffer[10];
volatile int unsigned num;
volatile int unsigned freq;
volatile int unsigned num1;
volatile int unsigned asd;
volatile int unsigned asd1;
volatile static int analog_result;
volatile static unsigned char analog_busy;
const int channel = 4;
volatile div_t dfg;

SIGNAL (SIG_INTERRUPT1)
{
 num1=num;
 num=0;
 PORTB|= _BV(PB1);
}

INTERRUPT (SIG_ADC) {
        unsigned char adlow,adhigh;
        adlow=ADCL; //Do not combine the two lines into one C statement
        adhigh=ADCH;
        analog_result=(adhigh<<8)|(adlow & 0xFF);
        analog_busy=0;
}

SIGNAL (SIG_OVERFLOW2) /* Note [2] */
{
TCNT2=255-196;  //okolo 20Hz - 19.92Hz
PORTB&= ~_BV(PB1);
if ( ++asd == 10 )
 {
	
	lcd_gotoxy(0,0);
        itoa( num, buffer, 10);
	lcd_puts(buffer);
	lcd_puts("Hz");
	lcd_puts(",");
	dfg = div(analog_result,400);
	itoa( dfg.quot, buffer, 10);
	lcd_puts(buffer);
	lcd_puts(".");
	if (dfg.rem / 0.4 < 100) {lcd_puts("0");};
	if (dfg.rem / 0.4 < 10)  {lcd_puts("0");};
	itoa( dfg.rem / 0.4, buffer, 10);
	lcd_puts(buffer);
	lcd_puts("V     \n");
	itoa( num1, buffer, 10);
        lcd_puts(buffer);
	itoa( ++asd1, buffer, 10);
	lcd_puts(",");
	lcd_puts(buffer);
	lcd_puts("    ");
 	asd=0;
 }
 num=0;
}

SIGNAL (SIG_OVERFLOW1) /* Note [2] */
{
TCNT1=65535-20;
    num++;
/*    switch (direction) 
    {
        case UP:
            if (++pwm == 900)
                direction = DOWN;
            break;
        case DOWN:
            if (--pwm == 50)
                direction = UP;
            break;
    }
    OCR = pwm; */
}

void
ioinit (void) /* Note [5] */
{
    cli();
    /* tmr1 is 10-bit PWM */
//    TCCR1A = _BV (PWM10) | _BV (PWM11) | _BV (XCOM11);
//     TCCR1A = _BV (XCOM11);
    /* tmr1 running on full MCU clock */
    TCCR1B = _BV (CS10); // 4MHz/256 = 15625Hz
    TCCR2 = _BV (CS21)|_BV (CS20)|_BV (CS22);
    /* set PWM value to 0 */
//     OCR = 511;
    /* enable OC1 and PB2 as output */
//     DDROC = _BV (OC1);
    timer_enable_int ( _BV (TOIE1) | _BV (TOIE2) );
    /* enable interrupts */
//    MCUCR = _BV (ISC11) | _BV (ISC10);
    MCUCR = _BV (ISC11);
    GICR = _BV(INT1);
    ADMUX  = (1<<REFS1)|(1<<REFS0)|(channel );
    ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADIF)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADFR)|(1<<ADSC);
    sei ();
}

int main(void)
{
    DDRD &=~ (1 << PD3); /* Pin PD2 input*/
    PORTD |= (1 << PD3); /* Pin PD2 pull-up enabled*/
    /*eeprom_write_byte (uint8_t *addr, uint8_t value)*/
    /* initialize display, cursor off */
    lcd_init(LCD_DISP_ON);
    ioinit ();
    DDRB|= _BV(PB1);
    /*    eeprom_busy_wait ();
    num = eeprom_read_byte (sdf); */
    lcd_clrscr();   /* clear display home cursor */ 
    DDRB|= _BV(PB1);
    for (;;) {                           /* loop forever */
        /*wait_until_key_pressed();*/
//   	loop_until_bit_is_set(PIND,PIND3);
//  	freq=num;num=0;
/*	eeprom_busy_wait ();
        eeprom_write_byte ( sdf, num); */
    }
}
