/*************************************************************************
 * ad9850 controller and display with Atmega8, HD44780-compatible 2-line LCD
 * and basic 5-pin rotary encoder
 *
 * uses Peter Fleury's excellent LCD routine
 *
 *  Created on: March 4th, 2014
 *      (c) Peter Barnes March 2014
 *      Modify/distribute freely but please include original file and above info!
**************************************************************************/
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd.h"
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "ad9850.h"

// Rotary encoder pin A on INT0 (PD2). Pin of encoder goes through 10K res. to atmega pin, atmega pin then .1 cap to gnd
// Rotary Encoder pin B on PD6		   "																				"
// Encoder push switch grounded with .1 cap on atmega pin

volatile unsigned long frequency = 10000000, frequency_1;
volatile long add_this = 1000 ;	// 'add_this' holds the amount by which we increment (or decrement) the frequency when turning the knob
volatile uint8_t refresh_lcd = 0;	// when we need to increment the freq. change amount, refresh the lcd (even if the freq. isn't changing)
volatile uint8_t f_index = 0;	// index to freq. change amount when turning the dial
void initInterrupts(void)
{
 // Falling edge in INT0 (PD2 / pin4 / INT0) to cause interrupt
 MCUCR |= (1<<ISC01);
 // Enable INT0
 GICR |= (1<<INT0);
 // Falling edge in INT1 (PD3 / pin5 / INT1) to cause interrupt
 MCUCR |= (1<<ISC11);
 // Enable INT1
 GICR |= (1<<INT1);
}

ISR(INT0_vect)
{
	// on interrupt, check level of pin PD6 to get direction.
	if (PIND & _BV(PD6))
 	{
		frequency = frequency + add_this;
 	}
	else if (!(PIND & _BV(PD6)))
	{
		frequency = frequency - add_this;
	}
	_delay_ms(5);	// a little debouncing
}

ISR(INT1_vect)
{
	switch (f_index)
	{
	case 0:
		add_this = 1000;
		break;
	case 1:
		add_this = 10000;
		break;
	case 2:
		add_this = 100000;
		break;
	case 3:
		add_this = 1;
		break;
	case 4:
		add_this = 10;
		break;
	case 5:
		add_this = 100;
		break;
	default:
		add_this = 1000;
	}
	refresh_lcd = 1;
	f_index++;
	if (f_index == 6)
	{
		f_index = 0;
	}
	_delay_ms(70);	// debounce
}

int main(void)
{
	DDRD = 0x80;	// PD7 set to output
	PORTD = 0x7f;	// pullups on inputs PD0-6
	unsigned long temp, temp1, temp2 = 0;
	char freq_string[11];
	char delta_string[16];
	// initialize ad9850
	setup_serial();

	// initialize LCD, cursor off
    lcd_init(LCD_DISP_ON);
    lcd_puts("     AA1YY     \n");
    lcd_puts("   AD9850 VFO   ");
    _delay_ms(2000);
    initInterrupts();
    sei();

    while (1)	// main loop
    {
    	if ((PIND & _BV(PD4)) == 0)	// if we've pressed the encoder button (making pin6 '0'), start a timer
     	{
    		cli();
    		temp2++;
    		if (temp2 == 300000)	// we've reached the limit, activate (or deactivate) something attached to PD7
    		{
    			temp2 = 0;
    			PORTD ^= (1 << PD7);	// flip pin at PD7
    		}
    		sei();
    	}
    	else
    	{
    		temp2 = 0;	// so that continued short button presses won't activate the above timer after a while
    	}
    	if ((frequency != frequency_1) || (refresh_lcd ==  1))
    	{
    		temp = frequency / 1000000;
    		ultoa(temp, freq_string, 10);
    		switch (add_this)
    		{
    			case 1:
    				strcpy (delta_string, "           :1\n");
    				break;
    			case 10:
    				strcpy (delta_string, "          :10\n");
    				break;
    			case 100:
    				strcpy (delta_string, "         :100\n");
    				break;
    			case 1000:
    				strcpy (delta_string, "          :1K\n");
    				break;
    			case 10000:
    				strcpy (delta_string, "         :10K\n");
    				break;
    			case 100000:
    				strcpy (delta_string, "        :100K\n");
    				break;
    			default:
    				strcpy (delta_string, "          ERR\n");
    		}
    		lcd_clrscr();
    		lcd_puts("VFO");
    		lcd_puts(delta_string);
    		lcd_puts(freq_string);
    		lcd_puts(".");
    		temp1 = (frequency / 1000) - (1000 * temp);
    		if (temp1 < 10)
    		{
    			ultoa(temp1, freq_string, 10);
    			lcd_puts("00");
    			lcd_puts(freq_string);
    		}
    		else if (temp1 < 100)
    		{
    			ultoa(temp1, freq_string, 10);
    			lcd_puts("0");
    			lcd_puts(freq_string);
    		}
    		else
    		{
    			ultoa(temp1, freq_string, 10);
    			lcd_puts(freq_string);
    		}

    		lcd_puts(" MHZ  +");
		    temp = temp - ((frequency / 1000000)*1000000);
    		temp = frequency - ((frequency / 1000)*1000);
		    ultoa(temp, freq_string, 10);
    		lcd_puts(freq_string);
    		// send frequency to AD9850
    		;
    		load_tuning_word_serial(make_tuning_word(frequency));
    		frequency_1 = frequency;
        	refresh_lcd = 0;
    	}
    }
}


