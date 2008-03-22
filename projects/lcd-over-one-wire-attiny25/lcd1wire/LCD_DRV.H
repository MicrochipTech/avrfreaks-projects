#ifndef _lcd_drv_h_
#define _lcd_drv_h_


void lcd_command( u8 d );
void lcd_data( u8 d );
void lcd_text( u8 *t );
void lcd_init( void );
void lcd_pos( u8 line, u8 column );


#endif
