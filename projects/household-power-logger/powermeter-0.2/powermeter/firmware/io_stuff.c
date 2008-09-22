#include <avr/io.h>

#include "config.h"
#include "io_stuff.h"
#include "KS0108.h"

uint8_t can_read_buttons, debounce;
uint8_t last_buttons;
int8_t buttons_x, buttons_y;

void io_init() {
#if USE_GLCD
	release_glcd();
#endif
	deselect_mmc();

	DDRB = MMC_CS | KS0108_CS_ALL;

#if USE_UART
	enable_uart();
#endif

	// TWI for RTC
	TWSR = 0;
	TWBR = (F_CPU / 100000UL - 16) / 2;
}

void enable_spi_mmc() {
	DDRB |= (SCK | MOSI | MMC_CS);
	DDRB &= ~MISO;

	// enable SPI: master, clock=fosc/64
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);
}

void release_spi_mmc() {
	deselect_mmc();

	// send one last byte (this is from the
	// original mmc.c driver)
	SPDR = 0xFF;
	while(!(SPSR & (1<<SPIF)));

	// disable SPI
	SPCR &= ~_BV(SPE);

	DDRB &= ~(SCK | MOSI | MMC_CS);
}

void iostuff_tick(){
	if(debounce) debounce--;
}

void poll_buttons() {
	if(debounce || !can_read_buttons) return;
	uint8_t buttons = PIND & 0xf0;
	if(buttons == last_buttons) return;
	debounce = 5;
	if(!(buttons & 0x10) && (last_buttons & 0x10)) { buttons_y++; }
	if(!(buttons & 0x20) && (last_buttons & 0x20)) { buttons_x--; }
	if(!(buttons & 0x40) && (last_buttons & 0x40)) { buttons_y--; }
	if(!(buttons & 0x80) && (last_buttons & 0x80)) { buttons_x++; }
	last_buttons = buttons;
}

#if USE_GLCD
void enable_glcd() {
	poll_buttons();
	can_read_buttons = 0;
	GLCD_Delay();
#if USE_UART
	disable_uart();
#endif
	KS0108_CTRL_PORT &= ~KS0108_CS_ALL;
	KS0108_CTRL_DIR |= (KS0108_CS_ALL | KS0108_RS | KS0108_RW | KS0108_EN);
	GLCD_Delay();
}

void release_glcd() {
	GLCD_Delay();
#if USE_UART
	enable_uart();
#endif
	KS0108_DATA_DIR = 0x00;
	KS0108_DATA_PORT = 0;
	KS0108_CTRL_PORT &= ~KS0108_CS_ALL;
	KS0108_CTRL_DIR &= ~(KS0108_RS | KS0108_RW | KS0108_EN);
	GLCD_Delay();
	can_read_buttons = 1;
	poll_buttons();
}
#endif

#if USE_UART
void enable_uart() {
	UCSR0A = _BV(U2X0);
	UCSR0B = _BV(TXEN0) | _BV(RXEN0);
	UBRR0L = 16; // 115.2k for 16MHz clock
	//UBRR0L = 1; // 1M baud for 16MHz clock
}

void disable_uart() {
	UCSR0B = 0;
}
#endif
