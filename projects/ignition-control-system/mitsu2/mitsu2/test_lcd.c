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

#define delim 74

//enum { UP, DOWN };

//volatile uint16_t pwm; /* Note [1] */
//volatile uint8_t direction;
volatile char buffer[10];
volatile int unsigned num;
volatile int unsigned num1;
volatile int unsigned num2;
volatile int unsigned asd;
volatile int unsigned asd1;
volatile static int analog_result;
volatile static unsigned char analog_busy;
#define channel  4
volatile int i;
volatile div_t dfg;
#define del 40000*60/4
const unsigned int PROGMEM fgh[] =
{
	del/500,  0,  	//500 RPM
	del/600,  1,    //1000RPM
	del/700,  2,
	del/800,  3,
	del/900,  4,
	del/1000, 5,
	del/1100, 6,
	del/1200, 7,
	del/1300, 8,
	del/1400, 9,
	del/1500, 10,
	del/1600, 11,
	del/1700, 12,
	del/1800, 13,
	del/1900, 14,
	del/2000, 15,
	del/2100, 16,
	del/2200, 17,
	del/2300, 18,
	del/2400, 19,
	del/2500, 20,
	del/2600, 21,
	del/2700, 22,
	del/2800, 23,
	del/2900, 24,
	del/3000, 25,
	del/3100, 26,
	del/3200, 27,
	del/3300, 28,
	del/3400, 29,
	del/3500, 30,
	del/3600, 31,
	del/3700, 32,
	del/3800, 33,
	del/3900, 34,
	del/4000, 35,
	del/4100, 36,
	del/4200, 37,
	del/4300, 38,
	del/4400, 39,
	del/4500, 40,
	del/4600, 41,
	del/4700, 42,
	del/4800, 43,
	del/4900, 44,
	del/5000, 45,
	del/5100, 46,
	del/5200, 47,
	del/5300, 48,
	del/5400, 49,
	del/5500, 50,
	del/5600, 51,
	del/5700, 52,
	del/5800, 53,
	del/5900, 54,
	del/6000, 55,
	del/6100, 56,
	del/6200, 57,
	del/6300, 58,
	del/6400, 59,
	del/6500, 60,
	del/6600, 61,
	del/6700, 62,
	del/6800, 63,
	del/6900, 64,
	del/7000, 65
};

SIGNAL (SIG_INTERRUPT1)
{
 TCNT1=65535-delim;
 num1=num;
 num=0;
 PORTB|= _BV(PB1);
}

SIGNAL (SIG_ADC) {
        unsigned char adlow,adhigh;
        adlow=ADCL;
        adhigh=ADCH;
        analog_result=(adhigh<<8)|(adlow & 0xFF);
        analog_busy=0;
}

INTERRUPT (SIG_OVERFLOW2) /* Note [2] */
{
TCNT2=0;  //okolo 20Hz - 19.92Hz
PORTB&= ~_BV(PB1);
for (i=0;i<66;i++) {if ( num1 >= fgh[i*2] ) break;}
eeprom_busy_wait ();
num2 = eeprom_read_word (i*2);

if ( ++asd == 20 )
 {
//	ADCSRA = (1<<ADSC);
//	PORTB|= _BV(PB0);
	lcd_gotoxy(0,0);
        itoa( 40000/num1, buffer, 10);
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
//	itoa( fgh[i*2+1], buffer, 10);
	itoa( num2, buffer, 10);
	lcd_puts(",");
	lcd_puts(buffer);
	lcd_puts("      ");
 	asd=0;
//	PORTB&= ~_BV(PB0);
 }
}

SIGNAL (SIG_OVERFLOW1) /* Note [2] */
{
// if ( num == num2 )
// {
//  PORTB|= _BV(PB0);
// }
 PORTB&= ~_BV(PB1);
 TCNT1=65535-delim;
 num++;
}

void
ioinit (void)
{
    cli();
    TCCR1B = _BV (CS10);
    TCCR2 = _BV (CS21)|_BV (CS20)|_BV (CS22);
    timer_enable_int ( _BV (TOIE1) | _BV (TOIE2) );
    MCUCR = _BV (ISC11);
    GICR = _BV(INT1);
    ADMUX  = (1<<REFS1)|(1<<REFS0)|(channel );
    ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADIF)|(1<<ADPS2)|(1<<ADFR)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADSC);
    for (i=0;i<66;i++) {eeprom_write_word (i*2, fgh[i*2+1]);eeprom_busy_wait ();}
    sei ();
}

int main(void)
{
    DDRD &=~ (1 << PD3); /* Pin PD2 input*/
    PORTD |= (1 << PD3); /* Pin PD2 pull-up enabled*/
    lcd_init(LCD_DISP_ON);
    ioinit ();

    WDTCR=0;
    lcd_clrscr();   /* clear display home cursor */ 
    DDRB|= _BV(PB1);
    DDRB|= _BV(PB0);
    for (;;) {}
}
