#ifndef ILI9341_H
#define ILI9341_H

#include<avr/io.h>
#include<util/delay.h>
#include<avr/pgmspace.h>
#include<limits.h>
#include<inttypes.h>

#define controlport PORTB
#define controlddr DDRB
#define controlpin PINB
#define rstport PORTD
#define rstddr DDRD
#define rstpin PIND
#define dc 0
#define cs 1
#define rst 7
#define ILI9341_TFTHEIGHT 240 
#define ILI9341_TFTWIDTH 320




#define BLACK       0x0000      
#define NAVY        0x000F      
#define DARKGREEN   0x03E0      
#define DARKCYAN    0x03EF      
#define MAROON      0x7800      
#define PURPLE      0x780F      
#define OLIVE       0x7BE0      
#define LIGHTGREY   0xC618      
#define DARKGREY    0x7BEF      
#define BLUE        0x001F      
#define GREEN       0x07E0      
#define CYAN        0x07FF      
#define RED         0xF800     
#define MAGENTA     0xF81F      
#define YELLOW      0xFFE0      
#define WHITE       0xFFFF      
#define ORANGE      0xFD20      
#define GREENYELLOW 0xAFE5     
#define PINK        0xF81F





void ili9341_hard_init(void);
void ili9341_spi_init(void);
void ili9341_spi_send(unsigned char spi_data);
void ili9341_writecommand8(uint8_t com);
void ili9341_writedata8(uint8_t data);
void ili9341_setaddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void ili9341_hard_reset(void);
void ili9341_init(void);
void ili9341_pushcolour(uint16_t colour);
void ili9341_clear(uint16_t colour);
void ili9341_drawpixel(uint16_t x3,uint16_t y3,uint16_t colour1);
void ili9341_drawvline(uint16_t x,uint16_t y,uint16_t h,uint16_t colour);
void ili9341_drawhline(uint16_t x,uint16_t y,uint16_t w,uint16_t colour);
void ili9341_fillrect(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t colour);
void ili9341_setRotation(uint8_t x);
#endif

