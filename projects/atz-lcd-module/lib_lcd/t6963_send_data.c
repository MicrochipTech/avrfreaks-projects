#include <avr/io.h>

#include "lcd.h"

void t6963_send_data(uint8_t data) {
    t6963_status_check();
    LCD_CND_SET_LOW();
    t6963_send(data);
}
