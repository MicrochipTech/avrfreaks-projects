/*******************************************************************/
/*          VS1003 diriver for  Mega8 MP3 Player                   */
/*                                                                 */
/* Platform   : AVRStudio4.13 b528 + WinAVR20070122                */
/*              optimize -0s                                       */
/* Author     : bozai(Zhang Qibo)                                  */
/* E-mail     : sudazqb@163.com                                    */
/* MSN        : zhangqibo_1985@hotmail.com                         */
/* Date       : 2006-05-09                                         */
/*******************************************************************/
/*2007-05-04: add slow start up code, and add enough delay         */
/*2007-04-21:                                                      */


#include<avr/io.h>
#include"VS1003B.h"

#define uchar unsigned char
#define uint  unsigned int

//以最低速度运行	//low speed
void VS1003B_SPI_Low(void)
{
	SPCR =   _BV(SPE)|_BV(MSTR)|_BV(SPR1)|_BV(SPR0);
	SPSR &= ~_BV(SPI2X);
}

//以FCK/4速度运行	//full speed
void VS1003B_SPI_High(void)
{
	SPCR =  _BV(SPE)|_BV(MSTR);
	//SPSR &= ~_BV(SPI2X);
	SPSR |= _BV(SPI2X);
}

//延时		//delay
void VS1003B_Delay( unsigned int n)
{
	while(n--)asm("nop");
}

//向VS1003写一字节		//write one byte to vs1003
unsigned char VS1003B_WriteByte(unsigned char CH)
{   
	SPDR = CH;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}

//从VS1003读一字节		//read one byte from vs1003
unsigned char VS1003B_ReadByte()
{
    SPDR = 0xff;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}

//写寄存器，参数，地址和数据	//config register
void VS1003B_WriteCMD(unsigned char addr, unsigned int dat)
{
	VS1003B_XDCS_H();
	VS1003B_XCS_L();
	VS1003B_WriteByte(0x02);
	VS1003B_WriteByte(addr);
	VS1003B_WriteByte(dat>>8);
	VS1003B_WriteByte(dat);
	VS1003B_XCS_H();
}

//读寄存器，参数 地址 返回内容	//read register
unsigned int VS1003B_ReadCMD(unsigned char addr)
{
	unsigned int temp;
	VS1003B_XDCS_H();
	VS1003B_XCS_L();
	VS1003B_WriteByte(0x03);
	VS1003B_WriteByte(addr);
	temp = VS1003B_ReadByte();
	temp <<= 8;
	temp += VS1003B_ReadByte();
	VS1003B_XCS_H();
	return temp;
}

//写数据，音乐数据		//write data (music data)
void VS1003B_WriteDAT(unsigned char dat)
{
	VS1003B_XDCS_L();
	VS1003B_WriteByte(dat);
	VS1003B_XDCS_H();
	VS1003B_XCS_H();
}

// 1 means fail, 0 OK!
unsigned char VS1003B_Init()
{
	unsigned char retry;
	PORT_INI();
	VS1003B_DDR &=~_BV(VS1003B_DREQ);
	VS1003B_XCS_H();
	VS1003B_XDCS_H();
	VS1003B_XRESET_L();
	VS1003B_Delay(0xffff);
	VS1003B_XRESET_H();//使能芯片	//chip select
	VS1003B_SPI_Low();//先以低频操作	//low speed
	VS1003B_Delay(0xffff);//延时	//delay
	VS1003B_Delay(0xffff);
	retry=0;

	while(VS1003B_ReadCMD(0x03) != CLOCK_REG)//写时钟寄存器	//set PLL register
	{
		VS1003B_WriteCMD(0x03,CLOCK_REG);
		if(retry++ >10 )return 1;
	}

	VS1003B_Delay(0xffff);
	
	VS1003B_WriteCMD(0x05,0x000a);
	
	retry=0;
	while(VS1003B_ReadCMD(0x0b) != 0xfefe)//设音量	//set Volume
	{
		VS1003B_WriteCMD(0x0b,0xfefe);
		if(retry++ >10 )return 1;
	}

	VS1003B_WriteCMD(0x05,0xac45);

	retry=0;
	while(VS1003B_ReadCMD(0x0b) != DefaultVolume)//设音量	//set Volume
	{
		VS1003B_WriteCMD(0x0b,DefaultVolume);
		if(retry++ >10 )return 1;
	}

	retry=0;
	while(VS1003B_ReadCMD(0x00) != 0x0800)//写mode寄存器	//set mode register
	{
		VS1003B_WriteCMD(0x00,0x0800);
		if(retry++ >10 )return 1;//
	}

	VS1003B_Delay(0xffff);

	VS1003B_SoftReset();
	VS1003B_SPI_High();
	return 0;
}

//VS1003软件复位	//VS1003 soft reset
void VS1003B_SoftReset()
{
	VS1003B_WriteCMD(0x00,0x0804);//写复位		//reset
	VS1003B_Delay(0xffff);//延时，至少1.35ms //wait at least 1.35ms
}


