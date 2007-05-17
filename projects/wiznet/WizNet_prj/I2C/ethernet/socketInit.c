#include <avr/io.h>
#include "headers/macro.h"
#include "headers/w3100a.h"
#include "headers/defConstant.h"
#include "headers/defStruct.h"

extern struct		TWI_status	TWI_stat;
extern struct 	Descript	ETH_dsc;		//рабочая структура функции обслуживания модуля Ethernet
extern const unsigned char	Socket_Settings[19];

void socketInit(void)
{
	static unsigned char phase=0;
	unsigned char i,j;
	
	switch(phase)
	{
	case 0:											// Init socket
	/* установка параметров сокета из Socket_Settings[19] */
	TWI_stat.TWI_state=TWI_BUSY;
	TWI_stat.TWI_msgSize=sizeof(Socket_Settings)+3;	// leng message
	TWI_stat.TWI_buf[0]=SLA_W;
	TWI_stat.TWI_buf[1]=0x00;					// high adress
	TWI_stat.TWI_buf[2]=OPT_PROTOCOL;			// low adress OPT_PROTOCOL=0xA1
	for(i=3,j=0; j<19; j++)
	TWI_stat.TWI_buf[i++]=Socket_Settings[j];	// data
	TWI_Start();
	phase=1;
	break;
	case 1:									// CSOCK_INIT command 
	TWI_stat.TWI_state=TWI_BUSY;
	TWI_stat.TWI_msgSize=4;						// leng message
	TWI_stat.TWI_buf[0]=SLA_W;
	TWI_stat.TWI_buf[1]=0x00;					// high adress
	TWI_stat.TWI_buf[2]=0x00;					// low adress
	TWI_stat.TWI_buf[3]=CSOCK_INIT;			// data CSOCK_INIT=0x02
	TWI_Start();
	ETH_dsc.phase++;
	phase=0;
	break;
	default:
	phase=0;
	break;
	}
}

