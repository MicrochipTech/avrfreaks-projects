/*________________________________________________________________________________________*\

  File:   periph.c

  This module comprises low-level hardware-dependent I/O interface functions.
\*________________________________________________________________________________________*/

#include  "system.h"
#include  "periph.h"
#include  "cli.h"


/*________________________________________________________________________________________*\

    MCU device initialisation and on-chip peripheral driver functions
\*________________________________________________________________________________________*/

static  uint32  ulClockTicks;     // General-purpose "tick" counter


void  initMCUports( void )
{
//	MCUCR = 0x80;             // Enable the external memory i/face (if used)
	DDRA  = 0xFF;             // TEMP: Port A pins are outputs (debug usage)
	DDRC  = 0xFF;             // TEMP: Port C pins are outputs (debug usage)
	DDRD |= BIT_5;            // TEMP: PD5 is "heartbeat" LED output
	PORTD &= ~BIT_5;          // TEMP: PD5 = 0 -- LED initially ON
}


/*
*   MCU timer/counter configuration --
*   Timer/counter #1 is set up to generate a 5mSec periodic "tick" interrupt.
*   MCU clock frequency is defined by CLOCK_FREQ (Hz) in system.h.
*   Acceptable values are 8000000 (8MHz) or 16000000 (16MHz).
*/
void  initMCUtimers( void )
{
//	WDTCR = 0x0C;                       // TODO: Enable watchdog timer; Timeout = 260mSec
	TCCR1A = 0x40;                      // Mode = CTC; Toggle OC1A pin on TC
	TCCR1B = 0x0A;                      // Mode = CTC; Prescale f/8 (Tc=1us @ 8MHz)
#if  (CLOCK_FREQ == 8000000)
	OCR1AH = HI_BYTE( 5000 );           // Load TOP register for 5mSec Top count
	OCR1AL = LO_BYTE( 5000 );
#elif (CLOCK_FREQ == 16000000)
	OCR1AH = HI_BYTE( 2500 );           // Load TOP register for 5mSec Top count
	OCR1AL = LO_BYTE( 2500 );
#elif (CLOCK_FREQ == 7372800)
	OCR1AH = HI_BYTE( 5425 );           // Load TOP register for 5mSec Top count
	OCR1AL = LO_BYTE( 5425 );
#else
#pragma message ("CLOCK_FREQ definition is invalid")
#endif
	DDRB  |= BIT_5;                     // TEMP: Enable output pin PB5/OC1A (debug usage)
	ENABLE_TICK_TIMER;                  // Interrupt on Timer1 output compare
}


/*
*   MCU Timer/Counter1 interrupt service routine --
*   Tick Handler, periodic task scheduler.
*/
_INTERRUPT_ void  isrMCU_Timer1_compA( void )
{
	static  uint8  b500msecTimer = 0;
	static  uint8  b50mSecTimer = 0;

	ulClockTicks++;
	b5msecTaskReq = 1;
	if ( ++b50mSecTimer >= 10 )
	{
		b50mSecTaskReq = 1;
		b50mSecTimer = 0;
	}
	if ( ++b500msecTimer >= 100 )
	{
		b500msecTaskReq = 1;
		b500msecTimer = 0;
	}
}


/*
*   Return the value of ulClockTicks, which is incremented every 5mSec tick.
*   For use as a general-purpose timer by functions outside of the Timer ISR.
*/
uint32  getTickCount( void )
{
	uint32  ulTemp;

	DISABLE_TICK_TIMER;
	ulTemp = ulClockTicks;
	ENABLE_TICK_TIMER;

	return  ulTemp;
}


/*________________________________________________________________________________________*\

    USART0 driver functions for Debug port / Command Line user Interface (CLI)
\*________________________________________________________________________________________*/
/*
*   Initialise MCU UART (USART0) for the Debug Port / CLI.
*   When global interrupts are enabled, this will also enable the UART Rx interrupt.
*   Called from main() before using service port CLI.
*   CLOCK_FREQ is defined in system.h
*/
void  init_UART0( void )
{
	UBRR0H = (uchar) HI_BYTE( CLOCK_FREQ/(CLI_BAUDRATE*16L) -1 );
	UBRR0L = (uchar) LO_BYTE( CLOCK_FREQ/(CLI_BAUDRATE*16L) -1 );
	UCSR0C = (3<<UCSZ00);    // Set frame format: 8,N,1 (data,parity,stop)
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);   // Enable Receiver and Transmitter

	CLI_RxBufferReset();           // Reset the serial input (RX) buffer
}


/*****
*	UART0 receiver interrupt control.
*	Masks or unmasks RX INT without affecting global interrupt status.
*
*	Args:	bool yIRQmask = {0 | 1};  0 => RX INT is masked (disabled)
*/
void  UART0_RX_IRQctrl( bool yIRQmask )
{
	if ( yIRQmask ) UCSR0B |= (1<<RXCIE0);   // RX INT enabled
	else  UCSR0B &= ~(1<<RXCIE0);            // RX INT disabled
}


/*****
*	UART0 receiver interrupt service routine.
*	The IRQ signals that one or more bytes have been received by the UART;
*	the byte(s) must be read out of the UART RX data register(s) and stored in the
*	RX buffer in RAM (circular FIFO) by a call to CLI_RxBufferStore().
*/
// #pragma  vector = USART0_RXC_vect
__interrupt void  uart0_isr( void )
{
    if ( UCSR0A & (1<<RXC0) )    // Data ready in UART RX register
    {
        CLI_RxBufferStore( UDR0 );	 // move RX byte to FIFO buffer
    }
    else
    {
    	// Clear source of unexpected IRQ
    }
}


/*****
*	UART Transmitter Ready check.
*	Returns TRUE if the UART is ready to accept a new data byte to be transmitted;
*	otherwise it returns FALSE (0).
*/
bool  UART0_TX_Ready( void )
{
	return ( UCSR0A & (1<<UDRE0) );   // TX register empty
}


/*****
*	UART Transmit Data.
*	Writes a data byte into the UART TX Data register to be transmitted.
*	It is assumed that the Transmitter is ready to accept the data,
*	as determined by a prior call to the function UART_TX_Ready().
*/
void  UART0_TX_byteOut( uchar bTXdata )
{
	UDR0 = bTXdata;
}


// end
