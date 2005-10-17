#include "main.h"
#include "uart.h"
#include "rc_matsushita.h"
#include "timers.h"

#include <stdio.h>
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>



extern struct {
				word device;
				word command;
				} matsushita;

extern volatile byte rc_new_input;



void base_init(void)
{
	//init Ports
	DDRA=0xEF;
	PORTA=0x10;			//activate p/u for master/slave detection
	DDRB=0xEB;		
	PORTB=0xFF;
	DDRC=0xC3;
	PORTC=0x3C;
	DDRD=0xBE;
	PORTD=0x03;


}


int main (void)
{	char outstring[50];


	base_init();
	main_timer_init();
	rc_init();
	uart_transmit_init (1,0);
	sei();

	while (1)
	{
		if (rc_new_input)
		{	sprintf_P(outstring,PSTR("device %04X  command %04X \r"),matsushita.device,matsushita.command);
			uart_transmit (outstring,strlen(outstring));
			rc_new_input=0;
		}
		
	}

}	

