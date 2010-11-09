/**
 * @file pal_sio_hub.c
 *
 * @brief Stream I/O API functions
 *
 * This file implements the Stream I/O API functions.
 *
 * $Id: pal_sio_hub.c 19039 2009-11-03 09:27:02Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
 
 // Extensively modified to use Teensy2++ USB as a serial port emulator.
 //	9/12/2010         -jkl
 // Assume that UART1 will be used, not USB0.
/* === Includes ============================================================ */

//#ifdef SIO_HUB

#include <stdint.h>
//#include "pal_config.h"

#include "pal.h"
#include "return_val.h"
#include "usb_serial.h"			// modified
#include "pal_uart.h"
#include <util/delay.h>
//#include "pal_usb.h"

/* === Globals =============================================================*/


/* === Prototypes ==========================================================*/
uint8_t sio_rx(uint8_t *, uint8_t);     // local function to get data

/* === Implementation ======================================================*/

/**
 * @brief Initializes the requested SIO unit
 *
 * This function initializes the requested SIO unit.
 *
 * @param sio_unit Specifies the SIO uint to be initialized
 *
 * @return MAC_SUCCESS  if SIO unit is initialized successfully, FAILURE
 * otherwise
 */
 // This should simply be a matter of calling usb_init(). Since a return value is required, could
 //		call usb_configured() and make sure it's not 0.
 //!!!!!!!!!!!!!!!!!!!! Note that calling the usb_init turns on interrupts! Be sure this is OK!!!!!
 
retval_t pal_sio_init(uint8_t sio_unit)
{
    retval_t status = MAC_SUCCESS;
	/*
	uint8_t buf[10];
	buf[0] = 't';
	buf[1] = 'e';
	buf[2] = 's';
	buf[3] = 't';
	buf[4] = 0;
*/
	uint8_t buf[] = "ready\n";
    switch (sio_unit)
    {
//#ifdef UART1
        case SIO_1:
            usb_init();
			/*  -- Not sure when and how to use this??
			if (usb_configured() == 0)
				status = FAILURE;	
*/				
            break;
//#endif

        default:
            status = FAILURE;
            break;
    }
	// let's try reading and writing the USB port here using code from
	//	Paul's example.
	// first two lines will wait until a serial port on a PC is connected.
	while (!usb_configured()) /* wait */ ;
	_delay_ms(1000);
	
	while (!(usb_serial_get_control() & USB_SERIAL_DTR)) /* wait */ ;
	
	usb_serial_flush_input();
	
	// Now let's put out a string
	pal_sio_tx(SIO_1, buf, strlen((char *)buf));

// Echoing character worked!
//This makes it wait until I enter a character
	while (1) {
		int n = usb_serial_getchar();
		//if (n >= 0) usb_serial_putchar(n);
		if (n >= 0){ usb_serial_putchar(buf[1]); break;}
		//if (n >= 0) pal_sio_tx(SIO_1, buf, 4);
	}

    return status;
}


/**
 * @brief Transmits data through selected SIO unit
 *
 * This function transmits data through the selected SIO unit.
 *
 * @param sio_unit Specifies the SIO unit
 * @param data Pointer to the data to be transmitted is present
 * @param length Number of bytes to be transmitted
 *
 * @return Actual number of transmitted bytes
 */
 // This one should be easy - just use usb_serial_write
 // int8_t usb_serial_write(const uint8_t *buffer, uint16_t size)
uint8_t pal_sio_tx(uint8_t sio_unit, uint8_t *data, uint8_t length)
{
    uint8_t number_of_bytes_transmitted;

    switch (sio_unit)
    {
//#ifdef UART1
        case SIO_1: number_of_bytes_transmitted = (uint8_t)usb_serial_write(data, (uint16_t)length);
                    break;
//#endif

        default:    number_of_bytes_transmitted = 0;
                    break;
    }
    return (number_of_bytes_transmitted);
}


/**
 * @brief Receives data from selected SIO unit
 *
 * This function receives data from the selected SIO unit.
 *
 * @param sio_unit Specifies SIO unit
 * @param[out] data Pointer to the buffer to store received data
 * @param[in] max_length Maximum number of bytes to be received
 *
 * @return Actual number of received bytes
 */
 // This function is slightly more complicated. Uses sio_rx to get a buffer.
uint8_t pal_sio_rx(uint8_t sio_unit, uint8_t *data, uint8_t max_length)
{
    uint8_t number_of_bytes_received;

    switch (sio_unit)
    {
//#ifdef UART1
        case SIO_1: number_of_bytes_received = sio_rx(data, max_length);
                    break;
//#endif

        default:    number_of_bytes_received = 0;
                    break;
    }

    return (number_of_bytes_received);
}
//
// sio_rx -- local function to actually get the data from the usb.
// uses uint8_t usb_serial_available(void) to get number of characters available
// and int16_t usb_serial_getchar(void) to get them.
//
uint8_t sio_rx(uint8_t *data, uint8_t max_length)
{
    uint8_t data_received;

	data_received = usb_serial_available();		// find out how many characters are available
    if (data_received == 0) 
    {
        /* no characters available */
        return 0;
    }

    // Two possible conditions must be handled:
	//  - if the number of bytes received is >= to max_length, then will transfer only max_length characters.
	//		so max_length is unchanged.
	//  - if the number of bytes received is < the max_length, then the number transfered will be the number received.
	//		so max_length becomes data_received.
        /* Bytes received is more than receive buffer maximum length. */
	if (data_received < max_length)
	{
		/*
		 * Requested receive length (max_length) is more than the data
		 * present in receive buffer. Hence only the number of bytes
		 * present in receive buffer are read.
		 */
		max_length = data_received;
	}

    data_received = max_length;

    while (max_length > 0)
    {
        *data = (int8_t) usb_serial_getchar();
        data++;
        max_length--;
    }

    return data_received;
}
//#endif /* SIO_HUB */

/* EOF */
