/*
,-----------------------------------------------------------------------------------------.
| io/softuart
|-----------------------------------------------------------------------------------------
| this file implements a very basic 19200baud software uart
| - USE AN 7.3728MHz OSC !
| - does NOT use any timer
| - might not work with every uart
| - if interrupted by timer interrupt there might be some garbage
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
#include "softuart.h"

void softuart_put_ip(uint32_t ip){
	softuart_put_uint8(ip>>24); softuart_putc('.');
	softuart_put_uint8(ip>>16); softuart_putc('.');
	softuart_put_uint8(ip>> 8); softuart_putc('.');
	softuart_put_uint8(ip); 
}

void softuart_put_uint16(unsigned int c){
	unsigned int tmp;
	unsigned int mul;
	unsigned char l;
	
	l=0;
	for(mul=10000; mul>0; mul = mul/10){
		tmp = '0';
		while(c>=mul){
			c -= mul;
			tmp++;
			l=1;
		}
		if (l==0 && tmp == '0' && mul!=1)
			softuart_putc(' ');
		else
			softuart_putc(tmp);
	}
}

void softuart_put_uint8(unsigned char c){
	unsigned int tmp;
	unsigned char mul;
	unsigned char l;
	
	l=0;	
	for(mul=100; mul>0; mul = mul/10){
		tmp = '0';
		while(c>=mul){
			c -= mul;
			tmp++;
			l=1;
		}
		if (l==0 && tmp == '0' && mul!=1)
			softuart_putc(' ');
		else
			softuart_putc(tmp);
	}
}

void softuart_putnewline(){
	softuart_puts_progmem("\r\n");
}

void softuart_puts_progmem2(PGM_P pointer){
	unsigned char c = pgm_read_byte(pointer++);
	while(c){
		softuart_putc(c);
		c = pgm_read_byte(pointer++);
	}
}

void softuart_puts (char *s){
	while (*s){
		softuart_putc(*s);
		s++;
	}
}

void softuart_putc(unsigned char c){
		unsigned char cnt = 8;
		//send startbit: (=low)
		SOFTUART_TXPIN_LO();
		SOFTUART_DELAY();
		
		while(cnt > 0){
			if (c & 0x01)
				SOFTUART_TXPIN_HI();
			else
				SOFTUART_TXPIN_LO();

			//wait
			SOFTUART_DELAY();

			c = c>>1;
			cnt--;
		}

		//send stopbit:
		SOFTUART_TXPIN_HI();
		SOFTUART_DELAY();
}

inline void softuart_init(void){
	//set up txpin as output:
	SOFTUART_DDR |= (1<<SOFTUART_TXPIN);
}


/*
void softuart_putc(unsigned char c){
		unsigned char cnt = 8;
		//send startbit: (=low)
		SOFTUART_TXPIN_LO();
		SOFTUART_DELAY();
		
		while(cnt > 0){
			if (c & 0x01)
				SOFTUART_TXPIN_HI();
			else
				SOFTUART_TXPIN_LO();

			//wait
			SOFTUART_DELAY();

			c = c>>1;
			cnt--;
		}

		//send stopbit:
		SOFTUART_TXPIN_HI();
		SOFTUART_DELAY();
}

/*
inline void softuart_init(void){
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = _BV(URSEL) | 0x06;
 UBRRL = 0x0C; //set baud rate lo
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0x08;
}
*/
