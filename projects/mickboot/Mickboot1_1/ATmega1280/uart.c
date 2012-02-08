#include <includes.h>

#define UART_FIFO_RX_SIZE	255
#define UART_FIFO_TX_SIZE	64

/*
*/

void uart3_isrtx_enable(void);
void uart2_isrtx_enable(void);
void uart0_isrtx_enable(void);
void uart1_isrtx_enable(void);

// External prototypes

// Global variables

	char uart_fifo_data_tx[UART_FIFO_TX_SIZE];
	char uart_fifo_data_rx[UART_FIFO_RX_SIZE];

	struct fifo_control uart_fifo_tx;
	struct fifo_control uart_fifo_rx;

char uart_init(void)
{
	int tempint;
	char found=FALSE;

	//Uart0 already configured? (means bootloader called from main app)
	if(UCSR0B & _BV(RXEN0))
	{
		UCSR0B |= _BV(RXCIE0);	// enable rx interrupt (writes to fifo)
		fifo_init(&uart_fifo_tx,UART_FIFO_TX_SIZE, uart0_isrtx_enable, uart_fifo_data_tx);
		fifo_init(&uart_fifo_rx,UART_FIFO_RX_SIZE, NULL, uart_fifo_data_rx);
		found=TRUE;
	}
	else if(UCSR1B & _BV(RXEN1))
	{
		UCSR1B |= _BV(RXCIE1);	// enable rx interrupt (writes to fifo)
		fifo_init(&uart_fifo_tx,UART_FIFO_TX_SIZE, uart1_isrtx_enable, uart_fifo_data_tx);
		fifo_init(&uart_fifo_rx,UART_FIFO_RX_SIZE, NULL, uart_fifo_data_rx);
		found=TRUE;
	}
	else if(UCSR2B & _BV(RXEN2))
	{
		UCSR2B |= _BV(RXCIE2);	// enable rx interrupt (writes to fifo)
		fifo_init(&uart_fifo_tx,UART_FIFO_TX_SIZE, uart2_isrtx_enable, uart_fifo_data_tx);
		fifo_init(&uart_fifo_rx,UART_FIFO_RX_SIZE, NULL, uart_fifo_data_rx);
		found=TRUE;
	}
	else if(UCSR3B & _BV(RXEN3))
	{
		UCSR3B |= _BV(RXCIE3);	// enable rx interrupt (writes to fifo)
		fifo_init(&uart_fifo_tx,UART_FIFO_TX_SIZE, uart3_isrtx_enable, uart_fifo_data_tx);
		fifo_init(&uart_fifo_rx,UART_FIFO_RX_SIZE, NULL, uart_fifo_data_rx);
		found=TRUE;
	}
	else 
	{
		// Setup Uart0
		DDRE &=~_BV(PE0);		// UTX0 input
		UCSR0B |= _BV(RXEN0);	// enable rx
		UCSR0B |= _BV(RXCIE0);	// enable rx interrupt (writes to fifo)
		UCSR0B &= ~_BV(UDRIE0);	// disable tx interrupt (for now)	
		
		// Setup Uart1
		DDRD &=~_BV(PD2);		// URX1 input
		UCSR1B |= _BV(RXEN1);	// enable rx
		UCSR1B |= _BV(RXCIE1);	// enable rx interrupt (writes to fifo)
		UCSR1B &= ~_BV(UDRIE1);	// disable tx interrupt (for now)	
		
		// Setup Uart2
		DDRH &=~_BV(PH0);		// UTX2 input
		UCSR2B |= _BV(RXEN2);	// enable rx
		UCSR2B |= _BV(RXCIE2);	// enable rx interrupt (writes to fifo)
		UCSR2B &= ~_BV(UDRIE2);	// disable tx interrupt (for now)	
		
		// Setup Uart3
		DDRJ &=~_BV(PJ0);		// URX3 input
		UCSR3B |= _BV(RXEN3);	// enable rx
		UCSR3B |= _BV(RXCIE3);	// enable rx interrupt (writes to fifo)
		UCSR3B &= ~_BV(UDRIE3);	// disable tx interrupt (for now)	
		
		#define BAUD_TOL	5
		#define BAUD		UART0BAUD
		#include <util/setbaud.h>
		UBRR0H = (char)(UBRR_VALUE>>8);
		UBRR0L = (char)(UBRR_VALUE);
		#if USE_2X
			UCSR0A |= _BV(U2X0);
		#else
			UCSR0A &= _BV(U2X0);
		#endif
		
		#undef	BAUD
		#define	BAUD		UART1BAUD
		#include <util/setbaud.h>
		UBRR1H = (char)(UBRR_VALUE>>8);
		UBRR1L = (char)(UBRR_VALUE);
		#if USE_2X
			UCSR1A |= _BV(U2X1);
		#else
			UCSR1A &= _BV(U2X1);
		#endif
		
		#undef	BAUD
		#define	BAUD		UART2BAUD
		#include <util/setbaud.h>
		UBRR2H = (char)(UBRR_VALUE>>8);
		UBRR2L = (char)(UBRR_VALUE);
		#if USE_2X
			UCSR2A |= _BV(U2X2);
		#else
			UCSR2A &= _BV(U2X2);
		#endif
		
		#undef	BAUD
		#define	BAUD		UART3BAUD
		#include <util/setbaud.h>
		UBRR3H = (char)(UBRR_VALUE>>8);
		UBRR3L = (char)(UBRR_VALUE);
		#if USE_2X
			UCSR3A |= _BV(U2X3);
		#else
			UCSR3A &= _BV(U2X3);
		#endif
		
		tempint=BOOTWINDOW;
		while(tempint--)
		{
			if(UCSR0A & _BV(RXC0))
			{
				if(UDR0==' ')
				{
					UCSR1B &=~_BV(RXEN1);	// disable rx
					UCSR1B &=~_BV(RXCIE1);	// disable rx interrupt (writes to fifo)
					UCSR2B &=~_BV(RXEN2);	// disable rx
					UCSR2B &=~_BV(RXCIE2);	// disable rx interrupt (writes to fifo)
					UCSR3B &=~_BV(RXEN3);	// disable rx
					UCSR3B &=~_BV(RXCIE3);	// disable rx interrupt (writes to fifo)
					
					fifo_init(&uart_fifo_tx,UART_FIFO_TX_SIZE, uart0_isrtx_enable, uart_fifo_data_tx);
					fifo_init(&uart_fifo_rx,UART_FIFO_RX_SIZE, NULL, uart_fifo_data_rx);
					DDRE |=_BV(PE1);		// UTX0 output
					UCSR0B |= _BV(TXEN0);	// enable tx
					UCSR0B |= _BV(UDRIE0);	// enable tx interrupt
					tempint=0;
					found=TRUE;
				};
			};
			
			if(UCSR1A & _BV(RXC1))
			{
				if(UDR1==' ')
				{
					UCSR0B &=~_BV(RXEN0);	// disable rx
					UCSR0B &=~_BV(RXCIE0);	// disable rx interrupt (writes to fifo)
					UCSR2B &=~_BV(RXEN2);	// disable rx
					UCSR2B &=~_BV(RXCIE2);	// disable rx interrupt (writes to fifo)
					UCSR3B &=~_BV(RXEN3);	// disable rx
					UCSR3B &=~_BV(RXCIE3);	// disable rx interrupt (writes to fifo)
					
					fifo_init(&uart_fifo_tx,UART_FIFO_TX_SIZE, uart1_isrtx_enable, uart_fifo_data_tx);
					fifo_init(&uart_fifo_rx,UART_FIFO_RX_SIZE, NULL, uart_fifo_data_rx);
					DDRD |=_BV(PD3);		// UTX1 output
					UCSR1B |= _BV(TXEN1);	// enable tx
					UCSR1B |= _BV(UDRIE1);	// enable tx interrupt
					tempint=0;
					found=TRUE;
				};
			};

			if(UCSR2A & _BV(RXC2))
			{
				if(UDR2==' ')
				{
					UCSR0B &=~_BV(RXEN0);	// disable rx
					UCSR0B &=~_BV(RXCIE0);	// disable rx interrupt (writes to fifo)
					UCSR1B &=~_BV(RXEN1);	// disable rx
					UCSR1B &=~_BV(RXCIE1);	// disable rx interrupt (writes to fifo)
					UCSR3B &=~_BV(RXEN3);	// disable rx
					UCSR3B &=~_BV(RXCIE3);	// disable rx interrupt (writes to fifo)
					
					fifo_init(&uart_fifo_tx,UART_FIFO_TX_SIZE, uart2_isrtx_enable, uart_fifo_data_tx);
					fifo_init(&uart_fifo_rx,UART_FIFO_RX_SIZE, NULL, uart_fifo_data_rx);
					DDRH |=_BV(PH1);		// UTX2 output
					UCSR2B |= _BV(TXEN2);	// enable tx
					UCSR2B |= _BV(UDRIE2);	// enable tx interrupt
					tempint=0;
					found=TRUE;
				};
			};

			if(UCSR3A & _BV(RXC3))
			{
				if(UDR3==' ')
				{
					UCSR0B &=~_BV(RXEN0);	// disable rx
					UCSR0B &=~_BV(RXCIE0);	// disable rx interrupt (writes to fifo)
					UCSR1B &=~_BV(RXEN1);	// disable rx
					UCSR1B &=~_BV(RXCIE1);	// disable rx interrupt (writes to fifo)
					UCSR2B &=~_BV(RXEN2);	// disable rx
					UCSR2B &=~_BV(RXCIE2);	// disable rx interrupt (writes to fifo)
					
					fifo_init(&uart_fifo_tx,UART_FIFO_TX_SIZE, uart3_isrtx_enable, uart_fifo_data_tx);
					fifo_init(&uart_fifo_rx,UART_FIFO_RX_SIZE, NULL, uart_fifo_data_rx);
					DDRJ |= _BV(PJ1);		// UTX3 output
					UCSR3B |= _BV(TXEN3);	// enable tx
					UCSR3B |= _BV(UDRIE3);	// enable tx interrupt
					tempint=0;
					found=TRUE;
				};
			};

			_delay_ms(1);
		};
	};
	
	if(!found)
	{
		UCSR0B=0;
		UCSR1B=0;
		UCSR2B=0;
		UCSR3B=0;
	
		UBRR0H = 0;
		UBRR0L = 0;
		UBRR1H = 0;
		UBRR1L = 0;
		UBRR2H = 0;
		UBRR2L = 0;
		UBRR3H = 0;
		UBRR3L = 0;
	
		UCSR0A =0x20;
		UCSR1A =0x20;	
		UCSR2A =0x20;
		UCSR3A =0x20;	
	};
	return found;
}

void uart_write_string(char*string)
{
	while(*string)
	{
		while(uart_fifo_tx.bytes_free==0);
		
		*((char*)uart_fifo_tx.head_ptr) = *string;
		uart_fifo_tx.head_ptr++;
		
		if(uart_fifo_tx.head_ptr == uart_fifo_tx.end)
			uart_fifo_tx.head_ptr = uart_fifo_tx.start;
		
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			uart_fifo_tx.bytes_free --;
			uart_fifo_tx.bytes_used ++;
		}
		
		uart_fifo_tx.post_fptr();
		string++;
	};
}

void uart0_isrtx_enable(void)
{
	UCSR0B |= _BV(UDRIE0);	// tx interrupt
}

void uart1_isrtx_enable(void)
{
	UCSR1B |= _BV(UDRIE1);	// tx interrupt
}

void uart2_isrtx_enable(void)
{
	UCSR2B |= _BV(UDRIE2);	// tx interrupt
}

void uart3_isrtx_enable(void)
{
	UCSR3B |= _BV(UDRIE3);	// tx interrupt
}

ISR(USART0_RX_vect)
{
	*((char*)uart_fifo_rx.head_ptr) = UDR0;
	uart_fifo_rx.head_ptr++;

	if(uart_fifo_rx.head_ptr == uart_fifo_rx.end)
		uart_fifo_rx.head_ptr = uart_fifo_rx.start;	
	uart_fifo_rx.bytes_used ++;
}

ISR(USART1_RX_vect)
{
	*((char*)uart_fifo_rx.head_ptr) = UDR1;
	uart_fifo_rx.head_ptr++;

	if(uart_fifo_rx.head_ptr == uart_fifo_rx.end)
		uart_fifo_rx.head_ptr = uart_fifo_rx.start;	
	uart_fifo_rx.bytes_used ++;
}

ISR(USART2_RX_vect)
{
	*((char*)uart_fifo_rx.head_ptr) = UDR2;
	uart_fifo_rx.head_ptr++;

	if(uart_fifo_rx.head_ptr == uart_fifo_rx.end)
		uart_fifo_rx.head_ptr = uart_fifo_rx.start;	
	uart_fifo_rx.bytes_used ++;
}

ISR(USART3_RX_vect)
{
	*((char*)uart_fifo_rx.head_ptr) = UDR3;
	uart_fifo_rx.head_ptr++;

	if(uart_fifo_rx.head_ptr == uart_fifo_rx.end)
		uart_fifo_rx.head_ptr = uart_fifo_rx.start;	
	uart_fifo_rx.bytes_used ++;
}

ISR(USART0_UDRE_vect)
{
	if(uart_fifo_tx.bytes_used==0)
		UCSR0B &= ~_BV(UDRIE0);			// disable tx interrupt
	else
	{
		UDR0=*((char*)uart_fifo_tx.tail_ptr);
		uart_fifo_tx.tail_ptr++;	
		
		if(uart_fifo_tx.tail_ptr == uart_fifo_tx.end)
			uart_fifo_tx.tail_ptr = uart_fifo_tx.start;
		
		uart_fifo_tx.bytes_free ++;
		uart_fifo_tx.bytes_used --;
	};
}

ISR(USART1_UDRE_vect)
{
	if(uart_fifo_tx.bytes_used==0)
		UCSR1B &= ~_BV(UDRIE1);			// disable tx interrupt
	else
	{
		UDR1=*((char*)uart_fifo_tx.tail_ptr);
		uart_fifo_tx.tail_ptr++;
		
		if(uart_fifo_tx.tail_ptr == uart_fifo_tx.end)
			uart_fifo_tx.tail_ptr = uart_fifo_tx.start;
		
		uart_fifo_tx.bytes_free ++;
		uart_fifo_tx.bytes_used --;
	};
}

ISR(USART2_UDRE_vect)
{
	if(uart_fifo_tx.bytes_used==0)
		UCSR2B &= ~_BV(UDRIE2);			// disable tx interrupt
	else
	{
		UDR2=*((char*)uart_fifo_tx.tail_ptr);
		uart_fifo_tx.tail_ptr++;
		
		if(uart_fifo_tx.tail_ptr == uart_fifo_tx.end)
			uart_fifo_tx.tail_ptr = uart_fifo_tx.start;
		
		uart_fifo_tx.bytes_free ++;
		uart_fifo_tx.bytes_used --;
	};
}

ISR(USART3_UDRE_vect)
{
	if(uart_fifo_tx.bytes_used==0)
		UCSR3B &= ~_BV(UDRIE3);			// disable tx interrupt
	else
	{
		UDR3=*((char*)uart_fifo_tx.tail_ptr);
		uart_fifo_tx.tail_ptr++;
		
		if(uart_fifo_tx.tail_ptr == uart_fifo_tx.end)
			uart_fifo_tx.tail_ptr = uart_fifo_tx.start;
		
		uart_fifo_tx.bytes_free ++;
		uart_fifo_tx.bytes_used --;
	};
}