/****************************************************************************
 *
 *  Module:       $HeadURL: http://svn2.hosted-projects.com/sh/DS1821/tempmeas.c $
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
 #include "ds1821.h"
 #include "lcd.h"
 #include "onewire.h"
 #include "tempmeas.h"


/****************************************************************************
 * Start measurement
 ****************************************************************************/
 void start_meas( void )
 {
   if( W1_IN & 1<< W1_PIN )
   {
     w1_byte_wr( 0xEE );
   }
 }


/****************************************************************************
 * Display measurement result
 ****************************************************************************/
 void read_meas( void )
 {
   char s[20];
   int t;
   int r;
   unsigned int c;

   long temperature;

   w1_reset();
   w1_byte_wr( 0xAA );			// read temp
   t = w1_byte_rd();			// low byte
   
   w1_reset();
   w1_byte_wr( 0xA0 );          // read remaining count
   r = w1_read(9);

   w1_reset();
   w1_byte_wr( 0x41 );          // load counter

   w1_reset();
   w1_byte_wr( 0xA0 );          // read slope counter
   c = w1_read(9);

   lcd_gotoxy( 0,2 );
   sprintf( s, "RAW  :  %03d %03d %03d", t, r, c );
   lcd_puts( s );

   if ( t > 0x80 ) 
   {
      t = t-256;
   }

   temperature   = 10 * t - 5;
   temperature  += ( 10 * ( c - r )) / c ;

   t = temperature / 10;
   r = temperature % 10;

   lcd_gotoxy( 0,3 );
   sprintf( s, "TEMP :  %3d.%d C", t, r );
   lcd_puts( s );
 }
