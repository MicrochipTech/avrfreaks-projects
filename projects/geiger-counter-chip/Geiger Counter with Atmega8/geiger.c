/*
 * geiger.c
 * Geiger Counter backend for Atmega8 and LCD
 * Displays counts collected on TI (pin 11) on LCD in counts-per-second and counts-per-minute
 * Flashes LED on pin PC4. Generates 2.8 KHz square wave on pin PC5 to run high-voltage section.
 *
 *  Created on: Dec 12, 2012
 *      Author: Peter Barnes
 *
 *      LCD routines use Peter Fleury's excellent AVR LCD library
 *      See: http://homepage.hispeed.ch/peterfleury/avr-lcd44780.html
 *
 *      No external crystal, set your fuses for 8MHz internal clock!
 *      2x16 HD44780 LCD
 *      LCD is attached to PORTB:
 *      	D1	PB0
 *      	D2	PB1
 *      	D3 	PB2
 *      	D4	PB3
 *      	RS	PB4
 *      	R/W	PB5
 *      	E	PB6
 *
 *      	Pulse Input in on ATMEGA8 PD5, expects 5 volt square pulse
 *
 */
// ATMEGA8 - 8 MHZ internal clock
#define F_CPU 8000000UL

#include "lcd.h"   // LCD library by Peter Fleury
#include <util/delay.h>
#include<stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/signal.h>

unsigned long int cpm = 0;
unsigned int cps = 0;
int k = 0, l = 0, update_cpm = 0, update_cps = 0, event_flag = 0, debounce = 0;
char cpm_string[5];
char cps_string[6];

int main (void)
{
    lcd_init(LCD_DISP_ON);    // initialize display, cursor off 
    lcd_clrscr();  // clear display and home cursor
    lcd_puts("     AA1YY\n");   // Greeting string
    lcd_puts(" Geiger Counter");

    // setup timer0, our one-second timer
    TIMSK |= (1<<TOV0);  //  enable timer overflow interrupt
    TCNT0=0x00;     // set timer counter initial value
    TCCR0 |= (1 << CS02);    // prescale by 256

     // setup counter 1 to receive events via PD5 (T1)
    DDRD &= ~(1 << DDD5);
    PORTD |= (1 << PORTD5);   // turn On the Pull-up, PD5 is now an input with pull-up enabled
    TIMSK |= (1 << TOIE1);    // enable timer interrupt
    TCCR1B |= (1 << CS12) | (1 << CS11) | (1 << CS10);    // Turn on the counter

    // setup timer2 to generate pulses for the HV transformer
    TIMSK |= (1 << OCIE2);   //  enable compare interrupt
    TCCR2 |= (1 << CS21) | (1 << WGM21);   // divide by 8 prescale, CTC mode
    OCR2 = 0xBF;    // adjust frequency of pulses here. At 8MHz clock output is ~ 2.8 KHz
    
    DDRD &= ~(1 << DDD2);     // PD2 (INT0) is now an input
    PORTD |= (1 << PORTD2);   // PD2 is input with pull-up
    MCUCR |= (1 << ISC01);    // set INT0 to trigger on falling edge
    GICR |= (1 << INT0);      // Turn on INT0

	  // Set Port C indicator pins
    DDRC = 0x3F;  // PC0-5 as outputs
    PORTC = 0x1F;   //for testing

    _delay_ms(2000);
    lcd_clrscr();
    lcd_puts("CPS   | CPM\n");
    lcd_puts(" -    |   -  ");

    while(1)   //main program loop - just display updated counts as they come in every second and every minute
      {
      sei();  // enable interrupts
      if (update_cpm == 1)
      	  {
    	  	  sprintf(cpm_string, "%lu", cpm);  // form the string to send to the LCD
    	  	  lcd_home();
    	  	  lcd_gotoxy(8,1);
    	  	  lcd_puts("          ");   // clear this section of the display
    	  	  lcd_gotoxy(8,1);
    	  	  lcd_puts(cpm_string);   // and print the string to the LCD
    	  	  update_cpm=0;
    	  	  cpm = 0;
    	  }

      if (update_cps == 1)
      	  {
    	  	  cps = cps + TCNT1;
    	  	  sprintf(cps_string, "%u", cps);   // form the string to send to the LCD
    	  	  lcd_gotoxy(0,1);
    	  	  lcd_puts("      |");   // clear this section of the display
    	  	  lcd_gotoxy(0,1);
    	  	  lcd_puts(cps_string);   // and print the string to the LCD
    	  	  cpm += cps;
    	  	  update_cps=0;
    	  	  cps = 0;
    	  	  TCNT1 = 0;
      	  }

      if (event_flag == 1 || debounce != 0)  // lengthen the event pulse enough for an indicator
      {
    	  debounce++;
    	  event_flag = 0;
    	  PORTC |= _BV(PC4);   //turn on PC4
    	  if (debounce > 500 )  // this number determines the length of the "event happened" signal (LED or Speaker click)
    	  {
    		  PORTC &=~_BV(PC4);;   //turn off PC4
    		  debounce = 0;  // reset debounce flag
    	  }
      }
   }
}

/* signal handler for timer interrupt TOV0 */
ISR(TIMER0_OVF_vect) {  // timer0 is our one-second interrupter
	k++;
	if (k == 123)   // we're here every second at 8MHz clock
		{
		k=0;
		update_cps=1;   //display counts-per-second
		l++;
		if (l == 60)  // one minute
			{
			update_cpm = 1;  // flag to display updated counts per minute
			l=0;
			}
		}
	}

ISR(INT0_vect )
{
	event_flag = 1;
}

ISR(TIMER1_OVF_vect) {
   // timer1 overflow (event counter has gone over 65536)
	cps = (cps + 65536);
}

ISR (TIMER2_COMP_vect)
{
	PORTC ^= (1<<PC5); // flip PC5
}
