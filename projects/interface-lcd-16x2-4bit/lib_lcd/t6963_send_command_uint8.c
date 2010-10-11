#include "lcd_t6963.h"

void t6963_send_command_uint8(uint8_t command,uint8_t data) {
    t6963_send_data(data);
    t6963_send_command(command);
}
