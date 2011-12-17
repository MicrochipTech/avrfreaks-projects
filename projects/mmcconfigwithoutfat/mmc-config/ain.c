#include <avr/io.h>
#include <util/delay.h>

#include "lcd/lcd.h"
#include "lcd/lcdconf.h"
#include "mmc/mmc.h"
#include "mmc/mmcconf.h"
#include "global/global.h"

int main(void)
{
	
	u08 i=0;
	u08 buffer[64]="hassan ghorbany";
	
	lcdInit();
	lcdClear();

	lcdPrintData("MMC test",8);
	_delay_ms(3000);
    lcdClear();


	mmcInit();
	lcdPrintData("mmc init",8);
	_delay_ms(1000);  
	if (mmcReset()==0)
	{
		lcdGotoXY(10,1);
		lcdPrintData("success",7);
	}
	else 
	{
		lcdGotoXY(10,1);
		lcdPrintData("faild",5);
	}

	_delay_ms(3000);
	lcdClear();

	lcdPrintData("writing ",8);
	_delay_ms(1000);
	if(mmcWrite(100,buffer)==0)
	{
		lcdGotoXY(10,1);
		lcdPrintData("success",7);
	}
	else
	{
		lcdGotoXY(10,1);
		lcdPrintData("faild",5);
	}
	_delay_ms(3000);
	lcdClear();
	
	i=100;
	lcdPrintData("reading 512-byte",16);
	_delay_ms(1000);
	if(mmcRead(i,buffer)==0)
	{
		lcdGotoXY(10,1);
		lcdPrintData("success",7);
		_delay_ms(3000);
		lcdClear();
		for(i=0;i<15;i++)
		{
			lcdDataWrite(buffer[i]);
			_delay_ms(500);
		}
	
	}
	else 
	{
		lcdGotoXY(10,1);
		lcdPrintData("faild",5);
	}
	_delay_ms(5000);
	lcdClear();
	lcdPrintData("mmc test fnish",14);


	
	while(1)

	{
	};
	return 0;
}
