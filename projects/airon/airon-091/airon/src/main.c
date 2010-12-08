/**
 * airon project
 *
 * (C) 2010, Angelo Dureghello, Trieste, Italy
 *
 * Consumption data:
 *
 * 21.0mA 13.6V display light off
 * 45.3mA 13.6V display light on
 *
 */
#include "init.h"
#include "tasks.h"

#include <avr/io.h>

int main (void)
{
	SystemInit ();

	return Run();
}
