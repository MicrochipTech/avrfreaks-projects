/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2010  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the CDC class bootloader. This file contains the complete bootloader logic.
 */

#define  INCLUDE_FROM_BOOTLOADERCDC_C
#include "BootloaderCDC.h"

/** Contains the current baud rate and other settings of the first virtual serial port. This must be retained as some
 *  operating systems will not open the port unless the settings can be set successfully.
 */
CDC_LineEncoding_t LineEncoding = { .BaudRateBPS = 0,
                                    .CharFormat  = CDC_LINEENCODING_OneStopBit,
                                    .ParityType  = CDC_PARITY_None,
                                    .DataBits    = 8                            };

/** Flag to indicate if the bootloader should be running, or should exit and allow the application code to run
 *  via a watchdog reset. When cleared the bootloader will exit, starting the watchdog and entering an infinite
 *  loop until the AVR restarts and the application runs.
 */
bool RunBootloader = true;

#ifdef USE_EXIT
volatile uint8_t timer1 = 120;
volatile uint8_t but_timer = 0;
#endif

#if KEY_COUNT > 0
static unsigned char PROGMEM initialVector[8] =
{
	(unsigned char)(INITIALVECTOR_HI >> 24),
	(unsigned char)(INITIALVECTOR_HI >> 16),
	(unsigned char)(INITIALVECTOR_HI >> 8),
	(unsigned char)(INITIALVECTOR_HI >> 0),
	(unsigned char)(INITIALVECTOR_LO >> 24),
	(unsigned char)(INITIALVECTOR_LO >> 16),
	(unsigned char)(INITIALVECTOR_LO >> 8),
	(unsigned char)(INITIALVECTOR_LO >> 0)
};
static unsigned char chainCipherBlock[8];		// Buffer for Cipher Block
#endif // KEY_COUNT > 0

											
#ifdef UART_DEBUG
#define BAUD 38400
#include <util/setbaud.h>

static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE);

static int uart_putchar(char c, FILE *stream)
{
	if (c == '\n') uart_putchar('\r',stream);
	loop_until_bit_is_set(UCSR1A, UDRE1);
	UDR1 = c;
	return 0;
}

#endif

static void AppStart(void) 
{
	AppPtr_t AppStartPtr = (AppPtr_t)0x0000;

	AppStartPtr();
}

/** Resets all configured hardware required for the bootloader back to their original states. */
void ResetHardware(void)
{
#ifdef USE_EXIT
	TCCR3B = 0;
	TCCR3A = 0;
	TCCR3C = 0;
	OCR3A = 0;
	TIMSK3 = 0;
#endif
	/* Shut down the USB subsystem */
	USB_ShutDown();

	cli();

	/* Relocate the interrupt vector table back to the application section */
	MCUCR = (1 << IVCE);
	MCUCR = 0;
}

#ifdef USE_EXIT
static void CheckExit(void)
{
	static uint8_t button = 0;

	if (!timer1) RunBootloader=false;
	if (bit_is_set(PINC,PC6)) {
		if (!button) {
			button = 1;
			but_timer=2;
		} else {
			if ((!but_timer) && (bit_is_set(PINC,PC6))) {
				RunBootloader=false;
			}
		}
	} else 
		button = 0;											
}
#endif
/** Main program entry point. This routine configures the hardware required by the bootloader, then continuously
 *  runs the bootloader processing routine until instructed to soft-exit, or hard-reset via the watchdog to start
 *  the loaded application code.
 */
 
int __attribute__((OS_main)) main(void) //__attribute__((noreturn))
{
	/* Setup hardware required for the bootloader */
	SetupHardware();

#if KEY_COUNT > 0	
	// Copy Initial Vector to start the cipher block unchaining
	#if defined(RAMPZ)
	memcpy_PF(chainCipherBlock, GET_FAR_ADDRESS(initialVector), 8);
	#else	
    memcpy_P(chainCipherBlock, initialVector, 16);
	#endif
#endif

	/* Enable global interrupts so that the USB stack can function */
	sei();

	while (RunBootloader)
	{
		CDC_Task();
		USB_USBTask();
#ifdef USE_EXIT
		CheckExit();
#endif
	}
	/* Re-enable RWW section */
	boot_rww_enable();
	/* Reset configured hardware back to their original states for the user application */
	ResetHardware();
	/* Start the user application */
	AppStart();

	/* Enable the watchdog and force a timeout to reset the AVR */
	//wdt_enable(WDTO_250MS);
	//wdt_enable(WDTO_2S);
	//for (;;);
}
#ifdef USE_EXIT
ISR(TIMER3_COMPA_vect)
{
	if (but_timer) but_timer--;
	if (timer1) timer1--;
}
#endif
/** Configures all hardware required for the bootloader. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	//MCUSR &= ~(1 << WDRF);
	MCUSR = 0;
	wdt_disable();

	MCUCR |= _BV(JTD);
	MCUCR &= ~_BV(PUD);

	DDRB |= _BV(PB6);
	PORTB &= ~_BV(PB6);

	DDRC = 0;
	PORTC = 0;

#ifdef UART_DEBUG
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;
	#if USE_2X
	UCSR1A |= (1 << U2X1);
	#else
	UCSR1A &= ~(1 << U2X1);
	#endif

	UCSR1B = (1<<RXEN1) | (1<<TXEN1);
	stdout = &mystdout;
#endif
#ifdef USE_EXIT
	TCCR3A = 0;
	TCCR3C = 0;
#if (F_CPU == 16000000)
	OCR3A = 62500;
#else
	OCR3A = 31250;
#endif
	TIMSK3 = _BV(OCIE3A);
	TCCR3B = _BV(WGM32) | _BV(CS32);
#endif
	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Relocate the interrupt vector table to the bootloader section */
	MCUCR = (1 << IVCE);
	MCUCR = (1 << IVSEL);

	/* Initialize USB Subsystem */
	USB_Init();
}


/** Event handler for the USB_ConfigurationChanged event. This configures the device's endpoints ready
 *  to relay data to and from the attached USB host.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup CDC Data Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_IN,
	                                            CDC_NOTIFICATION_EPSIZE, ENDPOINT_BANK_SINGLE);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_TX_EPNUM, EP_TYPE_BULK, ENDPOINT_DIR_IN,
	                                            CDC_TXRX_EPSIZE, ENDPOINT_BANK_SINGLE);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_RX_EPNUM, EP_TYPE_BULK, ENDPOINT_DIR_OUT,
	                                            CDC_TXRX_EPSIZE, ENDPOINT_BANK_SINGLE);

    /* Reset line encoding baud rate so that the host knows to send new values */
	LineEncoding.BaudRateBPS = 0;
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{		
	/* Process CDC specific control requests */
	switch (USB_ControlRequest.bRequest)
	{
		case CDC_REQ_GetLineEncoding:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Write the line coding data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(&LineEncoding, sizeof(CDC_LineEncoding_t));
				Endpoint_ClearOUT();
			}

			break;
		case CDC_REQ_SetLineEncoding:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Read the line coding data in from the host into the global struct */
				Endpoint_Read_Control_Stream_LE(&LineEncoding, sizeof(CDC_LineEncoding_t));
				Endpoint_ClearIN();
			}

			break;
		case CDC_REQ_SetControlLineState:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				/* NOTE: Here you can read in the line state mask from the host, to get the current state of the output handshake
				         lines. The mask is read in from the wValue parameter in USB_ControlRequest, and can be masked against the
						 CONTROL_LINE_OUT_* masks to determine the RTS and DTR line states using the following code:
				*/
			}

			break;
	}
}

/** Retrieves the next byte from the host in the CDC data OUT endpoint, and clears the endpoint bank if needed
 *  to allow reception of the next data packet from the host.
 *
 *  \return Next received byte from the host in the CDC data OUT endpoint
 */
static uint8_t FetchNextCommandByte(void)
{
	/* Select the OUT endpoint so that the next data byte can be read */
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);

	/* If OUT endpoint empty, clear it and wait for the next packet from the host */
	while (!(Endpoint_IsReadWriteAllowed()))
	{
		Endpoint_ClearOUT();

		while (!(Endpoint_IsOUTReceived()))
		{
			if (USB_DeviceState == DEVICE_STATE_Unattached)
			  return 0;
		}
	}

	/* Fetch the next byte from the OUT endpoint */
	return Endpoint_Read_Byte();
}

/** Writes the next response byte to the CDC data IN endpoint, and sends the endpoint back if needed to free up the
 *  bank when full ready for the next byte in the packet to the host.
 *
 *  \param[in] Response  Next response byte to send to the host
 */
static void WriteNextResponseByte(const uint8_t Response)
{
	/* Select the IN endpoint so that the next data byte can be written */
	Endpoint_SelectEndpoint(CDC_TX_EPNUM);

	/* If IN endpoint full, clear it and wait until ready for the next packet to the host */
	if (!(Endpoint_IsReadWriteAllowed()))
	{
		Endpoint_ClearIN();

		while (!(Endpoint_IsINReady()))
		{
			if (USB_DeviceState == DEVICE_STATE_Unattached)
			  return;
		}
	}

	/* Write the next byte to the OUT endpoint */
	Endpoint_Write_Byte(Response);
}

unsigned char rxBuffer[BUFFER_SIZE];	// Receive buffer
unsigned char pageBuffer[PAGE_SIZE];	// Page is assembled here before

/** Task to read in AVR910 commands from the CDC data OUT endpoint, process them, perform the required actions
 *  and send the appropriate response back to the host.
 */
void CDC_Task(void)
{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	/* Select the OUT endpoint */
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);

	/* Check if endpoint has a command in it sent from the host */
	if (!(Endpoint_IsOUTReceived()))
	  return;    

	frameindex_t frameSize;
	uint16_t crc;

	// Get the frame size
	frameSize =	((frameindex_t)FetchNextCommandByte() << 8) | FetchNextCommandByte();
	// Restrict frame size to the allocated buffer size
    // Should result in a crc failure if adjusted
    frameSize = (frameSize > BUFFER_SIZE ? BUFFER_SIZE : frameSize);

	if (!frameSize) {
		WriteNextResponseByte(ERROR_CRC);
		return;
	}	

	// Receive a frame of data from communication interface and calculate
	// its CRC
	{
		uint8_t *p = rxBuffer;
		frameindex_t n = frameSize;

		crc = 0;
		do
		{
			uint8_t ch = FetchNextCommandByte();

			*p++ = ch;
			crc = CRC(crc, ch);				
			wdt_reset();

//#ifdef UART_DEBUG
//			printf("N: %u, C: 0x%.2X\n",n,ch);
//#endif
		}
		while (--n);
	}
	// CRC is OK?
	if (crc == 0x0000)
	{
#ifdef USE_EXIT
		timer1 = 120;
#endif
		uint8_t *pRcvBuf = rxBuffer;			
		// Decrypt 8 bytes, CRC-bytes are ignored.
		#if KEY_COUNT > 0
		{
			frameSize -= 2;
			uint8_t *pTmpBuf = rxBuffer;
			do
			{
		    	wdt_reset();
				desDecrypt(pTmpBuf, chainCipherBlock);
				pTmpBuf += 8;
			}
			while (frameSize -= 8);
		}
		#endif // KEY_COUNT > 0
		// Check that the signature is correct
		if ((*pRcvBuf++ == (unsigned char)(SIGNATURE >> 24)) &&
			(*pRcvBuf++ == (unsigned char)(SIGNATURE >> 16)) &&
			(*pRcvBuf++ == (unsigned char)(SIGNATURE >>  8)) &&
			(*pRcvBuf++ == (unsigned char)(SIGNATURE >>  0)))
		{
			uint_farptr_t address;
			uint8_t bits;
			frameindex_t size;
			uint8_t type;

			// Continue parsing the frames until the 'End Of Frame' is
			// received
			while ((type = *pRcvBuf++) != TYPE_EOF)
			{
				wdt_reset();

				// Receive Lock Bits (used in TYPE_LOCKBITS)
				bits = *pRcvBuf++;

				// Destination Address (note: 'bits' and 'address' overlap)
				address = (uint_farptr_t)((uint32_t)bits << 16);
				address = (uint_farptr_t)((uint32_t)address | ((uint32_t)*pRcvBuf++ << 8));
				address = (uint_farptr_t)((uint32_t)address | *pRcvBuf++);

				size = ((uint16_t)*pRcvBuf++ << 8);
				size |= *pRcvBuf++;

#ifdef UART_DEBUG
				printf("A:0x%.5lX, S:0x%.3X, T:%u\n",address,size,type);
#endif

				switch (type)
				{
					// Erase page
					case TYPE_ERASE:
						boot_page_erase(
						#ifdef RAMPZ
						(uint32_t)
						#else
						(uint16_t)
						#endif
						address);
						boot_spm_busy_wait();
/*						boot_page_write(
						#ifdef RAMPZ
						(uint32_t)
						#else
						(uint16_t)
						#endif
						address);     // Store buffer in flash page.
						boot_spm_busy_wait();       			// Wait until the memory is written.*/
						// Fall-through!

					// Prepare for incoming data chunks by copying the page
					// contents into the page buffer
					case TYPE_PREPARE:
					{
						#ifdef RAMPZ
                            memcpy_PF(pageBuffer, (uint32_t)address, size);
						#else
                            memcpy_P(pageBuffer, (uint16_t)address, size);
						#endif
						break;
					}

					// Chunk of data is written into the page buffer
					case TYPE_DATA:
					{
						uint8_t *pPageBuff = &pageBuffer[(uint16_t)(address) & 0xffff];
						do {
							*pPageBuff++ = *pRcvBuf++;
						} while (--size);
						break;
					}

					// Program page buffer into flash page
					case TYPE_PROGRAM:
					{
						boot_page_erase ((uint16_t)address);
						boot_spm_busy_wait ();      			// Wait until the memory is erased.
						uint8_t *pPageBuff = pageBuffer;
						for (frameindex_t i=0; i<size; i++)
						{
							wdt_reset();						// Set up little-endian word.
							uint16_t w = *pPageBuff++;
							w += (*pPageBuff++) << 8;

							boot_page_fill((uint16_t)address+i*2, w);
						}
						boot_page_write((uint16_t)address);     // Store buffer in flash page.
						boot_spm_busy_wait();       			// Wait until the memory is written.
						break;
					}

					// Write a chunk of data into EEPROM
					case TYPE_EEPROM:
						do
						{
							wdt_reset();
							eeprom_write_byte((uint8_t*)((intptr_t)((uint16_t)address++)),*(pRcvBuf++));
							eeprom_busy_wait();
						}
						while (--size);
						break;

					// Write Lock Bits
					case TYPE_LOCKBITS:
						boot_lock_bits_set(bits);
						break;

					// Reset (let the Watchdog Timer expire)!
					case TYPE_RESET:
#ifdef UART_DEBUG
						printf("R_OK\n");
#endif
						WriteNextResponseByte(ERROR_OK);

						RunBootloader = false;						
					// Nonsense frames, one byte long
					default:
						pRcvBuf -= 5;
						break;
				}
			}
		}
		WriteNextResponseByte(ERROR_OK);
	} else
		WriteNextResponseByte(ERROR_CRC);

	/* Select the IN endpoint */
	Endpoint_SelectEndpoint(CDC_TX_EPNUM);

	/* Remember if the endpoint is completely full before clearing it */
	bool IsEndpointFull = !(Endpoint_IsReadWriteAllowed());

	/* Send the endpoint data to the host */
	Endpoint_ClearIN();

	/* If a full endpoint's worth of data was sent, we need to send an empty packet afterwards to signal end of transfer */
	if (IsEndpointFull)
	{
		while (!(Endpoint_IsINReady()))
		{
			if (USB_DeviceState == DEVICE_STATE_Unattached)
			  return;
		}

		Endpoint_ClearIN();
	}

	/* Wait until the data has been sent to the host */
	while (!(Endpoint_IsINReady()))
	{
		if (USB_DeviceState == DEVICE_STATE_Unattached)
		  return;
	}

	/* Select the OUT endpoint */
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);

	/* Acknowledge the command from the host */
	Endpoint_ClearOUT();
}

// Event handler for the library USB Connection event.
void EVENT_USB_Device_Connect(void)
{
	if (!(USB_Options & USB_OPT_MANUAL_PLL))
	{
		USB_PLL_On();
		while (!(USB_PLL_IsReady()));
	}

	USB_CLK_Unfreeze();
}
