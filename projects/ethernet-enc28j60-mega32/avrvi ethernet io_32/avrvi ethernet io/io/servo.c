/*
,-----------------------------------------------------------------------------------------.
| io/servo
|-----------------------------------------------------------------------------------------
| this file implements functions to control a rc-car servo motor
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
#include "servo.h"
#include "../debug.h"

#include <util/delay.h>
unsigned char servo_pos;

#define SERVO_BMP_HEADER_LENGTH 62
//bmp data must be a multiple of 16 ?! -> maybe add padding !
#define SERVO_BMP_WIDTH 256
//256 pixel -> 256/8 = 32 ! must be divided by 4 -> ok!
#define SERVO_BMP_LINE_WIDTH 256
#define SERVO_BMP_HEIGHT 1
#define SERVO_BMP_FILESIZE (SERVO_BMP_LINE_WIDTH/8)*(SERVO_BMP_HEIGHT)+62

PROGMEM unsigned char SERVO_BMP_HEAD [] = {
0x42, 0x4D, 0x5E, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //10
0x3E, 0x00, 0x00, 0x00, //first image data at pos 0x3E -> at byte 62
0x28, 0x00, 0x00, 0x00, //header size (infoheader only!!) -> 40 bytes
lo8(SERVO_BMP_WIDTH), hi8(SERVO_BMP_WIDTH), 0x00, 0x00, //width <lo,hi,0,0>
lo8(SERVO_BMP_HEIGHT), hi8(SERVO_BMP_HEIGHT), 0x00, 0x00, //height <lo,hi,0,0>
0x01, 0x00, //1color plane
0x01, 0x00, //1bit
0x00, 0x00, 0x00, 0x00, //no compression
lo8(SERVO_BMP_FILESIZE), hi8(SERVO_BMP_FILESIZE), 0x00, 0x00, //imagesize in bytes
0x10, 0x0B, 0x00, 0x00, //x pixels per meter
0x10, 0x0B, 0x00, 0x00, //y pixels per meter
0x02, 0x00, 0x00, 0x00, //2 colors (B/W)
0x02, 0x00, 0x00, 0x00, //2 important colors

0xFF, //background B
0xFF, //background G
0xFF, //background R
0x00,
0xC0, //foreground B
0xC0, //foreground G
0xC0, //foreground R
0x00 //62
};

void servo_init(){
	//set up fast pwm mode:
	#define WGM_CFG (1<<WGM00 | 1<<WGM01) //fast pwm
	#define COM_CFG (1<<COM01 | 0<<COM00) //clr on match, set on max
	#define CLK_CFG (0<<CS00 | 1<<CS01 | 1<<CS02) //set up clock source
	TCCR2 = WGM_CFG | COM_CFG | CLK_CFG;

	//set pin as output
	SERVO_DDR |= (1<<SERVO_PIN);

	//initialise
	servo_set_pos(127);
}

void servo_set_pos(unsigned char val){
	#define CALC_1MS F_CPU/256/1000 //=28.8
	servo_pos = val;
	OCR2 = (CALC_1MS - 5 + (unsigned char)(((unsigned int)servo_pos*CALC_1MS)/255));
}

unsigned int servo_generate_bmp(unsigned char *buffer, unsigned int datapos, unsigned int len, unsigned int streampos, unsigned char source){
	unsigned char out;
	unsigned int x;

	//step1: send bmp header (if requested)
	PGM_P header_ptr = SERVO_BMP_HEAD;

	for(x=streampos; x<SERVO_BMP_HEADER_LENGTH; x++){
		buffer[datapos++] = pgm_read_byte(header_ptr++);
		streampos++;
		len--;
		//no more data allowed
		if (len == 0)
			return 0;
	}

	//step2 generate plot:
	//calculate position: streampos must be > PLOT_BMP_HEADER_LEN now...
	streampos -= SERVO_BMP_HEADER_LENGTH;

	//now show image:
	out = 0;
	unsigned char spos = servo_pos;
	for(unsigned char i=streampos; i<256/8; i++){
		for(unsigned char b=0; b<8; b++){
			out = out << 1;
			if (i*8+b < spos){
				out |= 1;
			}
		}
		buffer[datapos++] = out;
		len--;
		//streampos++;

		if (len == 0)
			return 0;
	}

	return datapos;
}






