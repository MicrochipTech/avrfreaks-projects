/********************************************************************************

DS1307 Utility

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
  05 Sep 2005  Created.

********************************************************************************/



/********************************************************************************

                                    includes

********************************************************************************/

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "twi.h"



/********************************************************************************

                                Macros & Defines

********************************************************************************/



// Timer2 Clock Select and Output Compare value

#define TIMER2_TARGET_RATE 1000UL  // target rate in microseconds

#define TIMER2_MAX_COUNT    256UL

#if \
     ( ( TIMER2_MAX_COUNT * 1000UL *    1UL ) / ( F_CPU / 1000UL ) ) > \
     TIMER2_TARGET_RATE
#  define TIMER2_PRESCALER 1
#  define TIMER2_CS        ( 0 << CS02 ) | ( 0 << CS01 ) | ( 1 << CS00 )
#elif \
     ( ( TIMER2_MAX_COUNT * 1000UL *    8UL ) / ( F_CPU / 1000UL ) ) > \
     TIMER2_TARGET_RATE
#  define TIMER2_PRESCALER 8
#  define TIMER2_CS        ( 0 << CS02 ) | ( 1 << CS01 ) | ( 0 << CS00 )
#elif \
     ( ( TIMER2_MAX_COUNT * 1000UL *   64UL ) / ( F_CPU / 1000UL ) ) > \
     TIMER2_TARGET_RATE
#  define TIMER2_PRESCALER 64
#  define TIMER2_CS        ( 0 << CS02 ) | ( 1 << CS01 ) | ( 1 << CS00 )
#elif \
     ( ( TIMER2_MAX_COUNT * 1000UL *  256UL ) / ( F_CPU / 1000UL ) ) > \
     TIMER2_TARGET_RATE
#  define TIMER2_PRESCALER 256
#  define TIMER2_CS        ( 1 << CS02 ) | ( 0 << CS01 ) | ( 0 << CS00 )
#elif \
     ( ( TIMER2_MAX_COUNT * 1000UL * 1024UL ) / ( F_CPU / 1000UL ) ) > \
     TIMER2_TARGET_RATE
#  define TIMER2_PRESCALER 1024
#  define TIMER2_CS        ( 1 << CS02 ) | ( 0 << CS01 ) | ( 1 << CS00 )
#else
#  error TIMER2_TARGET_RATE not attainable
#endif

#define TIMER2_OUTPUT_COMPARE \
     ( TIMER2_TARGET_RATE * ( F_CPU / 100UL ) ) / \
     ( TIMER2_PRESCALER * 10000UL )



// DS1307 defines
#define DS1307_SLA 0x68  // Slave Address



#ifndef UCSRB
#  ifdef UCSR1A
    // ATmega128
#   define UCSRA UCSR1A
#   define UCSRB UCSR1B
#   define UBRR UBRR1L
#   define UDR UDR1
# else
    // ATmega8
#   define UCSRA USR
#   define UCSRB UCR
# endif
#endif

#ifndef UBRR
#  define UBRR UBRRL
#endif



#define ESC 0x1B            // keyboard Esc sequence

#define ESC_SEQ_TIMEOUT 25  // ESC sequence timeout in Timer0 counts



/********************************************************************************

                                   Typedef's

********************************************************************************/

// Note: in the following structure declaration, bitfields are assigned starting
// from the low-order bit.  Some compilers assign bitfields starting from the
// high-order bit.

typedef struct {

  uint8_t seconds1  : 4;
  uint8_t seconds10 : 3;
  uint8_t ch        : 1;  // clock hold

  uint8_t minutes1  : 4;
  uint8_t minutes10 : 3;
  uint8_t           : 1;

  uint8_t hours1    : 4; 
  uint8_t hours10   : 2;
  uint8_t mode12    : 1;  // 12-hour mode (1) or 24-hour mode (0)
  uint8_t           : 1;

  uint8_t day       : 3;
  uint8_t           : 5;

  uint8_t date1     : 4;
  uint8_t date10    : 1;
  uint8_t           : 3;

  uint8_t month1    : 4;
  uint8_t month10   : 1;
  uint8_t           : 3;

  uint8_t year1     : 4;
  uint8_t year10    : 4;

  uint8_t rs        : 2;  // rate select
  uint8_t           : 2;
  uint8_t sqwe      : 1;  // square wave enable
  uint8_t           : 2;
  uint8_t out       : 1;  // output control

} ds1307Reg_t;



/********************************************************************************

                                  Global Data

********************************************************************************/

PROGMEM char     twiWriteErrorFmt[ ] = "twiWrite returned error (%d).\n";

PROGMEM char     twiReadErrorFmt[ ]  = "twiRead returned error (%d).\n";

volatile uint8_t escSeqTimeOut;  // ESC sequence timeout



/********************************************************************************

                          External Interrupt 2 Handler

********************************************************************************/

SIGNAL( SIG_OUTPUT_COMPARE2 )
{



  twiDecTo( );  // decrement TWI time-out counter



  // decrement ESC sequence timeout

  if ( escSeqTimeOut != 0 ) escSeqTimeOut--;



} // end SIGNAL( SIG_OUTPUT_COMPARE2 )



/*******************************************************************************

                                  uartPutChar

*******************************************************************************/

static
int
uartPutChar(
  char c
)
{

  if ( c == '\n' ) uartPutChar( '\r' );

  loop_until_bit_is_set( UCSRA, UDRE );

  UDR = c;

  return 0;

} // end uartPutChar



/*******************************************************************************

                                  uartGetChar

*******************************************************************************/

static
int
uartGetChar(
  void
)
{

   loop_until_bit_is_set( UCSRA, RXC );

   return UDR;

} // end uartGetChar



/*******************************************************************************

                                  showDateTime

*******************************************************************************/

static
void
showDateTime(
  void
)
{

  uint8_t     regAddress;

  ds1307Reg_t timeAndDate;

  uint8_t     rc;

  uint8_t     monthIndex;

  PGM_P       months[ 12 ] = {
       PSTR( "Jan" ), PSTR( "Feb" ), PSTR( "Mar" ), PSTR( "Apr" ),
       PSTR( "May" ), PSTR( "Jun" ), PSTR( "Jul" ), PSTR( "Aug" ),
       PSTR( "Sep" ), PSTR( "Oct" ), PSTR( "Nov" ), PSTR( "Dec" )
  };

  char        month[ 4 ];



  // write DS1307 register address

  regAddress = 0;
  rc = twiWrite( DS1307_SLA, &regAddress, 1 );
  if ( rc != 0 )
  {
    printf_P( twiWriteErrorFmt, rc );
    return;
  } // end if



  // read DS1307 registers

  rc = twiRead( DS1307_SLA, (uint8_t *) &timeAndDate, 8 );
  if ( rc != 0 )
  {
    printf_P( twiReadErrorFmt, rc );
    return;
  } // end if



  // display time

  if ( timeAndDate.mode12 == 0 )
  {
    // 24-hour mode
    printf_P(
         PSTR( "%d%d:%d%d:%d%d" ),
         timeAndDate.hours10, timeAndDate.hours1,
         timeAndDate.minutes10, timeAndDate.minutes1,
         timeAndDate.seconds10, timeAndDate.seconds1
    );
  }
  else
  {
    // 12-hour mode
    printf_P(
         PSTR( "%d%d:%d%d:%d%d %cM" ),
         timeAndDate.hours10 & 0x01, timeAndDate.hours1,
         timeAndDate.minutes10, timeAndDate.minutes1,
         timeAndDate.seconds10, timeAndDate.seconds1,
         ( ( timeAndDate.hours10 & 0x2 ) == 0 ) ? 'A' : 'P'
    );
  } // end if



  // display date and day

  monthIndex =
       timeAndDate.month1 +
       ( ( timeAndDate.month10 == 0 ) ? 0 : 10 );
  if ( ( monthIndex >= 1 ) && ( monthIndex <= 12 ) )
  {
    strcpy_P( month, months[ monthIndex - 1 ] );
  }
  else
  {
    strcpy_P( month, PSTR( "???" ) );
  } // end if
  printf_P(
       PSTR( "   %d%d %s %d%d   Day %d\n" ),
       timeAndDate.date10, timeAndDate.date1,
       month,
       timeAndDate.year10, timeAndDate.year1,
       timeAndDate.day
  );



} // end if



/*******************************************************************************

                                    setTime

*******************************************************************************/

static
void
setTime(
  char *command
)
{

  PGM_P   usage = PSTR( "Usage: thh:mm:ss or thh:mm:ssa or thh:mm:ssp" );

  uint8_t hours[ 2 ], minutes[ 2 ], seconds[ 2];

  bool    mode24, pm;

  uint8_t ioBuffer[ 4 ];

  uint8_t rc;



  if ( ( ( strlen( command ) != 9 ) && ( strlen( command ) != 10 ) ) ||
       ( command[ 3 ] != ':' ) || ( command[ 6 ] != ':' ) )
  {
    puts_P( usage );
    return;
  } // end if

  mode24 = strlen( command ) == 9;

  pm = false;
  if ( !mode24 )
  {
    // 12-hour mode, check for AM or PM
    if ( ( command[ 9 ] == 'p' ) || ( command[ 9 ] == 'P' ) )
    {
      pm = true;
    }
    else if ( ( command[ 9 ] != 'a' ) && ( command[ 9 ] != 'A' ) )
    {
      puts_P( usage );
      return;
    } // end if
  } // end if

  hours[ 0 ] = command[ 1 ] - '0';
  hours[ 1 ] = command[ 2 ] - '0';

  minutes[ 0 ] = command[ 4 ] - '0';
  minutes[ 1 ] = command[ 5 ] - '0';

  seconds[ 0 ] = command[ 7 ] - '0';
  seconds[ 1 ] = command[ 8 ] - '0';

  if ( ( hours[ 0 ] > ( mode24 ? 2 : 1 ) ) || ( hours[ 1 ] > 9 ) ||
       ( minutes[ 0 ] > 5 ) || ( minutes[ 1 ] > 9 ) ||
       ( seconds[ 0 ] > 5 ) || ( seconds[ 1 ] > 9 ) ||
       ( ( 10 * hours[ 0 ] + hours[ 1 ] ) > ( mode24 ? 23 : 12 ) ) ||
       ( !mode24 && ( hours[ 0 ] == 0 ) && ( hours[ 1 ] == 0 ) ) )
  {
    puts_P( usage );
    return;
  } // end if



  // write time

  ioBuffer[ 0 ] = 0;
  ioBuffer[ 1 ] = ( seconds[ 0 ] << 4 ) | seconds[ 1 ];
  ioBuffer[ 2 ] = ( minutes[ 0 ] << 4 ) | minutes[ 1 ];
  ioBuffer[ 3 ] =
       ( mode24 ? 0x00 : 0x40 ) |
       ( ( !mode24 && pm ) ? 0x20 : 0x00 ) |
       ( hours[ 0 ] << 4 ) | hours[ 1 ];

  rc = twiWrite( DS1307_SLA, ioBuffer, 4 );

  if ( rc != 0 )
  {
    printf_P( twiWriteErrorFmt, rc );
    return;
  } // end if



} // end setTime



/*******************************************************************************

                                    setDate

*******************************************************************************/

static
void
setDate(
  char *command
)
{

  PGM_P   usage = PSTR( "Usage: dyy-mm-dd" );

  uint8_t years[ 2 ], months[ 2 ], days[ 2 ];

  uint8_t ioBuffer[ 8 ];

  uint8_t rc;



  if ( ( strlen( command ) != 9 ) ||
       ( command[ 3 ] != '-' ) || ( command[ 6 ] != '-' ) )
  {
    puts_P( usage );
    return;
  } // end if

  years[ 0 ] = command[ 1 ] - '0';
  years[ 1 ] = command[ 2 ] - '0';

  months[ 0 ] = command[ 4 ] - '0';
  months[ 1 ] = command[ 5 ] - '0';

  days[ 0 ] = command[ 7 ] - '0';
  days[ 1 ] = command[ 8 ] - '0';

  if ( ( years[ 0 ] > 9 ) || ( years[ 1 ] > 9 ) ||
       ( months[ 0 ] > 1 ) ||
       ( ( months[ 0 ] == 0 ) && months[ 1 ] > 9 ) ||
       ( ( months[ 0 ] == 1 ) && months[ 1 ] > 2 ) ||
       ( days[ 0 ] > 3 ) || ( days[ 1 ] > 9 ) )
  {
    puts_P( usage );
    return;
  } // end if

  switch ( months[ 1 ] + ( ( months[ 0 ] == 0 ) ? 0 : 10 ) )
  {

    case 2:
      if ( ( days[ 0 ] == 3 ) || ( ( days[ 0 ] == 2 ) && ( days[ 1 ] > 9 ) ) )
      {
        puts_P( usage );
        return;
      } // end if
      break;

    case 4:
    case 6:
    case 9:
    case 11:
      if ( ( days[ 0 ] == 3 ) && ( days[ 1 ] > 0 ) )
      {
        puts_P( usage );
        return;
      } // end if
      break;

    default:
      if ( ( days[ 0 ] == 3 ) && ( days[ 1 ] > 1 ) )
      {
        puts_P( usage );
        return;
      } // end if

  }




  // write date
  ioBuffer[ 0 ] = 4;
  ioBuffer[ 1 ] = ( days[ 0 ] << 4 ) | days[ 1 ];
  ioBuffer[ 2 ] = ( months[ 0 ] << 4 ) | months[ 1 ];
  ioBuffer[ 3 ] = ( years[ 0 ] << 4 ) | years[ 1 ];

  rc = twiWrite( DS1307_SLA, ioBuffer, 4 );

  if ( rc != 0 )
  {
    printf_P( twiWriteErrorFmt, rc );
    return;
  } // end if

  

} // end setDate



/*******************************************************************************

                                     setDay

*******************************************************************************/

static
void
setDay(
  char *command
)
{

  PGM_P   usage = PSTR( "Usage: yd" );

  uint8_t day;

  uint8_t ioBuffer[ 2 ];

  uint8_t rc;



  if ( strlen( command ) != 2 )
  {
    puts_P( usage );
    return;
  } // end if

  day = command[ 1 ] - '0';

  if ( ( day < 1 ) || ( day > 7 ) )
  {
    puts_P( usage );
    return;
  } // end if



  // write day

  ioBuffer[ 0 ] = 3;
  ioBuffer[ 1 ] = day;

  rc = twiWrite( DS1307_SLA, ioBuffer, 4 );

  if ( rc != 0 )
  {
    printf_P( twiWriteErrorFmt, rc );
    return;
  } // end if



} // end setDay



/*******************************************************************************

                                 outputControl

*******************************************************************************/

static
void
outputControl(
  char *command
)
{

  PGM_P   usage = PSTR( "Usage: o0 or o1" );

  uint8_t c;

  uint8_t ioBuffer[ 2 ];

  uint8_t rc;



  if ( strlen( command ) != 2 )
  {
    puts_P( usage );
    return;
  } // end if

  c = command[ 1 ] - '0';

  if ( c > 1 )
  {
    puts_P( usage );
    return;
  } // end if



  // write output control

  ioBuffer[ 0 ] = 7;
  ioBuffer[ 1 ] = ( ( c == 0 ) ? 0 : 0x80 );

  rc = twiWrite( DS1307_SLA, ioBuffer, 4 );

  if ( rc != 0 )
  {
    printf_P( twiWriteErrorFmt, rc );
    return;
  } // end if


  
} // end outputControl



/*******************************************************************************

                               squareWaveControl

*******************************************************************************/

static
void
squareWaveControl(
  char *command
)
{

  PGM_P   usage = PSTR( "Usage: s0, s1, s2 or s3" );

  uint8_t rs;

  uint8_t ioBuffer[ 2 ];

  uint8_t rc;



  if ( strlen( command ) != 2 )
  {
    puts_P( usage );
    return;
  } // end if

  rs = command[ 1 ] - '0';

  if ( rs > 3 )
  {
    puts_P( usage );
    return;
  } // end if



  // write output control

  ioBuffer[ 0 ] = 7;
  ioBuffer[ 1 ] = 0x10 | rs;

  rc = twiWrite( DS1307_SLA, ioBuffer, 4 );

  if ( rc != 0 )
  {
    printf_P( twiWriteErrorFmt, rc );
    return;
  } // end if


  
} // end squareWaveControl
          



/*******************************************************************************

                                    showHelp

*******************************************************************************/

static
void
showHelp(
  void
)
{

  puts_P( PSTR( "? - show time, date and day of week." ) );
  puts_P( PSTR( "t - set time." ) );
  puts_P( PSTR( "d - set date." ) );
  puts_P( PSTR( "o - set SQW/OUT pin to logic 0 or 1." ) );
  puts_P( PSTR( "s - set SQW/OUT pin to squarewave output." ) );
  puts_P( PSTR( "h - help (this output)." ) );

} // end showHelp



/*******************************************************************************

                                      main

*******************************************************************************/

int
main(
  void
)
{



  // disable analog comparator

  ACSR = ( ACSR & (uint8_t) ~( _BV( ACIE ) ) ) | _BV( ACD );



  // initialize TWI

  twiInit( 10 );



  // initialize Timer/Counter2

  OCR2 = TIMER2_OUTPUT_COMPARE - 1;
                           // select CTC mode (WGM21..WGM20 = 10) and Clock Select
  TCCR2 = ( 1 << WGM21 ) | ( 0 << WGM20 ) | TIMER2_CS;
  TIMSK |= _BV( OCIE2 );  // enable Timer/Counter2 Output Compare Match interrupt



  // initialize UART

  UCSRB = _BV( TXEN ) | _BV( RXEN );  // set Tx and Rx enable

  // set baud rate to 9600
  UBRR = ( F_CPU / ( 16 * 9600UL ) ) - 1;

#ifdef TWPS0
  // has prescaler (ATmega128 & newer)
  TWSR = 0;
#endif

  TWBR = ( F_CPU / 100000UL - 16 ) / 2;

  fdevopen( uartPutChar, uartGetChar, 0 );



  // set Global Interrupt Enable

  sei( );



  puts_P( PSTR( "Enter \"h\" for help." ) );



  while ( true )
  {

    uint8_t i;

    char    c, command[ 12 ];

    for ( i = 0; ; )
    {

      c = getchar( );

      if ( ( c == '\r' ) || ( c == '\n' ) ) break;

      if ( c == ESC )
      {

        c = getchar( );
        
        if ( c == '[' || c == 'O' )
        {

          // discard all Esc sequences

          escSeqTimeOut = ESC_SEQ_TIMEOUT;
          do
          {
            if ( bit_is_set( UCSRA, RXC ) )
            {
              getchar( );
              escSeqTimeOut = ESC_SEQ_TIMEOUT;
            } // end if
          } while ( escSeqTimeOut != 0 );

          continue;

        } // end if

      } // end if

      putchar( c );

      switch ( c )
      {
        case '\b':
          if ( i > 0 )
          {
            i--;
            printf_P( PSTR( " \b" ) );
          } // end if
          break;
        default:
          if ( i < ( sizeof( command ) - 1 ) ) command[ i ] = c;
          i++;        
      } // end switch ( c)

    } // end for ( i )

    puts( "" );

    if ( i < ( sizeof( command ) - 1 ) )
    {

      command[ i ] = '\0';

      switch ( command[ 0 ] )
      {

        case '?':
          showDateTime( );
          break;

        case 't':
          setTime( command );
          break;

        case 'd':
          setDate( command );
          break;

        case 'y':
          setDay( command );
          break;

        case 'o':
          outputControl( command );
          break;

        case 's':
          squareWaveControl( command );
          break;

        case 'h':
          showHelp( );
          break;

        default:
          puts_P( PSTR( "Unknown command." ) );

      } // end switch

    }
    else
    {

      puts_P( PSTR( "Command too long." ) );

    } // end if

  } // end while ( true )



  return 0;



} // end main
