 #ifndef _config_h_
 #define _config_h_
/****************************************************************************
 *
 *  Module:       $HeadURL$
 *  Author:       $Author$
 *  Revision:     $Revision$
 *  Date:         $Date$
 *
 *  Copyright:    (C) 2008 by Dipl.-Ing. Stefan Heesch
 *                address:  Bollstrasse 14, CH-5619 Büttikon, Switzerland
 *                callsign: DB7HS, HB9TWS  
 *                email:    radio@heesch.net
 *
 *  Description:  hardware configuration
 *
 ****************************************************************************/
 
 #include <avr/io.h>

/****************************************************************************
 * CPU clock
 ****************************************************************************/
 #ifndef F_CPU
 #define F_CPU 8000000UL
 #endif

/****************************************************************************
 * LED Port
 ****************************************************************************/
 #define LED_PIN	PD4
 #define LED_OUT	PORTD
 #define LED_DDR	DDRD


/****************************************************************************
 * The end
 ****************************************************************************/
 #endif
