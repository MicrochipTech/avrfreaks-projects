/*
 * stk600.c
 *
 *  Created on: Jul 7, 2009
 *      Author: nl9x1nt
 */

#include "stk600.h"
#include "compiler.h"

#if BOARD == STK600

void LED_Toggle(U32 leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Tgl_bits(LED_State, leds);

  // While there are specified LEDs left to manage...
  /*
  while (leds)
  {
    // Select the next specified LED and toggle it.
    led_shift = 1 + ctz(leds);

    led_descriptor += led_shift;
    gpio_tgl_gpio_pin();

    leds >>= led_shift;
  }*/

  int x;
  for (x = 0;x < LED_COUNT;x++)
  {
	  if (leds & (1 << (x + LED_START)))
	  {
		  gpio_tgl_gpio_pin(x + LED_START);
	  }
  }
}

#endif
