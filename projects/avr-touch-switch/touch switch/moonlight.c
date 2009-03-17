/*** Moonlight Mobile *********************************************************
 *
 *	File Name  : moonlight.c
 *	Title      : MOONLIGHT Mobile
 *  Description: MOONLIGHT remote control firmware
 *	Author     : Muhammad J. A. Galadima
 *	Created    : 2008 / 03 / 18
 *	Modified   : 2008 / 04 / 03	-- v0.9
 *	Modified   : 2008 / 04 / 05	-- v0.99	080405002
 *	Modified   : 2008 / 05 / 24	-- v1.00	080524001
 *
 *	Version    : 1.01
 *	Target MCU : tiny13
 *	
 *	License: GPL v3
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <util/delay.h>
#include "serial.h"		// just used for debug in this app


FUSES = {
	.low = 0x6A,					// value taken from avrstudio
	.high = 0xFF,					// value taken from avrstudio
//	.extended = EFUSE_DEFAULT,		// not applicable on tiny13
};


// MG lighting commands (infra-red)
#define IRCMD_PWR_ON		0X01
#define IRCMD_AWAKE_30		0X21	// upper 3 bits used to distinguish wake-up modes
#define IRCMD_AWAKE_60		0X41
#define IRCMD_AWAKE_120		0X61
#define IRCMD_AWAKE_180		0X81
#define IRCMD_AWAKE_06h		0XC1
#define IRCMD_AWAKE_12h		0XE1
#define IRCMD_PWR_OFF		0X02
#define IRCMD_SLEEP_30		0X22	// upper 3 bits used to distinguish sleep modes
#define IRCMD_SLEEP_60		0X42
#define IRCMD_SLEEP_120		0X62
#define IRCMD_SLEEP_180		0X82
#define IRCMD_SLEEP_06h		0XC2
#define IRCMD_SLEEP_12h		0XE2
#define IRCMD_SLEEP			0X03	// tell target to switch to next sleep state (if on) or awake state (if off)
#define IRCMD_PWR			0X04	// issue cmd to toggle power status
#define IRCMD_DIM			0X05	// issue cmd w/ subsequent repeats to slowly dim lamp till it reaches a limit; issue cmd again to change dir
#define IRCMD_DIM_UP		0X06
#define IRCMD_DIM_DOWN		0X07

#define IRCMD_REPEAT		0XE0	// tell target to perform last received command again
#define IRCMD_EXTEND		0X1F	// data in next whole byte(s), number denoted by content of upper 3 bits

// 27.3 is overflow period in ms of timer0
#define TIMER_TAP_WND	300/27.3	// 10 time before tap-group window closes
#define TIMER_HOLD_TRIG	600/27.3	// 21 time threshhold to distinuish between tap and hold
#define TIMER_HOLD_REP	150/27.3	// 5 between re-triggers during a hold
#define TIMER_IDLE		3000/27.3	// 5 between re-triggers during a hold
#define TIMER_STOP		0xFF		// TOP of timer, used to signify stop condition in timer interrupt (i.e. don't decrement counter)

#define TS0		3
#define TS1		4
#define IR		1

#define IRADDR		0x01			// modify this (lower nibble only)
#define IRADDR1		(0xA0 | (IRADDR&0x0F))	// put this in include file

#define TS_BACKLOG	4
#define TS_BACKLOG1	(TS_BACKLOG-1)
#define MAF_LOOPS	16
#define THRESH		(MAF_LOOPS*2/3)	// low value for prox / medium for light touch / high for firm touch

#define wdt_reset()		__asm__ __volatile__ ("wdr")

// modes defined using ts_stat (0001,0010,0100,1000,1111)
#define TS_STAT_CALIBRATE	0xF0
#define TS_STAT_0			0		// bitmask, switch 0
#define TS_STAT_1			1		// bitmask, switch 1
#define TS_STAT_IDLE		4		// bitmask, idle mode

uint8_t ts_stat=TS_STAT_CALIBRATE;	// touch switch status, 0 or 1 to indicate TS; between init value & 255 -> used to calibrate system
uint8_t ts0_taps, ts0_hold, ir_stat;	// number of taps in curr tap group, time key held (in hold mode), ir status
uint8_t ts0_timer=TIMER_STOP;			// timer top value, or stop


/*** ir_out *******************************************************************
 *	send raw data to receiver
 *****************************************************************************/
void ir_out(uint8_t tx_buf) {
	uint8_t i=8;
	while(i) {
		i--;
		
		// delay 216us (8 clocks @ 36.7kHz) for 0, twice that for 1
		_delay_us(216);
		if((tx_buf&128)!=0)	// msb first
			_delay_us(216);
		tx_buf <<= 1;
		
		uint8_t j;
		for(j=0; j<8; j++) {	// 8 clocks at 36.7kHz (1,000,000/28 ~= 35714Hz, /27 ~= 37037)
			_delay_us(7);
			PORTB |= _BV(IR);
			_delay_us(20);
			PORTB &= ~(_BV(IR));
		}
	}
}
// void ir_out(uint8_t)


/*** ir_tx ********************************************************************
 *	send framed data to receiver
 *****************************************************************************/
void ir_tx(uint8_t data) {
	cli();
	ir_out(IRADDR1);					// start bits (1010), address (0001, lighting)
	ir_out(data);						// send data
	sei();
}
// void ir_tx(uint8_t)


/*** TIM0_OVF_vect ************************************************************
 *	Interrupt on timer 0 overflow; occurs every 27.3ms (1024*256/9600000)
 *****************************************************************************/
SIGNAL (TIM0_OVF_vect) {
	wdt_reset();
	
	if(ts0_timer==TIMER_STOP) {
		if(!(ts_stat & _BV(TS_STAT_IDLE))) {	// if not in idle mode, set idle mode, start idle counter
			ts_stat |= _BV(TS_STAT_IDLE);				
			ts0_timer = TIMER_IDLE;
		}
		// else do nothing, wait for main loop to call sleep
	}
	else if(ts0_timer == 0) {	// occurs TIMER_TAP_WND*27.3 ms after last tap
		ts0_timer = TIMER_STOP;
		
		if(ts_stat & _BV(TS_STAT_IDLE)) {
			// do nothing; will go to TIMER_STOP (above) & wait for main loop to call sleep
		}
		else if(!(ts_stat & _BV(TS_STAT_0))) {	// key not pressed; window for ts0_taps expired (ts0_timer=0)
//			ss_putc('1');
//			ss_putr();
			switch(ts0_taps) {
			  case 1:	// toggle power
				ir_tx(IRCMD_PWR);
				break;
			  case 2:	// next sleep/awake state
				ir_tx(IRCMD_PWR_ON);
				break;
			  case 3:	// next sleep/awake state
				ir_tx(IRCMD_PWR_OFF);
//				break;
			}
			ts0_taps=0;
		}
		else {			// key pressed; indicates key has been held; can be used to repeat prior cmd and/or multiplex cmds on same key
//			ss_putc('0');
//			ss_putr();
			switch(ts0_taps) {	// ts0_taps before key was held (if any)
			  case 0:	// toggle power
				if(ts0_hold == 0)				// 1st cycle; send command? ignore?...
					ir_tx(IRCMD_DIM);
				else if(ts0_hold > 1)			// delay a cycle (or more) before repeating
					ir_tx(IRCMD_REPEAT);
					
				break;
			  case 1:	// next sleep/awake state
				if(ts0_hold == 0)				// 1st cycle; send command? ignore?...
					ir_tx(IRCMD_SLEEP);
				else if(ts0_hold > 1)			// delay a cycle (or more) before repeating
					ir_tx(IRCMD_REPEAT);
//				break;
			}
			
			if(ts0_hold != 0xFF)
				ts0_hold++;
			
			ts0_timer = TIMER_HOLD_REP;			// reset re-trigger period
		}
	}
	else //if(ts0_timer != TIMER_STOP)
		ts0_timer--;
}
// SIGNAL (TIM0_OVF_vect)


/*** TIM0_COMPA_vect **********************************************************
 *	Interrupt on output compare 0A match
 *****************************************************************************/
/*
SIGNAL (TIM0_COMPA_vect) {
	
}
*/
// SIGNAL (TIM0_COMPA_vect)


/*** WDT_vect *****************************************************************
 *	Watch dog timer interrupt (called on WDT interrupt or after WDT reset)
 *****************************************************************************/
SIGNAL (WDT_vect) {
	//	nothing for now...
}
// SIGNAL (WDT_vect)


/*** init *********************************************************************
 *	init io ports, interrupts, registers, etc
 *****************************************************************************/
void init(void) {
	// init ports
	DDRB = 0xFF;	// all out, except PB0 which can be serial rx (half duplex)
	PORTB = 0x01;	// all low, pull-up/high (idle state) on serial port
	
	PORTB |= _BV(TS1);
	DDRB &= ~(_BV(TS1));	// set ts1 (pb4) to input, pull-up on
	
	ACSR = _BV(7);						// write 1 to ACD is ACSR to disable analog comparator
	//set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	MCUCR |= _BV(SM1);					// set sleep mode to power-down
	
	// start WDT	set at a few seconds, in case remote crashes
	WDTCR |= _BV(WDCE) | _BV(WDE);				// timed sequence
	WDTCR = _BV(WDE) | _BV(WDTIE) | _BV(WDP1) | _BV(WDP0);
		// enable WDT reset+int mode, timeout 0.125s (anything above 27.3 will do; longer delay should save more power)
	
	TIMSK0 |= _BV(TOIE0);						// enable overflow int to clock ts_timer
	TCCR0B |= _BV(CS02);						// clock timer at /256
	
//	CLKPR = _BV(CLKPCE);
//	CLKPR = _BV(CLKPS1);						// set clock to 2.4MHz
	
	sei();
}
// void init_io(void)


int main(void) {
	init();

	uint16_t i, ts0, ts0b, ts0bc;				//16 bit to allow mult / div
	uint8_t j;
	
//	ss_puts(PSTR("\n\rMoonlight Interface, by Muhammad J. A. Galadima.\n\r\n\r"));
//	ss_putc('M');
//	ss_putc('G');
//	ss_putr();
/*	CLKPR = _BV(CLKPCE);
	CLKPR = _BV(CLKPS1);						// set clock to 2.4MHz
	for(;;) {
		_delay_ms(900);
		ir_tx(IRCMD_SLEEP);
		_delay_ms(300);
		ir_tx(IRCMD_REPEAT);
		_delay_ms(300);
		ir_tx(IRCMD_REPEAT);
		_delay_ms(300);
		ir_tx(IRCMD_REPEAT);
		_delay_ms(300);
		ir_tx(IRCMD_REPEAT);
		_delay_ms(300);
		ir_tx(IRCMD_REPEAT);
		//ir_tx(IRCMD_PWR);
	}
*/	
	// loop forever
    for (;;) {
		cli();									// so as not to screw up the count...
		for(j=MAF_LOOPS,i=0; j>0; j--) {
			//_delay_ms(.1);						// wait for plate to completely discharge; PINB
			_delay_ms(1);						// wait for plate to completely discharge; PINB
												//	is already outut/lo from init (above)
			TCCR0B |= _BV(CS00);					// clock timer at /1024 (1/4 previous speed)
			CLKPR = _BV(CLKPCE); 					//
			CLKPR = 0;								// set clock to 9.6MHz (x4 previous speed)
			
			DDRB &= ~(_BV(TS0));	// clear TS0 on DDRB (set the pin to input)
										//	will charge through pull-up; takes longer to
										//	charge with finger present (higher capacitance)
			
			// find out how long it takes to charge
/*			while(bit_is_clear(PINB, TS0)) {
				PORTB |= _BV(TS0);
				PORTB &= ~(_BV(TS0));			// charge for 1 clock via pull-up
				i++;
			}
*/			
			while(bit_is_clear(PINB, TS0))
				i++;
			
			DDRB |= _BV(TS0);					// set TS0 on DDRB (output), port set to ground
												//	in init will discharge plate
			CLKPR = _BV(CLKPCE);					//
			CLKPR = _BV(CLKPS1);					// set clock to 2.4MHz (/4)
			TCCR0B &= ~(_BV(CS00));				// clock timer at /256 (4x)
		}
		sei();
		
		ts0 = ((ts0<<1) + ts0 + i) / 4;
		//ts0 = ((ts0*3) + i) / 4;
		
		
		// update baseline
		if(ts0b < ts0) {
			if(!(ts0bc&0x8000))
				ts0bc = 0x8000;
			else if(ts0bc==0x81FF) {
				ts0b++;
				ts0bc = 0x8000;
			}
			ts0bc++;
		}
		else if(ts0b > ts0) {
			if(ts0bc&0x8000)
				ts0bc = 0x0000;
			else if(ts0bc==0x01FF) {
				ts0b++;
				ts0bc = 0x0000;
			}
			ts0bc++;
		}
		else
			ts0bc &= 0xF0;
		
/*		ss_putn(ts0);
		ss_putc(',');
		ss_putn(ts0b);
		ss_putr();
*/		
		if(ts_stat>=TS_STAT_CALIBRATE) {					// calibration mode allow a few loops for values accross vX[] to stabilize (++ until it resets to 0)
			if(abs(i - ts0) < THRESH) {
				ts_stat++;
				ts0b = ts0;									// get baseline up to speed quickly
//				if(ts_stat==0xFF)
//					printf_P(PSTR("Calibrated!\n\r"));
			}
		}
		else if((ts0 > ts0b) && ((ts0-ts0b) > THRESH)) {	// && (calibrated==0)) {
			if(!(ts_stat & _BV(TS_STAT_0)) && (ts0bc>0x84)) {		// only run once while buffers are compensating for key press
				ts_stat |= _BV(TS_STAT_0);			// ts on
				ts_stat &= ~_BV(TS_STAT_IDLE);		// clear idle mode flag (if enabled)
				
				ss_putc('+');
//				ss_putr();
				//beep();
				
				ts0_hold = 0;
				TCNT0 = 0;							// reset for more consistent counts (negligible?)
				ts0_timer = TIMER_HOLD_TRIG;		// reset tap window every tap
			}
		}
		else {
			if(ts_stat & _BV(TS_STAT_0)) {			// only run once while buffers are compensating for key release 
				if(ts0_hold) {						// held long enough to trigger a hold, so this is the end of the hold (i.e. was not a tap)
					ts0_timer=TIMER_STOP;
					
					ts0_hold = 0;
					ts0_taps = 0;
				}
				else {								// tap; inc tap count
					ts0_timer = TIMER_TAP_WND;		// reset tap window every tap (group taps together)
					if(ts0_taps<0xFF)
						ts0_taps++;
				}
				ts_stat &= ~(_BV(TS_STAT_0)) & ~_BV(TS_STAT_IDLE);// clear ts0 & idle mode (if set) flags
				
				ss_putc('-');
			}
			else if(ts0b > ts0)						// speed up baseline drop
				ts0b = ts0;
		}
		
		if((ts_stat&_BV(TS_STAT_IDLE)) && (ts0_timer==TIMER_STOP)) {	// idle mode set in timer (go to timer_stop, set timer to idle delay, idle delay expires, go to timer_stop 2nd time & wait)
			WDTCR |= _BV(WDTIE);						// enable WDT interrupt before going to sleep, so it doesn't reset MCU on wakeup
			sleep_mode();
			ts_stat &= ~(_BV(TS_STAT_IDLE));
//			_delay_ms(10);
			//ir_tx(IRCMD_PWR);
		}
	}
}
//
