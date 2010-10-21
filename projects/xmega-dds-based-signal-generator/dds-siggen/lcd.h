// lcd.h
// Created by Frank Van Hooft 26 Sept 2010
// to support the lcd.c file

void lcd_set_write_instruction();
void lcd_set_write_data();
void lcd_write_byte (char c);
void lcd_clear_and_home();
void lcd_home();
void lcd_goto(uint8_t line, uint8_t pos);
void lcd_line_one();
void lcd_line_two();
void lcd_write_data(char c);
void lcd_write_string(char *x, uint8_t len );
void lcd_write_string_0(char *x);
void lcd_write_string_p(const char *s);
void lcd_init();



