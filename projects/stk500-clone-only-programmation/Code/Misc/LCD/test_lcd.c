#include <avr/io.h>
#include "lcd.h"


int main(void) {
  lcd_init(LCD_DISP_ON);
  lcd_clrscr();
  
  lcd_gotoxy(0,0);
  lcd_puts("Line1");
  delay_ms(100);
  
  lcd_gotoxy(0,1);
  lcd_puts("Line2");
  delay_ms(100);
  
  lcd_gotoxy(0,2);
  lcd_puts("Line3");
  delay_ms(100);
  
  lcd_gotoxy(0,3);
  lcd_puts("Line4");
  delay_ms(100);
  
  lcd_gotoxy(5,2);
  lcd_puts("Linex");
  delay_ms(100);

  while(1) {};
  return 0;
}