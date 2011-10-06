
#include "includes.h"

/*
*/

// Global variables

	struct fifo_control uart0_fifo_tx;
	struct fifo_control *uart0_fifo_rx_ptr;
	struct fifo_control uart1_fifo_tx; 
	struct fifo_control *uart1_fifo_rx_ptr;

	volatile char uart_isr_nest=FALSE;


void uart0_isrtx_enable(void)
{
	UCSR0B |= _BV(UDRIE0);	// tx interrupt
}

void uart1_isrtx_enable(void)
{
	UCSR1B |= _BV(UDRIE1);	// tx interrupt
}

void uart0_init(int baud_ubrr, char use_2x, int fifo_tx_size, void* fifo_tx_data)
{
	fifo_init(&uart0_fifo_tx, fifo_tx_size, uart0_isrtx_enable, NULL, fifo_tx_data);

	// Setup Uart0
	DDRE |=_BV(PE1);		// UTX0 output
	DDRE &=~_BV(PE0);		// UTX0 input
	UCSR0B |= _BV(TXEN0);	// enable tx
	UCSR0B |= _BV(RXEN0);	// enable rx
	UCSR0B |= _BV(RXCIE0);	// enable rx interrupt (writes to fifo)
	UCSR0B &= ~_BV(UDRIE0);	// disable tx interrupt (for now)

	UBRR0H = (char)(baud_ubrr>>8);
	UBRR0L = (char)(baud_ubrr);

	if(use_2x)
		UCSR0A |= _BV(U2X0);
	else
		UCSR0A &= _BV(U2X0);

}

void uart1_init(int baud_ubrr, char use_2x, int fifo_tx_size, void* fifo_tx_data)
{
	fifo_init(&uart1_fifo_tx, fifo_tx_size, uart1_isrtx_enable, NULL, fifo_tx_data);

	// Setup Uart1
	DDRD |=_BV(PD3);		// UTX1 output
	DDRD &=~_BV(PD2);		// URX1 input
	UCSR1B |= _BV(TXEN1);	// enable tx
	UCSR1B |= _BV(RXEN1);	// enable rx
	UCSR1B |= _BV(RXCIE1);	// enable rx interrupt (writes to fifo)
	UCSR1B &= ~_BV(UDRIE1);	// disable tx interrupt (for now)	

	UBRR1H = (char)(baud_ubrr>>8);
	UBRR1L = (char)(baud_ubrr);

	if(use_2x)
		UCSR1A |= _BV(U2X1);
	else
		UCSR1A &= _BV(U2X1);
}


void uart0_tx_stream(char data, FILE junk)
{
	fifo_write_char(&uart0_fifo_tx, data);
}

void uart1_tx_stream(char data, FILE junk)
{
	fifo_write_char(&uart1_fifo_tx, data);
}

char uart0_rx_stream(char data, FILE junk)
{
	return fifo_read_char(uart0_fifo_rx_ptr);
}

char uart1_rx_stream(char data, FILE junk)
{
	return fifo_read_char(uart1_fifo_rx_ptr);
}

//reduced version of fifo_write_char in macro form to speed up uart service time
#define UART_FIFO_WRITE_CHAR(fifo_ptr,data) 	\
{												\
	*((char*)fifo_ptr->head_ptr) = data;		\
	fifo_ptr->head_ptr++;						\
	if(fifo_ptr->head_ptr == fifo_ptr->end)		\
		fifo_ptr->head_ptr = fifo_ptr->start;	\
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)			\
	{											\
		fifo_ptr->bytes_free --;				\
		fifo_ptr->bytes_used ++;				\
		if(fifo_ptr->bytes_free == 0)			\
			fifo_ptr->full=TRUE;				\
		if(fifo_ptr->bytes_used != 0)			\
			fifo_ptr->empty=FALSE;				\
	}											\
	if(fifo_ptr->post_fptr)						\
			fifo_ptr->post_fptr();				\
}												\

ISR(USART0_RX_vect)
{
	volatile char temp;
	if(uart_isr_nest)
	{
		UCSR0B &=~_BV(RXCIE0);	// disable this ISR
		sei();
	};
	if(uart0_fifo_rx_ptr)
	{
		if(!uart0_fifo_rx_ptr->full)
			UART_FIFO_WRITE_CHAR(uart0_fifo_rx_ptr,UDR0)
		else
			temp=UDR0;
	}
	else
		temp=UDR0;
		
	cli();					// disable nesting
	UCSR0B |=_BV(RXCIE0);	// enable this ISR
}

ISR(USART1_RX_vect)
{
	volatile char temp;
	if(uart_isr_nest)
	{
		UCSR1B &=~_BV(RXCIE1);	// disable this ISR
		sei();
	};
	if(uart1_fifo_rx_ptr)
	{
		if(!uart1_fifo_rx_ptr->full)
			UART_FIFO_WRITE_CHAR(uart1_fifo_rx_ptr,UDR1)
		else
			temp=UDR1;
	}
	else
		temp=UDR1;
	cli();					// disable nesting
	UCSR1B |=_BV(RXCIE1);	// enable this ISR
}

//macro version of fifo_read_char to speed up uart ISR
#define UART_FIFO_READ_CHAR(target, source_fifo)	\
{													\
	target=*((char*)source_fifo.tail_ptr);			\
	source_fifo.tail_ptr++;							\
													\
	if(source_fifo.tail_ptr == source_fifo.end)		\
		source_fifo.tail_ptr = source_fifo.start;	\
													\
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)				\
	{												\
		source_fifo.bytes_free ++;					\
		source_fifo.bytes_used --;					\
		if(source_fifo.bytes_used == 0)				\
			source_fifo.empty=TRUE;					\
		if(source_fifo.bytes_free != 0)				\
			source_fifo.full=FALSE;					\
	}												\
}													\

ISR(USART0_UDRE_vect)
{
	//enable nesting
	if(uart_isr_nest)
	{
		UCSR0B &=~_BV(UDRIE0);	// disable this ISR
		sei();
	};

	//read next byte if available, else disable this isr
	if(!uart0_fifo_tx.empty)
	{
		UART_FIFO_READ_CHAR(UDR0, uart0_fifo_tx);
		cli();	
		UCSR0B |=_BV(UDRIE0);
	}
	else
	{
		cli();	
		UCSR0B &=~_BV(UDRIE0);
	};
}

ISR(USART1_UDRE_vect)
{
	//enable nesting
	if(uart_isr_nest)
	{
		UCSR1B &=~_BV(UDRIE1);	// disable this ISR
		sei();
	};

	//read next byte if available, else disable this isr
	if(!uart1_fifo_tx.empty)
	{
		UART_FIFO_READ_CHAR(UDR1, uart1_fifo_tx);
		cli();	
		UCSR1B |=_BV(UDRIE1);
	}
	else
	{
		cli();	
		UCSR1B &=~_BV(UDRIE1);
	};
}
