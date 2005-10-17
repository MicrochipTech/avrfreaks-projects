/// uart.c ///

#include "main.h"
#include "uart.h"
#include "timers.h"

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

word uart_buffersize;
char *uart_startpointer;

SIGNAL (SIG_UART_RECV)
{	byte u_data;
	
	u_data=UDR;
	*(ringpointer++)=u_data;
	if (++receive_counter>=uart_buffersize)
	{	ringpointer=uart_startpointer;
		receive_counter=0;
	}
}



SIGNAL (SIG_UART_TRANS)
{	if (transmit_counter<transmit_size)
	{	UDR=*(ringpointer++);
		transmit_counter++;
	}	
	else 
		uart_complete=1;
}



byte  uart_receive_init (byte interrupt, byte doublestop,char* start,word buffersize)
{   byte regset;
	
	outp (MCU_CLK/(UART_BAUDRATE*16) - 1,UBRRL);		//set baudrate
	
	DDRD&=0xF7;
	PORTD|=0x01;		//switch port to input w. pullup (valid for MEGA8)
	regset=UART_RX_ENABLE;
	if (interrupt)
		regset|=UART_RX_INT_ENABLE;
	outp (regset,UCSRB);
	regset=0x86;			//8bit data
	if (doublestop)
		regset|=UART_2STOPBITS;
	outp (regset,UCSRC);			//(8bit data)
	outp (0,UBRRH);
	uart_startpointer=start;
	ringpointer=uart_startpointer;
	receive_counter=0;
	uart_buffersize=buffersize;
	return 0;
}



byte  uart_transmit_init (byte interrupt,byte doublestop)
{   byte regset;
	
	DDRD|=0x02;
	PORTD|=0x02;		//switch port to output (valid for MEGA8)
	regset=UART_TX_ENABLE;
	outp (MCU_CLK/(UART_BAUDRATE*16) - 1,UBRRL);		//set baudrate
	if (interrupt)
		regset|=UART_TX_INT_ENABLE;
	outp (regset,UCSRB);
	regset=0x86;			//8bit data
	if (doublestop)
		regset|=UART_2STOPBITS;
	outp (regset,UCSRC);			//(8bit data)
	outp (0,UBRRH);
	
	return 0;
}

/*

int uart_receive_byte (signed char* data)
{  volatile byte status,error;
	
	do 
	{  status=inp (UCSRA);
	}	while ((!(status&(UART_RX_COMPLETE|UART_FRAME_ERROR|UART_OVERRUN))));
	if (!error)
		error=  status&(UART_FRAME_ERROR|UART_OVERRUN);
	*data= UDR;
	return error;
	
}

*/


byte uart_transmit (byte *data,word size)
{	volatile byte status;
	byte retval;
	if ((transmit_counter<transmit_size)||(size==0))
		retval=0;
	else
	{	do
		{	status = UCSRA;
		}	while (!(status&UART_DR_EMPTY));
		UDR=*data;
		transmit_counter=1;
		transmit_size=size;
		ringpointer=data+1;
		retval=1;
		uart_complete=0;
	}
	return retval;
}



