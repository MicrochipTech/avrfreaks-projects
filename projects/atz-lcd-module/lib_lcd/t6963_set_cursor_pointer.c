#include "lcd_t6963.h"

void t6963_set_cursor_pointer(uint8_t x_address,uint8_t y_address) {
    t6963_send_command_uint8_uint8(T6963_SET_CURSOR_POINTER,x_address,y_address);
}
