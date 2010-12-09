#include <avr/io.h>

#include <misc.h>

#include "lcd.h"

void t6963_send(uint8_t data) {
    LCD_NCE_SET_LOW();
    LCD_DB_SET(data);
    LCD_NWR_SET_LOW();
    LCD_WAIT_80NS();
    LCD_NWR_SET_HIGH();
    LCD_NCE_SET_HIGH();
    LCD_WAIT_40NS();
}
