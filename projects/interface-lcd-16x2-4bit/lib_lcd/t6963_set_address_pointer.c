#include "lcd_t6963.h"

void t6963_set_address_pointer(uint16_t address) {
    t6963_send_command_uint16(T6963_SET_ADDRESS_POINTER,address);
}
