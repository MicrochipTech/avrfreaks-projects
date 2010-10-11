#ifndef __LCD_H__
#define __LCD_H__

#include <inttypes.h>

#include "lcd_defaults.h"

#ifdef LCD_CONTROLLER_T6963
#include "lcd_t6963.h"
static inline void lcd_init(void) {t6963_init();}
static inline void lcd_set_position(uint8_t x,uint8_t y) {t6963_set_position(x,y);}
static inline void lcd_scroll_up(void) {t6963_scroll_up();}
static inline void lcd_tx_uint8(uint8_t c) {t6963_data_write_and_increment_ap(c);}
#else
#error "LCD_CONTROLLER_T6963 not defined"
#endif

#endif
