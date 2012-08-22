/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 *      XMEGA USART driver source file.
 *
 ****** Modified for use in FreeRTOS, VG Feb,Mar, Oct 2011
 *
 *      This file contains the function implementations the XMEGA interrupt
 *      and polled USART driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA ADC module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 *      Some functions use the following construct:
 *          "some_register = ... | (some_parameter ? SOME_BIT_bm : 0) | ..."
 *      Although the use of the ternary operator ( if ? then : else ) is discouraged,
 *      in some occasions the operator makes it possible to write pretty clean and
 *      neat code. In this driver, the construct is used to set or not set a
 *      configuration bit based on a boolean input parameter, such as
 *      the "some_parameter" in the example above.
 *
 * \par Application note:
 *      AVR1307: Using the XMEGA USART
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1694 $
 * $Date: 2008-07-29 14:21:58 +0200 (ti, 29 jul 2008) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include "all.h"


#if SERIAL_SIMULATION == 1

static char pcontent[] PROGMEM = "$ABCDEFGH*08\r\n"; // test datagram 

#endif


/*! \brief Initializes buffer and selects what USART module to use.
 *
 *  Initializes receive and transmit buffer and selects what USART module to use,
 *  and stores the data register empty interrupt level.
 *  A queue is allocated only if used (either for transmitting, receiving or both).
 *  
 *
 *
 *  \param usart_data           The USART_data_t struct instance.
 *  \uxQueueLength              Length of queues.
 *  \dirctrl					Which queus to allocate, receive or tranmit. 
 *  \param usart                The USART module.
 *  \param dreIntLevel          Data register empty interrupt level.
 */
void USART_InterruptDriver_Initialize(USART_data_t * usart_data,
		                              unsigned portBASE_TYPE uxQueueLength, 
									  dirctrl_t dirctrl,
									  USART_t * usart,
		                              USART_DREINTLVL_t dreIntLevel)
{
	usart_data->usart = usart;
	usart_data->dreIntLevel = dreIntLevel;

	if((dirctrl == receivetransmit) || (dirctrl == receiveonly))
	{
		if((usart_data->xRxQueue = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE ) sizeof(signed char))) == NULL)
			error(ERR_QUEUE_CREATE_HANDLE_NULL);
	}
	if((dirctrl == receivetransmit) || (dirctrl == transmitonly))
	{
		if((usart_data->xTxQueue = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE ) sizeof(signed char))) == NULL)
			error(ERR_QUEUE_CREATE_HANDLE_NULL);
	}

	#if SERIAL_CHECK_SEND == 1
	sm_init(&(usart_data->sm)); // init the datagram state machine
 	#endif

	#if SERIAL_SIMULATION == 1
	//
	// state variables for the simulation and stability tests only.
	//
	usart_data->RecDataByte  = 0;
	usart_data->rCounter     = 0;
	usart_data->sCounter     = 0;
	#endif
}

/*! \brief Set USART DRE interrupt level.
 *
 *  Set the interrupt level on Data Register interrupt.
 *
 *  \note Changing the DRE interrupt level in the interrupt driver while it is
 *        running will not change the DRE interrupt level in the USART before the
 *        DRE interrupt have been disabled and enabled again.
 *
 *  \param usart_data         The USART_data_t struct instance
 *  \param dreIntLevel        Interrupt level of the DRE interrupt.
 */
void USART_InterruptDriver_DreInterruptLevel_Set(USART_data_t * usart_data,
		USART_DREINTLVL_t dreIntLevel)
{
	usart_data->dreIntLevel = dreIntLevel;
}



/*! \brief Test if there is data in the transmitter software buffer.
 *
 *  This function can be used to test if there is free space in the transmitter
 *  software buffer.
 *
 *  \param usart_data The USART_data_t struct instance.
 *
 *  \retval true      There is data in the receive buffer.
 *  \retval false     The receive buffer is empty.
 */
bool USART_TXBuffer_FreeSpace(USART_data_t * usart_data)
{
	/* Just to stop compiler warnings. */
	(void) usart_data;

// not implemented

	return true;
}



/*! \brief Put data (5-8 bit character).
 *
 *  Stores data byte in TX queue and enables DRE interrupt if there
 *  is free space in the TX queue block time expired.
 *
 *  \param usart_data The USART_data_t struct instance.
 *  \param data       The data to send.
 *  \xBlockTime       Block time to wait for room on the Tx queue.
 */
bool USART_TXBuffer_PutByte(USART_data_t * usart_data, uint8_t data,
		portTickType xBlockTime)
{
	// Return false if after the block time there is no room on the Tx queue.
	if (xQueueSend( usart_data->xTxQueue, &data, xBlockTime ) != pdPASS)
	{
		return pdFAIL;
	}

	// Enable DRE interrupt.

	USART_DreInterruptLevel_Set(usart_data->usart, usart_data->dreIntLevel);

	return pdPASS;
}


/*! \brief Test if there is data in the receive software buffer.
 *
 *  This function can be used to test if there is data in the receive software
 *  buffer.
 *
 *  \param usart_data         The USART_data_t struct instance
 *
 *  \retval true      There is data in the receive buffer.
 *  \retval false     The receive buffer is empty.
 */
bool USART_RXBufferData_Available(USART_data_t * usart_data)
{
	/* Just to stop compiler warnings. */
	(void) usart_data;


// not implemented


	return true;
}



/*! \brief Get received data (5-8 bit character).
 *
 *  The function USART_RXBufferData_Available should be used before this
 *  function is used to ensure that data is available.
 *
 *  Returns data from RX queue.
 *
 *  \param usart_data       The USART_data_t struct instance.
 *
 *  \return         Received data.
 */
bool USART_RXBuffer_GetByte(USART_data_t * usart_data, uint8_t *data, portTickType xBlockTime)
{
	/* Get the next character from the buffer.  Return false if no characters
	 are available, or arrive before xBlockTime expires. */
	return xQueueReceive( usart_data->xRxQueue, data, xBlockTime );
}

/*! \brief RX Complete Interrupt Service Routine.
 *
 *  RX Complete Interrupt Service Routine.
 *  Stores received data in RX queue.
 *
 *  \param usart_data      The USART_data_t struct instance.
 */

#if SERIAL_SIMULATION == 1

bool USART_RXComplete_sim(USART_data_t * usart_data)
{
	signed char cChar;
	signed portBASE_TYPE xTaskWokenBySend = pdFALSE;

	// Get the character and post it on the queue of Rxed characters.
	// If the post causes a task to wake force a context switch as the woken task
	// may have a higher priority than the task we have interrupted.
	//
	cChar = pgm_read_byte_near(&pcontent[usart_data->RecDataByte]);

	if(xQueueSendFromISR( usart_data->xRxQueue, &cChar, &xTaskWokenBySend )!=pdTRUE)
	{
		errorNonFatal(ERR_BYTE_LOST_ON_RECEIVE);
	}

	if(cChar == '\n')
		usart_data->RecDataByte = 0;
	else
		usart_data->RecDataByte++;

	usart_data->rCounter++;
	return xTaskWokenBySend; 
}
#endif

bool USART_RXComplete(USART_data_t * usart_data)
{
	signed char cChar;
	signed portBASE_TYPE xTaskWokenBySend = pdFALSE;

	// Get the character and post it on the queue of Rxed characters.
	// If the post causes a task to wake force a context switch as the woken task
	// may have a higher priority than the task we have interrupted.
	//
	cChar = usart_data->usart->DATA;
	if(xQueueSendFromISR( usart_data->xRxQueue, &cChar, &xTaskWokenBySend )!=pdTRUE)
	{
		errorNonFatal(ERR_BYTE_LOST_ON_RECEIVE);
	}
	return xTaskWokenBySend; 
}


/*! \brief Data Register Empty Interrupt Service Routine.
 *
 *  Data Register Empty Interrupt Service Routine.
 *  Transmits one byte from TX queue. Disables DRE interrupt if buffer
 *  is empty. Argument is pointer to USART (USART_data_t).
 *
 *  \param usart_data      The USART_data_t struct instance.
 */

bool USART_DataRegEmpty(USART_data_t * usart_data)
{
	char cChar;
	signed portBASE_TYPE xTaskWokenByReceive = pdFALSE;

	// Check if all data is transmitted.
	if (xQueueReceiveFromISR(usart_data->xTxQueue, &cChar, &xTaskWokenByReceive) == pdTRUE)
	{
		#if SERIAL_CHECK_SEND == 1
		if( !sm_setState(&(usart_data->sm), cChar) )
				errorNonFatal(ERR_BYTE_LOST_ON_SEND);
		#endif

		// Start transmitting.
		//
		usart_data->usart->DATA = cChar;
		#if SERIAL_SIMULATION == 1
		usart_data->sCounter++;
		#endif
	}
	else
	{
		// Disable DRE interrupt

		USART_DreInterruptLevel_Set(usart_data->usart, USART_DREINTLVL_OFF_gc);
	}
	return xTaskWokenByReceive; 
}


/*! \brief Put data (9 bit character).
 *
 *  Use the function USART_IsTXDataRegisterEmpty before using this function to
 *  put 9 bit character to the TX register.
 *
 *  \param usart      The USART module.
 *  \param data       The data to send.
 */

/*
void USART_NineBits_PutChar(USART_t * usart, uint16_t data)
{
	if (data & 0x0100)
	{
		usart->CTRLB |= USART_TXB8_bm;
	}
	else
	{
		usart->CTRLB &= ~USART_TXB8_bm;
	}

	usart->DATA = (data & 0x00FF);
}
*/

/*! \brief Get received data (9 bit character).
 *
 *  This function reads out the received 9 bit character (uint16_t).
 *  Use the function USART_IsRXComplete to check if anything is received.
 *
 *  \param usart      The USART module.
 *
 *  \retval           Received data.
 */

/*
uint16_t USART_NineBits_GetChar(USART_t * usart)
{
	if (usart->CTRLB & USART_RXB8_bm)
	{
		return (0x0100 | usart->DATA);
	}
	else
	{
		return (usart->DATA);
	}
}
*/

/*! \brief Receive complete interrupt service routines.
 *
 *  Receive complete interrupt service routine.
 *  Calls the common receive complete handler with pointer to the correct USART
 *  as argument.
 */

ISR(USARTC0_RXC_vect, ISR_NAKED)
{
	portSTART_ISR();

	portEND_SWITCHING_ISR( USART_RXComplete(&USARTC0_data) );
}

ISR(USARTC1_RXC_vect, ISR_NAKED)
{
	portSTART_ISR();

	portEND_SWITCHING_ISR( USART_RXComplete(&USARTC1_data) );
}

ISR(USARTE0_RXC_vect, ISR_NAKED)
{
	portSTART_ISR();

	portEND_SWITCHING_ISR( USART_RXComplete(&USARTE0_data) );
}


/*! \brief Data register empty  interrupt service routines.
 *
 *  Data register empty  interrupt service routine.
 *  Calls the common data register empty complete handler with pointer to the
 *  correct USART as argument.
 */

ISR(USARTC0_DRE_vect, ISR_NAKED)
{
	portSTART_ISR();

	portEND_SWITCHING_ISR( USART_DataRegEmpty(&USARTC0_data) );
}


ISR(BADISR_vect)
{
    error(ERR_BAD_ISR);
}


// eof freertos_usart_driver.c

