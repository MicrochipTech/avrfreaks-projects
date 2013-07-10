#ifndef _LCD_H_
#define _LCD_H_

#include "avr_compiler_.h"
#include "delay.h"
#include "variable.h"

///////////////////////////////////////////////////////
#define	BLACK	        0x0000
#define	BLUE	        0x001F
#define	RED 	        0xF800
#define	GREEN 	        0x07E0
#define CYAN	        0x07FF
#define MAGENTA         0xF81F
#define YELLOW	        0xFFE0
#define WHITE	        0xFFFF		

#define ID_AM           011

#define DATA_LCD_DDR    PORTE_DIR
#define DATA_LCD_PORT   PORTE_OUT
#define DATA_LCD_PIN    PORTE_IN

#define DATA_INPUT()    DATA_LCD_DDR = 0x00;
#define DATA_OUPUT()    DATA_LCD_DDR = 0xFF;

#define LCD_CS_DDR      PORTF_DIR
#define LCD_CS_PORT     PORTF_OUT
#define LCD_CS_PORTSET  PORTF_OUTSET
#define LCD_CS_PORTCLR  PORTF_OUTCLR
#define LCD_CS_PIN      PORTF_IN
#define LCD_CS_BIT      1

#define LCD_RS_DDR      PORTF_DIR
#define LCD_RS_PORT     PORTF_OUT
#define LCD_RS_PORTSET  PORTF_OUTSET
#define LCD_RS_PORTCLR  PORTF_OUTCLR
#define LCD_RS_PIN      PORTF_IN
#define LCD_RS_BIT       0

#define LCD_RD_DDR      PORTF_DIR
#define LCD_RD_PORT     PORTF_OUT
#define LCD_RD_PORTSET  PORTF_OUTSET
#define LCD_RD_PORTCLR  PORTF_OUTCLR
#define LCD_RD_PIN      PORTF_IN
#define LCD_RD_BIT      2

#define LCD_WR_DDR      PORTC_DIR
#define LCD_WR_PORT     PORTC_OUT
#define LCD_WR_PORTSET  PORTC_OUTSET
#define LCD_WR_PORTCLR  PORTC_OUTCLR
#define LCD_WR_PIN      PORTC_IN
#define LCD_WR_BIT      0

#define LCD_RST_DDR     PORTF_DIR
#define LCD_RST_PORT    PORTF_OUT
#define LCD_RST_PORTSET PORTF_OUTSET
#define LCD_RST_PORTCLR PORTF_OUTCLR
#define LCD_RST_PIN     PORTF_IN
#define LCD_RST_BIT     4

#define LCD_BL_DDR      PORTF_DIR
#define LCD_BL_PORT     PORTF_OUT
#define LCD_BL_PORTSET  PORTF_OUTSET
#define LCD_BL_PORTCLR  PORTF_OUTCLR
#define LCD_BL_PIN      PORTF_IN
#define LCD_BL_BIT      5

#define LCD_CS_H() LCD_CS_PORTSET=1<<LCD_CS_BIT
#define LCD_CS_L() LCD_CS_PORTCLR=1<<LCD_CS_BIT

#define LCD_RS_H() LCD_RS_PORTSET=1<<LCD_RS_BIT
#define LCD_RS_L() LCD_RS_PORTCLR=1<<LCD_RS_BIT

#define LCD_WR_L() LCD_WR_PORTSET=1<<LCD_WR_BIT
#define LCD_WR_H() LCD_WR_PORTCLR=1<<LCD_WR_BIT

#define LCD_RD_H() LCD_RD_PORTSET=1<<LCD_RD_BIT
#define LCD_RD_L() LCD_RD_PORTCLR=1<<LCD_RD_BIT

#define LCD_RST_H() LCD_RST_PORTSET=1<<LCD_RST_BIT
#define LCD_RST_L() LCD_RST_PORTCLR=1<<LCD_RST_BIT

#define LCD_BL_H() LCD_BL_PORTSET=1<<LCD_BL_BIT
#define LCD_BL_L() LCD_BL_PORTCLR=1<<LCD_BL_BIT

#define LCD_PORT_INI(){ LCD_BL_DDR |= 1<<LCD_BL_BIT;\
			LCD_CS_DDR |= 1<<LCD_CS_BIT;\
			LCD_RS_DDR |= 1<<LCD_RS_BIT;\
			LCD_WR_DDR |= 1<<LCD_WR_BIT;\
			LCD_RD_DDR |= 1<<LCD_RD_BIT;\
			LCD_RST_DDR|= 1<<LCD_RST_BIT;\
                      }

void GUI_Point(unsigned int x, unsigned int y,unsigned int color) ;
void LCD_WR_REG(unsigned int index,unsigned int data);
void LCD_WR_REG16(unsigned int index);
void LCD_WR_DATA16(unsigned int data);
void LCD_Init(void);
void LCD_Clear(unsigned int Color);
void LCD_write_english(unsigned char data,unsigned int color,unsigned int xcolor);
void LCD_write_english_string(unsigned int x,unsigned int y,/*unsigned*/ char *s,unsigned int color,unsigned int xcolor);
void LCD_SetCursor(unsigned int Xpos, unsigned int Ypos);
void Lcd_SetBox(unsigned int xStart,unsigned int yStart,unsigned int xLong,unsigned int yLong);
void Test(void);

#endif