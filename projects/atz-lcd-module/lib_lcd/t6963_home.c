#include "lcd_t6963.h"

void t6963_home(void) {
    t6963_set_address_pointer(T6963_TEXT_HOME_ADDRESS);
}
