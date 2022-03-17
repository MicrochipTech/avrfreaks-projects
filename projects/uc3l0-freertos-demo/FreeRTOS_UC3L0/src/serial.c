/*****************************************************************************
 *
 * \file
 *
 * \brief FreeRTOS Serial Port management example for AVR32 UC3.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/


/*
  BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR USART.
*/

#include <avr32/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "gpio.h"
#include "usart.h"
#include "sysclk.h"
#include "print_funcs.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Demo application includes. */
#include "serial.h"
#include "conf_usart.h"
#include "board.h"
#include "gloabals.h"

/*-----------------------------------------------------------*/

/* Constants to setup and access the USART. */
#define serINVALID_COMPORT_HANDLER        ( ( xComPortHandle ) 0 )
#define serINVALID_QUEUE                  ( ( xQueueHandle ) 0 )
#define serHANDLE                         ( ( xComPortHandle ) 1 )
#define serNO_BLOCK                       ( ( portTickType ) 0 )

/*-----------------------------------------------------------*/

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
static xQueueHandle xRxedChars;
static xQueueHandle xCharsForTx;

// Main Menu Task Handle
extern xTaskHandle menuTermHandle;
/*-----------------------------------------------------------*/

/* Forward declaration. */
static void vprvSerialCreateQueues( unsigned portBASE_TYPE uxQueueLength,
									xQueueHandle *pxRxedChars,
									xQueueHandle *pxCharsForTx );

/*-----------------------------------------------------------*/

#if __GNUC__
	__attribute__((__noinline__))
#elif __ICCAVR32__
	#pragma optimize = no_inline
#endif

static portBASE_TYPE prvUSART_ISR_NonNakedBehaviour( void )
{
	/* Now we can declare the local variables. */
	unsigned portCHAR     cChar;
	portBASE_TYPE     xHigherPriorityTaskWoken = pdFALSE;
	unsigned portLONG     ulStatus;
	volatile avr32_usart_t  *usart = serialPORT_USART;
	portBASE_TYPE retstatus;

	/* What caused the interrupt? */
	ulStatus = usart->csr & usart->imr;

	if (ulStatus & AVR32_USART_CSR_TXRDY_MASK)
	{
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit?
		Because FreeRTOS is not supposed to run with nested interrupts, put all OS
		calls in a critical section . */
		portENTER_CRITICAL();
			retstatus = xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken );
		portEXIT_CRITICAL();

		if (retstatus == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			 THR now. */
			usart->thr = cChar;
		}
		else
		{
			/* Queue empty, nothing to send so turn off the Tx interrupt. */
			usart->idr = AVR32_USART_IDR_TXRDY_MASK;
		}
	}

	if (ulStatus & AVR32_USART_CSR_RXRDY_MASK)
	{
		/* The interrupt was caused by the receiver getting data. */
		cChar = usart->rhr; //TODO
            
		//TODO: Implement Command history, catch the up or down arrow
		//      wake menu up and send the up or down. 
		//      vMenuHandleHistory outputs the history relative to arrow
		//      once cr is pressed, command is processes as normal. 
		//      menu checks var if it matches output command from history
		//      if match put value in front, if not store new command in front

		/* Because FreeRTOS is not supposed to run with nested interrupts, put all OS
		calls in a critical section . */
		portENTER_CRITICAL();
			//Upon recieving a serialDELIMITER character, wake the Terminal Up
			if (cChar == serialDELIMITER)
			{
				// Queue the CR then send the line feed
				xQueueSendFromISR( xCharsForTx, "\r", &xHigherPriorityTaskWoken);
				usart->thr = 0x0A; // Insert Line Feed
				usart->ier = (1 << AVR32_USART_IER_TXRDY_OFFSET);
				
				//Resume the Terminal Task to process data.
				xTaskResumeFromISR(menuTermHandle);
			}
			// Is char a backspace or delete key?  Test how delete key functions
			else if(cChar == BS || cChar == DEL)
			{
				// pop off last char, but don't do anything with it
				xQueueReceiveFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken);
				usart->thr = 0x1B; //Send esc
				// Queue the rest of the chars
				xQueueSendFromISR( xCharsForTx, "[", &xHigherPriorityTaskWoken);
				xQueueSendFromISR( xCharsForTx, "1", &xHigherPriorityTaskWoken);
				xQueueSendFromISR( xCharsForTx, "D", &xHigherPriorityTaskWoken);
				usart->ier = (1 << AVR32_USART_IER_TXRDY_OFFSET);
				//Send out cursor move left one?
			}
			else
			{
				xQueueSendFromISR(xRxedChars, &cChar, &xHigherPriorityTaskWoken);
				// Echo Char
				#if serialSERIAL_ECHO
				usart->thr = cChar; 
				#endif
			}
			
		portEXIT_CRITICAL();
	}

	/* The return value will be used by portEXIT_SWITCHING_ISR() to know if it
	should perform a vTaskSwitchContext(). */
	return ( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

/*
 * USART interrupt service routine.
 */
#if __GNUC__
	__attribute__((__naked__))
#elif __ICCAVR32__
	#pragma shadow_registers = full   // Naked.
#endif

static void vUSART_ISR( void )
{
	/* This ISR can cause a context switch, so the first statement must be a
	call to the portENTER_SWITCHING_ISR() macro.  This must be BEFORE any
	variable declarations. */
	portENTER_SWITCHING_ISR();

	prvUSART_ISR_NonNakedBehaviour();

	/* Exit the ISR.  If a task was woken by either a character being received
	or transmitted then a context switch will occur. */
	portEXIT_SWITCHING_ISR();
}
/*-----------------------------------------------------------*/


/*
 * Init the serial port for the Minimal implementation.
 */
xComPortHandle xSerialPortInitMinimal( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
    static const gpio_map_t USART_GPIO_MAP =
    {
	    { serialPORT_USART_RX_PIN, serialPORT_USART_RX_FUNCTION },
	    { serialPORT_USART_TX_PIN, serialPORT_USART_TX_FUNCTION }
    };

    xComPortHandle xReturn = serHANDLE;
    volatile avr32_usart_t  *usart = serialPORT_USART;

	// USART options.
	usart_options_t USART_OPTIONS =
	{
		.baudrate     = 57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

	USART_OPTIONS.baudrate = ulWantedBaud;

	/* Create the rx and tx queues. */
	vprvSerialCreateQueues( uxQueueLength, &xRxedChars, &xCharsForTx );

	/* Configure USART. */
	if( ( xRxedChars != serINVALID_QUEUE ) &&
	  ( xCharsForTx != serINVALID_QUEUE ) &&
	  ( ulWantedBaud != ( unsigned portLONG ) 0 ) )
	{
		portENTER_CRITICAL();
		{
			/**
			** Configure USART.
			**/
			/* Enable USART RXD & TXD pins. */
			gpio_enable_module( USART_GPIO_MAP, sizeof( USART_GPIO_MAP ) / sizeof( USART_GPIO_MAP[0] ) );

            sysclk_enable_peripheral_clock(usart);
			// Initialize USART in RS232 mode.
			#if configDBG
			if (usart_init_rs232(usart, &USART_OPTIONS, sysclk_get_peripheral_bus_hz(usart)) == USART_SUCCESS)
			{
				portDBG_TRACE("...USART Initialized Sucessfully!");
			}
			else
			{
				portDBG_TRACE(CL_RED BOLD "!!!--USART FAILED TO init--!!!" NORMAL);
			}
			#else
			usart_init_rs232(usart, &USART_OPTIONS, sysclk_get_peripheral_bus_hz(usart));
			#endif

			/* We're not fully done yet: disable receiver and transmitter. */
			usart->cr |= AVR32_USART_CR_RXDIS_MASK | AVR32_USART_CR_TXDIS_MASK;

			/* Register the USART interrupt handler to the interrupt controller and
			 enable the USART interrupt. */
			INTC_register_interrupt((__int_handler)&vUSART_ISR, serialPORT_USART_IRQ, AVR32_INTC_INT0);

			/* Enable USART interrupt sources (but not Tx for now)... */
			usart->ier = AVR32_USART_IER_RXRDY_MASK;

			/* Enable receiver and transmitter... */
			usart->cr |= AVR32_USART_CR_TXEN_MASK | AVR32_USART_CR_RXEN_MASK;
		}
		portEXIT_CRITICAL();
	}
	else
	{
		xReturn = serINVALID_COMPORT_HANDLER;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, unsigned portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString( char * pcString )
{
    unsigned portCHAR *pxNext;

	/* Send each character in the string, one at a time. */
	pxNext = ( unsigned portCHAR * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar( NULL, *pxNext, serNO_BLOCK );
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, unsigned portCHAR cOutChar, portTickType xBlockTime )
{
volatile avr32_usart_t  *usart = serialPORT_USART;

    (void) pxPort;

	/* Place the character in the queue of characters to be transmitted. */
	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
	{
		return pdFAIL;
	}

	/* Turn on the Tx interrupt so the ISR will remove the character from the
	queue and send it.   This does not need to be in a critical section as
	if the interrupt has already removed the character the next interrupt
	will simply turn off the Tx interrupt again. */
	usart->ier = (1 << AVR32_USART_IER_TXRDY_OFFSET);

	return pdPASS;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
  /* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/

/*###########################################################*/

/*
 * Create the rx and tx queues.
 */
static void vprvSerialCreateQueues(  unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, xQueueHandle *pxCharsForTx )
{
	/* Create the queues used to hold Rx and Tx characters. */
	xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( unsigned portCHAR ) );
	xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( unsigned portCHAR ) );

	/* Pass back a reference to the queues so the serial API file can
	post/receive characters. */
	*pxRxedChars = xRxedChars;
	*pxCharsForTx = xCharsForTx;
}

/*
 *	Prints an integer to the usart
 *	n is the integer to print
 *	s is digits to the left of the decimal, prints a decimal point when s reaches 0
 *	    -1 presents no decimal
 */
void vSerialPutLong( unsigned long n, signed long s )
{
	static char buf[19 + 2];
    char *p = buf + 19 + 1;	/* points to terminating '\0' */
    if (n >= 0) {
        do {
	        if(!s--)
		    {
		        *--p = '.';
		    }
		    else
		    {
             *--p = '0' + (n % 10);
             n /= 10;
		    }		 
        } while (n != 0);
	}	
		
	vSerialPutString( p );
}

/*-----------------------------------------------------------*/

