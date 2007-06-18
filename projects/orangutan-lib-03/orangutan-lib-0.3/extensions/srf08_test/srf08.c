/*
**	Orangutan (ATmega168) Devantech SRF08 Access Library
**
**	Steven G. Judd, Tentacle Robotics
**	sgjudd@gmail.com
**
**	V0-00	Feb-1-2007.
*/

#include "device.h"
#include "srf08.h"
#include "util/delay.h"

unsigned char srf08_ping(unsigned char i2c_addr, 
						 unsigned char i2c_cmd) {
	unsigned char ret;

    i2c_start_wait(i2c_addr+I2C_WRITE);     // set device address and write mode
    ret=i2c_write(0x0);						// write to location zero
	if (ret) return(ret);
 	ret=i2c_write(i2c_cmd);					// send ping command
	if (ret) return(ret);

	return(I2C_OK);
}

unsigned char srf08_echo(unsigned char i2c_addr, unsigned char nEcho, int *echo) {

	unsigned char ret, i;
	int	*buf;

	union 	{int iRange;
			 unsigned char ucRange[2];
			} uRange;

	buf=echo;

	if (nEcho > 34) return(SRF_BADPARAM);

    i2c_start_wait(i2c_addr+I2C_WRITE);     // set device address and write mode
    ret=i2c_write(0x2);						// set loc 0x2
	if (ret) return(ret);
	i2c_stop();
	i2c_start_wait(i2c_addr+I2C_READ);

//	ret=i2c_readAck();						// byte 0 - vers
//	ret=i2c_readAck();						// byte 1 - light

	for (i=0;i<nEcho-1;i++) {				// bytes n -> n-1

		uRange.ucRange[1]=i2c_readAck();
		uRange.ucRange[0]=i2c_readAck();
		*buf++=uRange.iRange;
	}
		uRange.ucRange[1]=i2c_readAck();
		uRange.ucRange[0]=i2c_readNak();
		*buf=uRange.iRange;

	i2c_stop();

	return(I2C_OK);
}

unsigned char srf08_light(unsigned char i2c_addr, unsigned char *ucLight) {
	unsigned char ret;

	i2c_start_wait(i2c_addr+I2C_WRITE);     // set device address and write mode
	ret=i2c_write(SRF08_LIGHT);				// set address 1
	if (ret) return(ret);
	i2c_rep_start(i2c_addr+I2C_READ);
	*ucLight=i2c_readNak();
	i2c_stop();
	return(I2C_OK);
}

unsigned char srf08_setRange(unsigned char i2c_addr, int iRangeMax) {
	unsigned char ret;

	i2c_start_wait(i2c_addr+I2C_WRITE);     // set device address and write mode
	ret=i2c_write(0x2);						// write to location two
	if (ret) return(ret);
	ret=i2c_write((unsigned char)(iRangeMax/43));	// range in 43mm units
	if (ret) return(ret);

    i2c_stop(); 


	return(I2C_OK);
}

unsigned char srf08_setGain(unsigned char i2c_addr, unsigned char gain) {
	unsigned char ret;

	i2c_start_wait(i2c_addr+I2C_WRITE);     // set device address and write mode
	ret=i2c_write(0x1);						// write to location two
	if (ret) return(ret);
	ret=i2c_write(gain);					// analog gain
	if (ret) return(ret);

    i2c_stop(); 


	return(I2C_OK);
}

unsigned char srf08_setAddr(unsigned char i2c_addr, unsigned char new_addr) {
	unsigned char ret;

	i2c_start_wait(i2c_addr+I2C_WRITE);     // set device address and write mode
	ret=i2c_write(0x0);					
	if (ret) return(ret);
	ret=i2c_write(0xA0);
	if (ret) return(ret);
	i2c_stop();
						
	i2c_start_wait(i2c_addr+I2C_WRITE);     // set device address and write mode
	ret=i2c_write(0x0);					
	if (ret) return(ret);
	ret=i2c_write(0xAA);						
	if (ret) return(ret);
	i2c_stop();

	i2c_start_wait(i2c_addr+I2C_WRITE);     // set device address and write mode
	ret=i2c_write(0x0);					
	if (ret) return(ret);
	ret=i2c_write(0xA5);						
	if (ret) return(ret);
	i2c_stop();

	i2c_start_wait(i2c_addr+I2C_WRITE);     // set device address and write mode
	ret=i2c_write(0x0);					
	if (ret) return(ret);
	ret=i2c_write(new_addr);					
	if (ret) return(ret);
    i2c_stop(); 


	return(I2C_OK);
}
