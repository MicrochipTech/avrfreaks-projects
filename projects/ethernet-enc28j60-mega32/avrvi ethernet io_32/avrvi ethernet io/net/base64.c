/*
,-----------------------------------------------------------------------------------------.
| net/base64
|-----------------------------------------------------------------------------------------
| this file implements base64 decoding 
| - used for HTTP auth for example
|
| -> for base64 protocol info see http://base64.sourceforge.net/b64.c 
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
#include "base64.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

unsigned char base64_reverse_lookup(unsigned char c){
	if (c=='=')
		return 0;

	if (c == '/')
		return (2*26+10+2);

	if (c == '+')
		return (2*26+10+1);

	if (c <= '9')
		return c+4; //(62+c)-'0';

	if (c <= 'Z')
		return c-'A';
	
	//else
	return (26+c)-'a';
}

//decode helper
void base64_decode_4to3(unsigned char* buf, unsigned char *res){
	for(unsigned char c=0; c<4; c++){
		buf[c] = base64_reverse_lookup(buf[c]);
	}
	res[0] = (buf[0]<<2) | (buf[1]>>4);
	res[1] = (buf[1]<<4) | (buf[2]>>2);
	res[2] = ((buf[2]<<6)&0xC0) | buf[3];
}

//decode string given by *buf, return decoded string in *buf!
void base64_decode(unsigned char *buf, unsigned char len){
unsigned char i=0;
unsigned char cnt = len;

while(cnt>=4){
		/*softuart_putc(buf[i*4]);
		softuart_putc(buf[i*4+1]);
		softuart_putc(buf[i*4+2]);
		softuart_putc(buf[i*4+3]);*/
		base64_decode_4to3(&buf[i*4], &buf[i*3]);
		/*softuart_putc(buf[i*3]);
		softuart_putc(buf[i*3+1]);
		softuart_putc(buf[i*3+2]);*/
		i++;

		//len MUST be a multiple of 4 !
		cnt-=4;
}
buf[(len/4)*3+1]=0;


/*	unsigned char i,len,v;
	unsigned int t=0;
	
	while( *buf){
		for( len = 0, i = 0; i < 4 && len<l; i++ ) {
			v = 0;
			while( len<l && v == 0 ) {
				v = buf[len];
				//v = ((v < 43 || v > 122) ? 0 : base64_decode_table[ v - 43 ]);
				v = base64_decode_table[ v - 43 ];
				//if(v)
				//	v = ((v == '$') ? 0 : v - 61);
				if (v=='$')
					v=0;
				else
					v=v-61;
			}
			if(len<l){
				len++;
				if(v)
					buf[t+i] = (v - 1);
			}else{
				buf[t+i] = 0;
			}
		}
		if(len){
			base64_decode_4to3(&buf[t*4], &buf[t*3]);
			t++;
		}
	} 
*/
}

/*
//translation table (RFC1113)
const unsigned char base64_table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	
//convert buffer:
//in : buffer with byte 0..2 as input
//out: same buffer with byte 0..3 as output
void base64_encode_3to4(unsigned char *buf){
	buf[3] = base64_table[ buf[2] & 0x3F ];
	buf[2] = base64_table[ ((buf[1]&0x0F)<<2) | ((buf[2]&0xC0)>>6) ];
	buf[1] = base64_table[ ((buf[0]&0x03)<<4) | ((buf[1]&0xF0)>>4) ];
	buf[0] = base64_table[ (buf[0]&0xFC)>>2 ];
}

void base64_encode(unsigned char *string){
	unsigned char buf[4];
	unsigned char cnt;
	unsigned char i;

	cnt = 0;
	//while string is not finished (=\0)
	while (*string){
		//read a char:
		buf[cnt++] = *string++;
		
		//if we have read 3 chars -> convert them to 4 6bit "chars":
		if (cnt==3){
			base64_encode_3to4(buf);
			softuart_putc(buf[0]);
			softuart_putc(buf[1]);
			softuart_putc(buf[2]);
			softuart_putc(buf[3]);
			cnt=0;
		}
	}

	if (cnt != 0){
		//fill string with zeros
		for(i=cnt;i<3; i++)
			buf[i] = 0x00;

		//decode last part
		base64_encode_3to4(buf);

		if (cnt<3)
			buf[3] = '=';
		if (cnt<2)
			buf[2] = '=';
		softuart_putc(buf[0]);
		softuart_putc(buf[1]);
		softuart_putc(buf[2]);
		softuart_putc(buf[3]);
	}
}

*/
