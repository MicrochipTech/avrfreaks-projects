#include <avr/io.h>
#include <avr/interrupt.h>

#include "lcd8310.h"

void Init(void)
{
	PORTB = 0x0f;
	DDRB  = 0xf0;
}

void spi_init(void)
{
	SPSR = 0x01;
}

int main(void)
{
	Init();
	spi_init();
	Lcd_Init();
//	Gotoxy(0,0);
//	print_message(" Hello,Rainbow!  Nokia 8310 LCD.");
//	Gotoxy(00,3);
//	print_message("ATMEGA16");
//	print_message("00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555555566666666666666667777777777777777");
	while(1)
	{
		Lcd_Clear();
		Gotoxy(8,0);
		
//		print_string("0");
//		print_string("00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555555566666666666666667777777777777777");
		print_string("NOKIA 8310 LCD");
		Gotoxy(0,1);
		print_string("Referce program:PIC 18f1320 to Nokia 1100 LCD Interface");
		Gotoxy(0,5);
		print_string("http://sunbizhosting.com/~spiral/");
		delay_ms(3000);
//		Lcd_Write(CMD,0xa7);
		Lcd_Clear();
		Gotoxy(0,0);
		
		print_string("XTAL:12MHz");
		Gotoxy(0,1);
		print_string("SPI Clock:6Mhz");
//		print_string("1");
//		print_string("AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCCDDDDDDDDDDDDDDDDEEEEEEEEEEEEEEEEFFFFFFFFFFFFFFFFGGGGGGGGGGGGGGGGHHHHHHHHHHHHHHHH");
		delay_ms(3000);
//		Lcd_Write(CMD,0xa6);
	}
}