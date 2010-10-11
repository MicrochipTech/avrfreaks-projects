#include "lcd_t6963.h"

void t6963_send_command_uint16(uint8_t command, uint16_t data) {
    t6963_send_command_uint8_uint8(command,data,data>>8);
}
