
/*
Header for Atmega1281 uart module

use util/setbaud.h to calculate ubrr and use_2x values
eg.

	#define BAUD_TOL	5
	#define BAUD		9600
	#include <util/setbaud.h>
	uart0_init(UBRR_VALUE, USE_2X, 64, 64);
	
	#undef	BAUD
	#define	BAUD		1200
	#include <util/setbaud.h>
	uart1_init(UBRR_VALUE, USE_2X, 64, 64);
	
Size of FIFO buffers can be as small as 1

*/

	extern struct fifo_control uart0_fifo_tx;
	extern struct fifo_control *uart0_fifo_rx_ptr;
	extern struct fifo_control uart1_fifo_tx;
	extern struct fifo_control *uart1_fifo_rx_ptr;

//set TRUE to enable nesting during uart ISR
	volatile char uart_isr_nest;

void uart0_init(int baud_ubrr, char use_2x, int fifo_tx_size, void* fifo_tx_data);
void uart1_init(int baud_ubrr, char use_2x, int fifo_tx_size, void* fifo_tx_data);

//these functions are for configuring streams
void uart0_tx_stream(char data, FILE junk);
void uart1_tx_stream(char data, FILE junk);
char uart0_rx_stream(char data, FILE junk);
char uart1_rx_stream(char data, FILE junk);
