//**********************************************************************//
//FILE: i2c_eeprom.h 
//AUTHOR: Adam Kadolph
//DATE:	12-5-2008
//DESCRIPTION: Header file to declare i2c eeprom functions
//**********************************************************************//

#ifndef i2c_eeprom_h
#define i2c_eeprom_h

unsigned char buffer[8];

//define EEPROM R/W addresses
#define  EEPROMA_W			0xA8
#define  EEPROMA_R			0xA9
#define  EEPROMB_W			0xAA
#define  EEPROMB_R			0xAB
#define  EEPROMC_W			0xAC
#define  EEPROMC_R			0xAD
#define  EEPROMD_W			0xAE
#define  EEPROMD_R			0xAF

//declare eeprom functions
unsigned char i2c_EEPROM_byte_read(unsigned int addr);
void i2c_EEPROM_byte_write(unsigned int addr, unsigned char data);
void i2c_EEPROM_page_read( unsigned int page);
void i2c_EEPROM_page_write(unsigned int page);
void i2c_EEPROM_erase(void);

#endif
