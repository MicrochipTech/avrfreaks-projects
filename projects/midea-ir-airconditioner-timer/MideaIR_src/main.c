/*
 * MideaIR
 *
 * Programmer:  Tom Blough
 * Revision:    2009/01/27
 * History:     2009/01/27 - Initial version
 *
 * This program emits the "ON" code from a Midea Air Conditioner remote control.
 * Here in China, there is no central air, so my office is heated/cooled by a
 * room air conditioner.  Because of this, it's cold when I arrive at work in
 * the winter, and hot in the summer.
 *
 * The idea was to create a small, battery powered, IR emitter that
 * will issue the "ON" command and hour or so before I get to work in the
 * morning.  The AC unit should start up with the last set point programmed.
 *
 * I originally envisioned this as having an elaborate real time clock and
 * entering the current time and a start-up time via a serial interface.  I
 * finally decided just to turn the unit on at 5:00pm before going home and
 * have it just count 14.5 hours to start up at 7:30am.  I'll have it re-issue
 * the start command every 10 minutes or so between 7:30 and my arrival at 8:30
 * just in case someone sees the AC running in an unoccupied office and turns
 * the AC unit off.
 *
 * BACKGROUND
 *
 * I hooked an IR diode up to my oscilloscope and transmitted the start code
 * from the AC remote to determine the pulse timing and carrier frequency.  I
 * then hooked up a IR receiver module of the correct carrier frequency to a
 * logic analyzer to determine the actual data transmitted by pressing the "ON"
 * button.
 *
 * The Midea IR signal uses pulse distance encoding.  Each pulse (1T) is 21
 * cycles of a 38KHz carrier burst (about 560µs long). A logic "1" is a 1T pulse
 * followed by a 3T space and takes 2.25ms to transmit.  A logic "0" is a 1T
 * pulse followed by a 1T space and takes 1.125ms.  The AGC burst consists of a
 * 4.5ms burst followed by 4.5ms space (8T pulse + 8T space).  The Midea remote
 * "ON" command consists of 3 bytes of information.  Each byte was first
 * transmitted normal and then inverted for a total of 48 bits of data.
 * This was followed by what appeared to be a single "0" stop bit followed by
 * 4.5ms space (8T).
 *
 * This entire command including the AGC pulse and stop pulse was then
 * immediately transmitted again.
 *
 * Example Midea remote control output:
 *                                                      _____                                            _____                                        _____
 *    +-----Start-----+---------Byte1---------+---------Byte1---------+----------Byte2----------+--------Byte2--------+-------Byte3-------+-----------Byte3-----------+---Stop---+
 *    |               | LSB               MSB | LSB               MSB | LSB                 MSB | LSB             MSB | LSB           MSB | LSB                   MSB |          |
 *    |               |                       |_  _  _ _  _   _  _  _ |                         | _   _   _  _ _ _ _ _|                   | _  _  _   _  _  _   _   _ |          |
 *    |  AGC    Space | 1  0  1   1  0 0  1  0|1  0  1 1  0   0  1  0 |0 0 0  1   1   1   1   1 | 0   0   0  1 1 1 1 1|0  1  0 0  1  0 0 0| 0  1  0   0  1  0   0   0 |0   Space |
 *    +------+--------+---+-+---+---+-+-+---+-+-+---+-+-+---+---+-+---+-+-+-+---+---+---+---+---+---+---+---+-+-+-+-+-+-+---+-+-+---+-+-+-+---+-+---+---+-+---+---+---+-+--------+
 *
 *    ########        #   # #   #   # # #   # # #   # # #   #   # #   # # # #   #   #   #   #   #   #   #   # # # # # # #   # # #   # # # #   # #   #   # #   #   #   #         
 * ___########________#___#_#___#___#_#_#___#_#_#___#_#_#___#___#_#___#_#_#_#___#___#___#___#___#___#___#___#_#_#_#_#_#_#___#_#_#___#_#_#_#___#_#___#___#_#___#___#___#_____________
 *
 * Note:  Byte1 appears to be a constant and does not change with any control inputs.
 *
 *
 * THEORY OF OPERATION
 *
 * Timing will be handled by using the watchdog interrupt.  The watchdog will
 * fire every 8 seconds (450 ticks per hour).  If we set up a 24 hour counter,
 * then the counter should roll over every 10,800 ticks.  If we start our IR
 * emitter at 5:00pm we should initialize the timer counter to 7,650.
 *
 * We want the IR emitter to send the "ON" command to the AC unit at 7:30am
 * (3,375 ticks), then again every 10 minutes (75 ticks) until 8:30am (3,825
 * ticks).
 *
 * The internal RC clock is not very accurate.  We can minimize the error from
 * this by turning the unit off and back on every day at 5:00pm to reset it.
 * We can also measure the actual length of our 8 second timer tick at the
 * uC/temperature/supply voltage/clock frequency we will operate at and
 * adjust the target tick points accordingly. (see TICK_ADJ)
 *
 * Some example timer intervals measured:
 *	 8.127s -> ATtiny2313-20PU, 23°C, 3.1V, 4MHz internal RC oscillator
 *   8.125s -> ATtiny2313-20PU, 23°C, 3.1V, 8MHz internal RC oscillator 
 *   8.227s -> ATtiny2313-20PU, 23°C, 5.1V, 8MHz internal RC oscillator 
 *   8.427s -> ATtiny13V-10PU, 23°C, 5.1V, 9.6MHz internal RC oscillator 
 *   8.342s -> ATtiny13V-10PU, 23°C, 3.3V, 4.8MHz internal RC oscillator
 *   8.231s -> ATtiny13V-10PU, 23°C, 1.8V, 4.8MHz internal RC oscillator
 *   8.275s -> ATtiny13V-10PU, 23°C, 2.4V, 4.8MHz internal RC oscillator
 *
 * The carrier frequency CARRIER_ADJ factor should also be adjusted to match the
 * original total message length at the uC/temperature/voltage/clock
 * settings used during operation.  This factor increases/reduces the carrier
 * period by 2X the specified number of microseconds.  This will probably
 * end up somewhere between 0 and -2.0us.
 *
 * The device needs to transmit one command for heating during the winter, and
 * a different command for cooling during the summer.  To keep from having
 * to reprogram the chip for each season, and to minimize the user interface
 * on the device, a simple season select is implemented using the power switch.
 *
 * Seasons are denoted by the color of the heartbeat LED.  Red for winter
 * (heating mode) and green for Summer (cooling).  If the device is switched
 * off during the first 8 seconds, it will start back up in the next "season".
 * If the startup season is correct and the device is allowed to run longer
 * than 8 seconds, then the next startup will be the same as the current season.
 *
 * Season information is stored in non-volatile EEPROM.  Assuming the device
 * is started twice per day (worst case when changing seasons), then four EEPROM
 * writes occur per day.  With a life of 100,000 write/erase cycles, that gives
 * us a 70 year life span for the EEPROM memory.
 *
 * Season mode changing is a little tricky because of sleeping in power-down
 * mode.  The processor is using very little power so there is enough power
 * stored in the filter capacitors to execute a few cycles after waking even
 * if the power was switched off long ago.  Therefore, when changing modes, be
 * sure to leave the power off at least 8 seconds (1 timer tick).  That way,
 * the processor will awaken and then attempt to flash the heartbeat
 * LED effectively draining the stored energy, and preventing the mode toggle
 * from executing unexpectedly.  Be sure to enable Brown Out Detection in the
 * fuses to get reliable shutdowns.
 *
 * Interesting anecdote:  I originally had the main loop sleep, check if first
 * time, do time switch statement, and finally pulse the heartbeat.  Sleeping
 * took so little power, and the supply filter caps held enough charge, that the
 * startup mode always got toggled after waking up even though the power had been
 * turned off.  There was just enough stored energy to execute a few instructions
 * after waking with the power off.  The fix was to move the EEPROM byte toggle 
 * to the end of the loop so that it would execute after everything else.
 *
 * Copyright (c) 2009 Tom Blough
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdint.h> 
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "utility.h"

// current configuration is for ATtiny13V-10PU, 3.1V, 4.8MHz internal osc, BOD @ 2.7V.  


// Infrared signal parameters
#define CARRIER_FREQ 38000  // IR carrier frequency
#define CYCLES_PER_PULSE 21  // width of 1T IR carrier pulse
#define US_PER_SECOND 1e6  // microseconds per second for freq. period conversion

// Carrier frequency adjustment.  Will need to be adjusted depending on
// actual uC/temperature/voltage/clock settings.
#define CARRIER_ADJ -0.8  // in microseconds to adjust 38KHz signal period.

// Watchdog clock adjustment.  Theoretical/Actual scales all time setpoints.
// Will need to be adjusted depending on actual uC/temperature/voltage/clock.
// Measure period between heartbeat pulses and enter the actual time here.
#define TICK_ADJ (8.0 / 8.34875)  // theoretical timer interval / actual interval

// I/O configuration
#define IR_LED( reg) BIT( B,0,reg)  // IR emitter
#define STATUS_LED_1( reg) BIT( B,1,reg)  // bi-color LED heartbeat status indicator
#define STATUS_LED_2( reg) BIT( B,2,reg)  // pin 1 high for green, pin 2 high for red
#define MARKER( reg) BIT( B,3,reg)  // for debugging purposes - not connected on hardware
									// marks location of command bytes in IR stream
									// can be used to trigger logic analyzer
// Persistent EEPROM state storage for current "season" operating mode
#define HEAT 0x00
#define COOL (uint8_t)~HEAT
uint8_t EEMEM nv_mode;	// startup mode



// Forward declarations
void sendCmd( uint8_t *cmd);
void sendByte( uint8_t byte);
void onePulse( uint8_t state);


/* Initialize hardware then enter endless loop of checking clock and sleeping.
 */
int main( void)
{
	// Media IR commands { Byte1, Byte2, Byte3 }
	// on, 27°C, heating + aux electric, auto fan
	uint8_t heat[3] = { 0b01001101, 0b11111101, 0b00111011 };
	// on, 22°C, cooling, auto fan
	uint8_t cool[3] = { 0b01001101, 0b11111101, 0b00001110 };

	uint16_t ticks;  // counter for 8 second watchdog timer ticks
	uint8_t first_time = TRUE;  // flag to check for our very first wake from sleep
	uint8_t *cmd;  // pointer to current seasons command to transmit

	// get previous mode from EEPROM and set the appropriate IR command
	switch( eeprom_read_byte( &nv_mode))
	{
		case HEAT:
			cmd = heat;
			break;
		case COOL:
			cmd = cool;
			break;
		default:  // invalid EEPROM data so initialize it to heating
			cmd = heat;
			eeprom_write_byte( &nv_mode, HEAT);
	}

	// now toggle the persistent mode for the next power-up
	eeprom_write_byte( &nv_mode, ~eeprom_read_byte( &nv_mode));

	// setup our I/O ports
	MARKER( DDR) = OUT;   
	STATUS_LED_1( DDR) = OUT;   
	STATUS_LED_2( DDR) = OUT;   
	STATUS_LED_1( PORT) = LOW;
	STATUS_LED_2( PORT) = LOW;
	IR_LED( DDR) = OUT;   
	IR_LED( PORT) = LOW;

	// signal startup and current mode to user
	if( cmd == heat)
	{
		// heating so light the red LED
		STATUS_LED_2( PORT) = HIGH;
		_delay_ms( 1000);
		STATUS_LED_2( PORT) = LOW;
	}
	else
	{
		// cooling - flash the green LED
		STATUS_LED_1( PORT) = HIGH;
		_delay_ms( 1000);
		STATUS_LED_1( PORT) = LOW;
	}

	// assume we are powering up at 5:00pm
	ticks = (int)(7650 * TICK_ADJ);// 17:00 @ 450 ticks per hour (8 second ticks)
	//ticks = (int)(3373 * TICK_ADJ);  // testing - send command two ticks after startup

    // we'll use the WD timer to wake us from sleep mode.  We'll power down
	// when we aren't doing anything to save the batteries.
	// setup the watchdog timer to interrupt (not reset) every 8 seconds
	MCUSR &= ~(1 << WDRF);  // clear the watchdog reset flag
	WDTCR = (1 << WDE) | (1 << WDCE);  // start watchdog setup
	WDTCR = (1 << WDTIF) | (1 << WDTIE) | (1 << WDP0) | (1 << WDP3);  // WD interrupt, 8s
	set_sleep_mode( SLEEP_MODE_PWR_DOWN);
	sei();  // enable interrupts


	/* used for adjusting carrier frequency, verifying IR LED output, and mode change
	 * testing.  Un-commenting this endless loop will transmit a 38KHz signal on the
	 * IR_LED output.  Connect a frequency counter to this output and change
	 * CARRIER_ADJ until close to 38KHz.  You can check if the IR LED is working
	 * by looking at the LED with a digital camera.  The IR LED should glow purple
	 * in the camera viewfinder.  The initial heartbeat LED color should
	 * also change with each power cycle.
	 while( TRUE)
		onePulse( ON);
     */


	/* main program loop - sleep, flash heartbeat LED, check time to see if we need
	 * to do anything, repeat.
	 */
	while( TRUE)
	{
		// power-down until our next WD interrupt wakes us in 8 seconds
		sleep_enable();
		sleep_cpu();
		sleep_disable();
		
		// if we reach this point, watchdog timer just fired to wake us up
		// re-enable watchdog interrupt mode since it is cleared each time
		WDTCR |= (1 << WDTIF) | (1 << WDTIE);

		ticks++;  // another 8 seconds have passed
		if( ticks >= (int)(10800 * TICK_ADJ)) ticks = 0;  // 24:00 midnight - roll over

		// give us a heartbeat so user knows we are active and in which mode
		if( cmd == heat)
		{
			// heating so light the red LED
			STATUS_LED_2( PORT) = HIGH;
			_delay_ms( 100);
			STATUS_LED_2( PORT) = LOW;
		}
		else
		{
			// cooling - flash the green LED
			STATUS_LED_1( PORT) = HIGH;
			_delay_ms( 100);
			STATUS_LED_1( PORT) = LOW;
		}

		// check the tick counter to see if it's time to do anything
		switch( ticks)
		{
			case (int)(3375 * TICK_ADJ):  // 7:30am @ 450 ticks per hour
			case (int)(3450 * TICK_ADJ):  // 7:40am
			case (int)(3525 * TICK_ADJ):  // 7:50am
			case (int)(3600 * TICK_ADJ):  // 8:00am
			case (int)(3675 * TICK_ADJ):  // 8:10am
			case (int)(3750 * TICK_ADJ):  // 8:20am
			case (int)(3825 * TICK_ADJ):  // 8:30am
				sendCmd( cmd);  // Midea remote control repeats command twice
				sendCmd( cmd);
				break;
			default:
				/* do nothing */;
		}

		// If we make it through the first sleep period, assume the
		// device started up in the right mode, so undo the previous
		// mode toggle we made before entering the main loop so we will
		// power up next time in the current mode.
		// Do this only on the first sleep so we are not continually
		// writing EEPROM.
		if( first_time)
		{
			first_time = FALSE;
			eeprom_write_byte( &nv_mode, ~eeprom_read_byte( &nv_mode));
		}
	}
}


ISR( WDT_vect)
{
	/* do nothing - the ISR just wakes us back up
	 *
	 * originally had the counter code below, but moving it to main loop saved
	 * 44 bytes program space and making ticks an instance variable in main instead
	 * of a global volatile saved 2 bytes in data.
	 *
	ticks++;  // another 8 seconds have passed
	if( ticks >= 10800) ticks = 0;  // midnight
	 */
} 

/*
 * Generate an IR command pulse train
 * 
 * Parameters:
 *    *cmd is a 3 byte array containing the Midea command data
 *
 * Returns: nothing
 */
void sendCmd( uint8_t *cmd)
{
	// send start - 8T on + 8T off
	for( uint8_t i = 0; i < 8; i++)
		onePulse( ON);
	for( uint8_t i = 0; i < 8; i++)
		onePulse( OFF);

	// send 48 bits of data
	MARKER( PORT) = LOW;  // used for testing to mark byte boundaries
	sendByte( cmd[0]);
	sendByte( (uint8_t) ~cmd[0]);
	sendByte( cmd[1]);
	sendByte( (uint8_t) ~cmd[1]);
	sendByte( cmd[2]);
	sendByte( (uint8_t) ~cmd[2]);

	// send stop - looks like a "0" bit followed by 8T space
	// 1T on + 1T off + 8T off
	onePulse( ON);
	for( uint8_t i = 0; i < 9; i++)
		onePulse( OFF);
}

/*
 * Encode a single data byte into IR pulses
 * 
 * Parameters:
 *    byte is the data to encode
 *
 * Returns: nothing
 */
void sendByte( uint8_t byte)
{
	MARKER( PORT) = ~MARKER( PORT);  // used for testing to mark byte boundaries
	for( uint8_t i = 0; i < 8; i++)
	{
		// "1" = 1T on + 3T off; "0" = 1T on + 1T off
		onePulse( ON);
		for( uint8_t i = 0; i < ((byte & 0x01) ? 3 : 1); i++)
			onePulse( OFF);
		byte >>= 1;  // get the next most significant bit
	}
}

/*
 * Generate a 1T carrier frequency IR pulse
 * 
 * Parameters:
 *    state is output level of IR pulse: HIGH or LOW 
 *
 * Returns: nothing
 */
void onePulse( uint8_t state)
{
	for( uint8_t i = 0; i < CYCLES_PER_PULSE; i++)
	{
		// 50% duty cycle carrier
		if( state) IR_LED( PORT) = HIGH;
		_delay_us( (US_PER_SECOND / (CARRIER_FREQ * 2)) + CARRIER_ADJ);
		IR_LED( PORT) = LOW;
		_delay_us( (US_PER_SECOND / (CARRIER_FREQ * 2)) + CARRIER_ADJ);
	}
}
