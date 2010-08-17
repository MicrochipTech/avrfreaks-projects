/*-------------------------------------------------------------------
  ktms1201.c   by Jack Botner

  Collection of C functions for usng the KTM-S1201 12 digit
  serial LCD module.

  When using these functions, keep in mind that asserting /CS
  causes the address pointer in the LCD to be set to 0. This
  may cause unexpected results.

  jb110809 Added delays to try to eliminate display errors that
           occasionally pop up.
  jb123009 Reorganize delays to try to eliminate display errors
           that still pop up (although less often)
-------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "ktms1201.h"

#define KTM_S1201_COMMAND	1		// data_out parameter
#define KTM_S1201_DATA		0

// This table maps part of the ASCII code to nibbles that
// correspond to the 7 segments of each LCD digit. A 7 segment
// display is designed to display the numbers 0 to 9 only.
// However with a stretch of the imagination, most letters can
// be accomodated. The ASCII range supported is '-' (x'2d')
// through 'Z' (x'5a'), although a few of the symbols cannot
// be mapped. Lower case letters are translated to uppercase
// to keep the table short (although letters are displayed in
// upper or lower case, whichever fits best). Codes not 
// supported will be displayed as blank.

static const uint8_t LCD_SegTable[] PROGMEM =
{
    0x02,     // '-'
    0x80,     // '.'
    0x00,     // '/'
    0x7d,     // '0'
    0x60,     // '1'
    0x3e,     // '2
    0x7a,     // '3
    0x63,     // '4
    0x5b,     // '5
    0x5f,     // '6
    0x70,     // '7
    0x7f,     // '8
    0x7b,     // '9'
    0x00,     // ':'
    0x00,     // ';'
    0x00,     // '<'
    0x00,     // '='
    0x00,     // '>'
    0x00,     // '?'
    0x00,     // '@'
    0x77,     // 'A' (+ 'a')
    0x4f,     // 'B' (+ 'b')
    0x0e,     // 'C' (+ 'c')
    0x6e,     // 'D' (+ 'd')
    0x1f,     // 'E' (+ 'e')
    0x17,     // 'F' (+ 'f')
    0x7b,     // 'G' (+ 'g')
    0x67,     // 'H' (+ 'h')
    0x05,     // 'I' (+ 'i')
    0x6c,     // 'J' (+ 'j')
    0x67,     // 'K' (+ 'k')
    0x0d,     // 'L' (+ 'l')
    0x54,     // 'M' (+ 'm')
    0x46,     // 'N' (+ 'n')
    0x7d,     // 'O' (+ 'o')
    0x37,     // 'P' (+ 'p')
    0x73,     // 'Q' (+ 'q')
    0x06,     // 'R' (+ 'r')
    0x5b,     // 'S' (+ 's')
    0x0f,     // 'T' (+ 't')
    0x6d,     // 'U' (+ 'u')
    0x4c,     // 'V' (+ 'v')
    0x29,     // 'W' (+ 'w')
    0x67,     // 'X' (+ 'x')
    0x6b,     // 'Y' (+ 'y')
    0x3e      // 'Z' (+ 'z')
};

// Internal functions

static void data_out( unsigned char *pucBuffer, int8_t cBytes,
					  uint8_t bCommand );

/*-------------------------------------------------------------------
  LCD_init()

  Initialize the KTM-S1201: segment decoder on, unsynchronized
  transfer mode.
-------------------------------------------------------------------*/
void LCD_init()
{
  static unsigned char ucBuffer[6] =
  { 0x40, 		// mode set
	0x30,		// use unsynchronized transfer
	0x18,		// blink off
	0x11,		// display on
	0x15,		// segment decoder on
	0x20 };		// clear data memory

  clear_bit( LCD_CS_PORT, LCD_CS_PIN );				// assert /CS
  // wait a few us for /BUSY to exit high impedance state
  _delay_loop_1(10); 

  data_out( ucBuffer, 6, 1 );

  set_bit( LCD_CS_PORT, LCD_CS_PIN );				// clear /CS
}

/*-------------------------------------------------------------------
  LCD_blink(...)

  Turn blinking on or off. Use BLINK_ defines for parameter.

  Observation: SLOW blinks on and off in approximately 1 second
  			   FAST blinks twice the speed as slow
			   When blinking, some segments stay on all the time!
					why? (looks like crap).
-------------------------------------------------------------------*/
void LCD_blink( char bBlinkReq )
{
  static unsigned char ucBuffer[1] = { 0x18 }; 		// blinking off

  clear_bit( LCD_CS_PORT, LCD_CS_PIN );				// assert /CS
  // wait a few us for /BUSY to exit high impedance state
  _delay_loop_1(10); 

  if ( bBlinkReq == BLINK_ON_SLOW )
    ucBuffer[0] = 0x1a;								// blinking slow
  else
	if ( bBlinkReq == BLINK_ON_FAST )
      ucBuffer[0] = 0x1b;							// blinking fast

  data_out( ucBuffer, 1, KTM_S1201_COMMAND );

  set_bit( LCD_CS_PORT, LCD_CS_PIN );				// clear /CS
}

/*-------------------------------------------------------------------
  LCD_display_number(...)

  Display a number on the LCD at the specified position. Up to 12 
  digits are possible.

  cPosition = starting position relative to the left hand side,
  which is 0.
-------------------------------------------------------------------*/
void LCD_display_number( int32_t lNumber, int8_t cPosition )
{
  char		cBuffer[16];

  ltoa( lNumber, cBuffer, 10 );
  LCD_puts( cBuffer, cPosition );
}

/*-------------------------------------------------------------------
  LCD_puts(...)

  Send a string to the KTM-S1201. Allowed characters are
  '0' - '9' : numeric digits
  ' ' : blank
  '.' : decimal point
  '-' ": minus sign

  cPosition = starting position relative to the left hand side,
  which is 0.

  Limitation: only one decimal point is supported in the input
  string. If you need more decimal points, use 
  LCD_insert_decimal().
-------------------------------------------------------------------*/
void LCD_puts( char *pcBuffer, int8_t cPosition )
{
  static unsigned char ucDecimal[4] = { 0x14, 0xe0, 0xb8, 0x15 };
  static unsigned char ucDecoderOn[1] = { 0x15 };
  static unsigned char ucClear[1] = { 0x20 };
  static unsigned char ucSpace[1] = { 0x0f };
  static unsigned char ucMinus[1] = { 0x0a };

  unsigned char ucBuffer[2];
  int8_t		cPos, ii, jj, nn, cDec=0, cDpos=0, nChars;

  clear_bit( LCD_CS_PORT, LCD_CS_PIN );				// assert /CS
  // wait a few us for /BUSY to exit high impedance state
  _delay_loop_1(10); 

  data_out( ucClear, 1, KTM_S1201_COMMAND );		// clear memory
  data_out( ucDecoderOn, 1, KTM_S1201_COMMAND );	// decoder on

  // Derive the number of characters to be displayed
  // so that the proper position can be determined.
  // Remember that the LCD starts at the right hand side,
  // but our data position is from the left hand side.
  
  nn = strlen( pcBuffer );
  for ( ii=0, nChars=0; ii<nn; ++ii )
  {
	if ( pcBuffer[ii] != '.' )
	  ++nChars;
  }

  cPos = 12 - ( cPosition + nChars );
  if ( cPos > 11 ) cPos = 11;
  if ( cPos < 0 ) cPos = 0;
  ucBuffer[0] = 0xe0 | ( 2 * cPos );
  data_out( ucBuffer, 1, KTM_S1201_COMMAND );

  // The order of the digits must be reversed going in

  jj = nn - 1;			// access last digit
  for ( ii=0; ii<nn; ++ii )
  {
    if ( pcBuffer[jj] == '.' )
	{
	  // Remember but don't send any command now.
	  cDec = 1;
	  cDpos = cPos;
	}
	else
      if ( pcBuffer[jj] == ' ' )
	  {
		data_out( ucSpace, 1, KTM_S1201_DATA );
		++cPos;
	  }
	  else
        if ( pcBuffer[jj] == '-' )
		{
		  data_out(	ucMinus, 1, KTM_S1201_DATA );
		  ++cPos;
		}
	    else
          if ( pcBuffer[jj] >= '0' && pcBuffer[jj] <= '9' )
	      {
		    ucBuffer[0] = pcBuffer[jj] - '0';
		    data_out( ucBuffer, 1, KTM_S1201_DATA );
			++cPos;
	      }
	    
	--jj;
  }

  if ( cDec )
  {
	ucDecimal[1] = 0xe0 | ( 2 * cDpos );
  	data_out( ucDecimal, 4, KTM_S1201_COMMAND );
  }

  set_bit( LCD_CS_PORT, LCD_CS_PIN );				// clear /CS
}

/*-------------------------------------------------------------------
  LCD_puts2(...)

  Send a string to the KTM-S1201. Allowed characters are
  'A'-'Z' or 'a'-'z' : letter
  '0' - '9' : numeric digits
  ' ' : blank
  '.' : decimal point
  '-' ": minus sign

  cPosition = starting position relative to the left hand side,
  which is 0.

  This function does its own decoding and sends the data to the
  LCD, making crude letters possible.

  If a decimal point occurs in the string, the entire character
  position will be taken by the decimal point. To show a character
  and decimal point together, leave the decimal out of the string
  and use LCD_insert_decimal().
-------------------------------------------------------------------*/
void LCD_puts2( char *pcBuffer, int8_t cPosition )
{
  static unsigned char ucDecoderOff[1] = { 0x14 };
  static unsigned char ucClear[1] = { 0x20 };

  unsigned char ucBuffer[2];
  int8_t		cPos, ii, jj, nChars;
  uint8_t		ucByte, ucChar, ucIndex;

  clear_bit( LCD_CS_PORT, LCD_CS_PIN );				// assert /CS
  // wait a few us for /BUSY to exit high impedance state
  _delay_loop_1(10); 

  data_out( ucClear, 1, KTM_S1201_COMMAND );		// clear memory
  data_out( ucDecoderOff, 1, KTM_S1201_COMMAND );	// decoder off

  // Derive the number of characters to be displayed
  // so that the proper position can be determined.
  // Remember that the LCD starts at the right hand side,
  // but our data position is from the left hand side.
  
  nChars = strlen( pcBuffer );
  cPos = 12 - ( cPosition + nChars );
  if ( cPos > 11 ) cPos = 11;
  if ( cPos < 0 ) cPos = 0;
  ucBuffer[0] = 0xe0 | ( 2 * cPos );
  data_out( ucBuffer, 1, KTM_S1201_COMMAND );

  // The order of the digits must be reversed going in

  jj = nChars - 1;			// access the last digit
  for ( ii=0; ii<nChars; ++ii )
  {
	ucChar = pcBuffer[jj];

	if ( ucChar < 0x2d )
	  ucIndex = 2;
	else
	{
	  if ( ucChar >= 'a' && ucChar <= 'z' )
		ucChar -= 32;

	  if ( ucChar > 'Z' )
		ucIndex = 2;
	  else
		ucIndex = ucChar - 0x2d;
	}

	memcpy_P( &ucByte, &LCD_SegTable[ucIndex], 1 );
	ucBuffer[1] = 0xd0 | ( ucByte & 0x0f );
	ucBuffer[0] = 0xd0 | ( ucByte >> 4 );
	data_out( ucBuffer, 2, KTM_S1201_COMMAND );

	++cPos;	    
	--jj;
  }

  set_bit( LCD_CS_PORT, LCD_CS_PIN );				// clear /CS
}

/*-------------------------------------------------------------------
  LCD_insert_decimal(...)

  Insert a decimal point at the position specified (relative
  to the left hand side).
-------------------------------------------------------------------*/
void LCD_insert_decimal( int8_t cPosition )
{
  static unsigned char  ucDecimal[4] = { 0x14, 0xe0, 0xb8, 0x15 };
  int8_t				cPos;

  if ( cPosition > 11 ) cPosition = 11;
  if ( cPosition < 0 )  cPosition = 0;

  clear_bit( LCD_CS_PORT, LCD_CS_PIN );				// assert /CS
  // wait a few us for /BUSY to exit high impedance state
  _delay_loop_1(10); 

  cPos = 11 - cPosition;
  if ( cPos > 11 ) cPos = 11;
  if ( cPos < 0 ) cPos = 0;
  ucDecimal[1] = 0xe0 | ( 2 * cPos );
  data_out( ucDecimal, 4, KTM_S1201_COMMAND );

  set_bit( LCD_CS_PORT, LCD_CS_PIN );				// clear /CS
}

/*-------------------------------------------------------------------
  data_out(...)

  Note: /CS must be set before calling this function, and
  reset afterwards (because its the release of /CS that makes the
  LCD update its memory).
-------------------------------------------------------------------*/
static void data_out( unsigned char *pucBuffer, int8_t cBytes,
					  uint8_t bCommand )
{
  uint8_t		ucMask;
  int8_t		ii, jj;

  if ( bCommand )
    set_bit( LCD_CD_PORT, LCD_CD_PIN );				// command
  else
	clear_bit( LCD_CD_PORT, LCD_CD_PIN );			// data

  for ( ii=0; ii<cBytes; ++ii )
  {
    // Wait for /BUSY to  be asserted
	while ( !test_bit( LCD_BUSY_PORT, LCD_BUSY_PIN ) )
	  _delay_loop_1(2); 

	LCD_Delay();		// jb123009

	for ( jj=0, ucMask=0x80; jj<8; ++jj )
	{
	  clear_bit( LCD_SCK_PORT, LCD_SCK_PIN );		// assert /SCK
	  LCD_Delay();

	  if ( pucBuffer[ii] & ucMask )
	    set_bit( LCD_SI_PORT, LCD_SI_PIN );
	  else
	    clear_bit( LCD_SI_PORT, LCD_SI_PIN );

	  LCD_Delay();
	  set_bit( LCD_SCK_PORT, LCD_SCK_PIN );			// clear /SCK
	  LCD_Delay();
	  LCD_Delay();
	  ucMask = ucMask >> 1;							// address next bit
	}
  }
}

