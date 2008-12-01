 #ifndef _config_h_
 #define _config_h_
/****************************************************************************
 *
 *  Module:       $HeadURL: http://svn2.hosted-projects.com/sh/DS1821/config.h $
 *  Author:       $Author: sh $
 *  Revision:     $Revision: 7 $
 *  Date:         $Date: 2008-05-19 01:45:27 +0200 (Mo, 19 Mai 2008) $
 *
 *  Copyright:    (C) 2008 by Dipl.-Ing. Stefan Heesch
 *                address:  Bollstrasse 14, CH-5619 Büttikon, Switzerland
 *                callsign: DB7HS, HB9TWS  
 *                email:    radio@heesch.net
 *
 *  Description:  DS1821 Thermometer: hardware configuration
 *
 ****************************************************************************/
 
/****************************************************************************
 * CPU clock
 ****************************************************************************/
 #ifndef F_CPU
 #define F_CPU 8000000UL
 #endif


/****************************************************************************
 * OneWire Port
 ****************************************************************************/
 #define W1_PIN	PA1
 #define W1_IN	PINA
 #define W1_OUT	PORTA
 #define W1_DDR	DDRA


/****************************************************************************
 * LCD Display
 ****************************************************************************/
 #define DSP_PORT	PORTC
 #define DSP_LINES	    4
 #define DSP_LEN	   20


/****************************************************************************
 * The end
 ****************************************************************************/
 #endif
