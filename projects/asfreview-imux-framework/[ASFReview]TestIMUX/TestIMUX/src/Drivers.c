/*
 * Drivers.c
 *
 * Created: 21/08/2011 20:51:26
 *  Author: Guillem Planisi
 */ 
#include <conf_board.h>
#include <asf.h>

char SPI_Sync(SPI_t *SPIPort, char *Buff, unsigned int Count)
{
	char * p;
	
	if (((SPIPort->STATUS)&(1<<7)) == 0) //Check if SPI port is ready 
	  return 1; //The system is busy...
	p = Buff;//Init the pointer
	for(;Count;)//While there are data into the buffer to be sent or to be read
	{
		SPIPort->DATA=*p;//Send the byte
		while(((SPIPort->STATUS)&(1<<7)) == 0);//Wait until the transmission is done
		*p = SPIPort->DATA;//Read the byte received
		p++; //Increase the pointer to the next byte
		Count--;//Decrease the number of data remaining to be moved
	}
	return 0; //Everything had gone fine
}

void SetExpansionOutputs(char output)
{
	SPI_Sync(&SPIF,&output,1);
	STROBE_ON;
	asm("NOP");
	STROBE_OFF;
}

/*
 * MC14094 is MSB first, Strobe active high, mode 00, although probably mode 11 would work also. Default: 1110 1000 (MSB..LSB). Max 1.25Mbps.
 * SPIFlash is also MSB first, mode 00 although it could work with mode 11, per Atmel's datsheet. Must be checked against the real IC assembled. It should be standard, though. It's fast...
 * The Evil's Module uses mode 00 only. It seems to be also MSB first... It seem to be in the range of 2-5 Mbps.
 * The F***ing B***ard IC output SDO is not tri-stated, forcing to use a tri-state buffer when it is used into an SPI bus (that is the case in the present design) @#!FGrrr|¡#@¬
 * Luckily, I'm using a module that includes everything, from the transceiver and quartz crystal, to the required tri-state buffer...
 */

unsigned int SPIFlash_ReadID(SPI_t *SPIPort)
{
	char Data[4];
	unsigned int aux;
	
	ENABLE_MEM;
	Data[0]=CmdRDId;
	SPI_Sync(SPIPort,&Data[0],4);
	DISABLE_MEM;
	aux=256*Data[2]+Data[3];
	return aux;	
}