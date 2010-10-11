#include "lcd_t6963.h"

void t6963_data_write_and_increment_ap(uint8_t data) {
    t6963_send_command_uint8(T6963_DATA_WRITE_AND_INCREMENT_AP,data);
}
