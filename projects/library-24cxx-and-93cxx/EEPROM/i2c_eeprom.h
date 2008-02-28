

#ifndef	_I2C_EEPROM_H
#define	_I2C_EEPROM_H


#include <avr/io.h>
#include <util/delay.h>

#include "I2c_routines.h"

#define	WRITE	0
#define	READ	1

#define fTrue	255
#define fFalse	0




void			i2c_eeprom_init(void);
unsigned char 	i2c_eeprom_start(unsigned int address,unsigned char mode);
unsigned char   i2c_eeprom_start16(unsigned int address,unsigned char mode);  // for 2432 - 2464
void 		  	i2c_eeprom_stop();
unsigned char 	i2c_eeprom_write(unsigned char data);
unsigned char 	i2c_eeprom_read(unsigned char last);






#endif
