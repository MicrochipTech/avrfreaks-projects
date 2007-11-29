#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "NMEA0183Parser.h"
#include "lcd.h"
#include "RS232.h"
#include "status.h"

void initNMEA(){
	state=0;
	CMD_count=0;
	COMMAND_count=0;
	test=0;
	fora=0;
}

void NMEAFeedBuffer (char pdata)
{
	data=pdata;
	ProcessNMEA();
}

int ProcessCMD()
{

	if (CMD_count < 4)
	{
		CMD[CMD_count]=data;
		CMD_count++;
		return 0;
	}
	if (CMD_count == 4)
	{
		CMD[CMD_count]=data;
		CMD_count=0;
		return 1;
	}

	return -1;
}

 
void ProcessNMEA(){

	switch (state)
	{
		case 0: 
			if (data=='$')
			{
				state=1;
			}
		break;

		case 1:
			if (ProcessCMD()==1)
			{ 
				state=2;
			}
		break;
		case 2:
			if (CMD[4]=='A'&&CMD[3]=='G')
			{
				state=3;
			}
			else
			{
				state=0;
			}
		
		break;
		
		case 3:
			if (data!='$')
			{	
				COMMAND[COMMAND_count]=data; // niet goed gevuld ofzo ? 
			    COMMAND_count++;
			}
			if (data=='$')
			{
				
			cli(); //interrupts uit
				UpdateStatus();
				COMMAND_count=0;
				fora=0;
				state = 1;
			sei(); //interrupts aan
			}
			
		break;
		default : state=0;
	}
}

void GetField (int fieldnumber, int offset)
{
		int fieldcount=0; //veldpositie
		int s=0; //for-loop var
		for (; s < COMMAND_count; s++)
		{
			if (COMMAND[s]==',')
			{
				fieldcount++;
				if (fieldcount==fieldnumber)
				{
					int t=0;//for-loop var
					for (; t < offset; t++)
					{
						result[t]=COMMAND[t+s+1];
					}
				}
			}
		}
}

void UpdateStatus()
{
	//SetFixState
	GetField (5,2);
	setFixstate(result[0]);
	//Set NS indicator 
	GetField (2,2);
	setNS_indicator(result[0]);
	//Set EW indicator 
	GetField (4,2);
	setEW_indicator(result[0]);
	//Set Latitude 
	GetField (1,10);
	setLatitude(result);
	FormatLatitude();
	showLongitudeFormatted();
	GetField (3,10);
	setLongitude(result);
	FormatLongitude();
	showLatitudeFormatted();
	//This one is to send sms with position 
	if (test==0)
	{
//		ReportPosition();
		test=1;
	}

}






