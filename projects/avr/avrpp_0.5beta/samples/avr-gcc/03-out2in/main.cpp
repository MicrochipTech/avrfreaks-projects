/*
   Copyright 2011 Elijah M. Merkin aka Ellioh

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

/**
 * The following demo is an example of working with MCU GPIO ports
 * using AVR++ logical bus classes. It creates two 4-bit bus classes,
 * one for input and one for output. The output pins are electrically
 * connected to input pins, and when we send a 4-bit value to output
 * it is immediately read from input with a cycle shift.
 * 03-out2in continues the 02-counter & 01-blink demos, and only the
 * new features are commented. To start from the very beginning, please
 * open 01-blink.
 *
 * NOTE: A comment explaining a line of code usually goes BEFORE the line.
 *
 * HARDWARE REQUIREMENTS: an AVR (e.g. ATmega16). External electric
 * connections: PA0 to PB1, PA1 to PB2, PA2 to PB3, PA3 to PB0.
 * You may connect LEDs to PA0-PA3 to see how the lights move.
 * Each LED requires a resistor to limit output current. Value of about
 * 200-500 ohms probably is ok.
 *
 * EMULATED RUN: open the VMLAB project. VMLAB is free, and runs even on
 * Linux using WINE. All the necessary outputs are added to scope.
 */

#define AVRPP_AVR_GCC

#include <avrpp/ports.hpp>
#include <avrpp/delay.hpp>
#include <avrpp/bus.hpp>


using namespace ::avrpp::ports;
using namespace ::avrpp::delay;
using namespace ::avrpp::bus;


/**
 * Output pins: PA3-PA0
 */
typedef output<port_a,3> o3;
typedef output<port_a,2> o2;
typedef output<port_a,1> o1;
typedef output<port_a,0> o0;

/**
 * Input pins: PB3-PB0. Hi-Z mode (another options is to tell "pullup"
 * instead of "hi_z", that would enable pull-up resistors).
 */
typedef input<port_b,3,hi_z> i3;
typedef input<port_b,2,hi_z> i2;
typedef input<port_b,1,hi_z> i1;
typedef input<port_b,0,hi_z> i0;


/**
 * Output bus.
 */
typedef output_bus <TLIST4(o3,o2,o1,o0)> out_bus;

/**
 * Input bus. Creation is quite similar to the output one.
 */
typedef input_bus <TLIST4(i3,i2,i1,i0)> in_bus;


int main () {
	out_bus::config ();
	/**
	 * The input bus is configured quite like the output one. The call to
	 * config () configures both data direction bits and pull-up/hi-z
	 * bits.
	*/
	in_bus::config ();

	/**
	 * A value we are going to send to output. Only one bit
	 * is set.
	 */
	unsigned char val = 1;

	while(1) {
		out_bus::out(val);
		/** A small delay, just to make sure an output value have reached
		 * inputs. */
		us<1>::delay<default_delay_policy> ();
		/** Getting the values */
		val = in_bus::in ();
		ms<200>::delay<default_delay_policy> ();
	}
	return 0; //Just to avoid a warning in some configurations
}

