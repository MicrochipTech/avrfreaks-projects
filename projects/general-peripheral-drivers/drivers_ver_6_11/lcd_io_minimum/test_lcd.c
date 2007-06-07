#if !defined(AVRGCC_VERSION)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) > 303
#define AVRGCC_VERSION      304        /* AVRGCC version for including the correct files  */
#elif ((__GNUC__ * 100) + __GNUC_MINOR__) >= 300  &&  ((__GNUC__ * 100) + __GNUC_MINOR__) <= 303
#define AVRGCC_VERSION      303        /* AVRGCC version for including the correct files  */
#else 
#define AVRGCC_VERSION   300    
#endif
#endif


#if AVRGCC_VERSION >= 303

#include <avr/io.h>
#else
#include <io.h>
#endif

#include "lcd_io.h"

void main(void)
{
  
 /* Since we also test the eeprom DONT! forget to write the test_lcd.eep to AVR's eeprom 
     otherwise the lcd will not show the eeprom_string                                       */
  static  unsigned char eeprom_string[]__attribute__((section(".eeprom")))={"lcd_puts_e testing"};
  unsigned char ram_string[]={"DONE TESTING..."};
  unsigned int pos=0;
   
   lcd_init();
   lcd_clrscr();
   lcd_gotoxy(0,0);  lcd_puts_P( "Hello from progmem\nthis is line 2" );
   lcd_gotoxy(0,2);  lcd_puts_e(eeprom_string);
   lcd_gotoxy(0,3);  lcd_puts(ram_string);  
   lcd_gotoxy(16,3); lcd_puti(pos,2);  pos=lcd_getxy();
   lcd_gotoxy(16,3); lcd_puti(pos,2);  
  
}
