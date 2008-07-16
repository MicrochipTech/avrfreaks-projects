#ifndef _SOFTUART_H_
#define _SOFTUART_H_
#include "../config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdint.h>

//baudrate ist fixed !!!
//define delay: 52us -> 19200 baud
#define SOFTUART_BAUD_RATE 19200
#define SOFTUART_DELAY()  _delay_us(52)

//pinconfig:
#include "../config.h"

#define SOFTUART_TXPIN_LO() SOFTUART_PORT &= ~(1<<SOFTUART_TXPIN)
#define SOFTUART_TXPIN_HI() SOFTUART_PORT |= (1<<SOFTUART_TXPIN)

#define softuart_puts_progmem(x) softuart_puts_progmem2(PSTR(x))
void softuart_put_ip(uint32_t ip);
void softuart_put_uint16(unsigned int c);
void softuart_put_uint8(unsigned char c);
void softuart_putnewline(void);
void softuart_puts_progmem2(PGM_P pointer);
void softuart_putc(unsigned char c);
void softuart_puts (char *s);
inline void softuart_init(void);

#endif

