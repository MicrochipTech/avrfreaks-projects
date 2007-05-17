/****************************************************************************
	Установка сетевых настроек модуля Ethernet
****************************************************************************/
#include <avr/io.h>
#include "headers/macro.h"
#include "headers/w3100a.h"
#include "headers/defConstant.h"
#include "headers/defStruct.h"

extern struct	TWI_status		TWI_stat;
extern struct Descript	ETH_dsc;	//рабочая структура функции обслуживания модуля Ethernet

extern const unsigned char	Net_Settings[23];
/*-------------------------------------------------------------------------------------------
	struct TWI_status							// Status byte holding flags.
	{
		volatile unsigned char TWI_state;		// State byte. Default set to TWI_NO_STATE.
		unsigned char TWI_msgSize;				// Number of bytes to be transmitted.
		unsigned char TWI_buf[TWI_BUFFER_SIZE];	// Transceiver buffer
	};
-------------------------------------------------------------------------------------------*/
void netInit(void)
{
	static unsigned char phase=0;
	unsigned char i,j;
	
	switch(phase)
	{
		case 0:											// S/W Reset command
		ETH_dsc.countError=0;								// сброс счетчика ошибок
		/* передача команды сброса */
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=4;								// leng message
		TWI_stat.TWI_buf[0]=SLA_W;
		TWI_stat.TWI_buf[1]=0x00;							// high adress
		TWI_stat.TWI_buf[2]=0x00;							// low adress
		TWI_stat.TWI_buf[3]=CSW_RESET;						// data CSW_RESET=0x80
		TWI_Start();										// запуск TWI и разрешение прерываний от TWI.
		phase=1;
		break;
		/*
			Set network set Gateway IP Address,
			Subnet Mask, Source H/W Address,Source IP Addres.
		*/
		case 1:
		/* установка сетевых параметров
			из массива NetSettings[23]
			первая фаза до установки MAC
		*/
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=8+3;							// leng message=11
		TWI_stat.TWI_buf[0]=SLA_W;
		TWI_stat.TWI_buf[1]=0x00;							// high adress
		TWI_stat.TWI_buf[2]=NET_SETTINGS;					// low adress NET_SETTINGS=0x80
		for(i=3,j=0; j<8; j++)
		TWI_stat.TWI_buf[i++]=Net_Settings[j];				// data
		TWI_Start();										// запуск TWI и разрешение прерываний от TWI.
		phase=2;
		break;
		case 2:
		/* установка сетевых параметров
			из массива NetSettings[23]
			вторая фаза после установки MAC
		*/
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=9+3;							// leng message=12
		TWI_stat.TWI_buf[0]=SLA_W;
		TWI_stat.TWI_buf[1]=0x00;							// high adress
		TWI_stat.TWI_buf[2]=SRC_IP;						// low adress SRC_IP=0x8E
		for(i=3,j=14; j<23; j++)
		TWI_stat.TWI_buf[i++]=Net_Settings[j];				// data
		TWI_Start();										// запуск TWI и разрешение прерываний от TWI.
		phase=3;
		break;
		case 3:											// Sys_Init command
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=4;								// leng message
		TWI_stat.TWI_buf[0]=SLA_W;
		TWI_stat.TWI_buf[1]=0x00;							// high adress
		TWI_stat.TWI_buf[2]=0x00;							// low adress
		TWI_stat.TWI_buf[3]=CSYS_INIT;						// data CSYS_INIT=0x01
		TWI_Start();
		ETH_dsc.phase++;									// установка признака сетевых настроек = 1	
		phase=0;
		break;
		default:
		phase=0;
		break;
	}
}


