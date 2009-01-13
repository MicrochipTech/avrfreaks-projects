
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>


void uart_tx1(char *);
void uart_tx1_P(PGM_P);

void thread1(void);
void thread2(void);

void makethreads(void) __attribute__ ( ( naked ) ) __attribute__ ( ( noinline ) );
void next_thread(void) __attribute__ ( ( naked ) ) __attribute__ ( ( noinline ) );


// **** Global variables ****
	extern volatile unsigned char current_thread;


// Main application routine. 
int main(void)
{
	unsigned char tempchar=5;

	// Setup Uart1
	DDRD |=_BV(PD3);		// UTX1 output
	DDRD &=~_BV(PD2);		// URX1 input
	UCSR1B |= _BV(TXEN1);	// enable tx
	UBRR1H = 0;
	UBRR1L = 95;			// 9600 baud at 14.7456MHz

	makethreads();
	switch(current_thread)
	{
		case 1:
			thread1();
		case 2:
			thread2();
	};

	while(tempchar--)
	{
		uart_tx1_P(PSTR("thread0\r\n"));
		next_thread();
	};

	while(1);	
}

void thread1(void)
{
	while(1)
	{
		uart_tx1_P(PSTR("thread1\r\n"));
		next_thread();
	};
}

void thread2(void)
{
	while(1)
	{
		uart_tx1_P(PSTR("thread2\r\n"));
		next_thread();
	};
}


void uart_tx1_P(PGM_P string)
{
	char tempchar;
	while((tempchar=pgm_read_byte(string)))
	{
		UDR1=tempchar;
		string++;
		while(!(UCSR1A & _BV(UDRE1)));
	};
}

