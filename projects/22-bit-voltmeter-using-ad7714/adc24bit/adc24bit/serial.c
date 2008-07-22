#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

volatile char rxbuff;
volatile uint8_t rx_count = 0;

#define TX_BUF_LEN 100
volatile char tx_buf[TX_BUF_LEN];
volatile uint8_t tx_buf_size = 0;
volatile uint8_t tx_buf_tail = 0;

ISR(USART_RX_vect) {
	uint8_t c;

	c = UDR0;
	if(bit_is_clear(UCSR0A, FE0)) {
		rxbuff = c;
		rx_count++;
	}
}

ISR(USART_UDRE_vect) {
	if(tx_buf_size) {
		if(tx_buf_tail - tx_buf_size < 0) {
			UDR0 = tx_buf[tx_buf_tail - tx_buf_size + TX_BUF_LEN];
		} else {
			UDR0 = tx_buf[tx_buf_tail - tx_buf_size];
		}
		tx_buf_size--;
	} else {
		UCSR0B &= ~_BV(UDRIE0);
	}
}

void init_serial() {
	UCSR0A = _BV(U2X0);
	UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
	UBRR0L = 16; // 115.2k for 16MHz clock
	//UBRR0L = 0; // 2M baud for 16MHz clock
}

void putchr(char c) {
	if(c == '\n') putchr('\r');

	while(tx_buf_size == TX_BUF_LEN) {
		//sleep_mode();
		wdt_reset();
	}

	UCSR0B &= ~_BV(UDRIE0);
	tx_buf[tx_buf_tail++] = c;
	if(tx_buf_tail == TX_BUF_LEN) tx_buf_tail = 0;
	tx_buf_size++;
	UCSR0B |= _BV(UDRIE0);
}

uint8_t chr_avail() {
	return rx_count;
}

char getchr() {
	while(!rx_count) wdt_reset();
	char c = rxbuff;
	rx_count = 0;
	return c;
}

void printstr(const char *s) {
	while(*s) putchr(*s++);
}

void printstr_p(const char *s) {
	char c;
	while((c = pgm_read_byte(s++))) putchr(c);
}

void zeropad(char *s, int target_len) {
	int len = strlen(s);
	int diff = target_len - len;
	if(diff <= 0) return;
	int i;
	for(i=len; i>=0; i--) s[i+diff] = s[i];
	while(diff) s[--diff] = '0';
}

void print_uint16(uint16_t val) {
	char s[6];
	ltoa(val, s, 10);
	printstr(s);
}

void print_uint32(uint32_t val) {
	char s[11];
	ltoa(val, s, 10);
	printstr(s);
}
