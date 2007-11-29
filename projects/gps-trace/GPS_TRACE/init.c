#include "lcd.h"
#include "RS232.h"
#include <avr/interrupt.h>

void delay_ms(unsigned short ms) {
   unsigned short outer1, outer2;
   outer1 = 200;
   while (outer1) {
      outer2 = 1000;
      while (outer2) {
         while ( ms ) ms--;
         outer2--;
      }
      outer1--;
   }
}


void InitLCDText()
{
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
        lcd_puts("Welcome to GPS-Trace");
	lcd_gotoxy(0,2);
        lcd_puts("GPS-Fix: Unknown");
	lcd_gotoxy(0,1);
	lcd_puts("N: No fix");
	lcd_gotoxy(0,3);
	lcd_puts("E: No fix");
}

void InitUarts()
{
   	uart0_init(); // init USART1
   	uart1_init(); // init USART0
   	sei();  // enable interrupts
	uart0_send_initial_char();
}

void initGSM()
{
  	uart0_puts("AT\r"); //--> Verbinding controleren
  	delay_ms(1000);
    
	uart0_puts("AT+CMGF=1\r"); //--> Textmode
	delay_ms(1000);

	uart0_puts("AT+CPIN=0000\r"); //--> Pin-code invoeren 
  	delay_ms(1000);

    uart0_puts("AT+CGMI\r"); //--> Verbinding controleren
}

//debugger function
void sendTestSMS()
{
	uart0_puts("AT+CMGS=xxxxxxxxxxx\r"); //on the place op xxxxxxx place phonenumber to send testsms to 
	delay_ms(1000);
	uart0_puts("test\x1a");
}
