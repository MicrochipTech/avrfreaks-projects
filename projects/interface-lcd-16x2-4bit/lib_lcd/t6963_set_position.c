#include "lcd_t6963.h"

void t6963_set_position(uint8_t x,uint8_t y) {
    if (x<LCD_COLUMNS && y<LCD_LINES) {
	t6963_set_address_pointer(y*LCD_COLUMNS+x+T6963_TEXT_HOME_ADDRESS);
    }
}
