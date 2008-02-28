

//#ifdef _I2C_EEPROM_H



#include "i2c_eeprom.h"

void i2c_eeprom_init(void)
{
	i2cInit();
}

/////////////////////////////////////////////////////////////////////


unsigned char i2c_eeprom_start(unsigned int address,unsigned char mode)
{
	unsigned char ack;
	unsigned char add_mask;

	add_mask = (address & 0xff00) >> 7;
	start();
	sendByte(0b10100000 | add_mask);
	ack = receiveAck();
	if(ack == fFalse)
		return fFalse;
	add_mask = address & 0x00ff;
	sendByte(add_mask);
	ack = receiveAck();
	if(ack == fFalse)
		return fFalse;
	if(mode == READ)
	{
		add_mask = (address & 0xff00) >> 7;
		restart();
		sendByte(0b10100001 | add_mask);
		ack = receiveAck();
		if(ack == fFalse)
			return fFalse;
	}
	
	return fTrue;
}
/////////////////////////////////////////////////////////////////////

unsigned char i2c_eeprom_start16(unsigned int address,unsigned char mode)
{
	unsigned char ack;
	unsigned char add_mask;


	start();
	sendByte(0b10100000);
	ack = receiveAck();
	if(ack == fFalse)
		return fFalse;

	add_mask = (address & 0xff00) >> 8;
	sendByte(add_mask);
	ack = receiveAck();
	if(ack == fFalse)
		return fFalse;

	add_mask = (address & 0x00ff) ;
	sendByte(add_mask);
	ack = receiveAck();
	if(ack == fFalse)
		return fFalse;
	
	if(mode == READ)
	{
		restart();
		sendByte(0b10100001);
		ack = receiveAck();
		if(ack == fFalse)
			return fFalse;
	}
	return fTrue;
}
/////////////////////////////////////////////////////////////////////
void i2c_eeprom_stop()
{
	stop();

}
/////////////////////////////////////////////////////////////////////
unsigned char i2c_eeprom_write(unsigned char data)
{
	unsigned char ack;
	sendByte(data);
	ack = receiveAck();
	if(ack == fFalse)
		return fFalse;
	else
		return fTrue;
}
/////////////////////////////////////////////////////////////////////
unsigned char i2c_eeprom_read(unsigned char last)
{

	unsigned char data;
	data = receiveByte();
	if(last == 1)
		sendNack();
	else
		sendAck();
	return data;
}
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////

//#endif
