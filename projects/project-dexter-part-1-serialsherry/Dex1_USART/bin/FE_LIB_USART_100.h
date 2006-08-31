/*********************************************************************************************************************
 * FE_Lib_USART
 * 
 * Flying Electron USART Library
 *
 * v1.00 5/29/2006
 *
 *  The Flying Electron USART Library is designed to make the RS232 communications easy.
 *
 *  After calling the USART_Init function the serial port will behave like the default standard IO device and can be 
 *  used with standard IO calls such as printf, scanf, getch, etc.
 *
 * Copyright 2006 Flying Electron Inc.
  ********************************************************************************************************************/

// -------------------------------------------------------------------------------------------------------------------
// Includes
// -------------------------------------------------------------------------------------------------------------------
    #include <stdbool.h>

// -------------------------------------------------------------------------------------------------------------------
// Public Function Prototypes
// -------------------------------------------------------------------------------------------------------------------

    /**
	 *	USART_Init
	 *
	 *		This function is called to initialize the USART module to run at a specific baud rate at a specific
     *          frequency.  For example to use the USART at 9600 baud with a microcontroller running at 1MHz, you
     *          would call USART_Init(9600, 1).
	 *
     *      BaudRate - Number of bits per second that the USART should operate at.  Common values are 2400, 4800, 9600,
     *          14400, 19200, 28800, 38400, 57600, 76800, 115200, 230400, 250000, 500000, 1000000
     *
	 *		Frequency - Frequency of the oscillator the microcontroller is running at in Mhz.  For example if the
     *          microcontroller is running at 1Mhz, this value would be 1.  If the microcontroller is running at
     *          14.7456Mhz this value would be 14.7456
     *
     *      Return - Error percentage of this baud rate and frequency combination.  For example setting the USART to
     *          run at 28800 baud @ 4Mhz would result in an error of 2.1%.  The return value would be 2.1.  For more
     *          information on error ratings of baud rate and frequency combinations see the Atmel AVR datasheets.
	**/
	float USART_Init(uint32_t BaudRate, float Frequency);

    /**
	 *	USART_DataReady
	 *
	 *		This function is called to check if there is any data ready in the USART RX buffer.
	 *
	 *		Return - True if there is data waiting in the receive buffer, false if there is no data waiting.
	**/
    bool USART_DataReady(void);
