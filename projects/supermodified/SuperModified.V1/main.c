//****************************************************************************************
//	File Name	: main.c 
//	Title		: file containing main()
//	Created		: 25.10.2010
//	Revised		:
//	Target MCU	: Atmel AVR
//	Author		: Sissakis Giannis
//  email		: info@01mech.com
//
//  Copyright (C) 2010 Zero One Mechatronics LP
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************************************

#include "zoTypes.h"
#include "zoSms.h"
#include "zoSmsError.h"
#include "zoProtocol.h"
#include "zoCommands.h"

int main(void)
{
	ZO_PROTOCOL_PACKET packet;
	ZO_PROTOCOL_HAL hal;
		
	//uncomment below two lines to restore defaults by programming the MCU
	//zoSmsRestoreDefaults();
	//while(1);
	
	//initialize all hardware and control
	zoSmsInit();		
	
	//initialize commands
	zoCommandsInit();

	//initialize communications
	zoProtocolInit(ZO_PROTOCOL_HW_I2C,&hal,Sms.Settings.NodeID);
	zoProtocolSetBitrate(&hal,Sms.Settings.BaudI2C);

	zoProtocolSetLAM(&hal,Sms.Settings.localAcceptanceMask);
	
	while(1)
	{
		//TODO:watchdog and fail-continue implementation		
		
		//poll the hardware for errors
		zoSmsErrorPoll();		
		
		if(zoProtocolParse(&hal,&packet))
		{
			//if there arent any other errors present, execute the command.
			if( zoErrorIsEmpty(&zoSmsError) )
			{
				//if the command was wrong, store errors so they can be indicated by 
				//the command response
				if( !zoCommandsServiceCommand(&packet) )
					zoSmsErrorHandleServiceCommandFailure();
			}
			
			//respond to command, if we fail try another 5 times to respond
			if( !zoProtocolCommandResponse(&hal,&packet,&zoSmsError) )
				zoSmsErrorHandleCommandResponseFailure(&hal,&packet);
		}
		else
			zoSmsErrorHandleParseFailure(&hal,&packet);
	}

	return 0;
}


