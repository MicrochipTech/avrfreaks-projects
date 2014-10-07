/*
 * db_uart.h
 *
 * Created: 11.4.2014 11:48:49
 *  Author: PjV
 */ 

#ifndef DB_UART_H_
#define DB_UART_H_

#include <avr/pgmspace.h>

void db_uart_init(void);
void db_uart_putc(unsigned char c);
void db_uart_puts(const char* str);
void db_uart_puts_p(PGM_P str);

#endif /* DB_UART_H_ */
