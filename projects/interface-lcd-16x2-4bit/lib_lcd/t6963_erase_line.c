#include "lcd_t6963.h"

void t6963_erase_line(void) {
    unsigned i;
    t6963_set_data_auto_write();
    for (i=0; i<LCD_COLUMNS; i++) {
        t6963_send_data(0);
    }
    t6963_auto_reset();
}
