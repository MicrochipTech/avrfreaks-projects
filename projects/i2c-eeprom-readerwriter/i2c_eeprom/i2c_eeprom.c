//**********************************************************************//
//FILE: i2c_eeprom.c 
//AUTHOR: Adam Kadolph
//DATE:	12-7-2008
//DESCRIPTION: C file to handle i2c eeprom byte and page read/write operations
//**********************************************************************//

#include "i2c_eeprom.h"
#include "twi.h"
#include "lcd.h"

unsigned char i2c_EEPROM_byte_read(unsigned int addr)
{
	unsigned char low,high,status,data;

	high = (addr>>8);
	low = addr;
	
	status=i2c_start();
	if(status == 1)
	{
		LCDstring("EEPROM error 1",14);
		i2c_stop();
		return;
	}
	
	status=i2c_sendAddr(EEPROMA_W);
	if(status == 1)
	{
		LCDstring("EEPROM error 2",14);
		i2c_stop();
		return;
	}

	status=i2c_sendData(high);
	if(status == 1)
	{
		LCDstring("EEPROM error 3",14);
		i2c_stop();
		return;
	}

	status=i2c_sendData(low);
	if(status == 1)
	{
		LCDstring("EEPROM error 4",14);
		i2c_stop();
		return;
	}

	status=i2c_repeatStart();
	if(status == 1)
	{
		LCDstring("EEPROM error 5",14);
		i2c_stop();
		return;
	}
	
	status=i2c_sendAddr(EEPROMA_R);
	if(status == 1)
	{
		LCDstring("EEPROM error 6",14);
		i2c_stop();
		return;
	}

	data = i2c_receiveData_NACK();
	if(data == ERROR_CODE)
   	{
    	LCDstring("EEPROM error 7",14);
		i2c_stop();
	   	return;
   	}
	i2c_stop();
	return(data);
}
/////////////////////////////////////////////////////////////////
void i2c_EEPROM_byte_write(unsigned int addr, unsigned char data)
{
	unsigned char low,high,status;

	high = (addr>>8);
	low = addr;

	status=i2c_start();
	if(status == 1)
	{
		LCDstring("EEPROM error 8",14);
		i2c_stop();
		return;
	}
	
	status=i2c_sendAddr(EEPROMA_W);
	if(status == 1)
	{
		LCDstring("EEPROM error 9",14);
		i2c_stop();
		return;
	}

	status=i2c_sendData(high);
	if(status == 1)
	{
		LCDstring("EEPROM error 10",15);
		i2c_stop();
		return;
	}

	status=i2c_sendData(low);
	if(status == 1)
	{
		LCDstring("EEPROM error 11",15);
		i2c_stop();
		return;
	}

	status=i2c_sendData(data);
	if(status == 1)
	{
		LCDstring("EEPROM error 12",15);
		i2c_stop();
		return;
	}
	i2c_stop();
}
////////////////////////////////////////////////
void i2c_EEPROM_page_read(unsigned int page)
{
	int i;
	unsigned char low,high,addr,status,data;

	addr = page * 8;

	high = (addr>>8);
	low = addr;
	
	status=i2c_start();
	if(status == 1)
	{
		LCDstring("EEPROM error 13",15);
		i2c_stop();
		return;
	}
	
	status=i2c_sendAddr(EEPROMA_W);
	if(status == 1)
	{
		LCDstring("EEPROM error 14",15);
		i2c_stop();
		return;
	}

	status=i2c_sendData(high);
	if(status == 1)
	{
		LCDstring("EEPROM error 15",15);
		i2c_stop();
		return;
	}

	status=i2c_sendData(low);
	if(status == 1)
	{
		LCDstring("EEPROM error 16",15);
		i2c_stop();
		return;
	}

	status=i2c_repeatStart();
	if(status == 1)
	{
		LCDstring("EEPROM error 17",15);
		i2c_stop();
		return;
	}
	
	status=i2c_sendAddr(EEPROMA_R);
	if(status == 1)
	{
		LCDstring("EEPROM error 18",15);
		i2c_stop();
		return;
	}
	for(i=0; i<8; i++)
	{
		if(i == 7)
			data = i2c_receiveData_NACK();
		else
			data = i2c_receiveData_ACK();
		if(data == ERROR_CODE)
   		{
    		LCDstring("EEPROM error 19",15);
			i2c_stop();
	   		return;
   		}
		buffer[i] = data;
	}
	i2c_stop();
}
////////////////////////////////////////////////////////////////////
void i2c_EEPROM_page_write(unsigned int page)
{
	int i;
	unsigned char low,high,addr,status,data;

	addr = page * 8;	//8 bytes per page in Memory

	high = (addr>>8);	//split page integer into two bytes for transfer
	low = addr;

	status=i2c_start();
	if(status == 1)
	{
		LCDstring("EEPROM error 20",15);
		i2c_stop();
		return;
	}
	
	status=i2c_sendAddr(EEPROMA_W);
	if(status == 1)
	{
		LCDstring("EEPROM error 21",15);
		i2c_stop();
		return;
	}

	status=i2c_sendData(high);
	if(status == 1)
	{
		LCDstring("EEPROM error 22",15);
		i2c_stop();
		return;
	}

	status=i2c_sendData(low);
	if(status == 1)
	{
		LCDstring("EEPROM error 23",15);
		i2c_stop();
		return;
	}
	
	for(i=0; i<8; i++)
	{
		data = buffer[i];		//write data to memory from buffer array

		status=i2c_sendData(data);
		if(status == 1)
		{
			LCDstring("EEPROM error 24",15);
			i2c_stop();
			return;
		}
	}
	i2c_stop();
}
/////////////////////////////
void i2c_EEPROM_erase(void)
{
	int i;
	unsigned char data,status;

	status=i2c_start();
	if(status == 1)
	{
		LCDstring("EEPROM error 25",15);
		i2c_stop();
		return;
	}
	
	status=i2c_sendAddr(EEPROMA_W);
	if(status == 1)
	{
		LCDstring("EEPROM error 26",15);
		i2c_stop();
		return;
	}

	status=i2c_sendData(0x00);		//set memory pointer to location 0
	if(status == 1)
	{
		LCDstring("EEPROM error 27",15);
		i2c_stop();
		return;
	}

	status=i2c_sendData(0x00);
	if(status == 1)
	{
		LCDstring("EEPROM error 28",15);
		i2c_stop();
		return;
	}
	
	for(i=0; i<4096; i++)
	{
		data = 0xFF;		//clear all memory

		status=i2c_sendData(data);
		if(status == 1)
		{
			LCDstring("EEPROM error 29",15);
			i2c_stop();
			return;
		}
	}
	i2c_stop();
}
