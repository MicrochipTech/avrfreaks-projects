#include <avr/io.h>

#include "lcd.h"

void t6963_send_command(uint8_t command) {
    t6963_status_check();
    LCD_CND_SET_HIGH();
    t6963_send(command);
}
