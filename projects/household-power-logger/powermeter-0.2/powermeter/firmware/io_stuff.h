#include "config.h"

#define MMC_CS _BV(PB2)
#define SCK    _BV(PB5)
#define MISO   _BV(PB4)
#define MOSI   _BV(PB3)

#define select_mmc()   PORTB &= ~MMC_CS
#define deselect_mmc() PORTB |=  MMC_CS

#define KS0108_DATA_PORT	PORTD
#define KS0108_DATA_DIR		DDRD
#define KS0108_DATA_PIN		PIND

#define KS0108_CTRL_PORT	PORTB
#define KS0108_CTRL_DIR		DDRB

#define KS0108_RS			_BV(PB3)
#define KS0108_RW			_BV(PB4)
#define KS0108_EN			_BV(PB5)

#define KS0108_CS1			_BV(PB0)
#define KS0108_CS2			_BV(PB1)
#define KS0108_CS3			_BV(PB1)

#define KS0108_CS_ALL	(KS0108_CS1 | KS0108_CS2 | KS0108_CS3)

extern int8_t buttons_x, buttons_y;

void io_init();
void enable_spi_mmc();
void release_spi_mmc();
void iostuff_tick();
void poll_buttons();
#if USE_GLCD
void enable_glcd();
void release_glcd();
#endif
void enable_uart();
void disable_uart();
