#include <lcd.h>		
			        
#include <progmem.h>
							    
 
		
int main(void) {			 
//		     1234567890123456789012345678901234567890			
 char *text1 = PSTR("www.fhelectronic.de\n");
 char *text2 = PSTR("this is a test\n");

 LCD_init();			
 LCD_sendconststring (text1);	
 LCD_setpos (2,1);
 LCD_sendconststring (text2);
};

