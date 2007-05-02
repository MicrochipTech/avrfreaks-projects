#ifndef __LCD_H
#define __LCD_H
/************************************************************************
Projekti  : 44780 LCD-Näytön Ohjausfunktiot, atMega32
Versio    : 2.0
Päivä     : 30-Jan-2005
Tekijä    : Janne Särkilahti / Seppo Stenberg
Kommentit : Koodia saa käyttää vapaasti, mutta jos parannat sitä,
            ottaisin mielelläni parannetun version koodista.
Ver. Hist.: 2.0 (Janne Särkilahti)
            Koodia kirjoitettu osittain uusiks, koska se ei vielä toimi
            täydellisesti uuden porttivalintasysteemin kanssa ja
            muutenkin koodin rakennetta muutettu oleellisesti. Myös
            uusia toimintoja lisätty. Loppu koodista myös englanniksi,
            versiohistoria seuraavassa versiossa.

            1.1 (Seppo Stenberg)
            Versiossa muutettu funktioidet nimet.
            InitLCD(uint8_t portti) lcd-näytön portti annetaan numerona
            1 -> Port A
            2 -> Port B
            3 -> Port C
            4 -> Port D
            Myös osittain koodia muutettu englannin kieliseksi

            1.0 (Janne Särkilahti
            Ensimmäin kunnon julkinen versio. Mukana nyt kaikki
            perusfunktiot ja edelliseen versioon verrattuna
            tehty viimeiset bugikorjaukset.

            1.0_RC_3 (Janne Särkilahti)
            Viimeinen RC version, muuten toimiva, paitsi Hex
            lukuna kirjoitus puuttuu. Korjattu Int luvun
            kirjoituksessa ollut bugi, mikä tulosti ylimääräisen
            tyjän merkin luvun perään.

            1.0_RC_2 (Janne Särkilahti)
            Koodi kirjoitettu aikalailla uusiksi, koska lisätty
            monen näytön tuki. Pikkusia bugikorjauksia koodin
            uudelleen kirjoituksen myötä myös.

            1.0_RC_1 (Janne Särkilahti)
            Ensimmäinen julkinen versio.


************************************************************************/
#include <avr/io.h>

/* -- 10-pin connector --*/
/* - Portti  - */
/*  -- | D7 | D6 | D5 | D4 |   |  E  | R/W | R/S | -- */

/*    ----------_______--------- */
/*    | +5 | R/W |    | D5 | D7 | */
/*    |-------------------------| */
/*    | GND| R/S | E  | D4 | D6 | */
/*    -------------------------- */

/* <------------------------------------------> */
/* |  Lcd display properties                  | */
/* <------------------------------------------> */
/* |  These defines lcd display properties    | */
/* |  --------------------------------------  | */
/* |  ROWS = total rows on lcd display        | */
/* |  CHARS = total chars in one row          | */
/* <------------------------------------------> */
#define ROWS 4
#define CHARS 20

/* <------------------------------------------> */
/* |  MCU properties                          | */
/* <------------------------------------------> */
/* |  This defines mcu clock speed            | */
/* |  --------------------------------------  | */
/* |  CLOCK = mcu clock speed (MHz)           | */
/* <------------------------------------------> */
#define CLOCK 6

/* <------------------------------------------> */
/* |  Port macros                             | */
/* <------------------------------------------> */
/* |  These macros enable using ports with    | */
/* |  numbers 1-4.                            | */
/* |  --------------------------------------  | */
/* |  port = port number ( 1 - 4 )            | */
/* <------------------------------------------> */
//#define  PIN(port) ( *( volatile unsigned char * )( 0x19 - 3 * (port - 1) + 0x20 ) )

//#define  DDR(port) ( *( volatile unsigned char * )( 0x1A - 3 * (port - 1) + 0x20 ) )
//void DDR(uint8_t port, uint8_t data);

//#define PORT(port) ( *( volatile unsigned char * )( 0x1B - 3 * (port - 1) + 0x20 ) )
//uint8_t A=1, B=2, C=3, D=4;
void InitLCD( uint8_t port );
/* <------------------------------------------> */
/* |  Function InitLCD                        | */
/* <------------------------------------------> */
/* |  Initializes LCD                         | */
/* |  --------------------------------------  | */
/* |  port:                                   | */
/* |  1 = Port A, 2 = Port B                  | */
/* |  3 = Port C, 4 = Port D                  | */
/* <------------------------------------------> */

uint8_t SetLCDPort( uint8_t po );
/* <------------------------------------------> */
/* |  Function SetPort                        | */
/* <------------------------------------------> */
/* |  activates port                          | */
/* |  --------------------------------------  | */
/* |  port:                                   | */
/* |  1 = Port A, 2 = Port B                  | */
/* |  3 = Port C, 4 = Port D                  | */
/* |                                          | */
/* |  port < 1 or port > 4 only return port,  | */
/* |  what is active, else set and return.    | */
/* <------------------------------------------> */

void Delay_ms( uint16_t msec );
/* <------------------------------------------> */
/* |  Function Delay_ms                       | */
/* <------------------------------------------> */
/* |  Delay of time in milliseconds (about)   | */
/* |  --------------------------------------  | */
/* |  msec = time in millisecs (max. 65535)   | */
/* <------------------------------------------> */

void Delay_us( uint16_t usec );
/* <------------------------------------------> */
/* |  Function Delay_us                       | */
/* <------------------------------------------> */
/* |  Delay of time in microsecond (about)   | */
/* |  --------------------------------------  | */
/* |  usec = time in microsecs (max. 65535)   | */
/* <------------------------------------------> */

void WaitLCD( void );
/* <------------------------------------------> */
/* |  Function WaitLCD                        | */
/* <------------------------------------------> */
/* |  Read LCD busy flag and wait             | */
/* |  until it change to "0"                  | */
/* <------------------------------------------> */

void ClearLCD( void );
/* <------------------------------------------> */
/* |  Function ClearLCD                       | */
/* <------------------------------------------> */
/* |  Clears LCD and set cursor to            | */
/* |  row 1, place 1                          | */
/* <------------------------------------------> */

void SetLCDCursor( uint8_t row, uint8_t place );
/* <------------------------------------------> */
/* |  Function SetLCDCursor                   | */
/* <------------------------------------------> */
/* |  Moves cursor to defined position        | */
/* |  --------------------------------------  | */
/* |  row = row ( 1 - ROWS )                  | */
/* |  place = place in the row ( 1 - CHARS )  | */
/* <------------------------------------------> */

void LCD( uint8_t data, uint8_t mode );
/* <------------------------------------------> */
/* |  Function LCD                            | */
/* <------------------------------------------> */
/* |  Modify 8bit data to LCD                 | */
/* |  --------------------------------------  | */
/* |  data = 8bit data                        | */
/* |  mode = LCD control mode                 | */
/* |  -- mode 0 = modify LCD settings         | */
/* |  -- mode 1 = write data to display       | */
/* |  -- mode 2 = read BF and addr. counter   | */
/* |  -- else mode = 1                        | */
/* <------------------------------------------> */

void DDR(uint8_t port, uint8_t data);
/* <------------------------------------------> */
/* |  Function DDR                            | */
/* <------------------------------------------> */
/* |  Defines port input/output               | */
/* |  --------------------------------------  | */
/* |  port:                                   | */
/* |  1 = Port A, 2 = Port B                  | */
/* |  3 = Port C, 4 = Port D                  | */
/* |  data = input/output defination          | */
/* <------------------------------------------> */

void PORT(uint8_t port, uint8_t data);
/* <------------------------------------------> */
/* |  Function DDR                            | */
/* <------------------------------------------> */
/* |  Write data onto port                    | */
/* |  --------------------------------------  | */
/* |  port:                                   | */
/* |  1 = Port A, 2 = Port B                  | */
/* |  3 = Port C, 4 = Port D                  | */
/* |  data = 8 bit data                       | */
/* <------------------------------------------> */

uint8_t PIN(uint8_t port);
/* <------------------------------------------> */
/* |  Function DDR                            | */
/* <------------------------------------------> */
/* |  Returns data from port                  | */
/* |  --------------------------------------  | */
/* |  port:                                   | */
/* |  1 = Port A, 2 = Port B                  | */
/* |  3 = Port C, 4 = Port D                  | */
/* |  data = 8 bit data                       | */
/* <------------------------------------------> */

void Write( uint8_t row, uint8_t place, uint8_t *text );
/* <------------------------------------------> */
/* |  Function Write                          | */
/* <------------------------------------------> */
/* |  Writes text onto LCD                    | */
/* |  --------------------------------------  | */
/* |  row = row ( 1 - ROWS )                  | */
/* |  place = starting place ( 1 - CHARS )    | */
/* |  *text = address of string               | */
/* <------------------------------------------> */

void WriteInt( uint8_t row, uint8_t place, uint8_t size, uint8_t fz, uint32_t num );
/* <------------------------------------------> */
/* |  Function WriteInt                       | */
/* <------------------------------------------> */
/* |  Writes integer onto LCD                 | */
/* |  --------------------------------------  | */
/* |  row = row ( 1 - ROWS )                  | */
/* |  place = starting place ( 1 - CHARS )    | */
/* |  size = field size ( max 5 )             | */
/* |  fz = front zeros 1 -> 0023 0 -> 23      | */
/* |    -- fz = 0 -> ___23                    | */
/* |    -- fz = 1 -> 00023                    | */
/* |    -- fz = 2 -> 23___                    | */
/* |  num = integer ( 0 - 65535 )             | */
/* <------------------------------------------> */

void Delay_ms(volatile uint16_t msec )
{
  volatile uint16_t i,j;
  while( msec-- )
  {
    i=0x09*CLOCK;
    while ( i-- )
    {
      j=0x1B;
      while (j--)
        asm volatile ( "nop" : : );
    }
  }
}

void Delay_us( volatile uint16_t usec )
{
  while ( usec-- )
     asm volatile( "nop" : : );
}

void InitLCD( uint8_t port )
{
  uint8_t p;
  p = SetLCDPort( port );
  DDR(p, 0xFF);
  PORT(p,0x30);
  Delay_ms( 20 );      // Start delay
  // Start of lcd power on initialization
  // Set default parameters and 4bit transfer mode

  PORT(p, 0x34); Delay_us( 1 );
  PORT(p, 0x30); Delay_us( 1 );
  Delay_ms( 5 );

  PORT(p, 0x34); Delay_us( 1 );
  PORT(p, 0x30); Delay_us( 1 );
  Delay_us( 120 );

  PORT(p, 0x34); Delay_us( 1 );
  PORT(p, 0x30); Delay_us( 1 );
  Delay_us( 50 );

  PORT(p, 0x24); Delay_us( 1 );
  PORT(p, 0x20); Delay_us( 1 );

  LCD( 0x20, 0 );
  LCD( 0x08, 0 );
  LCD( 0x01, 0 );
  LCD( 0x06, 0 );

  // End of lcd power on initialization
  // Set lcd settings

  LCD( 0x2C, 0 ); // 4bit databus, 2 rows, 5x7 font
  LCD( 0x0C, 0 ); // Display on, cursor and blinkin of
  ClearLCD();     // Clear display
}

void DDR(uint8_t port, uint8_t data)
{
 switch(port)
 {
 	case 1: DDRA = data; break;
 	case 2: DDRB = data; break;
 	case 3: DDRC = data; break;
 	case 4: DDRD = data; break;
 }
}

void PORT(uint8_t port, uint8_t data)
{
 switch(port)
 {
 	case 1: PORTA = data; break;
 	case 2: PORTB = data; break;
 	case 3: PORTC = data; break;
 	case 4: PORTD = data; break;
 }
}

uint8_t PIN(uint8_t port)
{
 uint8_t data = 0x00;
 switch(port)
 {
 	case 1: data = PINA; break;
 	case 2: data = PINB; break;
 	case 3: data = PINC; break;
 	case 4: data = PIND; break;
 }
 return data;
}


void LCD( uint8_t data, uint8_t mode )
{
  uint8_t data_ala, p;
  p = SetLCDPort( 0 );
  DDR(p, 0xFF);
  if ( mode > 2 )
     mode = 1;
  data_ala = ( ( data << 4 ) & 0xF0 ) | mode;
  data = ( data & 0xF0 ) | mode;

  WaitLCD();
  PORT(p, data | 0x04); Delay_us( 1 );
  PORT(p, data); Delay_us( 1 );
  PORT(p, data_ala | 0x04); Delay_us( 1 );
  PORT(p, data_ala); Delay_us ( 1 );
}

uint8_t SetLCDPort( uint8_t po )
{
  static uint8_t p;
  if ( po > 0 && po < 5 )
  {
    p = po;
    return p;
  }else
    return p;
}

void WaitLCD( void )
{
  uint8_t p;
  p = SetLCDPort( 0 );                       // Set mcu's lcd datalines read mode
  DDR(p, 0x0F);
  PORT(p, 0x06);                             // Set lcd datalines read mode and enable "1"
  Delay_us( 1 );                             // Small delay
  while( PIN(p)& 0x80 );                     // Loop until lcd busyflag goes "0"
  PORT(p, PIN(p) & 0xFB); Delay_us( 1 );     // Set enable to "0"
  PORT(p, PIN(p) | 0x04); Delay_us( 1 );     // Set enable to "1" -> second part of 4bit transfer
  PORT(p, PIN(p) & 0xFB); Delay_us( 1 );     // Set enable to "0" -> end of second part
  DDR(p, 0xFF);                              // Mcu lcd data line back to write mode
  PORT(p, 0x00);                             // Lcd datalines write mode & enable to "0"
}

void SetLCDCursor( uint8_t row, uint8_t place )
{
 if(row < 3)
  LCD( ( row - 1 ) * 0x40 + 0x80 + ( place - 1 ), 0);
 else
  LCD( ( row - 3 ) * 0x40 + 0x94 + ( place - 1 ), 0);
}

void ClearLCD( void )
{
  LCD( 0x01, 0 );                            // 0x01 instruction mode clears display
}

void Write( uint8_t row, uint8_t place, uint8_t *text )
{
  uint8_t ch = 0;
  SetLCDCursor( row, place );
  while( *( text + ch ) != '\0' && place <= CHARS )
  {
    LCD( *( text + ch++ ), 1 );
    place++;
  }
}

void WriteInt( uint8_t row, uint8_t place, uint8_t size, uint8_t fz, uint32_t num )
{
  uint8_t numbers[10], i=9, em;
  if( size > 10 )
   size = 10;
  if ( fz == 0 )
     em = ' ';
  else
     em = '0';
  SetLCDCursor( row, place );
  do
  {
    if ( num > 0 )
    {
      numbers[i] = ( num % 10 ) | 0x30;
      num /= 10;
    }else
      if ( i == 9 )
        numbers[i] = 0 | 0x30;
      else if( fz != 2 )
        numbers[i] = em;
        else
          break;
  }while ( i-- );
  if ( fz != 2 )
    i = 10 - size;
  else
    i++;
  em = 0;
  do
  {
    if ( i < 10 )
      LCD( numbers[i], 1 );
    else
      LCD( ' ', 1 );
  }while( ( ( i++ < 9 && fz != 2 ) || ( em++ < size-1 && fz == 2 ) ) && place++ <= CHARS );
}

#endif

