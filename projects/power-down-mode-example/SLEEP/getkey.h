#ifndef getkey_h_
#define	getkey_h_

#include <util\atomic.h>                // need "--std=c99"
#include <avr\interrupt.h>

uint8_t get_key_press( uint8_t key_mask );

uint8_t get_key_idle( uint8_t key_mask );

void get_key_init( void );

#endif
