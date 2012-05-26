#ifdef __GNUC__ // to avoid error messages in visual studio editor
#include <avr/io.h>
#else
#include <avr/iom8.h>
#endif
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "timer.h"
#include "display.h"
#include "F-Bus.h"
#include "ADC.h"
extern unsigned char RecipientNo[9];
int main( void)
{
	unsigned char cnt=0;
	_init_timer1();
	InitADC();
	usart_init(8) ;//8:115.2 kbps,103:9600 kbps
	_delay_ms (1000);
	InitDisplay();
	initPhone();
	ReadTemperature();
	while( 1)
	{
		if(++cnt==50)
		{
			cnt =0;
			ReadTemperature();
		}
		RefreshDisplay ();
	}
	return 0;
}


