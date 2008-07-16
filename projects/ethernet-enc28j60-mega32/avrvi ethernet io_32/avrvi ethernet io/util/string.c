/*
,-----------------------------------------------------------------------------------------.
| util/string
|-----------------------------------------------------------------------------------------
| this file implements some string routines
| - replacing the huge printf/scanf routines
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


#include "string.h"

//find progmemstring in buffer (null-terminated!)
//return:
//  -1    -> not found
// <pos>  -> position of progmemstring in buffer
int string_search_progmem(PGM_P pointer, unsigned char *buffer){
	//use strcmp_P(txt, PSTR("aAuthorization: Basic ")))
	unsigned char c;	
	int found;
	int pos=0;
	PGM_P pointer2;
	
	while(buffer[pos] != 0){
		c = pgm_read_byte(pointer);
		pointer2 = pointer;
		found = pos;
		//match of first char
		if (buffer[pos] == c){
			//continue:
			pointer2++;
			pos++;
			c = pgm_read_byte(pointer2);
			while((c != 0) && (buffer[pos] == c)){
				softuart_putc(c); softuart_putc('=');softuart_putc(buffer[pos]); softuart_putnewline();
				pointer2++;
				pos++;
				c = pgm_read_byte(pointer);
			}

			//if c == 0 -> we have a match
			if (c==0){
				//match !
				return found;
			}
			pos=found;
		}
		pos++;
		pointer++;
	}
	return -1;
}

///WARNING: not tested!
//return 1 if both strings match
//return 0 otherwise
//string_compare_progmem is defined in string.h !
char string_compare_progmem2(PGM_P pointer, unsigned char *string, unsigned char check_eof){
	unsigned char i=0;
	while(pgm_read_byte(pointer)){
		//softuart_putc(string[i]);
		//softuart_putc('=');
		//softuart_putc(pgm_read_byte(pointer));
		//softuart_putnewline();
		if (string[i] != pgm_read_byte(pointer))
			return 0;
		pointer++;
		i++;
	}
	//check end of both strings ?
	if (check_eof && (string[i] != 0))
		return 0;
	else
		return 1;
}

char string_compare_progmem_plen2(PGM_P pointer, unsigned int plen, unsigned char *string){
	unsigned char i=0;
	while(plen--){
		//softuart_put_uint8(string[i]);
		//softuart_putc('=');
		//softuart_put_uint8(pgm_read_byte(pointer));
		//softuart_putnewline();
		if (string[i] != pgm_read_byte(pointer)){
			//softuart_putc('-');
			//while(plen--)
			//	softuart_put_uint8(string[i++]);
			return 0;
		}
		pointer++;
		i++;
	}
	return 1;
}

void string_uint16_to_buffer(unsigned char *buffer, unsigned int val){
	unsigned int fac = 10000;
	unsigned char c=0;
	for(fac=10000; fac>0; fac = fac/10){
		buffer[c] = '0';
		while(val >= fac){
			val -= fac;
			buffer[c]++;
		}
		c++;
	}
}

unsigned long string_buffer_to_uint32(unsigned char *buffer){
	unsigned char c;
	unsigned long res;

	res = 0;
	c=0;
	while ((buffer[c]>='0') && (buffer[c] <= '9') && c<5){
		res = res * 10;
		res += (buffer[c]-'0');
		c++;
	}
	return res;
}

unsigned int string_buffer_to_uint16(unsigned char *buffer){
	unsigned char c;
	unsigned int res;

	res = 0;
	c=0;
	while ((buffer[c]>='0') && (buffer[c] <= '9') && c<5){
		res = res * 10;
		res += (buffer[c]-'0');
		c++;
	}
	return res;
}

void string_uint8_to_buffer(unsigned char *buffer, unsigned char val){
	unsigned int fac = 100;
	unsigned char c=0;
	for(fac=100; fac>0; fac = fac/10){
		buffer[c] = '0';
		while(val >= fac){
			val -= fac;
			buffer[c]++;
		}
		c++;
	}
	for (c=0; c<2; c++){
		if (buffer[c] == '0')
			buffer[c] = ' ';
		else
			break;
	}
}

void string_uint8_to_buffer_02dez(unsigned char *buffer, unsigned char val){
	while(val>=100)
		val -= 100;

	buffer[0] = '0';
	while (val>= 10){
		val -= 10;
		buffer[0]++;
	}

	buffer[1] = '0' + val;
}

void string_progmem_to_buffer(PGM_P pointer, unsigned char *buffer, unsigned int len){
	while(len--){
		*buffer++ = pgm_read_byte(pointer++);
	}
}



