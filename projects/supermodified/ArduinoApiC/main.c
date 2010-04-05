#include <avr/io.h>
#include <util/delay.h>
#include "zoTypes.h"
#include "zoSmsMaster.h"
#include "zoProtocol.h"
#include "zoCommands.h"
#include "zoUart.h"
#include "zoString.h"
#include "zoError.h"
#include "zoAdc.h"

#define SUPERMODIFIED_NODE_ID	0x04

int main(void)
{
	ZO_PROTOCOL_HAL hal;
	s32 ticks = 0;
	//s32 pos;
	u08 str[10];
	//u08 i;
	
	zoUartInit();

	zoSmsMasterInit(ZO_PROTOCOL_HW_I2C,&hal,0x01);
	
	zoUartPutString("SuperModifiedTM Master Test\n\r");
	
	zoAdcInit();
	zoAdcChannelEnable(ZO_ADC_CHANNEL_0);
	
	zoCommandGlobalStart(0x00);
	//zoCommandProfiledAbsolutePositionMove(0x04,2048);
	//while(1);

	//zoCommandSetProfileAcceleration(0x04,800);
	//zoCommandSetProfileVelocity(0x04,800);
	//zoCommandVelocityMove(0x04,300);
	//while(1);

	while(1)
	{

		ticks = (s32)((double)zoAdcRead(ZO_ADC_CHANNEL_0)*0.2 + (double)ticks*0.8);
		
		if( !zoCommandAbsolutePositionMove(0x04,ticks) )
		{
			while(!zoErrorIsEmpty(zoSmsMasterError))
				zoUartPutChar(zoErrorGet(zoSmsMasterError));
		}
		else
		{
			s32ToStr(ticks,str);
			zoUartPutData(str,4);
		}

		//if( !zoCommandGetPosition(0x04,&pos) )
		//{
		//	while(!zoErrorIsEmpty(zoSmsMasterError))
		//		zoUartPutChar(zoErrorGet(zoSmsMasterError));
		//}
		//else
		//{
		//	s32ToStr(pos,str);
		//	zoUartPutData(str,4);
		//}

		_delay_ms(100.0);
	}

	return 1;
}