#include "LCD.h"
#include <stdlib.h>
#include <avr/pgmspace.h>

// Affichage LCD
#define EN _BV(PIND3)   // LCD Enable
#define RS _BV(PIND2)   // LCD Register select (RS=1)
#define CM 0            // LCD Commands        (RS=0)

void LCD_4bits( uint8_t data, uint8_t shift )
{  
  PORTD &= ~( RS | EN );
  PORTD = ( PORTD & 0x0F ) | (data<<4);
  _delay_us(1);
  PORTD |= ( shift | EN );
  _delay_us(1);
  PORTD &= ~( RS | EN );
  _delay_us(46);
}

void LCD_Envoi( uint8_t data, uint8_t shift )
{
  uint8_t xh = data >> 4;
  uint8_t xb = data & 0x0F;
  LCD_4bits( xh, shift );
  LCD_4bits( xb, shift );
}

void LCD_move( uint8_t Row, uint8_t Col )
{
  uint8_t Add;
  if( Row > LCDlastRow ) Row = LCDlastRow;
  if( Col > LCDlastCol ) Col = LCDlastCol;
#ifdef LCDmode1x16
  // For 1x16 display (Row ingored) ref. FDCC1601E
  Add = ( ( Col % 8 ) + 0x40 * ( ( Col / 8 ) % 2 ) );
#endif
#if defined(LCDmode4x20) || defined(LCDmode2x16)
  // For 2x16 display spec. 6672 ref. FDCC1602G
  // For 4x20 display            ref. FDCC2004B
  Add = Col;
  switch( Row )
  {
  case 0: 
    Add = Col; 
    break;
  case 1: 
    Add = Col + 0x40; 
    break;
  case 2: 
    Add = Col + 0x14; 
    break;
  case 3: 
    Add = Col + 0x54; 
    break;
  }
#endif
  LCD_Envoi( 0x80 | Add, CM );
}

void LCD_putc( uint8_t c )
{
  LCD_Envoi( c, RS );
}

void LCD_puts( char *s )
{
  for( char* p=s; *p; p++ ) LCD_Envoi( *p, RS );
}

void LCD_puts_P( char *s )  // String in PROGMEM
{
  for( char* p=s; pgm_read_byte(p); p++ ) LCD_Envoi( pgm_read_byte(p), RS );
}

void LCD_line( uint8_t l, char *s )
{
  uint8_t i;
  char* p=s;
  LCD_move( l, 0 );
  for( i=0; i <= LCDlastCol; i++ ) 
  {
    if( *p )
    {
      LCD_Envoi( *p, RS );
      p++;
    }
    else
    {
      LCD_Envoi( ' ', RS );
    }
  }
}

void LCD_line_P( uint8_t l, char *s )  // String in PROGMEM
{
  uint8_t i;
  char* p=s;
  LCD_move( l, 0 );
  for( i=0; i <= LCDlastCol; i++ ) 
  {
    if( pgm_read_byte(p) )
    {
      LCD_Envoi( pgm_read_byte(p), RS );
      p++;
    }
    else
    {
      LCD_Envoi( ' ', RS );
    }
  }
}

void LCD_cls()
{
  LCD_Envoi( 0x01, CM );
  _delay_us( 1640 );
}

void LCD_begin()
{
  //   P2..7 : EN, RS, Data
  DDRD = _BV(PIND2) | _BV(PIND3) | _BV(PIND4) | _BV(PIND5) | _BV(PIND6) | _BV(PIND7);
  PORTD &= 0x0F;
  PORTD &= ~( EN | RS );
  //  Start LCD
  _delay_ms(15);
  LCD_4bits( 0x3, CM ); // Retour en mode 8 bit
  _delay_us( 100 );
  LCD_4bits( 0x3, CM ); // 2 fois
  _delay_us( 100 );
  LCD_4bits( 0x3, CM ); // 3 fois pour etre sur ! 
  _delay_us( 100 );
  LCD_4bits( 0x2, CM ); // mode 4 bit
  LCD_Envoi( 0x0C, CM );  // 0x8 + display on + cursor off + blink off
  LCD_specialCar();
  LCD_cls();
}

void LCD_home()
{
  LCD_Envoi( 0x03, CM );
  _delay_us(1640);
}

void LCD_cursor( uint8_t CurOn, uint8_t CurBlink )
{
  uint8_t Code = 0x0C;
  if( CurOn    ) Code |= 0x02;
  if( CurBlink ) Code |= 0x01;
  LCD_Envoi( Code , CM );
}

void LCD_print32( uint32_t code, uint8_t base )
{
  char v[6];
  ultoa(code,v,base); 
  LCD_puts( v );
}

void LCD_specialCar()
{
  LCD_Envoi( 0x40, CM );
  // Char 0 SKIP
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00000, RS );
  // Char 1 PLAY
  LCD_Envoi( 0b01000, RS );
  LCD_Envoi( 0b01100, RS );
  LCD_Envoi( 0b01110, RS );
  LCD_Envoi( 0b01111, RS );
  LCD_Envoi( 0b01110, RS );
  LCD_Envoi( 0b01100, RS );
  LCD_Envoi( 0b01000, RS );
  LCD_Envoi( 0b00000, RS );
  // Char 2 STOP
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b11111, RS );
  LCD_Envoi( 0b11111, RS );
  LCD_Envoi( 0b11111, RS );
  LCD_Envoi( 0b11111, RS );
  LCD_Envoi( 0b11111, RS );
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00000, RS );
  // Char 3 PAUSE
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b11011, RS );
  LCD_Envoi( 0b11011, RS );
  LCD_Envoi( 0b11011, RS );
  LCD_Envoi( 0b11011, RS );
  LCD_Envoi( 0b11011, RS );
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00000, RS );
  // Char 4 PREV
  LCD_Envoi( 0b00001, RS );
  LCD_Envoi( 0b00011, RS );
  LCD_Envoi( 0b00111, RS );
  LCD_Envoi( 0b01111, RS );
  LCD_Envoi( 0b00111, RS );
  LCD_Envoi( 0b00011, RS );
  LCD_Envoi( 0b00001, RS );
  LCD_Envoi( 0b00000, RS );
  // Char 5 UP
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00100, RS );
  LCD_Envoi( 0b01110, RS );
  LCD_Envoi( 0b11111, RS );
  LCD_Envoi( 0b00100, RS );
  LCD_Envoi( 0b00100, RS );
  LCD_Envoi( 0b00100, RS );
  LCD_Envoi( 0b00000, RS );
  // Char 6 DOWN
  LCD_Envoi( 0b00000, RS );
  LCD_Envoi( 0b00100, RS );
  LCD_Envoi( 0b00100, RS );
  LCD_Envoi( 0b00100, RS );
  LCD_Envoi( 0b11111, RS );
  LCD_Envoi( 0b01110, RS );
  LCD_Envoi( 0b00100, RS );
  LCD_Envoi( 0b00000, RS );
  LCD_move(0,0);
}
