#ifndef _LCD_DRIVER_H
#define _LCD_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/////////////////////////////////////////////////////
/*
#define SPIPORT PORTA
#define SPIDDR DDRA
#define CS 3
#define CLK 2
#define SDA 1
#define RESET 0

#define cbi(reg, bit) (reg&=~(1<<bit))
#define sbi(reg, bit) (reg|= (1<<bit))

#define CS_0 cbi(SPIPORT,CS);
#define CS_1 sbi(SPIPORT,CS);
#define CLK0 cbi(SPIPORT,CLK);
#define CLK1 sbi(SPIPORT,CLK);
#define SDA0 cbi(SPIPORT,SDA);
#define SDA1 sbi(SPIPORT,SDA);
#define RESET0 cbi(SPIPORT,RESET);
#define RESET1 sbi(SPIPORT,RESET);

#define CS_BIT 0x08//0x80
#define SCLK_BIT 0x04//0x40
#define SDATA_BIT 0x02//0x20
#define RST_BIT 0x00
*/

void waitms(int ms);
const  char hello[] PROGMEM = " ÇÀÏÓÑÊ ÏÐÎØÅË !!! ";
const  char mounting_FAT[] PROGMEM = "mounting FAT-";
const  char move_to_DIR_SKINS[] PROGMEM = "move to DIR SKINS- ";


/////////////////////////////////////////////////////

void init_lcd(void);
void window(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy);
void clrscr(uint8_t x, uint8_t y, uint8_t dx, uint8_t dy,uint8_t bg); //ñîçäàíèå îêíà ñ öâåòîì bg

void send_byte_(uint8_t data);

void drawLine(int x1, int y1, int x2, int y2, uint8_t fg, uint8_t l);

void set_Pixel(int x1,int y1,uint8_t fg, uint8_t l);


uint8_t putch(uint8_t x, uint8_t y, char c, uint8_t fg, uint8_t bg);
uint8_t putstr_pgm(uint8_t x, uint8_t y, PGM_P p, uint8_t fg, uint8_t bg);
uint8_t putstr_ram(uint8_t x, uint8_t y, char * p, uint8_t fg, uint8_t bg);
uint8_t printDecimal(uint8_t x, uint8_t y, uint32_t val, uint8_t fg, uint8_t bg);


void lcd_set_brightness_parms(uint8_t a, uint8_t b);
void test_all_char(void);
void bmp_rcv_uart(void);

//void print_bmp_sd(FIL *fp);



#endif //_LCD_DRIVER_H
