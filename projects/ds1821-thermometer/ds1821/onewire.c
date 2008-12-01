/****************************************************************************
 *
 *  Module:       $HeadURL: http://svn2.hosted-projects.com/sh/DS1821/onewire.c $
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


bit w1_reset(void)
{
    W1_OUT &= ~(1<<W1_PIN);               // One-wire LOW
    W1_DDR |= 1<<W1_PIN;
  
    DELAY( DELAY_US( 480 ));			  // 480 us
  
    W1_DDR &= ~(1<<W1_PIN);               
    DELAY( DELAY_US( 65 ));               // Wait 15 + 50 us
    bit err = W1_IN & (1<<W1_PIN);		  // Sample bus
  
    DELAY( DELAY_US( 480 - 65 ));         // wait until cycle is gone 
  
    if( (W1_IN & (1<<W1_PIN)) == 0 )	  // check bus for HIGH, othwise we have a short circuit
      err = 1;

    return err;
}


uchar w1_bit_read( void )
{
    W1_OUT &= ~(1<<W1_PIN);               // One-wire LOW
    W1_DDR |= 1<<W1_PIN;
    
    DELAY( DELAY_US( 1 ));                // 1 us time 
    W1_DDR &= ~(1<<W1_PIN);               // One-wire HIGH and use as input

    DELAY( DELAY_US( 14 ));               // 14 us 
    uchar value = W1_IN & (1<<W1_PIN);    // sample bus
    DELAY( DELAY_US( 45 ));               // 45 us 

    return value;
}


void w1_bit_write( uchar b )
{
    W1_OUT &= ~(1<<W1_PIN);               // One-wire LOW
    W1_DDR |= 1<<W1_PIN;
    DELAY( DELAY_US( 10 ));               // 10 us 

    if ( b ) 
    {
        W1_DDR &= ~(1<<W1_PIN);           // One-wire input    
        DELAY( DELAY_US( 5 + 45 ));       // in total 60 us
    }
    else
    {
        DELAY( DELAY_US( 5 + 45 ));       // 60 us in total
        W1_DDR &= ~(1<<W1_PIN);           // One-wire HIGH and use as input    
    }
}


int w1_read( uchar bits)
{
    uchar i = bits;
    int  value = 1 << (bits-1);
    int  b = 0;

    do
    {
       b >>= 1;
       if( w1_bit_read() ) b |= value;
    
    } while( --i );

    return b;
}


uchar w1_byte_rd( void )
{
   return (uchar) w1_read(8);
}


void w1_byte_wr( uchar b )
{
  uchar i = 8;
  do
  {
    w1_bit_write( b & 1 );
    b >>= 1;

  } while( --i );
}

