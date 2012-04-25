//************************************************************************
// aprsAVR program.			ATmega324P
//
// Original idea/structure came from http://garydion.com/projects/whereavr/
// HEAVILY redesigned software.
//
//			(C) 2007 George Scolaro
//************************************************************************


#include <avr/io.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "main.h"
#include "avrio.h"
#include "ax25.h"
#include "nmea.h"

// local function prototypes

void hardware_init(void);
void read_eeprom(void);
void delay_100ms(void);
void bin2str(char *buff, char data);

//#define	DEBUGMODE	1		// if uncommented, runs debug code

uint16_t	gps_speed = 0;		// gps speed in knots
uint16_t	gps_course = 0;		// gps course in degrees true north

// These variables are stored in the EEPROM

uint8_t	e_beacon_interval	__attribute__((section(".eeprom"))) = 0;
uint8_t	e_sym_table			__attribute__((section(".eeprom"))) = 0;
uint8_t	e_sym_id			__attribute__((section(".eeprom"))) = 0;
uint8_t	e_comment_txt[40]	__attribute__((section(".eeprom")));
uint8_t	e_delta_beacon		__attribute__((section(".eeprom"))) = 0;

uint8_t	e_tx_delay			__attribute__((section(".eeprom"))) = 40;
uint16_t e_slow_beacon_rate	__attribute__((section(".eeprom"))) = 60*30;
uint8_t e_fast_beacon_rate	__attribute__((section(".eeprom"))) = 180;
uint8_t e_low_speed			__attribute__((section(".eeprom"))) = 5;
uint8_t e_high_speed		__attribute__((section(".eeprom"))) = 60;
uint8_t	e_min_turn_angle	__attribute__((section(".eeprom"))) = 20;
uint8_t	e_turn_slope		__attribute__((section(".eeprom"))) = 255;
uint8_t e_min_turn_time		__attribute__((section(".eeprom"))) = 10;

//**************************************************************
// Main entry point of program, never exits.


int main(void)
{
	uint8_t status, sendready = 0, otic = 0, tic = 0;
	uint8_t v_vbatr = 0, vbat_cnt = 0, line1_tog = 0;
	uint16_t timsec = 0, res, vbat = 0, vbat_dis = 0, vbatt, units, tenths, hunds;
	uint16_t beacon_rate = 0, turn_threshold, heading_change_since_beacon;
	uint16_t secs_since_beacon = 9000, packets_sent = 0;
	uint16_t previous_gps_course = 0, peak_speed = 0;
	uint8_t corner_peg = 0;
	char batst[9] = "V = 0.00";
	char packst[11] = "Pkt=      ";
	char ratest[11] = "Tm=      ";
	char spst[11] = "mph=      ";

	hardware_init();				// set up hardware registers

	read_eeprom();					// read configuration data

	i_wrptr0 = i_rdptr0 = 0;		// init usart input rd/wr pointers
	o_wrptr0 = o_rdptr0 = 0;		// init usart output rd/wr pointers

	gps_eol = 0;					// allow nmea packet to come through
	pc_eol = 0;						// allow PC data to come through
	time2send = 0;					// no valid packet to send yet

	sei();							// interrupts now enabled

	delay_100ms();					// wait 1/10 second for things to settle

	init_lcd(0);					// init the LCD with cursor off

	lcd("TRKavr", 0, 0);			// signon message

	tx_packet_done = 1;				// pretend packet is done, to start things

	// *** This is the mainline, runs until power is removed ***

	OCR0A = BIT_DELAY;				// 1200Baud (1200Hz = bit delay)

	adc_init();						// start up adc to measure battery voltage

	while (1) {

//		wdt_reset();				// keep the dog happy

		// See if a new GPS sentence is available. If so check to see if it
		// is one that we want and if so parse the sentence & build mic-e
		// fields ready for tranmission.

		status = gps_grab_nmea();	// try grab & store latest nmea packet

		if (tic != otic && tx_packet_done) {		// one per second
			if (status == 1)
				lcd("GPS BAD", 0, 0);		// invalid data
			else {
				if (!v_vbatr)			// ADC not ready
					lcd("GPS OK", 0, 0);
				else {					// ADC reading valid, convert to voltage
					vbatt = ((unsigned long)vbat_dis * 500L + 512L)/1024L;

					units = vbatt / 100;
					tenths = (vbatt - units * 100) / 10;
					hunds = (vbatt - units * 100 - tenths * 10);
					batst[4] = units + '0';
					batst[6] = tenths + '0';
					batst[7] = hunds + '0';
					lcd(batst, 0, 0);		// display x.yz voltage on LCD
				}
			}
			if (line1_tog && (PIND & 0x30) == 0x10)		// charging
				lcd("Charge", 1, 0);
			else if (line1_tog && (PIND & 0x30) == 0x20)	// charged
				lcd("Charged", 1, 0);
			else {
				itoa(packets_sent, &packst[4], 10);
				lcd(packst, 1, 0);
			}
			line1_tog ^= 1;			// toggle flag
			itoa(beacon_rate, &ratest[3], 10);
			lcd(ratest, 1, 8);
		}

		if (new_position) {			// new, valid GPS fix obtained

			if (switcha == 1 && !status) {
				sendready = 1;			// force immediate packet send
				switcha = 2;
			}

			if (gps_speed > peak_speed)
				peak_speed = gps_speed;		// store new peak speed for this session

			itoa(gps_speed, &spst[5], 10);
			lcd(spst, 0, 9);

			heading_change_since_beacon = abs(gps_course - previous_gps_course);
			if (heading_change_since_beacon > 180)	// largest direction change is 180 degrees!
				heading_change_since_beacon = 360 - heading_change_since_beacon;

			if (gps_speed < low_speed && peak_speed > low_speed) {	// if going very slow, force the rate
				beacon_rate = slow_beacon_rate;
				corner_peg = 0;
			}
			else if (gps_speed < low_speed && peak_speed <= low_speed) {	// must be walking

				beacon_rate = slow_beacon_rate;

				if (gps_speed > 1) {
					beacon_rate = 60;
					if ((heading_change_since_beacon > 30) &&
						(secs_since_beacon > 5))
						corner_peg = 1;
					else
						corner_peg = 0;
				}
			}
			else {			// Adjust beacon rate according to speed

				// Corner pegging - ALWAYS occurs if not "stopped"
	   			// Note turn threshold is speed-dependent

				turn_threshold = min_turn_angle + turn_slope / gps_speed;

				if ((heading_change_since_beacon > turn_threshold) &&
						(secs_since_beacon > min_turn_time))
					corner_peg = 1;
				else
					corner_peg = 0;

				if (gps_speed > high_speed)	// if going very fast, force the rate
					beacon_rate = fast_beacon_rate;

				else
					beacon_rate = fast_beacon_rate * high_speed / gps_speed;
			}

			if (secs_since_beacon > beacon_rate || corner_peg) {	// send GPS update
				sendready = 1;
				corner_peg = 0;		// clear it in case it was set
			}
			new_position = 0;		// new position processed
		}

		if (sendready && tx_packet_done) {	// new position computed
			lcd("TX Fix", 1, 0);	// Sending fix
			ax25_mice_packet();		// create bitstream with stuffing etc
			enable_transmit();		// enable transmitter function
			PORTC |= _BV(LED0);		// turn off LED when sent
			secs_since_beacon = 0;	// just sent position beacon
			tic = 0;
			packets_sent++;			// another packet sent
			previous_gps_course = gps_course;	// save course we last sent
			sendready = 0;
		}

		if (tx_mode && tx_packet_done) {	// transmitter was running
			gps_eol = 0;				// re-arm uart to get a sentence
			enable_receive();			// Disable transmitter
		}

		otic = tic;

		if (ticker > 7) {
			if (timsec && (timsec % 150) == 0) {	// another second
				tic++;
				if (tic > 59)
					tic = 0;			// wrap 1 minute
				if (!sendready)
					secs_since_beacon++;
			}
			if (timsec % 15) {			// 10 times per second
				res = adc_read();		// read battery voltage 10 times per second
				if (res != 0) {
					vbat += res;		// add up 10 readings
					vbat_cnt++;			// got another one
					if (vbat_cnt > 9) {
						v_vbatr = 1;	// valid vbat reading
						vbat_dis = vbat / 10;	// get average
						vbat_cnt = 0;
						vbat = 0;
					}
				}
			}
			if (timsec < 1500)
				timsec++;				// bump ticker, but wrap at 10 seconds
			else
				timsec = 0;

			ticker = 0;
		}
	}
	return(1);		// makes compiler happy
}

//*******************************************************************
// This function programs all the hardware registers to their
// working baseline values.

void hardware_init(void)
{

//	ADCInit();
	usart_init();			// Setup baud rate, data size, interrupts etc

	PORTB = 0x10;			// SIRF GPS is enabled
	DDRB = 0x17;			// PB4 = SIRF_ENABLE
							// PB2 = LCD_WR, PB1 = LCD_EN, PB0 = LCD_RS

	// PORT A - 4 bit D/A:
	//	Bit0 1k ohm resistor
	//	Bit1 2k ohm resistor
	//	Bit2 4k ohm resistor
	//	Bit3 8k ohm resistor
	//	Bit4 = LCD D0
	//	Bit5 = LCD D1
	//	Bit6 = LCD D2
	//	Bit7 = A/D for Vbat monitoring


	PORTA = 0x0;
	DDRA = 0x7f;			// 4 lsb = 4 bit D/A

	PORTC = 0x1;			// PC0 = LED (off), PC1 = LCD power (on) all others low
	DDRC = 0xc3;			// 2 msb and 2 lsbs output, rest input, PC7 = PTT, PC6 = LCD D3

	PORTD = 0xf0;			// bits low (PD0, PD1, PD2 = uart/serial), pullup on PD6 and PD7 (SWA/SWB)
	DDRD  = 0x0;			// and pullups on PD4 and PD5 (STAT2/STAT1), all bits input

	// Counter0 (8 bit), Timer1 (16 bits), Counter 2 (8 bits)
	// With prescaler set to 8, clock = 1.8432MHz

	TCCR0A = 0x2;			// 8 bit Counter0 CTC
	TCCR0B = 0x3; 			// Counter0 clock prescaler = 64
	OCR0A = BIT_DELAY;		// 1200Baud (1200Hz = bit delay)

	TCCR1A = 0x0;
	TCCR1B = 0x2;			// Timer1 clock prescale 8

	TCCR2A = 0x2;
	TCCR2B = 0x2; 			// Counter2 clock prescaler = 8
	OCR2A = 0xff;			// slowest interrupt rate, used as a timer

	TIMSK0 = _BV(OCIE0A);	// Enable Counter0 interrupt
	TIMSK2 = _BV(OCIE2A);	// Enable Counter2 interrupt


//	WDTCR	= _BV(WDTOE) | _BV(WDE);	// Wake-up the watchdog register
//	WDTCR	= _BV(WDE) | 7;				// Enable and timeout around 2.1s

	// Setup analog comparator (input from radio speaker)

	ACSR = _BV(ACBG) | _BV(ACIE);	// Select Bandgap for pos. input
}


//*******************************************************************
// Do all the setup to enable the AFSK transmitter
// Re-programs Counter0/2 to be able to generate 1200Hz/2200Hz tones
// Counter0 interrupts at the bit rate (1200Hz)
// Counter2 interrupts at 16 times 1200Hz or 2200Hz for sinewave generation.

extern void enable_transmit(void)
{
	ACSR &= ~_BV(ACIE);			// Disable the comparator
	TCCR0B = 0x3; 				// Counter0 clock prescaler = 64
	PORTC |= _BV(PTT);			// PTT = ON

	tx_ptr = 0;					// set read pointer (tx buffer) to zero
	tx_bit_cnt = 0;				// clear bit pointer
	tx_packet_done = 0;			// packet not done yet
	tx_delay_cnt = tx_delay;	// pre-preamble length
	tx_tone = SPACE;			// seed the starting tone

	tx_mode = 1;				// Transmitter enable flag = 1
}


//*******************************************************************
// Do all the setup to receive or wait.
// Disables AFSK transmitter, turns off PTT
// Reprograms Timer1/2

extern void enable_receive(void)
{
	tx_mode = 0;				// Transmitter enable flag = 0
	PORTC &= ~_BV(PTT);			// PTT = OFF
	PORTA &= 0xf0;				// D/A = low
//	TCCR0B = 0x2; 				// Counter0 clock prescale 8
	ACSR |= (1<<ACIE);			// enable the comparator
}

void bin2str(char *buff, char data)
{
	uint8_t i = 0;

	while (data - 10 >= 0) {
		data -= 10;
		i++;
	}
	*buff++ = i + '0';
	*buff = data + '0';
}


//*******************************************************************
// Wait a 100ms, using interrupts

void delay_100ms(void)
{
	delay_ticker = 0;
	while (delay_ticker < 120)
		;
}

//*******************************************************************
// Read configuration data from EEPROM into working variables

void read_eeprom()
{
	tx_delay = eeprom_rd((int)&e_tx_delay);

	slow_beacon_rate = eeprom_rd((int)&e_slow_beacon_rate) |
							(eeprom_rd((int)&e_slow_beacon_rate+1) << 8);
	fast_beacon_rate = eeprom_rd((int)&e_fast_beacon_rate);

	low_speed = eeprom_rd((int)&e_low_speed);
	high_speed = eeprom_rd((int)&e_high_speed);

	min_turn_time = eeprom_rd((int)&e_min_turn_time);
	min_turn_angle = eeprom_rd((int)&e_min_turn_angle);
	turn_slope = eeprom_rd((int)&e_turn_slope);
}

//*******************************************************************
// EEPROM read routine (returns byte)

extern uint8_t eeprom_rd(uint16_t address)
{ 
	while (EECR & _BV(EEPE))	// wait for previous write to complete
 	   ;
	EEAR = address;				// write 16 bits address (C does 2 outs)
	EECR = _BV(EERE);			// read the eeprom
	return (EEDR);
}

//*******************************************************************
// EEPROM write routine (writes one byte)

extern void eeprom_wr(uint16_t address, uint8_t data)
{
 	while (EECR & _BV(EEPE))	// wait for previous write to complete
 	   ;
	cli();		   				// disable interrupts
	EEAR = address;				// write 16 bits address (C does 2 outs)
	EEDR = data;
	EECR |= _BV(EEMPE);			// these 2 instructions are consecutive
	EECR |= _BV(EEPE);			// Start eeprom write by setting EEWE
	sei();						// enable interrupts
}


