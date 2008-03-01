/*
 * Super basic example code for nRF2401 code.  Just creates packets of
 *  printable characters & sends them via the RF module, and receives
 *  and prints packets.  Not really useful for anything but example/
 *  verifying modules are working.  Should work for M8,  M168; others
 *  may require tweaking.
 *
 *  (c) 2008 Tymm Twillman <tymmothy@gmail.com>
 *
 *  This file is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published 
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This file is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <avr/io.h>
#include <stdint.h>

#include "nrf2401.h"

/* Multiple units on same address can communicate with each other
 *  (useful for initial testing)
 */

#define REMOTE_ADDR { 0x00, 0x01, 0x02, 0x03, 0x04 }
#define LOCAL_ADDR  { 0x00, 0x01, 0x02, 0x03, 0x04 }


void uart_putchar(char c)
{
#if defined(UDR)
	while (!(UCSRA & _BV(UDRE)));
	
	UDR = c;
#else
	while (!(UCSR0A & _BV(UDRE0)));
	
	UDR0 = c;
#endif
}


void uart_puts(char *s)
{
	while (*s)
		uart_putchar(*s++);
}


void uart_init(int baud)
{
	uint16_t ubrr_val = ((((MCU_CLK / 16L) + (((uint32_t)baud) >> 1))
	                      / ((uint32_t)baud)) - 1L);
	
#ifdef UBRRH
	UBRRH = (ubrr_val & 0x7f00) >> 8;
	UBRRL = (ubrr_val & 0xff);
#else
	UBRR0H = (ubrr_val & 0x7f00) >> 8;
	UBRR0L = (ubrr_val & 0xff);
#endif
}


int main()
{
	uint8_t remote_addr[5] = REMOTE_ADDR;
	uint8_t local_addr[5] = LOCAL_ADDR;
	uint8_t pkt[32];
	int i;
	int len;
	unsigned long loop_count = 0;
	uint8_t last_char = 0;
	
	
	uart_init(9600);
	nrf2401_init(0x55, local_addr, 0x00);

	while(1) {
		if (nrf2401_available()) {
			len = nrf2401_recv(pkt);
			uart_puts("Received: ");
			for (i = 0; i < len; i++) {
				uart_putchar(pkt[i]);
			}
			uart_putchar(0x0d);
			uart_putchar(0x0a);
		}
		
		for (i = 0; i < 24; i++) {
			pkt[i] = '@' + last_char;
			last_char++;
			last_char &= 0x1f; // keep in small, readable range
		}
			
		len = nrf2401_send(remote_addr, pkt);
		uart_puts("Sent: ");
		for (i = 0; i < len; i++) {
			uart_putchar(pkt[i]);
		}
		uart_putchar(0x0d);
	        uart_putchar(0x0a);
                loop_count = 0;
	}
	
	return 0;
}
