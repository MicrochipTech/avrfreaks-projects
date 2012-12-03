/*
   ISR_Uart.c

   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System

    *********************************************************************************
	*    >>> See http://www.esyste.com for Hw Documentation <<<						*
	*																				*
	*    Thank you for using GSA, and thank you for your support!					*
	*																				*
    *********************************************************************************
    see header file for details
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "ISR_Uart.h"
#include "Define.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "Modem.h"
#include "Parser.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Global Variables
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Semaphore and queue to syncronyze Tx and Rx with a task
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile	xQueueHandle		UartMessageQueue;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Buffer for RX RS232
char								UART_RxBuf[UART_RX_BUFFER_SIZE];
//message command when TX enabled
volatile	UARTMessage				RxCommandToModem;
//error from ISR RX
volatile	uint8_t					UART_RxError;
//Buffer for TX RS232
char								UART_TxBuf[UART_TX_BUFFER_SIZE];
//message command when RX enabled
volatile	UARTMessage				TxCommandToModem;
//store incoming SMS number
//store phone number from incoming call or SMS received
char								PhoneNrRx[MAX_PHONE_NUMBER_SIZE];
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//reserved  to ISR RX
volatile uint8_t rx_i;
volatile uint8_t rx_ring;
volatile uint8_t rx_err;
volatile uint8_t rx_busy;
volatile uint8_t rx_no_carrier;
volatile uint8_t rx_no_answer;
volatile uint8_t rx_sms;
volatile uint8_t rx_ii;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//reserved  to ISR RX
volatile unsigned char SearchStr;
volatile unsigned char *SearchFor;
volatile unsigned char *SecondSearchFor;
volatile unsigned char SecondSearchStr;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Index to find string in SearchStrings
// FOLLOW THE INDEX IN SearchStrings DEFINITION
#define OK_             0                           //Used to look up Response
#define CMTI_           1                           //Used to look up Response
#define READY_          2                           //Used to look up Response
#define CRLF_           3                           //Used to look up Response
#define END_RING_       4                           //Used to look up Response
#define BUSY_           5                           //Used to look up Response
#define NCARRIER_       6                           //Used to look up Response
#define CTRLCALL_       7                           //Used to look up Response
#define NANSWER_        8                           //Used to look up Response
#define NO_DEFINED_     9
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Telit Modem Response in program space for IN SearchStrings
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const unsigned char _OK[]						PROGMEM = "OK\r\n";			//!< "OK"
const unsigned char _CMTI[]						PROGMEM = "+CMTI: ";		//!< New Message arrived
const unsigned char _READY[]					PROGMEM = "> ";				//!< Phone ready to receive text message
const unsigned char _CR_LF[]					PROGMEM = "\r\n";			//!< Carrige Return Line Feed
const unsigned char _RING[]						PROGMEM = "RING";			//!< Ring detection
const unsigned char _END_RING[]					PROGMEM = ",0\r\n";			//!< Carrige Return Line Feed
const unsigned char _ERROR[]					PROGMEM = "ERROR";			//!< Error
const unsigned char _BUSY[]						PROGMEM = "BUSY";			//!< connect controls
const unsigned char _NCARRIER[]					PROGMEM = "NO CARRIER";		//!< connect controls
const unsigned char _NANSWER[]					PROGMEM = "NO ANSWER";		//!< connect controls
const unsigned char _CTRLCALL[]					PROGMEM = "CPAS";			//!< connect controls
const unsigned char _NODEFINED[]				PROGMEM = {0xFF,0};			//!< connect controls
const unsigned char * const SearchStrings[]		PROGMEM = {	_OK,
															_CMTI,
															_READY,
															_CR_LF,
															_END_RING,
															_BUSY,
															_NCARRIER,
															_CTRLCALL,
															_NANSWER,
															_NODEFINED
														};		//!< Initialized data
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*************************************************************************
Function: uart_init()
Purpose:  initialize UART and set baudrate
Input:    baudrate using macro UART_BAUD_SELECT()
Returns:  none
**************************************************************************/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void xUartInit( unsigned int baudrate )

{

	//Initialize structure
	TxCommandToModem.MessageID		=	RxCommandToModem.MessageID	=	NO_CMD_ID;
	TxCommandToModem.UART_Index		=	RxCommandToModem.UART_Index	=	0;

	//only 1 buffer is used for memory space reason
	TxCommandToModem.ptrUART_Buf	=	UART_TxBuf;
	RxCommandToModem.ptrUART_Buf	=	UART_RxBuf;

	rx_ii = rx_ring = rx_err = rx_busy = rx_no_carrier = rx_no_answer = 0;

	SecondSearchFor  = (unsigned char *)pgm_read_word(&(SearchStrings[ NO_DEFINED_ ]));


//	portENTER_CRITICAL(); //---------------------------------------------------- NO NEED. ONLY THIS TASK MANAGES THESE PORT
//	{
	//Telit has inverted signals
	PORTD |= _BV(O_GsmRTS);  // to 1

	UCSR0A = 0;

	/* Set baud rate */
	UBRR0H = (unsigned char)(baudrate>>8);
	UBRR0L = (unsigned char) baudrate;

	/* Set frame format: asynchronous, 8data, no parity, 2 stop bit */
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01) | _BV(USBS0);

	/* Enable USART receiver and transmitter interrupt*/
	UCSR0B = (_BV(RXCIE0) | _BV(TXCIE0));

	/* Enable USART transmit*/
	UCSR0B |= _BV(TXEN0);

	/* Disable USART receive*/
    UCSR0B &= ~_BV(RXEN0);

//	}
//	portEXIT_CRITICAL();

	//Create UART message queue
	/* 5 items:1 for Tx (task only send a buffer and wait for answer)
	 * maximum we can have one message for answer, one for incoming call and one for SMS.
	 * Total 4 item so we set 5 for one message more
	 */
	UartMessageQueue = xQueueCreate( QUEUE_LENGTH, sizeof( UARTMessage ) );

	/* Unlike other ports, this serial code does not allow for more than one
	com port.  We therefore don't return a pointer to a port structure and can
	instead just no return. */
	return;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/************************************************************************************************************************************
Function: UartStartTX()
Purpose:  start transmission data from buffer to UART.
               Data MUST be prepared before. This function only starts interrupt
Input:    none
Returns:  none
*************************************************************************************************************************************/
portBASE_TYPE UartStartTX(  void )
{
//	portENTER_CRITICAL(); // NO NEED. ONLY THIS TASK MANAGES THESE PORT
//	{
		//reset interrupt
		cli();
		//Enable Tx
		UCSR0B |=  _BV(TXEN0);
		//Reset Rx buffer data empty
		UCSR0A |= _BV(UDRE0);
		//enable Rx interrupt
		UCSR0B |=  _BV(RXEN0);
		//enable Tx interrupt
		UCSR0B  |= _BV(UDRIE0);
		//set interrupt
		sei();
//	}
//	portEXIT_CRITICAL();

	return (portBASE_TYPE)NULL;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*************************************************************************
Function: UART Tx Complete
Purpose:  called when the UART has trasmitted a byte
**************************************************************************/
ISR(USART0_TX_vect)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	/*
	this interrupt is NEEDED because after sent last byte we need to WAIT for END trasmission before set RTS
	otherwise Telit modem loose last char.
	check if there is task with higher priority
	*/
	if( !TxCommandToModem.ptrUART_Buf[TxCommandToModem.UART_Index] )  // END STRING...... NOT ALLOWED To send binary chars
	{
		//Disable Tx
		UCSR0B &= ~_BV(TXEN0);

		//end RTS
		PORTD |= _BV(O_GsmRTS);  // to 1

		//insert message in to queue
		xQueueSendFromISR( 	UartMessageQueue,
							&TxCommandToModem,
							&xHigherPriorityTaskWoken );

		TxCommandToModem.UART_Index	=	0;

		if( xHigherPriorityTaskWoken != pdFALSE )
		{
			taskYIELD();
		}
	}

}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
ISR(USART0_UDRE_vect)
{

	//Set Request to send for telit
	PORTD &= ~_BV(O_GsmRTS);  // to 0

	//wait if CTS is high. If not High Telit is not able to receive data
	if( PIND & _BV(I_GsmCTS) )
		return;

	if( TxCommandToModem.ptrUART_Buf[TxCommandToModem.UART_Index] ) //till end string
	{
		UDR0 = TxCommandToModem.ptrUART_Buf[ TxCommandToModem.UART_Index++ ];  /* start next byte */
	}
	else
	{
		/* tx buffer empty, disable UDRE interrupt */
		UCSR0B &= ~_BV(UDRIE0);
	}
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*************************************************************************
Function: UART Receive Complete interrupt
Purpose:  called when the UART has received a character
**************************************************************************/
ISR(USART0_RX_vect)
 {
	signed char data, usr;
	int  i,	j;

	signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Get the character and post it  the buffer.
	If the post causes a task to wake force a context switch as the woken task
	may have a higher priority than the task we have interrupted. */

	// read UART status register and UART data register
	data	= UDR0;
	usr		= UCSR0A;
	UCSR0A	= 0;

	if( usr & (_BV(FE0) | _BV(DOR0) | _BV(UPE0) ) )
	{
		//flush data: string is not more valid
		RxCommandToModem.UART_Index	= 0;
		UART_RxError				= UART_RX_HW_ERROR;
		RxCommandToModem.MessageID	= SERIAL_ERROR_ID;

		//insert message in to queue
		xQueueSendFromISR( 	UartMessageQueue,
							&RxCommandToModem,
							&xHigherPriorityTaskWoken );

		if( xHigherPriorityTaskWoken != pdFALSE )
		{
			taskYIELD();
		}
		return;
	}

    // calculate buffer index
    if( RxCommandToModem.UART_Index >= UART_RX_BUFFER_SIZE)
	{
        // error: receive buffer overflow
        UART_RxError 				= UART_BUFFER_OVERFLOW;
		RxCommandToModem.UART_Index	= 0;
		RxCommandToModem.MessageID = SERIAL_ERROR_ID;

		//insert message in to queue
		xQueueSendFromISR( 	UartMessageQueue,
							&RxCommandToModem,
							&xHigherPriorityTaskWoken );

		if( xHigherPriorityTaskWoken != pdFALSE )
		{
			taskYIELD();
		}
		return;
	}

	// store received data in buffer
	RxCommandToModem.ptrUART_Buf[ RxCommandToModem.UART_Index++] = data;

	//detects end command from GSM (set by SetResponse )
	//(what really we want to expect)
	//This is only when we send a command and expect the answer.
	//--------------------------------------------------------
	if( pgm_read_byte(&(SearchFor[rx_i++])) == data )
	{
		if( !pgm_read_byte(&(SearchFor[rx_i])))
		{
			rx_i    = 0;

			//Set new message is ready after a command sent
			RxCommandToModem.MessageID = DATA_READY_ID;

			//set end string
			RxCommandToModem.ptrUART_Buf[ RxCommandToModem.UART_Index ] = 0;

			//insert message in to queue
			xQueueSendFromISR( 	UartMessageQueue,
								&RxCommandToModem,
								&xHigherPriorityTaskWoken );
			//reset index
			RxCommandToModem.UART_Index	= 0;

			if( xHigherPriorityTaskWoken != pdFALSE )
			{
				taskYIELD();
			}
			return;
		}
	}
	else
		rx_i    = 0;  //Not valid search pattern...start again.
	//-------------------------------------------------------- end

	//detects ERROR answer
	//--------------------------------------------------------
	if( pgm_read_byte(&(_ERROR[rx_err++])) == data )
	{
		if( !pgm_read_byte(&(_ERROR[rx_err])))
		{
			rx_err           = 0;
			SearchFor  = (unsigned char *)pgm_read_word(&(SearchStrings[ CRLF_ ])); //Wait for
			SearchStr  = ERR_;
			rx_i       = 0;  //start again wait for CR_LF
		}
	}
	else
		rx_err    = 0;
	//END detects ERROR answer
	//--------------------------------------------------------

	//---------------------------------------------------------------------------------------------------
	//second search for incoming strings outside TxRxCommand
	//---------------------------------------------------------------------------------------------------
	if( pgm_read_byte(&(SecondSearchFor[rx_ii++])) == data )
	{
		if( !pgm_read_byte(&(SecondSearchFor[rx_ii])))
		{
			rx_ii  = 0;
			//reset checking string
			SecondSearchFor  = (unsigned char *)pgm_read_word(&(SearchStrings[ NO_DEFINED_ ]));
			//Set new message flag
			switch( SecondSearchStr )
			{
				//---------------------------------------------------------------------------------------------------
				case END_RING_:
					//Read NumberPhone
					// '\"' start Num Tel
					j = StrIndex( RxCommandToModem.ptrUART_Buf , '\"'); //1 for first
					// '\"' end Num Tel
					i = StrIndex( &RxCommandToModem.ptrUART_Buf[ j ], '\"'); //1 for first
					//Prepare phone numbers
					RxCommandToModem.ptrUART_Buf[ j + i - 1 ] = 0; //cut the string
					//Phone
					strcpy( PhoneNrRx, &RxCommandToModem.ptrUART_Buf[ j ] );

					//Set incoming call
					RxCommandToModem.MessageID = INCOMING_CALL_ID;
					RxCommandToModem.UART_Index  = 0;

					//insert message in to queue
					xQueueSendFromISR( 	UartMessageQueue,
										&RxCommandToModem,
										&xHigherPriorityTaskWoken );
					//reset index
					RxCommandToModem.UART_Index	= 0;

					if( xHigherPriorityTaskWoken != pdFALSE )
					{
						taskYIELD();
					}
					return;

				//---------------------------------------------------------------------------------------------------
				case CMTI_:
					//Set incoming sms
					RxCommandToModem.MessageID = INCOMING_SMS_ID;
					//SMS number
					RxCommandToModem.UART_Index = DecodeCMTI( RxCommandToModem.ptrUART_Buf );    //Find SMS index
					//
					//insert message in to queue
					xQueueSendFromISR( 	UartMessageQueue,
										&RxCommandToModem,
										&xHigherPriorityTaskWoken );
					//reset index
					RxCommandToModem.UART_Index	= 0;

					if( xHigherPriorityTaskWoken != pdFALSE )
					{
						taskYIELD();
					}
					return;

				//---------------------------------------------------------------------------------------------------
				case BUSY_:
					//Set Busy command received
					RxCommandToModem.MessageID = BUSY_ID;
					RxCommandToModem.UART_Index  = 0;

					//insert message in to queue
					xQueueSendFromISR( 	UartMessageQueue,
										&RxCommandToModem,
										&xHigherPriorityTaskWoken );
					//reset index
					RxCommandToModem.UART_Index	= 0;

					if( xHigherPriorityTaskWoken != pdFALSE )
					{
						taskYIELD();
					}
					return;

				//---------------------------------------------------------------------------------------------------
				case NCARRIER_:
					//Set no carrier command received
					RxCommandToModem.MessageID = NCARRIER_ID;
					RxCommandToModem.UART_Index  = 0;

					//insert message in to queue
					xQueueSendFromISR( 	UartMessageQueue,
										&RxCommandToModem,
										&xHigherPriorityTaskWoken );
					//reset index
					RxCommandToModem.UART_Index	= 0;

					if( xHigherPriorityTaskWoken != pdFALSE )
					{
						taskYIELD();
					}
					return;

				//---------------------------------------------------------------------------------------------------
				case NANSWER_:
					//Set no answer command received
					RxCommandToModem.MessageID = NANSWER_ID;
					RxCommandToModem.UART_Index  = 0;

					//insert message in to queue
					xQueueSendFromISR( 	UartMessageQueue,
										&RxCommandToModem,
										&xHigherPriorityTaskWoken );
					//reset index
					RxCommandToModem.UART_Index	= 0;

					if( xHigherPriorityTaskWoken != pdFALSE )
					{
						taskYIELD();
					}
					return;

				//---------------------------------------------------------------------------------------------------
				default:
					break;
			}
		}
	}
	else
		rx_ii   = 0;  //Not valid search pattern...start again.
	//--------------------------------------------------------------------------------------------------- end

	//--------------------------------------------------------
	//detects incoming RING
	//--------------------------------------------------------
	if( pgm_read_byte(&(_RING[rx_ring++])) == data )
	{
		if( !pgm_read_byte(&(_RING[rx_ring])))
		{
			rx_ring          = 0;
			SecondSearchFor  = (unsigned char *)pgm_read_word(&(SearchStrings[ END_RING_ ])); //Wait for
			SecondSearchStr  = END_RING_;
			rx_ii            = 0;  //start again wait for END_RING
		}
	}
	else
		rx_ring    = 0;
	// end
	//--------------------------------------------------------------------------------------------------- end

	//--------------------------------------------------------
	//detects DIALLING answer : BUSY
	//--------------------------------------------------------
	if( pgm_read_byte(&(_BUSY[rx_busy++])) == data )
	{
		if( !pgm_read_byte(&(_BUSY[rx_busy])))
		{
			rx_busy          = 0;
			SecondSearchFor  = (unsigned char *)pgm_read_word(&(SearchStrings[ CRLF_ ])); //Wait for
			SecondSearchStr  = BUSY_;
			rx_ii            = 0;  //start again wait for CR_LF
		}
	}
	else
		rx_busy    = 0;
	// end
	//--------------------------------------------------------
	//detects DIALLING answer : NO CARRIER
	//--------------------------------------------------------
	if( pgm_read_byte(&(_NCARRIER[rx_no_carrier++])) == data )
	{
		if( !pgm_read_byte(&(_NCARRIER[rx_no_carrier])))
		{
			rx_no_carrier    = 0;
			SecondSearchFor  = (unsigned char *)pgm_read_word(&(SearchStrings[ CRLF_ ])); //Wait for
			SecondSearchStr  = NCARRIER_;
			rx_ii            = 0;  //start again wait for CR_LF
		}
	}
	else
		rx_no_carrier = 0;
	//--------------------------------------------------------------------------------------------------- end

	//--------------------------------------------------------
	//detects DIALLING answer : NO ANSWER
	//--------------------------------------------------------
	if( pgm_read_byte(&(_NANSWER[rx_no_answer++])) == data )
	{
		if( !pgm_read_byte(&(_NANSWER[rx_no_answer])))
		{
			rx_no_answer     = 0;
			SecondSearchFor  = (unsigned char *)pgm_read_word(&(SearchStrings[ CRLF_ ])); //Wait for
			SecondSearchStr  = NANSWER_;
			rx_ii            = 0;  //start again wait for CR_LF
		}
	}
	else
		rx_no_answer    = 0;
	//--------------------------------------------------------------------------------------------------- end

	//--------------------------------------------------------
	//detects Incoming SMS
	//--------------------------------------------------------
	if( pgm_read_byte(&(_CMTI[rx_sms++])) == data )
	{
		if( !pgm_read_byte(&(_CMTI[rx_sms])))
		{
			rx_sms           = 0;
			SecondSearchFor  = (unsigned char *)pgm_read_word(&(SearchStrings[ CRLF_ ])); //Wait for
			SecondSearchStr  = CMTI_;
			rx_ii            = 0;  //start again wait for CR_LF
		}
	}
	else
		rx_sms    = 0;
	//--------------------------------------------------------------------------------------------------- end

}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
