/*
,-----------------------------------------------------------------------------------------.
| io/lm75
|-----------------------------------------------------------------------------------------
| this file supports temperature reading of an i2c temp sensor
| - supports LM75 & clones, adjust LM75_DEVICE_ID in config.h !!
| - does not check for ACK/NACK/ERROR !
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

#include "lm75.h"
#include "../main.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

#include <util/delay.h>

#define LM75_READ  1
#define LM75_WRITE 0

#define LM75_STATE_IDLE            0
#define LM75_STATE_DEVICE_ADDRESS  1
#define LM75_STATE_WRITE_CMD0      2
#define LM75_STATE_START2          3
#define LM75_STATE_DEVICE_ADDRESS2 4
#define LM75_STATE_DATA_HI         5
#define LM75_STATE_DATA_LO         6
#define LM75_STATE_STOP            7
#define LM75_STATE_ERROR           8

#define LM75_ANSWER_START         0x08
#define LM75_ANSWER_RESTART       0x10
#define LM75_ANSWER_SLAVE_W_ACK   0x18
#define LM75_ANSWER_SLAVE_R_ACK   0x40
#define LM75_ANSWER_DATA_ACK      0x28
#define LM75_ANSWER_DATA_NACK     0x58

volatile unsigned char lm75_state;
volatile unsigned int lm75_temp_tmp;
void lm75_init(void) {
	//initialize TWI clock: TWPS = 0 => prescaler = 1 
	#if defined(TWPS0)
		//has prescaler (mega128 & newer)
		TWSR = 0;//0;
	#endif
	//slow...
	TWBR = 20; //f(SCL) = F_CPU / (16+2*TWBR)*4^TWPS)

	lm75_state = LM75_STATE_IDLE;

	lm75_temp_tmp = (32<<1);
}

//do a nonblocking read. this function must be called
// ? times in order to read a value.
// returns 1 if new value measured, otherwise 0
unsigned char lm75_nonblocking_temp_read(unsigned char *data){
	unsigned char retval = 0;

	switch(lm75_state){
		//send start cmd & wait for TWI to send it:
		case(LM75_STATE_IDLE):
			lm75_send_start();
			//check for error:
			if ((TWSR & 0xF8) != LM75_ANSWER_START)
				lm75_state = LM75_STATE_ERROR; 
			else
				lm75_state = LM75_STATE_DEVICE_ADDRESS;
			break;

		//send device address:
		case(LM75_STATE_DEVICE_ADDRESS):
			lm75_send_device_address(LM75_DEVICE_ID | LM75_WRITE);

			//check for error:
			if ((TWSR & 0xF8) != LM75_ANSWER_SLAVE_W_ACK)
				lm75_state = LM75_STATE_ERROR; 
			else
				lm75_state = LM75_STATE_WRITE_CMD0;
			break;

		//send "set temp pointer" cmd
		case(LM75_STATE_WRITE_CMD0):
			lm75_send_byte(0x00);

			//check for error:
			if ((TWSR & 0xF8) != LM75_ANSWER_DATA_ACK)
				lm75_state = LM75_STATE_ERROR; 
			else
				lm75_state = LM75_STATE_START2;
			break;

		//send a second start:
		case(LM75_STATE_START2):
			lm75_send_start();

			//check for error:
			if ((TWSR & 0xF8) != LM75_ANSWER_RESTART)
				lm75_state = LM75_STATE_ERROR; 
			else
				lm75_state = LM75_STATE_DEVICE_ADDRESS2;
			break;

		//send address again:
		case(LM75_STATE_DEVICE_ADDRESS2):
			lm75_send_device_address(LM75_DEVICE_ID | LM75_READ);

			//check for error:
			if ((TWSR & 0xF8) != LM75_ANSWER_SLAVE_R_ACK)
				lm75_state = LM75_STATE_ERROR; 
			else
				lm75_state = LM75_STATE_DATA_HI;
			break;

		//read data hi:
		case(LM75_STATE_DATA_HI):
			lm75_temp_tmp = (lm75_read()<<8);

			//check for error:
			if ((TWSR & 0xF8) != LM75_ANSWER_DATA_NACK)
				lm75_state = LM75_STATE_ERROR; 
			else
				lm75_state = LM75_STATE_DATA_LO;
			break;

		//read data lo:
		case(LM75_STATE_DATA_LO):
			lm75_temp_tmp |= lm75_read();

			//is temp negative ? -> convert
			if (lm75_temp_tmp&0x8000){
				//convert from 2s complement:
				lm75_temp_tmp = (0xFFFF-lm75_temp_tmp)|0x8000;
			}
			
			//tttt tttt t000 0000 -> 0000 000t tttt tttt
			lm75_temp_tmp = lm75_temp_tmp>>7;

			//now convert temperature to an 8 bit value.
			//-> we are interested in -32.0 to +96.5 degrees
			lm75_temp_tmp = lm75_temp_tmp + (32<<1);

			//limit range 1:
			if (lm75_temp_tmp < 0)
				lm75_temp_tmp = 0;

			//limit range 2:
			if (lm75_temp_tmp >= 255){
				lm75_temp_tmp = 255;
				return 0;
			}

/*
			//convert to temp vale. 205 = 20.5 °C
			//tttt tttt t000 0000 -> 0000 000t tttt tttt
			lm75_temp_tmp = lm75_temp_tmp>>7;

			//convert to temp vale. 205 = 20.5 °C
			(*data) = ((lm75_temp_tmp>>1)&0x007F)*10; 
	
			//add 0.5°C step:
			if (lm75_temp_tmp&0x0001)
				*data = (*data) + 5;
			//is temp negative ?
			if (lm75_temp_tmp&0x0100)
				*data = -(*data);
*/
			(*data) = (lm75_temp_tmp)&0xFF;
			//success:
			retval = 1;

			//dont check for error, next state is always stop:
		case(LM75_STATE_STOP):
		default:
			lm75_send_stop();
			lm75_state = LM75_STATE_IDLE; 
			break;
		case(LM75_STATE_ERROR):
			#if LM75_DEBUG
				softuart_puts_progmem("LM75: error in state ");
				softuart_put_uint8(lm75_state);
				softuart_puts_progmem(" TWSR=");
				softuart_put_uint8(TWSR);
				softuart_putnewline();
			#endif
			lm75_state = LM75_STATE_STOP;
	}

	return retval;
}

void lm75_send_start(){
	//start cond.
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

void lm75_send_stop(){
	//stop condition
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void lm75_send_device_address(unsigned char addr){
	//send device address (including pge offset + write/read flag)
	TWDR = addr;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

void lm75_send_byte(unsigned char byte){
	TWDR = byte;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

unsigned char lm75_read(){
	//start read transmission
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

void lm75_write(unsigned char data){
	//load byte to data reg and start transmission
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

/*
int lm75_read_temp(){
	unsigned int res;
	int tmp;

	//send start
	lm75_send_start();

return 0;

	//send device address 
	lm75_send_device_address(LM75_DEVICE_ID | LM75_WRITE);
	
	//send lm75 pointer cmd for temp
	lm75_send_byte(0x00);

	//send start
	lm75_send_start();

	//send device address
	lm75_send_device_address(LM75_DEVICE_ID | LM75_READ);
	
	//read data hi:
	res = (lm75_read()<<8);
	
	//read data_lo
	res |= lm75_read();

	//tttt tttt t000 0000 -> 0000 000t tttt tttt
	res = res>>7;

	//convert to temp vale. 205 = 20.5 °C
	tmp = ((res>>1)&0x007F)*10; 
	
	//add 0.5°C step:
	if (res&0x0001)
		tmp = tmp + 5;
	//is temp negative ?
	if (res&0x0100)
		tmp = -tmp;
	
	//stop
	lm75_send_stop();

	return tmp;
}	
*/
