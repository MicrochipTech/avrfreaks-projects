////////////////////////////////////////////////////////////////////////////
// Name: main.cpp
// Author: B. Gian James <gian@BattleDroids.net>
// Description: The main file for the AVR Shell. Written to be used with
//              the AVR Telnet Gateway.
//
// $Id: main.cpp,v 1.5 2009/09/02 12:55:17 gian Exp $
////////////////////////////////////////////////////////////////////////////
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "common.h"
#include "uart.h"
#include "shell.h"
#include "script.h"

static UART		uart(Baud57600,(UART_TX|UART_RX));
static AVRShell	shell(&uart);
static ScriptEngine	scriptEngine;

int main(void)
{
	scriptEngine.Init();	// initialize EEPROM variables
	shell.SetScriptEngine(&scriptEngine);

	uart.EnableInterrupt(UART_RX);
	sei();
	
	for (;;) 
	{	
		if (TCNT1 > CHECK_TIME)
		{
			StopTimer();
			ClearTimer();
			
			int	ret;
			if (  (ret = shell.Process(uart.GetRXBuff())) ) {
			}
			uart.FlushRXBuff();		
		}
	}
}

ScriptEngine & appGetScriptEngine()
{
	return scriptEngine;
}

void	WriteLine(const char * msg, uint8 doPrompt)
{
	uart.WriteLine(msg);
	if (doPrompt)
		uart.WriteRAM(shell.Prompt());
}

void	WriteRAM(const char * msg, uint8 doPrompt)
{
	uart.WriteRAM(msg);
	if (doPrompt)
		uart.WriteRAM(shell.Prompt());
}



ISR(TIMER0_COMPA_vect)
{
	if (++(shell.timect) >= 100) {
		shell.timect = 0;
		shell.AddSec();
	}
}

ISR(USART_RX_vect)
{
	StopTimer();
	ClearTimer();
	
	uchar c = UART::rx();
	uart.AddToReceive(c);
	
	StartTimer();
}
