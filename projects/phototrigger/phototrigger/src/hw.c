/*
 * hw.c
 *
 *  Created on: 2009-08-28
 *      Author: liku
 *
 *      This file contains all routines needed to setup all the peripherals, ie.
 *      ports, timers, ADC, switches.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/eeprom.h>
#include <avr/fuse.h>

#include <util/atomic.h>
#include <util/delay.h>

#include "hw.h"
#include "lcd.h"
#include "eeprom.h"
#include "rtc.h"
#include "hd44780.h"
#include "dlg.h"

/*
 * Fuse configuration (not supported by current avrdude)
 */
/*FUSES =
{
	.low = (FUSE_CKSEL0 | FUSE_CKSEL2 | FUSE_CKSEL3 | FUSE_SUT0 | FUSE_CKDIV8),
#if DEBUGWIRE
	.high = (FUSE_DWEN),
#else
	.high = (FUSE_SPIEN),
#endif
	.extended = EFUSE_DEFAULT,
};*/


static bool poweredon = false; // flag to indicate if device is powered on

/*
 * Setup all ports that are used in our design
 */
void hwSetupPorts(void)
{
	/* setup peripherals according to schematics */
	/* 0 - input, 1 - output */

	/* shutter release and other */
	DDRB  = _BV(PB1) | _BV(PB0); // unused pins as inputs, rest as outputs (p. 74)
	PORTB = _BV(PB5) | _BV(PB4) | _BV(PB3) | _BV(PB2); // pull-ups for unused pins (p. 74)

	/* ADC + encoder */
	DDRC  = _BV(LCDVCC); // one output - LCDVCC
	PORTC = _BV(ENCSW) | _BV(ENC0) | _BV(ENC1) | _BV(PC0); // pull-ups (also unused pins)
	DIDR0 = _BV(ADC5D); // disable digital function on pin 0

	/* LCD */
	DDRD  = 0xff; // all outputs (LCD)
	PORTD = 0;    // set all to 0

	/* Nop added for synchronisation */
	asm volatile("nop");
}

/*
 * Setups analog to digital converter
 */
void hwSetupADC(void)
{
	/*
	 * REFS1:0 = 3 => 1.1 V internal voltage reference
	 * ADLAR = 0   => right adjusted output
	 * MUX3:0 = 5  => ADC5 as input
	 */
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX2) | _BV(MUX0);

	/*
	 * ADEN = 1    => enable ADC
	 * ADSC = 0    => do not start conversion
	 * ADATE = 0   => no triggering
	 * ADIF = 1    => clear interrupt flag
	 * ADIE = 0    => no interrupts
	 * ADPS2:0 = 3 => prescaling /8 (ADC clk = 125 kHz)
	 */
	ADCSRA = _BV(ADEN) | _BV(ADIF) | _BV(ADPS1) | _BV(ADPS0);

	/*
	 * ACME = 0    => Analog Comparator Multiplexer disable
	 * ADTS2:0 = 0 => free running mode
	 */
	ADCSRB = 0;
}

/*
 * Setup timer 0 - used for LCD contrast & encoder
 */
void hwSetupT0(void)
{
	/*
	 * COM0A1 = 1, COM0A0 = 1 => Set OCB0 when up counting (Table 14-7)
	 * COM0B1 = 0, COM0B0 = 0 => Normal port operation (Table 14-4)
	 * WGM01  = 1, WGM00  = 1 => Fast PWM mode (Table 11-8)
	 */
	TCCR0A = _BV(COM0A1) | _BV(COM0A0) | _BV(WGM01) | _BV(WGM00);

	/*
	 * FOC0A = 0 => compatibility with future devices
	 * FOC0B = 0 => compatibility with future devices
	 * WGM02 = 0 => Fast PWM mode (Table 11-8)
	 * CS02 = 0, CS01 = 0, CS00 = 1 => no prescaling (=~ 4 kHz),
	*/
	TCCR0B = _BV(CS00);

	// no interrupts (for now)
	TIMSK0 = 0;

	// disable driving of contrast
	OCR0A = 255;
}

/*
 * Setups timer 1 used for LCD backlight (this one has higher resolution)
 */
void hwSetupT1(void)
{
	/*
	 * COM1A1 = 1, COM1A0 = 0 => Clear OCA1 when up counting (Table 15-3)
	 * COM1B1 = 0, COM1B0 = 0 => Normal port operation (Table 15-3)
	 * WGM11  = 1, WGM10  = 1 => Phase correct PWM mode 10 bit (Table 15-4)
	 */
	TCCR1A = _BV(COM1A1) | _BV(WGM11) | _BV(WGM10);

	/*
	 * ICNC1 = 0            => no need to use noise canceller
	 * ICES1 = 0            => not used (input capture edge)
	 * WGM13 = 0, WGM12 = 0 => Phase correct PWM mode 10 bit (Table 15-4)
	 * CS12:0 = 1           => prescale /8 (=~ 122 Hz)
	 *
	*/
	TCCR1B = _BV(CS11);

	// no need to setup TCCR1C

	// no interrupts (at the moment, overflow used for backlight dimming)
	TIMSK1 = 0;

	// this operation must be atomic
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		OCR1A = 0;
	}
}

/*
 * Setups timer 2 used for RTC interrupts
 */
void hwSetupT2(void)
{
	// special procedure for switching to asynchronous mode (for details see p. 150)
	TIMSK2 = 0;      // a. disable interrupts
	ASSR = _BV(AS2); // b. select clock source (external crystal)

	// c. write new values to registers
	/*
	 * COM2A1:0 = 0 => normal pin mode for OC2A
	 * COM2B1:0 = 0 => normal pin mode for OC2B (not a pin?)
	 * WGM22:0 = 2  => normal mode
	 */
	TCCR2A = 0;

	/*
	 * FOC2A = 0, FOC2B = 0 => ensure compatibility mode with future devices
	 * WGM22:0 = 2          => CTC mode
	 * CS22:0 = 6           => divide clock by /128 (overflow = 1 second)
	 */
	TCCR2B = _BV(CS22) | _BV(CS20);

	// d. wait for all writes to complete
	while(ASSR & (_BV(TCN2UB) |
			_BV(OCR2AUB) |
			_BV(OCR2BUB) |
			_BV(TCR2AUB) |
			_BV(TCR2BUB)));

	// e. clear interrupt flags
	TIFR2 = 0;

	// f. enable interrupts
	TIMSK2 = _BV(TOIE2); // interrupt on overflow
}

/*
 * Disables unneeded peripherals and sets power save mode
 */
void hwSetupPwr(void)
{
#ifndef DEBUGWIRE
	power_spi_disable(); // needed while debugging
#endif
	power_twi_disable();
	power_usart0_disable();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
}

/*
 * Setups all the hardware and turn on interrupts
 */
void hwSetupAll(void)
{
	hwSetupPwr();
	hwSetupPorts();
	hwSetupADC();
	hwSetupT0(); // LCD contrast
	hwSetupT1(); // LCD backlight
	hwSetupT2(); // RTC

	sei();
}

/*
 * Enter power off state (user treats this as the device was off)
 */
void hwEnterPwrSave(void)
{
	// LCD
	lcd_setbacklight(0);  // backlight off
	lcd_setcontrast(255); // contrast
	hwLCDOff();           // LCD module off
	// wait for backlight to go completely off (it may cause problems in power save state)
	_delay_ms((1023*8)/LCDBLSTEP); // worst case: 1023+50 /safety/ - no of bl steps, 8 - period in ms for T1
	OCR1A = 0;            // ensure BL is off (if may cause BL to be on all the time when T1 is turned off)
	_delay_ms(10);        // wait to ensure it will be off

	// Peripherals
	hwADCOff();           // ADC off
	hwShutOff();          // shutter off (just to be sure)
	hwEncOff();           // disable encoder (timer interrupt off, will avoid unnecessary keys when power on)
	power_timer0_disable();
	power_timer1_disable();
	power_adc_disable();

	// set the flag to indicate power off state (= not power on)
	poweredon = false;

	// power off loop
	while(1)
	{
		sleep_mode();         // ZzZz... only wake up by RTC timer (sleep mode already set in hw.c)
		if(rtcGetPwrOnFlag()) // power on?
			break;
	}

	// Peripherals
	power_adc_enable();
	power_timer1_enable();
	hwADCOn();
	power_timer0_enable();

	// LCD
	lcd_setbacklight(eeprom_read_word(&eebacklight));
	hwLCDOn();
	lcd_init();
	lcd_setcontrast(eeprom_read_byte(&eecontr));

	dlgInfo_P(PSTR("phototrigger")); // welcoming message

	// check battery
	if(hwGetBattVol() < HWBATTVOL(HWLOWBATTVAL))
		dlgInfo_P(PSTR("Low battery"));

	hwEncOn();        // must be as last so it doesn't trigger RTC activity mechanism
	poweredon = true; // set the flag that we have the power
}

/*
 * Turns off LCD and forces all to outputs and 0 state
 * This must be done during poweroff state
 */
void hwLCDOff(void)
{
	HD44780_PORTOUT = 0;        // force all to 0 (tristate inputs)
	HD44780_DDR = 0xff;         // all outputs
	LCDVCCPORT &= ~_BV(LCDVCC); // power off LCD
	asm volatile("nop");        // sync
}

/*
 * Disable encoder so it doesn't use additional uA
 */
void hwEncOn(void)
{
	ENCDDR  &= ~(_BV(ENC0) | _BV(ENC1)); // tristate
	ENCPORT |= _BV(ENC0) | _BV(ENC1);   // pullups
	TIMSK0  |= _BV(TOIE0);               // disable interrupt
}

/*
 * Enable encoder
 */
void hwEncOff(void)
{
	TIMSK0  &= ~(_BV(TOIE0));             // interrupt off
	ENCDDR  |= _BV(ENC0) | _BV(ENC1);    // tristate
	ENCPORT &= ~(_BV(ENC0) | _BV(ENC1)); // force 0 state
	asm volatile("nop");                 // sync
}

/*
 * Returns true is device is in power-on state,
 * false if is powered off.
 */
inline bool hwIsPwrOn(void)
{
	return poweredon;
}

/*
 * Returns ADC measurement
 */
inline uint16_t hwGetADC(void)
{
	ADCSRA |= _BV(ADSC);
	while(ADCSRA & _BV(ADSC));
	return ADCL | ADCH<<8;
}

/*
 * Return battery measurement
 */
uint16_t hwGetBattVol(void)
{
#define HWNOOFBATTVOLAVG 8

	uint16_t vol = 0;

	for(uint8_t i=0; i<HWNOOFBATTVOLAVG; i++)
		vol += hwGetADC();

	return vol/HWNOOFBATTVOLAVG;
}

/*
 * Returns in: higher byte - decimal, lower byte - fractional point
 * Example: 0xff03 => 255.03, 0x2020 => 32.32
 * Useful when computing ADC results
 * return value = (in*coeff1)/coeff2
  */
uint16_t hwMulCoeff(uint16_t in, uint16_t coeff1, uint16_t coeff2)
{
	uint16_t dec, tmp;
	uint8_t frac, dig1, dig2;

	// calculate decimal part (XX.00)
	in *= coeff1;
	dec = in / coeff2;

	// calculate fractional part (00.XX)
	tmp = (in - (dec*coeff2)) * 10;
	dig1 = tmp / coeff2;
	dig2 = ((tmp - (dig1*coeff2)) * 10) / coeff2;
	frac = dig1 * 10 + dig2;

	return dec<<8 | frac;
}
