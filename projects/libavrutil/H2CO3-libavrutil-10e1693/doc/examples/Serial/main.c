/*
 * main.c
 * Serial
 *
 * Created on 18/02/2012.
 * by H2CO3
 * License: CreativeCommons Attribution-ShareAlike Unported 3.0 License
 */

#include <stdlib.h>
#include <math.h>
#include <avrutil/avrutil.h>

int main()
{
	avr_usart_init(AVR_USART_CH0, 9600);
	
	avr_usart_printf(AVR_USART_CH0, "\r\nHello User! :-)\r\n\r\n");
		
	while (1)
	{	
		uint8_t chr = avr_usart_recv(AVR_USART_CH0);
		avr_usart_printf(AVR_USART_CH0, "You pressed: <%c>\r\n", chr);
	}
	
	return 0;
}

