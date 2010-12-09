#include "lcd_t6963.h"

void t6963_send_command_columns(uint8_t command) {
    t6963_send_command_uint8_nul(command,LCD_COLUMNS);
}
