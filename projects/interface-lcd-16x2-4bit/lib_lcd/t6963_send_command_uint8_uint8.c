#include "lcd_t6963.h"

void t6963_send_command_uint8_uint8(uint8_t command,uint8_t data1,uint8_t data2) {
    t6963_send_data(data1);
    t6963_send_command_uint8(command,data2);
}
