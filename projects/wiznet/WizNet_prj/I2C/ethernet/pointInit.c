/****************************************************************************
  Установки указателей буфера приемника и передатчика модуля ethernet
****************************************************************************/
#include <avr/io.h>
#include "headers/macro.h"
#include "headers/w3100a.h"
#include "headers/defConstant.h"
#include "headers/defStruct.h"

extern struct			TWI_status	TWI_stat;
extern struct 		Descript	ETH_dsc;	//рабочая структура функции обслуживания молуля Ethernet
extern unsigned char	Point_Settings[4];

void pointInit(void)
{
	static unsigned char phase;
	unsigned char i,id,j;
	
	if(TWI_stat.TWI_state == TWI_BUSY)	return;		// Если TWI занят
	switch(phase)
	{
	case 0:											// Init pointers
	/* установка указателей приемника Point_Settings[4] */
	TWI_stat.TWI_state=TWI_BUSY;
	TWI_stat.TWI_msgSize=11;							// leng message
	TWI_stat.TWI_buf[0]=SLA_W;
	TWI_stat.TWI_buf[1]=0x00;							// high adress
	TWI_stat.TWI_buf[2]=RW_PR;							// low adress RW_PR=0x10
	for(id=7,i=3,j=0; j<4; j++)
	{
		TWI_stat.TWI_buf[i++]=Point_Settings[j];		// data
		TWI_stat.TWI_buf[id++]=Point_Settings[j];
	}
	TWI_Start();
	phase=1;
	break;
	case 1:
	/* установка указателей передатчика из Point_Settings[4] */
	TWI_stat.TWI_state=TWI_BUSY;
	TWI_stat.TWI_msgSize=11;							// leng message
	TWI_stat.TWI_buf[0]=SLA_W;
	TWI_stat.TWI_buf[1]=0x00;							// high adress
	TWI_stat.TWI_buf[2]=TW_PR;							// low adress TW_PR=0x40
	for(id=7,i=3,j=0; j<4; j++)
	{
		TWI_stat.TWI_buf[i++]=Point_Settings[j];		// data
		TWI_stat.TWI_buf[id++]=Point_Settings[j];
	}
	TWI_Start();
	phase=2;
	break;	
	case 2:									
	ETH_dsc.phase++;
	phase=0;
	break;
	default:
	phase=0;
	break;
	}
}

