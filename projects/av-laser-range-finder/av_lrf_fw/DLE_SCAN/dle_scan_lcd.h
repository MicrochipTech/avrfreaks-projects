/***C*********************************************************************************************
**
** SRC-FILE     :   dle_scan_lcd.h
**                                        
** PROJECT      :   DLE SCAN
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   
**                  
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2011
**
****C*E******************************************************************************************/
#ifndef __DLE_SCAN_LCD_H__
#define __DLE_SCAN_LCD_H__

#define LCD_SET_DIGITS_NUM 5
#define LCD_SIGNS_NUM 32


typedef struct lcd_digit_decode_tag{
    uint8    uc_digit;
    uint8    uc_bitmask;
} T_LCD_DIGIT_DECODE;


typedef struct t_seg_map_tag {
    uint8 uc_addr;
    uint8 uc_mask;
} T_SEG_MAP;

typedef struct lcd_digit_segment_map_tag{
    uint8     uc_digit_pos;
    T_SEG_MAP ta_seg_map[7];
}T_DIGIT_MAP;


extern uint8 guc_lcd_redraw;
extern uint8 guca_lcd_digits_set0[LCD_SET_DIGITS_NUM];
extern uint8 guca_lcd_digits_set1[LCD_SET_DIGITS_NUM];
extern uint8 guca_lcd_signs[LCD_SIGNS_NUM>>3];

#define LCD_SET_SIGN(sign) \
      guca_lcd_signs[sign >> 3] |= (1 << (sign & 0x7))

#define LCD_CLR_SIGN(sign) \
      guca_lcd_signs[sign >> 3] &= ~(1 << (sign & 0x7))


void lcd_render();
void init_lcd();
void dec2bcd(uint8 *puc_dst, uint16 us_distance);

#endif // __DLE_SCAN_LCD_H__
