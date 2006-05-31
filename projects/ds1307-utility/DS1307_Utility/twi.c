/********************************************************************************

Two Wire Interface (TWI) utility functions.

Copyright (C) 2005 by Donald R. Blake
donblake at worldnet.att.net

This program is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

---------------------------------------------------------------------------------

Change Activity:

    Date       Description
   ------      -------------
  02 Sep 2005  Created.
  07 Sep 2005  Include <compat/twi.h>

********************************************************************************/



/********************************************************************************

                                    Includes

********************************************************************************/

#include <avr/io.h>
#include <stdbool.h>
#include <compat/twi.h>
#include "twi.h"



/********************************************************************************

                               Macros and Defines

********************************************************************************/

#define WAIT_FOR_TWINT \
  for ( \
       toCounter = toValue; \
       ( toCounter != 0 ) && ( ( TWCR & _BV( TWINT ) ) == 0 ); \
  )

#define WAIT_FOR_STOP_GEN \
  for ( \
       toCounter = toValue; \
       ( toCounter != 0 ) && ( ( TWCR & _BV( TWSTO ) ) != 0 ); \
  )

#define TWI_READY ( ( TWCR & _BV( TWINT ) ) != 0 )

#define SCL_FREQ 100000UL
#define TWPS     0
#if TWPS == 0
#  define TWBR_VALUE ( ( ( F_CPU / SCL_FREQ ) - 16 +  1UL ) / ( 2UL *  1UL ) )
#elif TWPS == 1
#  define TWBR_VALUE ( ( ( F_CPU / SCL_FREQ ) - 16 +  4UL ) / ( 2UL *  4UL ) )
#elif TWPS == 2
#  define TWBR_VALUE ( ( ( F_CPU / SCL_FREQ ) - 16 + 16UL ) / ( 2UL * 16UL ) )
#elif TWPS == 3
#  define TWBR_VALUE ( ( ( F_CPU / SCL_FREQ ) - 16 + 64UL ) / ( 2UL * 64UL ) )
#else
#  error Invalid TWPS value
#endif

#if ( TWBR_VALUE < 10 ) || ( TWBR_VALUE > 255 )
#  error Invalid TWBR_VALUE
#endif



/********************************************************************************

                                Global Variables

********************************************************************************/

static volatile uint8_t toCounter;  // time-out counter

static uint8_t toValue;             // starting time-out counter value



/********************************************************************************

                               Internal Functions

********************************************************************************/



/********************************************************************************

                                  generateStop

********************************************************************************/

static
int             // returns zero of no error; non-zero if error
generateStop(
  void
)
{

  // generate a STOP condition
  TWCR = _BV( TWINT ) | _BV( TWSTO ) | _BV( TWEN );

  // wait for STOP generated
  WAIT_FOR_STOP_GEN;

  // check for timeout
  if ( ( TWCR & _BV( TWSTO ) ) != 0 ) return 1;

  return 0;

} // end generateStop



/********************************************************************************

		       sendSla (send SLA - Slave Address)

********************************************************************************/

static
int             // returns zero of no error; non-zero if error
sendSla(
  uint8_t sla,  // slave address (0..127)
  uint8_t rw    // read (TW_READ) or write (TW_WRITE)
)
{

  // send START condition
  TWCR = _BV( TWINT ) | _BV( TWSTA ) | _BV( TWEN );

  // wait for start condition transmitted
  WAIT_FOR_TWINT;

  // clear TWI START condition bit
  TWCR = _BV( TWEN );

  // check for time-out and check status code
  if ( !TWI_READY ) return 2;
  if ( TW_STATUS != TW_START ) return 3;

  // load SLA+R or SLA+W
  TWDR = ( sla << 1 ) | ( rw & 0x01 );

  // transmit SLA
  TWCR = _BV( TWINT ) | _BV( TWEN );

  // wait for SLA transmitted
  WAIT_FOR_TWINT;

  // check for time-out and check status code
  if ( !TWI_READY ) return 4;
  if ( rw == TW_READ )
  {
    // receive mode
    if ( TW_STATUS != TW_MR_SLA_ACK )
    {
      if ( TW_STATUS == TW_MR_SLA_NACK ) generateStop( );
      return 5;
    } // end if
  }
  else
  {
    // transmit mode
    if ( TW_STATUS != TW_MT_SLA_ACK )
    {
      if ( TW_STATUS == TW_MT_SLA_NACK ) generateStop( );
      return 6;
    } // end if
  } // end if

  return 0;

} // end sendSla



/********************************************************************************

                               External Functions

********************************************************************************/



/********************************************************************************

                            twiInit - initialize TWI

********************************************************************************/

void
twiInit(
  uint8_t timeout
)
{

  // save starting time-out counter value
  toValue = timeout;

  // initialize TWI Bit Rate Prescaler
  TWSR =
       ( ( ( TWPS & _BV( 1 ) ) != 0 ) ? _BV( TWPS1 ) : 0 ) |
       ( ( ( TWPS & _BV( 0 ) ) != 0 ) ? _BV( TWPS0 ) : 0 );

  // initialize TWI Bit Rate
  TWBR = TWBR_VALUE;

} // end twiInit



/********************************************************************************

                          twiWrite - Master Transmist

********************************************************************************/

uint8_t
twiWrite(
  uint8_t  sla,
  uint8_t* data,
  uint8_t  count
)
{

  uint8_t rc;

  // send SLA+W
  rc = sendSla( sla, TW_WRITE );
  if ( rc != 0 ) return rc;

  // send data byte(s)

  do
  {

    // send next data byte
    TWDR = *data++;
    TWCR = _BV( TWINT ) | _BV( TWEN );

    // wait for data byte transmitted
    WAIT_FOR_TWINT;

    // check for timeout and check status code
    if ( !TWI_READY ) return 7;
    if ( TW_STATUS != TW_MT_DATA_ACK )
    {
      if ( TW_STATUS == TW_MT_DATA_NACK ) generateStop( );
      return 8;
    } // end if

  } while ( --count != 0 );

  // generate STOP
  rc = generateStop( );
  if ( rc != 0 ) return rc;

  return 0;

} // end twiWrite



/********************************************************************************

                            twiRead - Master Receive

********************************************************************************/

uint8_t
twiRead(
  uint8_t  sla,
  uint8_t* data,
  uint8_t  count
)
{

  uint8_t rc;

  // send SLA+R
  rc = sendSla( sla, TW_READ );
  if ( rc != 0 ) return rc;

  // read data byte(s)

  do
  {

    // set up to receive next data byte
    TWCR = _BV( TWINT ) | _BV( TWEN) | ( ( count != 1 ) ? _BV( TWEA ) : 0 );

    // wait for data byte
    WAIT_FOR_TWINT;

    // check for timeout and check status
    if ( !TWI_READY ) return 9;
    if ( TW_STATUS !=
         ( ( count != 1 ) ? TW_MR_DATA_ACK : TW_MR_DATA_NACK ) )
    {
      generateStop( );
      return 10;
    } // end if

    // read data byte
    *(data++) = TWDR;

  } while ( --count != 0 );

  // generate STOP
  rc = generateStop( );
  if ( rc != 0 ) return rc;

  return 0;

} // end read data



/********************************************************************************

                     twiDecTo - decrement time-out counter

********************************************************************************/

void
twiDecTo(
  void
)
{

  if ( toCounter != 0 ) toCounter--;

} // end twiDecTo
