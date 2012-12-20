// Written by Ozhan KD (www.knowledgeplus.ir)
#include "spi_xm_v1.h"

volatile bool SpiTransferComplete;

//---------------------------------

ISR(SPI_INTERRUPT)
 {
  SpiTransferComplete = true;
 }

//---------------------------------

void SpiInit(void)
{
 unsigned char temp_read;
 SPI_PORT_INIT;
 SPI_CTRL_SLOW;
 temp_read=SPI_STATUS;
 temp_read=SPI_DATA;// Clear SPI STATUS
 SpiTransferComplete = true;
 SPI_INTCTRL=0X01;
}

//---------------------------------

void SpiSendByte(unsigned char data)
{
 SpiTransferComplete = false;
 SPI_DATA=data;
 while(!SpiTransferComplete);
}

//------------------------------------------------

unsigned char SpiTransferByte(unsigned char data)
{
 // send the given data

 SpiTransferComplete = false;
 SPI_DATA=data;
 // wait for transfer to complete
 while(!SpiTransferComplete);	
 // return the received data
 return SPI_DATA;
}

//--------------------------------------------------

unsigned int SpiTransferWord(unsigned int data)
{
 unsigned int RxData = 0;

 // send MS byte of given data
 RxData = (SpiTransferByte((data>>8) & 0x00FF))<<8;
 // send LS byte of given data
 RxData |= (SpiTransferByte(data & 0x00FF));

 // return the received data
 return RxData;
}

//------------------------------------------------

unsigned char SpiReceiveByte(unsigned char *data)
{
 // send the given data
 SpiTransferComplete = false;
 SPI_DATA=0xFF;
 // wait for transfer to complete
 while(!SpiTransferComplete);	
 // return the received data
 return SPI_DATA;
}

//--------------------------------------------------
