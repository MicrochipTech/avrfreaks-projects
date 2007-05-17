/****************************************************************************
  Чтение и сброс регистра прерываний модуля ethernet
****************************************************************************/
#include <avr/io.h>
#include <compat/twi.h>
#include "headers/macro.h"

#include "headers/w3100a.h"
#include "headers/defConstant.h"
#include "headers/defStruct.h"

extern struct	TWI_status	TWI_stat;
extern struct	Descript	ETH_dsc;			//рабочая структура функции обслуживания модуля Ethernet

void ethRe(char mask)
{
	static unsigned char phase=0;
	
	switch(phase)
	{
	case 0:
	TWI_stat.TWI_state=TWI_BUSY;
	TWI_stat.TWI_msgSize=3;						// leng message
	TWI_stat.TWI_buf[0]=SLA_W;
	TWI_stat.TWI_buf[1]=0x00;					// high adress
	TWI_stat.TWI_buf[2]=INT_STATUS;			// low adress INT_STATUS=0x04
	TWI_Start();
	phase=1;
	break;
	case 1:											 
	TWI_stat.TWI_state=TWI_BUSY;
	TWI_stat.TWI_msgSize=2;						// leng message
	TWI_stat.TWI_buf[0]=SLA_R;
	TWI_Start();
	phase=2;
	break;
	case 2:
	if((TWI_stat.TWI_buf[1] & mask)==0) phase=0;//добавить инкремент ошибок
	else
	{
		ETH_dsc.phase++;
		phase=0;
	}
	break;
	default:
	phase=0;
	break;
	}
}
