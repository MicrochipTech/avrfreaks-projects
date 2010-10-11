#include "lcd_t6963.h"

void t6963_scroll_up(void) {
    uint8_t buf[LCD_COLUMNS],x,y;
    for (y=1;y<LCD_LINES;y++) {
        t6963_set_position(0,y);   
	t6963_set_data_auto_read();
	for (x=0;x<LCD_COLUMNS;x++) buf[x]=t6963_receive_data();
	t6963_auto_reset();
        t6963_set_position(0,y-1);   
	t6963_set_data_auto_write();
	for (x=0;x<LCD_COLUMNS;x++) t6963_send_data(buf[x]);
	t6963_auto_reset();
    }
    t6963_set_position(0,LCD_LINES-1);
    t6963_erase_line();
}
