/*******************************************************************************************
 * BuzzBox.c
 *
 * Created: 5/29/2012 9:34:23 PM
 *  Author: Sherman
 *
 * Description:
 * BuzzBox is an audible Ohm meter used to check for wiring errors in a protoboard.  
 * Design goals:
 *	1 - resolve between a good connection and a poor one
 *	2 - device must provide audible indication of connection quality
 *	3 - test voltage must not activate semiconductor junctions
 * 	4 - test current must be no more than a few mA
 *	5 - device should be low power
 *
 * This project started as an idea taken from elm-chan.org, but his goal was to make a 
 * flea-powered device that never needed to be turned off.  I wanted an accurate audible low-
 * ohms meter.  See the builder notes for more info.
 *
 * BuzzBox is licensed under a Creative Commons 
 * Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 *
 * History:
 * 5/29/12 - design and code
 * 6/1/12 - debug and put in enclosure
 * 6/2/12 -done
 *
 *******************************************************************************************/ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>

// IO
/*
PB0		output		OC0A drive for speaker
PB1		output		LED drive
PB2		spare		
PB3		spare
PB4		analog in	ADC2 voltage sense input
PB5		input		reset/debug
*/
#define	LED_RED		0x02
#define LED_GRN		0x04
#define LED_BLU		0x08
#define SPKR		0x01

//------------------------------------------------------------------------------------
// Useful constants
// ADC
#define ADC_START		0xD0
#define ADC_STAT_MASK	0x40
#define ADC_STAT_REG	ADCSRA
#define ADC_CLR_STAT	0x90

// General
#define ON				1
#define OFF				0

// converts an index to a frequency - these plug into OCR0B
/* I don't have a clue what frequency the clock is running at.  It's supposed
to be 128kHz, but based on the tweaking I had to do on the freq select values
below the clock is around 58kHz.  Anyway, the values below yield about 4kHz at
0 Ohms and 1700 Hz at 90 Ohms.  0 Ohms is shrillest/loudest, 90 is barely audible. 
These values work well with the piezo I happened to have laying around.  They may need
to be tweaked for a different pieze */
const uint8_t freqSel[11] = {
	0, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1A, 0x1C, 0x1F  };

//------------------------------------------------------------------------------------
// Prototypes
int adc_read (void);			// starts ADC and waits for result
void led_ctrl (uint8_t color, uint8_t ctrl);	// turns led on or off
void spkr_ctrl (int freq);		// sets speaker frequency; 0 is off
	/* freq ranges 0 (off) to 10 as determined by the freqSel[] array
	above. The point is to generate a tone whose frequency is inversely proportional to
	resistance */

//====================================================================================
int main(void)
{
	volatile int reading;
	
	// init ports
	DDRB |= LED_RED | LED_GRN | LED_BLU | SPKR;
	PORTB |= LED_RED | LED_GRN | LED_BLU;			// high == LED off
	
	// ADC init
	ADMUX = 0x02;		// routes PB4 to ADC
	ADCSRA = 0x90;		// manual start, polled (no ISR)
	ADCSRB = 0;
	DIDR0 = 0x10;		// disable PB4 digital
	
	// timer 0 init - taken care of in the spkr_ctrl function
	
	
    while(1) // loop forever
    {
        reading = adc_read();
		
		if (reading < 8) {				// < 1 Ohm
			spkr_ctrl(1);
			led_ctrl(LED_GRN, ON);
		}
		else if (reading < 24) {		// 1-3 Ohms
			spkr_ctrl(2);
			led_ctrl(LED_BLU, ON);
		}
		else if (reading < 48) {		// 3-6 Ohms
			spkr_ctrl(3);
			led_ctrl(LED_RED,ON);
		}
		else if (reading < 77) {		// 6-10 Ohms
			spkr_ctrl(4);
			led_ctrl(LED_GRN, OFF);
		}
		else if (reading < 110) {		// 10-15 Ohms
			spkr_ctrl(5);
			led_ctrl(LED_GRN, OFF);
		}
		else if (reading < 142) {		// 15-20 Ohms
			spkr_ctrl(6);
			led_ctrl(LED_GRN, OFF);
		}
		else if (reading < 196) {		// 20-30 Ohms
			spkr_ctrl(7);
			led_ctrl(LED_GRN, OFF);
		}
		else if (reading <243) {		// 30-40 Ohms
			spkr_ctrl(8);
			led_ctrl(LED_GRN, OFF);
		}
		else if (reading < 320) {		// 40-60 Ohms
			spkr_ctrl(9);
			led_ctrl(LED_GRN, OFF);
		}
 		else if (reading < 404) {		// 60-90 Ohms
			spkr_ctrl(10);
			led_ctrl(LED_GRN, OFF);
		}
		else {							// > 90 Ohms
			spkr_ctrl(0);
			led_ctrl(LED_GRN, OFF);
		}
   }
}

//====================================================================================
int adc_read (void) {
	// starts ADC and waits for result
	ADC_STAT_REG = ADC_START;
	
	while (ADC_STAT_REG & ADC_STAT_MASK) {		// wait for conversion complete
		// do nothing
		_NOP();
		}
	
	return (ADC);	
}

//-------------------------------------------------------------------------------------
void led_ctrl (uint8_t color, uint8_t ctrl) {
	// turns led on or off
	if (ctrl == OFF) {		// never mind the color, turn them all off
		PORTB |= LED_RED | LED_GRN | LED_BLU;
		}
	else if (ctrl == ON) { // don't know if any are on, do turn all off first
		PORTB |= LED_RED | LED_GRN | LED_BLU;
		PORTB &= ~color;	// then turn on the desired one
		}
	
}

//-------------------------------------------------------------------------------------
void spkr_ctrl (int freq) {
	// sets speaker frequency; 0 is off
	if (freq > 10) freq = 10;		// range limit
	if (freq == 0) {
		TCCR0B = 0;					// turn off clock
		}
	else {
		TCCR0A = 0x42;				// toggle OC0A on match
		TCCR0B = 0x00;				// no prescale
		OCR0A = freqSel[freq];		// set the frequency
		TCCR0B = 0x81;				// strobe in the OCR0B value
		TCCR0B = 0x01;				// reset the strobe
	}
	_delay_ms(250);	// spin a while,, then go take another reading
}
