#include <avr/io.h>
#include <util/delay.h>

//#define LCDmode1x16
//#define LCDmode2x16
#define LCDmode4x20

void LCD_begin( void );
void LCD_Envoi( uint8_t data, uint8_t shift );
void LCD_putc( uint8_t c );
void LCD_puts( char *s );
void LCD_puts_P( char *s );  // String in PROGMEM
void LCD_line( uint8_t l, char *s );
void LCD_line_P( uint8_t l, char *s );  // String in PROGMEM
void LCD_cls( void );
void LCD_home( void );
void LCD_cursor( uint8_t CurOn, uint8_t CurBlink );
void LCD_move( uint8_t Row, uint8_t Col );
void LCD_print32( uint32_t code, uint8_t base );
void LCD_specialCar( void );

// void LCD_4bits( uint8_t data, uint8_t shift ) is private

#ifdef LCDmode1x16
#define LCDlastRow 0
#define LCDlastCol 15
#endif

#ifdef LCDmode2x16
#define LCDlastRow 1
#define LCDlastCol 15
#endif

#ifdef LCDmode4x20
#define LCDlastRow 3
#define LCDlastCol 19
#endif