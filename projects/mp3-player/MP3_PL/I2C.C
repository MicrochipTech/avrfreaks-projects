#include "main.h"
#include "i2c.h"

#include <avr/io.h>

#define I2C_SPEED 200e3

#define START 0x08
#define REP_START 0x10
#define SLA_W_ACK 0x18
#define SLA_W_NACK 0x20
#define DATA_W_ACK 0x28
#define DATA_W_NACK 0x30
#define SLA_R_ACK 0x40
#define SLA_R_NACK 0x48
#define DATA_R_ACK 0x50
#define DATA_R_NACK 0x58
#define ARB_LOST 0x38



void i2c_init ()
{
	TWSR=0x01;							//prescaler 4
	TWBR=4;		//400kHz
	//(MCU_CLK/I2C_SPEED-16)/8;		//set bitrate, max 800kbit with this prescaler setting
	
	
}


byte i2c_transmit (byte adress, byte subadress, byte data)
{	byte error;
	
	TWCR=0xA4;							//send start condition
	while (!(TWCR&0x80));					//wait for int_flag
	error=((TWSR&0xF8)!=START);
	
	if (!error)
	{	TWDR=(adress<<1);				//load slave adress
		TWCR=0x84;						//clear int_flag to start transmission
		while (!(TWCR&0x80));				//wait for int_flag
		error=((TWSR&0xF8)!=SLA_W_ACK);
	}
	
	if (!error)
	{	TWDR=subadress;
		TWCR=0x84;						//clear int_flag to start transmission
		while (!(TWCR&0x80));				//wait for int_flag
		error=((TWSR&0xF8)!=DATA_W_ACK);
	}
	
		if (!error)
	{	TWDR=data;
		TWCR=0x84;						//clear int_flag to start transmission
		while (!(TWCR&0x80));				//wait for int_flag
		error=((TWSR&0xF8)!=DATA_W_ACK);
	}

	TWCR=0x94;						//generate stop condition
		
	return error;
}



byte i2c_receive (byte adress, byte subadress, byte* data)
{	byte error;
	
	TWCR=0xA4;							//send start condition
	while (!(TWCR&0x80));					//wait for int_flag
	error=((TWSR&0xF8)!=START);
	
	if (!error)
	{	TWDR=(adress<<1);			//load slave adress
		TWCR=0x84;						//clear int_flag to start transmission
		while (!(TWCR&0x80));				//wait for int_flag
		error=((TWSR&0xF8)!=SLA_W_ACK);
	}
	
	if (!error)
	{	TWDR=subadress;
		TWCR=0x84;						//clear int_flag to start transmission
		while (!(TWCR&0x80));				//wait for int_flag
		error=((TWSR&0xF8)!=DATA_W_ACK);
	}

	TWCR=0xA4;							//send start condition
	while (!(TWCR&0x80));					//wait for int_flag
	error=((TWSR&0xF8)!=REP_START);

	if (!error)
	{	TWDR=(adress<<1)|0x01;			//load slave adress + read bit
		TWCR=0x84;						//clear int_flag to start transmission
		while (!(TWCR&0x80));				//wait for int_flag
		error=((TWSR&0xF8)!=SLA_R_ACK);
	}
	
	if (!error)
	{	TWCR=0x84;						//clear int_flag to start transmission
		while (!(TWCR&0x80));				//wait for int_flag
		*data=TWDR;
	}

	TWCR=0x94;						//generate stop condition

	return error;
}
