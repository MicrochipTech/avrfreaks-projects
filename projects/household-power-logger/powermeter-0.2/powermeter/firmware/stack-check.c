#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "serial.h"

// http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=52249
extern uint8_t _end;

void init_stackcheck(void) {
	volatile uint8_t *d = &_end;
	cli();
	while(d <= ((uint8_t *)SP)) *d++ = 0x55;
	sei();
}

int16_t get_free_stack(void) {
	volatile uint8_t *d = &_end;
	while(d <= ((uint8_t *)SP) && (*d == 0x55)) ++d;
	return (int16_t)d - (int16_t)&_end;
}

void show_free_stack() {
	printP("[free=");
	print_int32(get_free_stack());
	printP("]\n");
}

void trap_stack_overflow() {
	if(get_free_stack() == 0) return;

	printP("Stack overflow.\n");
	for(;;) { }
}
