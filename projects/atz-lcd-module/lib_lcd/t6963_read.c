#include <avr/io.h>

#include <misc.h>

#include "lcd.h"

uint8_t t6963_read(void) {
    uint8_t data;
    LCD_NWR_SET_HIGH();
    LCD_DB_SET_INPUT();
    LCD_NCE_SET_LOW();
    LCD_NRD_SET_LOW();
    LCD_WAIT_150NS();
    data=LCD_DB_GET();
    LCD_NRD_SET_HIGH();
    LCD_NCE_SET_HIGH();    
    LCD_WAIT_50NS();
    LCD_DB_SET(data);
    LCD_DB_SET_OUTPUT();
    return data;
}
