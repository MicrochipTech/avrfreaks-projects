 #ifndef _ds1821_h_
 #define _ds1821_h_
/****************************************************************************
 *
 *  Module:       $HeadURL: http://svn2.hosted-projects.com/sh/DS1821/ds1821.h $
 *  Author:       $Author: sh $
 *  Revision:     $Revision: 7 $
 *  Date:         $Date: 2008-05-19 01:45:27 +0200 (Mo, 19 Mai 2008) $
 *
 *  Copyright:    (C) 2008 by Dipl.-Ing. Stefan Heesch
 *                address:  Bollstrasse 14, CH-5619 Büttikon, Switzerland
 *                callsign: DB7HS, HB9TWS  
 *                email:    radio@heesch.net
 *
 *  Description:  DS1821 Thermometer
 *
 ****************************************************************************/

/****************************************************************************
 * Include files
 ****************************************************************************/
 #include "config.h"

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <avr/pgmspace.h>
 #include <util/delay.h>

 #include <stdlib.h>
 #include <stdio.h>


/****************************************************************************
 * Defines
 ****************************************************************************/
 #define DELAY_US(x)	(x)
 #define DELAY(x)	delay(x)
 #define delay(x)	_delay_us(x)

 #define uchar unsigned char
 #define uint unsigned int
 #define bit uchar
 #define idata
 #define code


/****************************************************************************
 * The end
 ****************************************************************************/
 #endif
