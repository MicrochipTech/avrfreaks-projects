//----------------------------------------------------------------------------------
//
// UART Library
//
// History:
// 2003-07-06	BL	- Created
//
//----------------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include "uart.h"

unsigned char	RxBuffer[UART_RX_BUFFER_SIZE];
unsigned int	RxHead = 0;
unsigned int	RxTail = 0;
unsigned int	RxOverflow = 0;

unsigned char	TxBuffer[UART_TX_BUFFER_SIZE];
unsigned int	TxHead = 0;
unsigned int	TxTail = 0;
unsigned int	TxOverflow = 0;


// ------------------------------------------------------------------
//	Quick and dirty Delay (Chris Efstathiou)
// ------------------------------------------------------------------
static void Delay(unsigned long us)
{
 	// 6 cpu cycles per loop + 16 cycles overhead

 	__asm__ volatile (
 		"L_%=: \n\t" \
 		"subi %A0,lo8(-(-1)) \n\t" \
 		"sbci %B0,hi8(-(-1)) \n\t" \
 		"sbci %C0,hlo8(-(-1)) \n\t" \
 		"sbci %D0,hhi8(-(-1)) \n\t" \
 		"brne L_%= \n\t" \
 		: /* NO OUTPUT */ \
 		: "r" (us) \
 	); \

	return;
}

// ------------------------------------------------------------------
//	UART0 initialisation
// ------------------------------------------------------------------
void UartInit()
{
	UCSRB = 0x00; 							// disable while setting baud rate
	UCSRA = 0x00;
	UBRRH = (unsigned char)(UART_BAUD_SELECT>>8); 	// set baud rate hi
	UBRRL = (unsigned char)UART_BAUD_SELECT; 		// set baud rate lo
	UCSRB = BV(TXEN)|BV(RXEN)|BV(RXCIE)|BV(TXCIE);	// enable transmit, receive, receive complete and transmit complete interrupts
	UCSRC = BV(URSEL)|BV(UCSZ1)|BV(UCSZ0);	// 8-bit characters
}

// ------------------------------------------------------------------
//  Receive Complete Interrupt
// ------------------------------------------------------------------
SIGNAL(SIG_UART_RECV)
{

if ( (RxTail+1==RxHead) || ((RxTail==UART_RX_BUFFER_SIZE-1) && (RxHead==0)) )
	{
		char x = UDR;
		x = 0;
		RxOverflow = 1;
	}
	else
	{
		RxBuffer[RxTail] = UDR;	// read the character
		RxTail++;
		if (RxTail == UART_RX_BUFFER_SIZE)
		{
			RxTail = 0;
		}
	}
}

// ------------------------------------------------------------------
//  Data Register Empty Interrupt
// ------------------------------------------------------------------
SIGNAL(SIG_UART_DATA)
{
	if (TxHead == TxTail)
	{
		cbi(UCSRB, UDRIE);				// nothing to transmit, disable this interrupt
	}
	else
	{
	sbi(PORTD, 3);
	sbi(PORTD, 7);

		UDR = TxBuffer[TxHead];
		TxHead++;
		if (TxHead == UART_TX_BUFFER_SIZE)
		{
			TxHead = 0;
		}
	}
}

// ------------------------------------------------------------------
//	Transmit Complete Interrupt
// ------------------------------------------------------------------
SIGNAL(SIG_UART_TRANS)
{
	cbi(PORTD, 3);
	cbi(PORTD, 7);
}

// ------------------------------------------------------------------
//	Is there space to add another character to tx buffer
// ------------------------------------------------------------------
inline unsigned char IsTxSpaceAvailable(void)
{
	char c;
	cli();
	c = !( (TxTail+1==TxHead) || ((TxTail==UART_TX_BUFFER_SIZE-1) && (TxHead==0)) );
	sei();
	return c;
}

// ------------------------------------------------------------------
//	Is a character in the receive buffer?
// ------------------------------------------------------------------
inline unsigned char IsCharAvailable(void)
{
	return (RxHead != RxTail);
}

// ------------------------------------------------------------------
//	Is line of input available (search for \r\n);
// ------------------------------------------------------------------
unsigned char IsLineAvailable(void)
{
	unsigned int c0;
	unsigned int c1;
	char found;

	found = 0;

	c0 = RxHead;
	c1 = (c0+1) % UART_RX_BUFFER_SIZE;
	while ( (c0 != RxTail) && (c1 != RxTail) )
	{
		if( RxBuffer[c0]=='\r' && RxBuffer[c1]=='\n' )
		{
			found = 1;
			break;
		}

		c0 = (c0+1) % UART_RX_BUFFER_SIZE;
		c1 = (c0+1) % UART_RX_BUFFER_SIZE;
	}

	return( found );
}

// ------------------------------------------------------------------
//	How many characters are waiting in the receive buffer?
// ------------------------------------------------------------------
unsigned char NumRxCharAvailable(void)
{
	if (RxHead == RxTail)
	{
		return( 0 );
	}
	else if (RxTail > RxHead)
	{
		return( RxTail - RxHead);
	}
	else
	{
		return( (UART_RX_BUFFER_SIZE-RxHead) + (TxTail-1) );
	}
}

// ------------------------------------------------------------------
//	Retrieve a character from the UART buffer
//  If there is no data in the receive buffer, this function
//  blocks until one arrives. Use IsCharAvailable() to check
//	for waiting data
// ------------------------------------------------------------------
unsigned char RxChar(void)
{
	unsigned char c;

	while (!IsCharAvailable())
		Delay(100);

	c = RxBuffer[RxHead];

	cli();
	RxHead++;
	if (RxHead == UART_RX_BUFFER_SIZE)
	{
		RxHead = 0;
	}
	sei();

	return c;
}

// ------------------------------------------------------------------
//	Read characters until we get a newline (\r\n);
//  This method blocks until a newline is received. Use
//  IsLineAvailable() to determine if a full line
//	already exists in the buffer.
// 	Returns number of characters read not including the \r\n
//	or the terminating NULL.
// ------------------------------------------------------------------
unsigned char RxLine(char *buffer, unsigned char size)
{
	unsigned char numchars = 0;
	char found_newline = 0;

	// Stop when we fill the buffer (leave room for trailing
	// NULL) or when newline found.
	while ( (numchars<(size-1)) && (!found_newline) )
	{
		buffer[numchars] = RxChar();

		if ( buffer[numchars] == '\n' )
		{
			if ( numchars>0 )
			{
				if ( buffer[numchars-1] == '\r' )
				{
					found_newline = 1;
					numchars -= 2;
				}
			}
		}

		numchars++;
	}

	buffer[numchars] = '\0';

	return( numchars );
}

// ------------------------------------------------------------------
//  Write a byte to the UART buffer
// ------------------------------------------------------------------
void TxChar(unsigned char c)
{
	while (!IsTxSpaceAvailable())
		Delay(100);

	cli();
	TxBuffer[TxTail] = c;	// add character to transmit buffer
	TxTail++;
	if (TxTail == UART_TX_BUFFER_SIZE)
	{
		TxTail = 0;
	}
	sei();
	sbi(UCSRB, UDRIE);					// enable transmit interrupt
}


// ------------------------------------------------------------------
//	Write a line to the UART (includes a trailing newline)
//	Newlines ('\n') are translated into CRLF ('\r\n')
// ------------------------------------------------------------------
void TxLine(unsigned char buf[])
{
	TxString(buf);
	TxChar('\r');
	TxChar('\n');
}

// ------------------------------------------------------------------
//	Write a string to the UART
//	Newlines ('\n') are translated into CRLF ('\r\n')
// ------------------------------------------------------------------
void TxString(unsigned char buf[])
{
	while (buf[0] != 0)
	{
		if (buf[0] == '\n')
			TxChar('\r');
		TxChar(buf[0]);
		buf++;
	}
}

// ------------------------------------------------------------------
//	Write an integer to the UART
// ------------------------------------------------------------------
void TxUByte(unsigned char value, char radix)
{
	unsigned char uart_data[3]={'0','0','0'};
	unsigned char position=sizeof(uart_data);

    /* convert long to ascii  */
    do { position--; *(uart_data+position)=(value%radix)+'0'; value/=radix;  } while(value);

	/* start displaying the number */
   	for(;position<=(sizeof(uart_data)-1);position++)
   	{
		if (uart_data[position] > '9')
			uart_data[position] = uart_data[position] - '9' + 'A' - 1;
       TxChar(uart_data[position]);
	}

}

// ------------------------------------------------------------------
//	Write an integer to the UART
// ------------------------------------------------------------------
void TxUInt(unsigned int value, char radix)
{
	unsigned char uart_data[5]={'0','0','0','0','0' };
	unsigned char position=sizeof(uart_data);

    /* convert int to ascii  */
    do { position--; *(uart_data+position)=(value%radix)+'0'; value/=radix;  } while(value);

   /* start displaying the number */
   for(;position<=(sizeof(uart_data)-1);position++)
     {
		if (uart_data[position] > '9')
			uart_data[position] = uart_data[position] - '9' + 'A' - 1;
       TxChar(uart_data[position]);
     }

}

// ------------------------------------------------------------------
//	Write a long to the UART
// ------------------------------------------------------------------
void TxULong(unsigned long value, char radix)
{
	unsigned char uart_data[10]={'0','0','0','0','0','0', '0', '0', '0', '0' };
	unsigned char position=sizeof(uart_data);

	/* convert long to ascii  */
	do { position--; *(uart_data+position)=(value%radix)+'0'; value/=radix;  } while(value);

	/* start displaying the number */
	for(;position<=(sizeof(uart_data)-1);position++)
	{
		if (uart_data[position] > '9')
			uart_data[position] = uart_data[position] - '9' + 'A' - 1;
		TxChar(uart_data[position]);
	}

}
/*
void TxFloat(float value, unsigned char radix, unsigned char precision, unsigned char decimals)
{
	unsigned char uart_data[precision];
	unsigned char position=sizeof(uart_data);
	unsigned char c;

	for (c=0;c<sizeof(uart_data);c++)
		uart_data[c] = '0';

	value = value * 100;

	do {
		position--;
		if (position == sizeof(uart_data)-(decimals+1))
		{
			*(uart_data+position) = '.';
			position--;
		}
		*(uart_data+position)=(((int)value)%radix)+'0';

		if (*(uart_data+position) > '9')
			*(uart_data+position) += - '9' + 'A' - 1;

		value/=radix;

	} while(position>=1);

	// start displaying the number
	for(;position<=(sizeof(uart_data)-1);position++)
	{
		TxChar(uart_data[position]);
	}

}
*/

// ------------------------------------------------------------------
//	Clear the buffers, used when an overflow occurs
// ------------------------------------------------------------------
void UartClearBuffers()
{
	RxHead = 0;
	RxTail = 0;
	TxHead = 0;
	TxTail = 0;
}
