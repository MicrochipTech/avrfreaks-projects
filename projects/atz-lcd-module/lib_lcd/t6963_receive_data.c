#include <avr/io.h>

#include "lcd.h"

uint8_t t6963_receive_data(void) {
    t6963_status_check();
    LCD_CND_SET_LOW();
    return t6963_read();
}
