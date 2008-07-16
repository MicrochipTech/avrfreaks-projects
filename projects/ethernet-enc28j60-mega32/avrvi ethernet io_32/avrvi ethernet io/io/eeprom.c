/*
,-----------------------------------------------------------------------------------------.
| io/eeprom
|-----------------------------------------------------------------------------------------
| this file implements functions for reading & writing an 24LCxx I2C EEprom
| - BLOCKING !!! BE CAREFULL! 
|
| Author   : {{removed according to contest rules}}
|            -> circuitcellar.com avr design contest 2006
|            -> Entry #AT2616
|
|-----------------------------------------------------------------------------------------
| License:
| This program is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free Software
| Foundation; either version 2 of the License, or (at your option) any later
| version.
| This program is distributed in the hope that it will be useful, but
|
| WITHOUT ANY WARRANTY;
|
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with
| this program; if not, write to the Free Software Foundation, Inc., 51
| Franklin St, Fifth Floor, Boston, MA 02110, USA
|
| http://www.gnu.de/gpl-ger.html
`-----------------------------------------------------------------------------------------*/

#include "eeprom.h"
#include "../main.h"
#include <util/delay.h>

#define EEPROM_DEVICE_ID 0xA0
#define EEPROM_READ  1
#define EEPROM_WRITE 0

void eeprom_init(void) {
	//initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 
	#if defined(TWPS0)
		//has prescaler (mega128 & newer)
		TWSR = 0;
	#endif
	TWBR = 6;
}

void eeprom_send_start(){
	//start cond.
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

void eeprom_send_stop(){
	//stop condition
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void eeprom_send_device_address(unsigned char addr){
	//send device address (including pge offset + write/read flag)
	TWDR = addr;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

void eeprom_send_byte(unsigned char byte){
	TWDR = byte;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

unsigned char eeprom_read(){
	//start read transmission
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

void eeprom_write(unsigned char data){
	//load byte to data reg and start transmission
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

unsigned char eeprom_read_byte(unsigned int addr){
	unsigned char res;
	unsigned char eeprom_addr;

	eeprom_addr = (EEPROM_DEVICE_ID&0xF0) | ((addr>>7)&0x0E);

	//send start
	eeprom_send_start();

	//send device address & page info: (write cmd !)
	eeprom_send_device_address(eeprom_addr | EEPROM_WRITE);
	
	//send eeprom word address:
	eeprom_send_byte(addr&0xFF);

	//send start
	eeprom_send_start();

	//send device address & page info: (read cmd !)
	eeprom_send_device_address(eeprom_addr | EEPROM_READ);
	
	//read data:
	res = eeprom_read();

	//stop
	eeprom_send_stop();

	return res;
}	

void eeprom_wait_for_write_finished(unsigned int addr){
	unsigned char eeprom_addr;

	eeprom_addr = (EEPROM_DEVICE_ID&0xF0) | ((addr>>7)&0x0E);
	//pull write completion flag:
	while(1){
		eeprom_send_start();
		//eeprom_send_device_address(eeprom_addr | EEPROM_WRITE);
		TWDR = eeprom_addr | EEPROM_WRITE;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while (!(TWCR & (1 << TWINT)));
		
		//if we have an ack -> write is finished !
		if ((TWSR & 0xF8) == 0x18){
			eeprom_send_stop();
			return;
		}
		eeprom_send_stop();
	}
}


void eeprom_write_byte(unsigned int addr, unsigned char buf) {
	unsigned char eeprom_addr;

	eeprom_addr = (EEPROM_DEVICE_ID&0xF0) | ((addr>>7)&0x0E);
	
	eeprom_wait_for_write_finished(addr);

	//send start
	eeprom_send_start();

	//send device address & page info: (write cmd !)
	eeprom_send_device_address(eeprom_addr | EEPROM_WRITE);
	
	//send eeprom word address:
	eeprom_send_byte(addr&0xFF);

	//write data:
	eeprom_write(buf);

	//stop
	eeprom_send_stop();

	
}

void eeprom_dump(){
	unsigned int i;
	for(i=0; i<8*255; i++)
		uart_putc(eeprom_read_byte(i));	
}


//erases all data !!!!
void eeprom_clear(){
	unsigned int i;
	for(i=0; i<8*255; i++)
		eeprom_write_byte(i, 0x00);	
}
