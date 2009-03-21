/*
	FreeRTOS.org V5.2.0 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section 
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * SAVE TIME AND MONEY!  We can port FreeRTOS.org to your own hardware,    *
    * and even write all or part of your application on your behalf.          *
    * See http://www.OpenRTOS.com for details of the services we provide to   *
    * expedite your project.                                                  *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and 
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety 
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting, 
	licensing and training services.
*/

/*******************************************************************************
     FreeRTOS port to the CodeVisionAVR C compiler V2.04.0 or later
     by Pavel Haiduc, HP InfoTech s.r.l. www.hpinfotech.com
     V1.0, 16 March 2009
********************************************************************************/

/* Basic interrupt driven serial driver for the CodeVisionAVR port. */

#include <io.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "serial.h"

#if (defined _CHIP_ATMEGA128_) | (defined _CHIP_ATMEGA64_)
/* Make sure that we use USART0 registers & interrupt vectors for the ATmega64/128 chip */
#ifndef UDR
#define UDR UDR0
#endif

#ifndef UCSRA
#define UCSRA UCSR0A
#endif

#ifndef UCSRB
#define UCSRB UCSR0B
#endif

#ifndef UCSRC
#define UCSRC UCSR0C
#endif

#ifndef UBRRL
#define UBRRL UBRR0L
#endif

#ifndef UBRRH
#define UBRRH UBRR0H
#endif

#ifndef USART_DRE
#define USART_DRE USART0_DRE
#endif

#ifndef USART_RXC
#define USART_RXC USART0_RXC
#endif

#define serBAUD_DIV_CONSTANT ( ( unsigned portLONG ) 16 )

/* Constants for writing to UCSR0B. */
#define serRX_INT_ENABLE ( ( unsigned portCHAR ) 0x80 )
#define serRX_ENABLE ( ( unsigned portCHAR ) 0x10 )
#define serTX_ENABLE ( ( unsigned portCHAR ) 0x08 )
#define serTX_INT_ENABLE ( ( unsigned portCHAR ) 0x20 )

/* Constants for writing to UCSR0C. */
#define serEIGHT_DATA_BITS ( ( unsigned portCHAR ) 0x06 )

#else
#error please set the USART configuration for the AVR chip that you use
#endif

static xQueueHandle xRxedChars; 
static xQueueHandle xCharsForTx; 

#define vInterruptOn() UCSRB |= serTX_INT_ENABLE;
#define vInterruptOff() UCSRB &= ~serTX_INT_ENABLE;

xComPortHandle xSerialPortInitMinimal( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
unsigned portLONG ulBaudRateCounter;

	portENTER_CRITICAL();
	{
		/* Create the queues used by the com test task. */
		xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
		xCharsForTx = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

		/* Calculate the baud rate register value from the equation in the
		data sheet. */
		ulBaudRateCounter = ( configCPU_CLOCK_HZ / ( serBAUD_DIV_CONSTANT * ulWantedBaud ) ) - ( unsigned portLONG ) 1;

        #if (defined _CHIP_ATMEGA128_) | (defined _CHIP_ATMEGA64_)
		/* Set the baud rate. */	
		UBRRH = *( ( unsigned portCHAR *) &ulBaudRateCounter + 1 );
		UBRRL = *( ( unsigned portCHAR *) &ulBaudRateCounter );

        /* set asynchronous mode, 1x Baud rate */
        UCSRA = 0;
                
		/* Enable the Rx interrupt.  The Tx interrupt will get enabled
		later. Also enable the Rx and Tx. */
		UCSRB = serRX_INT_ENABLE | serRX_ENABLE | serTX_ENABLE;

		/* Set the data bits to 8. */
		UCSRC = serEIGHT_DATA_BITS;
        #else
        #error please set the USART configuration for the AVR chip that you use
        #endif
	}
	portEXIT_CRITICAL();
	
	/* Unlike other ports, this serial code does not allow for more than one
	com port.  We therefore don't return a pointer to a port structure and can
	instead just return NULL. */
	return NULL;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
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

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime )
{
	/* Return false if after the block time there is no room on the Tx queue. */
	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
	{
		return pdFAIL;
	}

	vInterruptOn();

	return pdPASS;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Turn off the interrupts.  We may also want to delete the queues and/or
	re-install the original ISR. */

	portENTER_CRITICAL();
	{
		vInterruptOff();
		UCSRB &= ~serRX_INT_ENABLE;
	}
	portEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

interrupt[USART_RXC] void SIG_UART_RECV( void )
{
signed portCHAR cChar;
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Get the character and post it on the queue of Rxed characters.
	If the post causes a task to wake force a context switch as the woken task
	may have a higher priority than the task we have interrupted. */
	cChar = UDR;
    
	xQueueSendFromISR( xRxedChars, &cChar, (signed portCHAR*) &xHigherPriorityTaskWoken );

	if( xHigherPriorityTaskWoken != pdFALSE )
	{
		taskYIELD();
	}
}
/*-----------------------------------------------------------*/

interrupt[USART_DRE] void SIG_UART_DATA( void )
{
signed portCHAR cChar, cTaskWoken = pdFALSE;

	if( xQueueReceiveFromISR( xCharsForTx, &cChar, &cTaskWoken ) == pdTRUE )
	{
		/* Send the next character queued for Tx. */
		UDR = cChar;
	}
	else
	{
		/* Queue empty, nothing to send. */
		vInterruptOff();
	}
}

