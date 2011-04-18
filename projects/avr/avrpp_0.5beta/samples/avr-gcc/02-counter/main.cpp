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
 * using an AVR++ logical bus. It implements a counter that displays
 * an 8-bit binary value using 8 LEDs. 02-counter continues the
 * 01-blink demo, and only the new features are commented. To start
 * from the very beginning, please open 01-blink.
 *
 * NOTE: A comment explaining a line of code usually goes BEFORE the line.
 *
 * HARDWARE REQUIREMENTS: an AVR (e.g. ATmega16) with a set of LEDs
 * connected to PA0-PA3 and PB0-PB3. Each LED requires a resistor to limit
 * output current. Value of about 200-500 ohms probably is ok.
 *
 * EMULATED RUN: open the VMLAB project. VMLAB is free, and runs even on
 * Linux using WINE. All the necessary outputs are added to scope.
 */

#define AVRPP_AVR_GCC

#include <avrpp/ports.hpp>
#include <avrpp/delay.hpp>

/** A new header. It contains logical bus support. */
#include <avrpp/bus.hpp>


using namespace ::avrpp::ports;
using namespace ::avrpp::delay;

/**
 * And a new namespace. Omit the directive if you prefer verbose
 * notation.
 */
using namespace ::avrpp::bus;


/**
 * We are going to define a logical bus containing of pins, so
 * let's define pins first. What the next 8 lines say is:
 * "PB3 is pin_7, PA3 is pin_6... PB0 is pin_1, PA0 is pin_0".
 * Certainly, it's a very peculiar and a very ineffective way
 * of attaching LEDs, and it is very unlikely you would like to
 * do so in a real design, but let's see how AVR++ manages this.
 *
 * NOTE: If later you decide to change the pins your LEDs are
 * attached to, the ONLY part of your code that needs to be changed
 * is the following 8 lines.
 */
typedef output<port_b,3> pin_7;
typedef output<port_a,3> pin_6;
typedef output<port_b,2> pin_5;
typedef output<port_a,2> pin_4;
typedef output<port_b,1> pin_3;
typedef output<port_a,1> pin_2;
typedef output<port_b,0> pin_1;
typedef output<port_a,0> pin_0;

/**
 * Now we define a bus. In fact the following line means:
 * I am going to join the following pins in a logical output
 * bus, and to set their values together. 8 pins
 * form a byte (though AVR++ is not limited to 8-pin buses),
 * and we are going to set pin values as a byte where pin_7
 * is the eldest bit. For instance, 0b10101010 will set
 * pin_7, pin_5, pin_3 and pin_1 to 1, others to 0.
 *
 * That strange curse "TLIST8()" is simply a shortcut macro to create
 * a list of 8 types. There are several such shortcuts (TLIST1,
 * TLIST2, TLIST3 and so on) in <avrpp/typelist.hpp>.
 */
typedef output_bus <TLIST8(pin_7,pin_6,pin_5,pin_4,pin_3,
				pin_2,pin_1,pin_0)> counter_bus;

int main () {
	/**
	 * Of course, we can configure the pins one by one by calling their
	 * config(). But why do so? output_bus has a handy shortcut for that.
	*/
	counter_bus::config ();

	/**
	 * Just a counter.
	 */
	unsigned char counter = 0;

	while(1) {
		/**
		 * Yes, it's that easy! Just out() a value.
		 */
		counter_bus::out(counter);
		/** Incrementing the counter */
		counter++;
		ms<100>::delay<default_delay_policy> ();
	}
	return 0; //Just to avoid a warning in some configurations
}
