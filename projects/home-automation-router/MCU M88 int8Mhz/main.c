//No main.h !

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "GlobalDefinitions.h"
#include "init_Pins.h"
#include "USART.h"
#include "RX.h"
#include "Timer.h"
#include "RF.h"

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Flash_LED (void)
{
	LEDon;
	_delay_ms(10);
	LEDoff;
}

int main(void)
{	
	init_Pins();
	init_Timers();
	set_sleep_mode(SLEEP_MODE_IDLE);	//all peripheral clocks are still running
	
	Flash_LED();

	Enable_UART();	//before sei() 
	Enable_UART_RXC_ISR;
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	sei();	//Enable interrupts

	while(1)
	{		
		sleep_mode();
		
		if  ( Packet_RX_ed == 1  )
		{
			UART_RX_Handler();	
		}
		
	}

	return 0;

}	//main

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

