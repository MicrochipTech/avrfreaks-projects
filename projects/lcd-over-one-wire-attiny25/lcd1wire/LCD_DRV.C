#include "main.h"
#include "lcd_drv.h"


static void lcd_nibble( u8 d )
{
  cli();
  LCD_D4 = 0; if( d & 1<<4 ) LCD_D4 = 1;
  LCD_D5 = 0; if( d & 1<<5 ) LCD_D5 = 1;
  LCD_D6 = 0; if( d & 1<<6 ) LCD_D6 = 1;
  LCD_D7 = 0; if( d & 1<<7 ) LCD_D7 = 1;

  RXD_DDR = 1;

  LCD_E0 = 1;
  _delay_us( 1 );			// 1us
  LCD_E0 = 0;

  RXD_DDR = 0;				// shared with UART input pin
  RXD = 1;				// pullup on
  sei();
}


static void lcd_byte( u8 d )
{
  lcd_nibble( d );
  lcd_nibble( d<<4 );
  _delay_us( 45 );			// 45us
}


void lcd_command( u8 d )
{
  LCD_RS = 0;
  lcd_byte( d );
  switch( d ){
    case 1:
    case 2:
    case 3: _delay_ms( 2 );		// wait 2ms
  }
}


void lcd_data( u8 d )
{
  LCD_RS = 1;
  lcd_byte( d );
}


void lcd_text( u8 *t )
{
  while( *t ){
    lcd_data( *t );
    t++;
  }
}


void lcd_init( void )
{
  LCD_DDR_D4 = 1;
  LCD_DDR_D5 = 1;
  LCD_DDR_D6 = 1;
  LCD_DDR_D7 = 1;
  LCD_DDR_RS = 1;
  LCD_DDR_E0 = 1;

  LCD_E0 = 0;
  LCD_RS = 0;				// send commands

  _delay_ms( 15 );			// wait 15ms

  lcd_nibble( 0x30 );
  _delay_ms( 5 );			// wait >4.1ms

  lcd_nibble( 0x30 );
  _delay_us( 100 );			// wait >100æs

  lcd_nibble( 0x30 );			// 8 bit mode
  _delay_us( 100 );			// wait >100æs

  lcd_nibble( 0x20 );			// 4 bit mode
  _delay_us( 100 );			// wait >100æs

  lcd_command( 0x28 );			// 2 lines 5*7
  lcd_command( 0x08 );			// display off
  lcd_command( 0x01 );			// display clear
  lcd_command( 0x06 );                  // cursor increment
  lcd_command( 0x0C );			// on, no cursor, no blink
}


void lcd_pos( u8 line, u8 column )
{
  if( line & 1 )
    column += 0x40;

  lcd_command( 0x80 + column );
}
