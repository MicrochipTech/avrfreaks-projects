#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "lcd.h"
#include "GSMParser.h"

int count;
int fora = 0;

void InitGSMParser()
{
	GSMstate=0;
	GSM_CMD_count=0;
	GSM_COMMAND_count=0;
	count=0;
}
void GSMFeedBuffer (char pdata)
{
	GSMdata=pdata;
	ProcessGSM();
}

int ProcessGSMCOM()
{
	if (GSM_CMD_count < 3)
	{
		GSM_CMD[GSM_CMD_count]=GSMdata;
		GSM_CMD_count++;
		return 0;
	}
	if (GSM_CMD_count == 3)
	{
		GSM_CMD[GSM_CMD_count]=GSMdata;
		GSM_CMD_count=0;
		return 1;
	}
	return -1;
}

int ProcessGSMLEX()
{
	if (GSM_COMMAND_count < 20)
	{
		GSM_COMMAND[GSM_COMMAND_count]=GSMdata;
		GSM_COMMAND_count++;
		return 0;
	}
	if (GSM_COMMAND_count == 20)
	{
		GSM_COMMAND[GSM_COMMAND_count]=GSMdata;
		GSM_COMMAND_count=0;
		return 1;
	}
	return -1;
}

void ProcessGSM()
{
	switch (GSMstate)
	{
		case 0: 
			if (GSMdata=='+')
			{
				GSMstate=3;
				//lcd_gotoxy(0,0);
			}
		break;
		case 1:
			if (ProcessGSMCOM()==1)
			{
				GSMstate=2;
			}
		case 2:
		if (GSM_CMD[1]=='C'&&GSM_CMD[1]=='L'&&GSM_CMD[1]=='I'&&GSM_CMD[1]=='P'){
			GSMstate=3;}
		break;
		case 3:
			lcd_gotoxy(0,2);
			lcd_puts("RING");
			if (ProcessGSMLEX()==1)
			{
			cli();	
				lcd_gotoxy(0,4);
					GSMstate=0;
			sei();
			if (GSM_COMMAND[7]=='+')
			{
				//phone number detected send SMS now !
			 	ReportPosition();
			}
			}

		break;
		default: GSMstate=0;
	}
}

