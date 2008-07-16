/*
,-----------------------------------------------------------------------------------------.
| apps/logger
|-----------------------------------------------------------------------------------------
| this file implements a temperature logger
| - store current day in eeprom (240 bytes)
| - (add later: backup day at 0 o'clock to dataflash)
|
| Author   : {{removed according to contest rules}}
|            -> circuitcellar.com avr design contest 2006
|            -> Entry #AT2616
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

#include "logger.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

//store current day in eeprom
unsigned char logger_temp_today[240] EEMEM;

//some status vars
unsigned char logger_count;
unsigned int  logger_lastsave;
unsigned char logger_data;

//log every 7*8 = 56 seconds:
#define LOGGER_COUNTER_INIT 7

//initialise logger
void logger_init(){
	logger_count    = 1;
	logger_lastsave = 0;
	logger_data = (32<<1); //0°C

	//let lm75 grab first data, try it 2 times
	//(2*8 calls!)
	unsigned char data;
	for(unsigned char c=0; c<16; c++)
		if (lm75_nonblocking_temp_read(&data)==1)
			break;
}

//call this function every second !
//it will grab the temperature every 56seconds
//and store the temperature in eeprom every 6 minutes:
void logger_do(){
	unsigned char data;
	
	logger_count--;

	//we need to log call the i2c statemachine:
	if (!logger_count){
		data = 0;
		logger_count = LOGGER_COUNTER_INIT;

		//if =1 -> there is a new temperature!
		if (lm75_nonblocking_temp_read(&data)==1){
			#if LOGGER_DEBUG
			softuart_puts_progmem("LOG : ");
			
			signed char t = data-(32<<1);
			if (t>=0){
				softuart_put_uint8(t>>1);
			}else{
				softuart_putc('-');
				softuart_put_uint8((-t)>>1);
			}
			softuart_putc('.');
			if ((t&1) == 0)
				softuart_putc('0');
			else
				softuart_putc('5');

			softuart_puts_progmem(" C := ");
			softuart_put_uint8(data);
			softuart_putnewline();
			#endif
				
			//calc eeprom pos:
			unsigned int tnow = (clock[CLOCK_HOUR]*60+clock[CLOCK_MIN])/6;

			//if this is the next day -> do backup!
			if (tnow >= 23*60+59/6){
				///BACKUP HERE to dataflash archive (add later!)
			}else{
				//save new data to eeprom:
				if (tnow != logger_lastsave){
					logger_lastsave = tnow;
					eeprom_write_byte(&logger_temp_today[(tnow & 0xFF)], data); 
				}
				logger_count = 1;
			}
			logger_data = data;
		}
	}
}

//return LOGGER_NOW, LOGGER_MIN or LOGGER_MAX (temperature now, today min, today max)
void logger_output_temp(unsigned char *buffer, unsigned char type){
	unsigned char min=0xFF;
	unsigned char max=0x00;
	unsigned char val;
	unsigned char bufpos=0;

	unsigned char tnow = ((clock[CLOCK_HOUR]*60+clock[CLOCK_MIN])/6);
	if (tnow > (23*60+59)/6)
		tnow = (23*60+59)/6;

	//only search from 0 o'clock to now !
	for(unsigned char i=0; i<=tnow; i++){
		val = eeprom_read_byte(&logger_temp_today[i]);
		if (val < min)
			min = val;
		if (val > max)
			max = val;
	}

	//now output data:
	if (type == LOGGER_MIN)
		val = min;
	else if (type == LOGGER_MAX)
		val = max;
	else
		val = logger_data; //eeprom_read_byte(&logger_temp_today[(clock[CLOCK_HOUR]*60+clock[CLOCK_MIN])/6]);

	signed char t = val-(32<<1);
	if (t>=0){
		//copy string to buf:
		string_uint8_to_buffer(&buffer[bufpos], t>>1);

		//overwrite left '0' with sign:
		buffer[bufpos] = ' ';

		bufpos+=3;
	}else{
		string_uint8_to_buffer(&buffer[bufpos], (-t)>>1);

		//overwrite left '0' with sign:
		buffer[bufpos] = '-';

		bufpos+=3;
	}
	buffer[bufpos++] = '.';
	if ((t&1) == 0)
		buffer[bufpos++] = '0';
	else
		buffer[bufpos++] = '5';

	#if LOGGER_DEBUG
	softuart_puts_progmem("LOG : requestet temp => ");
	softuart_put_uint8(val);
	softuart_puts_progmem(" -> buf[");
	for(t=0; t<5; t++)
		softuart_putc(buffer[t]);
	softuart_puts_progmem("]");
	softuart_putnewline();
	#endif
}
