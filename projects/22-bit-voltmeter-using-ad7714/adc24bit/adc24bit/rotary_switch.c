#include <avr/io.h>

#include "config.h"

volatile static uint8_t bounce_delay=0;
static uint8_t old_state1, old_state2;
static uint8_t ROT_TABLE[] = { 0, 3, 1, 2 };

static uint8_t read_state1() {
	return (ROTARY_PIN & _BV(ROTARY1A_BIT) ? 1 : 0) |
		   (ROTARY_PIN & _BV(ROTARY1B_BIT) ? 2 : 0);
}

static uint8_t read_state2() {
	return (ROTARY_PIN & _BV(ROTARY2A_BIT) ? 1 : 0) |
		   (ROTARY_PIN & _BV(ROTARY2B_BIT) ? 2 : 0);
}

static int8_t get_rotary_delta(uint8_t old, uint8_t new) {
	int8_t rot = ROT_TABLE[new & 3] - ROT_TABLE[old & 3];
	if(rot ==  3) rot = -1;
	if(rot == -3) rot =  1;
	return rot;
}

void init_rotary() {
	ROTARY_DDR  &= ~_BV(ROTARY1A_BIT);
	ROTARY_DDR  &= ~_BV(ROTARY1B_BIT);
	ROTARY_DDR  &= ~_BV(ROTARY2A_BIT);
	ROTARY_DDR  &= ~_BV(ROTARY2B_BIT);
	ROTARY_PORT |=  _BV(ROTARY1A_BIT);
	ROTARY_PORT |=  _BV(ROTARY1B_BIT);
	ROTARY_PORT |=  _BV(ROTARY2A_BIT);
	ROTARY_PORT |=  _BV(ROTARY2B_BIT);

	old_state1 = read_state1();
	old_state2 = read_state2();
}

void rotary_bounce_tick() {
	if(bounce_delay) bounce_delay--;
}

void get_rotary(int8_t *rotary1, int8_t *rotary2) {
	*rotary1 = *rotary2 = 0;
	if(!bounce_delay) {
		uint8_t new_state1 = read_state1();
		if(new_state1 != old_state1) {
			*rotary1 = get_rotary_delta(old_state1, new_state1);
			old_state1 = new_state1;
			bounce_delay = 20;
		}
		uint8_t new_state2 = read_state2();
		if(new_state2 != old_state2) {
			*rotary2 = get_rotary_delta(old_state2, new_state2);
			old_state2 = new_state2;
			bounce_delay = 20;
		}
	}
}
