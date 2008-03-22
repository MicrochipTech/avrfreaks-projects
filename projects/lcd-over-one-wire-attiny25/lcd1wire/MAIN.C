#include "main.h"
#include "abaud.h"
#include "lcd_drv.h"
#include "suart.h"


u8 hex( void )
{
  u8 i = sgetchar();

  switch( i ){
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': i -= '0'; break;

    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f': i += 10 - 'a'; break;

    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F': i += 10 - 'A'; break;

    default: i = 0xFF;
  }
  return i;
}


void commhex( void )
{
  u8 h, l;

  h = hex();
  if( h > 15 ) return;
  l = hex();
  if( l > 15 ) return;
  lcd_command( h * 16 + l );
}


void texthex( void )
{
  u8 h, l;

  for(;;){
    h = hex();
    if( h > 15 ) return;
    l = hex();
    if( l > 15 ) return;
    lcd_data( h * 16 + l );
  }
}


void textbin( void )
{
  for(;;){
    u8 i = sgetchar();

    switch( i ){
      case  0:
      case 10:
      case 13: return;

      default: lcd_data( i );
    }
  }
}


int main( void )
{
  lcd_init();
  lcd_pos( 0, 0 );
  lcd_text( (u8*)"Hello Peter" );

  while( suart_init( abaud() ));
  lcd_command( 1 );			// clear LCD

  for(;;){
    switch( sgetchar() ){

      case 'c': lcd_command( sgetchar() ); break;

      case 'C': commhex(); break;

      case 'd': textbin(); break;

      case 'D': texthex(); break;
    }
  }
}
