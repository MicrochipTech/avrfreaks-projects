/*
 * LCD-AVR-4f.h
 *
 *  Created on: 5 May, 2014
 *      Author: prageeth
 */

#ifndef LCD_AVR_4F_H_
#define LCD_AVR_4F_H_

// LCD interface (should agree with the diagram above)
#define lcd_D7_port     PORTD                   // lcd D7 connection
#define lcd_D7_bit      PORTD7
#define lcd_D7_ddr      DDRD
#define lcd_D7_pin      PIND                    // busy flag

#define lcd_D6_port     PORTD                   // lcd D6 connection
#define lcd_D6_bit      PORTD6
#define lcd_D6_ddr      DDRD

#define lcd_D5_port     PORTD                   // lcd D5 connection
#define lcd_D5_bit      PORTD5
#define lcd_D5_ddr      DDRD

#define lcd_D4_port     PORTD                   // lcd D4 connection
#define lcd_D4_bit      PORTD4
#define lcd_D4_ddr      DDRD

#define lcd_E_port      PORTC                  // lcd Enable pin
#define lcd_E_bit       PORTC1
#define lcd_E_ddr       DDRC

#define lcd_RW_port     PORTD                   // lcd Read/Write pin
#define lcd_RW_bit      PORTD3
#define lcd_RW_ddr      DDRD

#define lcd_RS_port     PORTC                   // lcd Register Select pin
#define lcd_RS_bit      PORTC0
#define lcd_RS_ddr      DDRC

// LCD module information
#define lcd_line_one     0x00                    // start of line 1
#define lcd_line_two     0x40                    // start of line 2
#define   lcd_line_three   0x14                  // start of line 3 (20x4)
#define   lcd_line_four    0x54                  // start of line 4 (20x4)
//#define   lcd_LineThree   0x10                  // start of line 3 (16x4)
//#define   lcd_lineFour    0x50                  // start of line 4 (16x4)

// LCD instructions
#define lcd_Clear           0b00000001          // replace all characters with ASCII 'space'
#define lcd_Home            0b00000010          // return cursor to first position on first line
#define lcd_EntryMode       0b00000110          // shift cursor from left to right on read/write
#define lcd_DisplayOff      0b00001000          // turn display off
#define lcd_DisplayOn       0b00001100          // display on, cursor off, don't blink character
#define lcd_FunctionReset   0b00110000          // reset the LCD
#define lcd_FunctionSet4bit 0b00101000          // 4-bit data, 2-line display, 5 x 7 font
#define lcd_SetCursor       0b10000000          // set cursor position

// Function Prototypes
void init_lcd_avr_4f(void);
void lcd_write_4(uint8_t);
void lcd_write_instruction_4f(uint8_t);
void lcd_write_character_4f(uint8_t);
void lcd_write_string_4f(char *);
void lcd_init_4f(void);
void lcd_check_BF_4(void);
void lcd_clear_line(uint8_t line);
void lcd_clear(void);
void lcd_write_float_4_decimal(float value, uint8_t line);
void lcd_write_float_1_decimal(float value, uint8_t line);

#endif /* LCD_AVR_4F_H_ */
