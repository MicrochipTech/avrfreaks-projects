/// lcd.h ///


#define LCD_RS_MASK 0x02
#define LCD_RW_MASK 0x04
#define LCD_EN1_MASK 0x08
//#define LCD_EN2_MASK 0x20

byte  lcd_write_string (char x,signed char y, char *text, char centered);
void lcd_init (void);
void lcd_cls (void);
