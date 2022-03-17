/*************************************************************/
/*                   SD/MMC����������                        */
/*  ����WinAVR 20060421                                      */
/*  ���ߣ�Bozai�����䲨��                                    */
/*  E-mail��sudazqb@163.com                                  */
/*  2006��11��26��                                           */
/*************************************************************/
/*          FAT diriver for  MiniMP3 Player                        */
/*                                                                 */
/* Platform   : AVRStudio4.12 sp4 + WinAVR20060421                 */
/*              optimize -0s                                       */
/* Author     : bozai(Zhang Qibo)                                  */
/* E-mail     : sudazqb@163.com                                    */
/* MSN        : zhangqibo_1985@hotmail.com                         */
/* Date       : 2006-12-26                                         */
/*******************************************************************/




#include <avr/io.h>
#include "MMC_SD.h"




//����ģʽ  //spi low speed
void SPI_Low(void)
{
	SPCR =   _BV(SPE)|_BV(MSTR)|_BV(SPR1)|_BV(SPR0);
	SPSR &= ~_BV(SPI2X);
}

//����ģʽ	//spi full speed
void SPI_High(void)
{
	SPCR =  _BV(SPE)|_BV(MSTR);
	SPSR |= _BV(SPI2X);
}

//�˿ڳ�ʼ����ģʽ��ʼ��  //port initialize
void SPI_Init(void)
{
	DDR_INI();
	SPI_Low();
}

//д��һ���ֽ�			//read and write one byte
uint8 SPI_WriteByte(uint8 val)
{
	SPDR = val;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}

/*uint8 SPI_ReadByte(void)
{
	SPDR = 0xff;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}*/
//sd����ʼ��		//sd card initialize
void MMC_SD_Init(void)
{
	SPI_Init();
	SPI_CS_Deassert();
}

//sd��д����		//sd send command
uint8 MMC_SD_SendCommand(uint8 cmd, uint32 arg)
{
	uint8 r1;
	uint8 retry=0;
	
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);

	SPI_CS_Assert();
	
	SPI_WriteByte(cmd | 0x40);//�ֱ�д������	//send command
	SPI_WriteByte(arg>>24);
	SPI_WriteByte(arg>>16);
	SPI_WriteByte(arg>>8);
	SPI_WriteByte(arg);
	SPI_WriteByte(0x95);
	
	while((r1 = SPI_WriteByte(0xff)) == 0xff)//�ȴ���Ӧ��	//wait response
		if(retry++ > 100) break;//��ʱ�˳�					//time out error

	SPI_CS_Deassert();

	return r1;//����״ֵ̬					//return state
}

//sd����λ		//reset sd card (software)
uint8 MMC_SD_Reset(void)
{
	uint8 i;
	uint8 retry;
	uint8 r1=0;
	retry = 0;
	SPI_Low();

	do
	{
		for(i=0;i<100;i++) SPI_WriteByte(0xff);
		r1 = MMC_SD_SendCommand(0, 0);//��idle����	//send idle command
		retry++;
		if(retry>10) return 1;//��ʱ�˳�		//time out
	} while(r1 != 0x01);	


	retry = 0;
	do
	{
		r1 = MMC_SD_SendCommand(1, 0);//��active����	//send active command
		retry++;
		if(retry>100) return 1;//��ʱ�˳�		//time out
	} while(r1);
	SPI_High();
	r1 = MMC_SD_SendCommand(59, 0);//��crc		//disable CRC

	r1 = MMC_SD_SendCommand(16, 512);//��������С512	//set sector size to 512
	return 0;//��������		//normal return
}

//��һ������		//read one sector
uint8 MMC_SD_ReadSingleBlock(uint32 sector, uint8* buffer)
{
	uint8 r1;
	uint16 i;
	uint8 retry=0;

	do
	{
		r1 = MMC_SD_SendCommand(17, sector<<9);//������	//read command
		retry++;
		if(retry>10) return 1;//��ʱ�˳�		//time out
	} while(r1 != 0x00);	


	SPI_CS_Assert();
	//�����ݵĿ�ʼ	//wait to start recieve data
	while(SPI_WriteByte(0xff) != 0xfe);//if(retry++ > 50){SPI_CS_Deassert();return 1;}

	for(i=0; i<512; i++)//��512������	//read 512 bytes
	{
		*buffer++ = SPI_WriteByte(0xff);
	}

	SPI_WriteByte(0xff);//αcrc
	SPI_WriteByte(0xff);
	
	SPI_CS_Deassert();

	return 0;
}


//дһ������		//wirite one sector //not used in this application
uint8 MMC_SD_WriteSingleBlock(uint32 sector, uint8* buffer)
{
	uint8 r1;
	uint16 i;
	uint8 retry=0;

	do
	{
		r1 = MMC_SD_SendCommand(24, sector<<9);//д����	//send command
		retry++;
		if(retry>10) return 1;//��ʱ�˳�		//time out
	} while(r1 != 0x00);	



	SPI_CS_Assert();
	
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);

	SPI_WriteByte(0xfe);//����ʼ��			//send start byte
	
	for(i=0; i<512; i++)//��512�ֽ�����		//send 512 bytes data
	{
		SPI_WriteByte(*buffer++);
	}
	
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	
	r1 = SPI_WriteByte(0xff);
	
	if( (r1&0x1f) != 0x05)//�ȴ��Ƿ�ɹ�	//judge if it successful
	{
		SPI_CS_Deassert();
		return r1;
	}
	//�ȴ�������		//wait no busy
	while(!SPI_WriteByte(0xff));//if(retry++ > 50){SPI_CS_Deassert();return 1;}

	SPI_CS_Deassert();

	return 0;
}

uint32 MMC_SD_ReadCapacity()
{
	uint8 r1;
	uint16 i;
	uint16 temp;
	uint8 buffer[16];
	uint32 Capacity;
	//uint8 retry=0;

	r1 = MMC_SD_SendCommand(9, 0);//д����	//send command  //READ CSD
	if(r1 != 0x00)
		return r1;

	SPI_CS_Assert();
	while(SPI_WriteByte(0xff) != 0xfe);
	
	for(i=0;i<16;i++)
	{
		buffer[i]=SPI_WriteByte(0xff);
	}	

	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	
	SPI_WriteByte(0xff);
	
	SPI_CS_Deassert();

/*********************************/
//	C_SIZE
	i = buffer[6]&0x03;
	i<<=8;
	i += buffer[7];
	i<<=2;
	i += ((buffer[8]&0xc0)>>6);

/**********************************/
//  C_SIZE_MULT

	r1 = buffer[9]&0x03;
	r1<<=2;
	r1 += ((buffer[10]&0x80)>>7);


/**********************************/
// BLOCKNR

	r1+=2;

	temp = 1;
	while(r1)
	{
		temp*=2;
		r1--;
	}
	
	Capacity = ((uint32)(i+1))*((uint32)temp);

/////////////////////////
// READ_BL_LEN

	i = buffer[6]&0x0f;

/*************************/
//BLOCK_LEN

	temp = 1;
	while(i)
	{
		temp*=2;
		i--;
	}
/************************/


/************** formula of the capacity ******************/
//
//  memory capacity = BLOCKNR * BLOCK_LEN
//	
//	BLOCKNR = (C_SIZE + 1)* MULT
//
//           C_SIZE_MULT+2
//	MULT = 2
//
//               READ_BL_LEN
//	BLOCK_LEN = 2
/**********************************************/

//The final result
	
	Capacity *= (uint32)temp;	 
	return Capacity;		
}
