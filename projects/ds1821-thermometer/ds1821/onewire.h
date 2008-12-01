 #ifndef _1wire_h_
 #define _1wire_h_
/****************************************************************************
 *
 *  Module:       $HeadURL: http://svn2.hosted-projects.com/sh/DS1821/onewire.h $
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
 * Function prototypes
 ****************************************************************************/
 bit w1_reset(void);

 void w1_bit_write( uchar b );
 uchar w1_bit_read( void );

 void w1_byte_wr( uchar b );
 uint w1_byte_rd( void );
 uint w1_read( uchar bits );

/****************************************************************************
 * The end
 ****************************************************************************/
 #endif
