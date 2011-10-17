#ifndef _LCD_DRIVER_H
#define _LCD_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
//
//#define lctCmd 0
//#define lctData 1

char get_data(void);
void setPixel(uint8_t r,uint8_t g,uint8_t b) ;
void send_data(uint8_t data);
void waitms(int ms);
void send_byte (uint8_t data);
void init_lcd(void);
void window(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy);
void clrscr(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy,uint8_t bg); //создание окна с цветом bg
void lcd_setup_window(uint8_t sx, uint8_t sy, uint8_t ex, uint8_t ey);
void fillrect(uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy, uint8_t color);
void setup_pix_blit(uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy);
void pix_blit(uint8_t color);
void sendByte(uint8_t cmd, uint8_t data);
void set_Pixel(int x1,int y1,uint8_t fg, uint8_t l);
void drawLine(int x1, int y1, int x2, int y2, uint8_t fg, uint8_t l);


uint16_t cpxSetAndValidateLCDRect(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy);

uint8_t putch(uint8_t x, uint8_t y, char c, uint8_t fg, uint8_t bg);
uint8_t putstr_pgm(uint8_t x, uint8_t y, PGM_P p, uint8_t fg, uint8_t bg);
uint8_t putstr_ram(uint8_t x, uint8_t y, char * p, uint8_t fg, uint8_t bg);
uint8_t printDecimal(uint8_t x, uint8_t y, uint32_t val, uint8_t fg, uint8_t bg);


void lcd_set_brightness_parms(uint8_t a, uint8_t b);
void test_all_char(void);
void bmp_rcv_uart(void);
void test_12bit_color(void);


#endif//_LCD_DRIVER_H
