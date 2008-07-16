/*
,-----------------------------------------------------------------------------------------.
| io/plot
|-----------------------------------------------------------------------------------------
| this file implements a very basic bmp generator
| - plot temperature graph as a black and white BMP
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
#include "plot.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

#define PLOT_BMP_HEADER_LENGTH 62
//bmp data must be a multiple of 16 ?! -> maybe add padding !
#define PLOT_BMP_WIDTH 264
//264 pixel -> 33 byte ! must be divided by 4 -> use 36 byte -> use 288px !
#define PLOT_BMP_LINE_WIDTH 288
#define PLOT_BMP_HEIGHT 136
//288 -> 36 bytes -> 36/4 = 9 -> OK!
#define PLOT_BMP_FILESIZE (PLOT_BMP_LINE_WIDTH/8)*(PLOT_BMP_HEIGHT)+62

PROGMEM unsigned char PLOT_BMP_HEAD [] = {
0x42, 0x4D, 0x5E, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //10
0x3E, 0x00, 0x00, 0x00, //first image data at pos 0x3E -> at byte 62
0x28, 0x00, 0x00, 0x00, //header size (infoheader only!!) -> 40 bytes
lo8(PLOT_BMP_WIDTH), hi8(PLOT_BMP_WIDTH), 0x00, 0x00, //width <lo,hi,0,0>
lo8(PLOT_BMP_HEIGHT), hi8(PLOT_BMP_HEIGHT), 0x00, 0x00, //height <lo,hi,0,0>
0x01, 0x00, //1color plane
0x01, 0x00, //1bit
0x00, 0x00, 0x00, 0x00, //no compression
lo8(PLOT_BMP_FILESIZE), hi8(PLOT_BMP_FILESIZE), 0x00, 0x00, //imagesize in bytes: -> 0x1320 = 4896 bytes
0x10, 0x0B, 0x00, 0x00, //x pixels per meter
0x10, 0x0B, 0x00, 0x00, //y pixels per meter
0x02, 0x00, 0x00, 0x00, //2 colors (B/W)
0x02, 0x00, 0x00, 0x00, //2 important colors

0xFF, //background B
0xFF, //background G
0xFF, //background R
0x00,
0xFF, //foreground B
0x00, //foreground G
0x00, //foreground R
0x00 //62
};

PROGMEM unsigned char PLOT_BMP_LEGEND[] = {
//0x00, 0x00, 0x00,
//0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x03, 0xC3, 0x81, 0x02, 0x04, 0x41, 0xE1, 0x04, 0x47,
0x00, 0x84, 0x41, 0x00, 0x44, 0x41, 0x02, 0x46, 0x41, 0x01, 0x83, 0x81,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x01, 0xC3, 0x81, 0x00, 0x84, 0x41, 0x70, 0x84, 0x47, 0x00, 0x84, 0x41,
0x00, 0x84, 0x41, 0x01, 0x86, 0x41, 0x00, 0x83, 0x81, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x03, 0x81,
0x00, 0x04, 0x41, 0x00, 0x04, 0x47, 0x00, 0x04, 0x41, 0x00, 0x04, 0x41,
0x00, 0x06, 0x41, 0x00, 0x03, 0x81, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0xC3, 0x81, 0x00, 0x84, 0x41,
0x00, 0x84, 0x47, 0x00, 0x84, 0x41, 0x00, 0x84, 0x41, 0x01, 0x86, 0x41,
0x00, 0x83, 0x81, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x03, 0xC3, 0x81, 0x02, 0x04, 0x41, 0x01, 0x04, 0x47,
0x00, 0x84, 0x41, 0x00, 0x44, 0x41, 0x02, 0x46, 0x41, 0x01, 0x83, 0x81,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x03, 0x83, 0x81, 0x02, 0x44, 0x41, 0x00, 0x44, 0x47, 0x00, 0xC4, 0x41,
0x01, 0x84, 0x41, 0x00, 0x46, 0x41, 0x03, 0x83, 0x81, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x43, 0x81,
0x00, 0x44, 0x41, 0x03, 0xE4, 0x47, 0x01, 0x44, 0x41, 0x01, 0x44, 0x41,
0x00, 0xC6, 0x41, 0x00, 0x43, 0x81, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x03, 0xC3, 0x81, 0x02, 0x64, 0x41,
0x00, 0x24, 0x47, 0x02, 0x24, 0x41, 0x03, 0xC4, 0x41, 0x02, 0x06, 0x41,
0x03, 0xC3, 0x81, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x1C, 0x01, 0x00, 0x20, 0x01,
0x00, 0x20, 0x01, 0x02, 0x10, 0x01, 0x05, 0x0C, 0x01, 0x02, 0x00, 0x01,
0x00, 0x00, 0x01
};

void plot_test(){
	//for (unsigned int x=0; x<288; x++)
	//	eeprom_write_byte(&logger_temp_today[x], 130-x/4+80.0*sin((3.14*x)/30.0)/3.14);//  70+(x)%60);
	//plot_generate_bmp(&temp[0]);
}

unsigned int plot_generate_bmp(unsigned char *buffer, unsigned int datapos, unsigned int len, unsigned int streampos, unsigned char source){
	unsigned int x;
	unsigned char y;
	unsigned char out;
	unsigned char eedata;
	unsigned char hour_counter;
	unsigned char eedata_old;
	int yee;
	unsigned char eepos;
	unsigned char b;

	eedata = (32<<1);
	eedata_old = (32<<1);

	//step1: send bmp header (if requested)
	PGM_P header_ptr = PLOT_BMP_HEAD;

	for(x=streampos; x<PLOT_BMP_HEADER_LENGTH; x++){
		buffer[datapos++] = pgm_read_byte(header_ptr++);
		streampos++;
		len--;
		//no more data allowed
		if (len == 0)
			return 0;
	}

	//step2 generate plot:
	//calculate position: streampos must be > PLOT_BMP_HEADER_LEN now...
	streampos -= PLOT_BMP_HEADER_LENGTH;

	//-> we must skip now #streampos bytes!
	
	//generate bottom border:
	for(x=streampos; x<PLOT_BMP_LINE_WIDTH/8; x++){
		if (x<24/8)
			buffer[datapos++] = 0x00;
		else
			buffer[datapos++] = 0xFF;
		streampos++;

		len --;
		//no more data allowed
		if (len == 0)
			return 0;
	}
	
	//recalculate pos:
	streampos -= PLOT_BMP_LINE_WIDTH/8;

	//skip now #streampos bytes
	y = (streampos/(PLOT_BMP_LINE_WIDTH/8));
	x = streampos - y*(PLOT_BMP_LINE_WIDTH/8);

	//calculate start index of hour counter (draws vertical hour marks)
	hour_counter = 10;
	if (x>3)
		hour_counter = 10 - (((x-3)*8) % 10);
	
	

	//softuart_puts_progmem("y: ");softuart_put_uint16(y);
	//softuart_puts_progmem("x: ");softuart_put_uint16(8*x);

	PGM_P legend_ptr = PLOT_BMP_LEGEND;
	if (y != 0)
		legend_ptr += 3*(y+1);

	if (x < 24/8){
		legend_ptr += x;
	}

	y = (135) - y;

	unsigned char picpos_now = (clock[CLOCK_HOUR]*60+clock[CLOCK_MIN])/6;
	if (picpos_now > (23*60+59)/6)
		picpos_now = (23*60+59)/6;

	//main image:
	for( ; y>1; y--){
		//show data:
		if (y==92){
			for( ; x<PLOT_BMP_LINE_WIDTH/8; x++){
				if (x<(24/8)){
					out = pgm_read_byte(legend_ptr++);
				}else{
					out = 0xFF;
				}
				buffer[datapos++] = out;
				len--;
				
				if (len == 0)
					return 0;
			}
			x=0;
		}else{
			//y = 91 - (((ee-(32<<1))*4)/3)
			//-> (ee-(32<<1))*4/3 = (91 - y) <=> (3(91-y))/4 = (ee-(32<<1)) <=> (3(91-y))/4 + (32<<1) = ee
			yee = ((92 - y)*4)/3 + (32<<1);
			
			//data source ?!
			if (source == PLOT_SOURCE_EEPROM)
				eedata_old = eeprom_read_byte(&logger_temp_today[0]); //EEPROM
			else
				eedata_old = eedata_old + 1; //DATAFLASH

			for( ; x<PLOT_BMP_LINE_WIDTH/8; x++){
				if (x<(24/8)){
					out = (pgm_read_byte(legend_ptr++));
				}else{
					out = 0x00;
					
					for(b=0; b<8; b++){
						out = out<<1;
						eepos   = ((x-3)*8+b);

						//get data:
						if (source == PLOT_SOURCE_EEPROM){
							eedata  = eeprom_read_byte(&logger_temp_today[eepos]); //EEPROM
						}else{
							eedata = eedata + 1; //DATAFLASH -> add later! FIXME
						}
						
						//bold line:
						//if ((yee+2 > (eedata&0xFF)) && (yee-2 < (eedata&0xFF)))
						//	out += 1;

						//filled:
						/*if (yee > (32<<1)){
							if (yee < (eedata&0xFF))
								out += 1;
						}else{
							if (yee > (eedata&0xFF))
								out += 1;
						}*/

						//connected line:
						if (eedata_old < eedata){
							if ((yee > eedata_old-1) && (yee <= eedata+1))
								out |= 1;
						}else{
							if ((yee <= eedata_old+1) && (yee > eedata-1))
								out |= 1;
						}
						eedata_old = eedata;

						//add now marker:
						if (eepos == picpos_now)
							out |= 1;
						

						//draw a vertical line every 60min (=10px)
						hour_counter--;
						if (!hour_counter){
							hour_counter = 10;
							if(!(y&0x03))
								out|=1;
						}
					}

					//mark every 48 min (=8 pixel)
					//if(((x-3)&0x01) && (y&0x01))
					//	out|=1;
				}

				//right border
				if (x == ((PLOT_BMP_WIDTH/8)-1)) //BMP WITH !!! (=280, do not use 288 here)
					out |= 1;

				buffer[datapos++] = out;
				len--;
				streampos++;

				if (len == 0)
					return 0;
			}
			x = 0;
			//restart hour_counter (draws vertical lines)
			hour_counter = 10;
		}
	}
	
	//recalc streampos:
	streampos  = streampos - 134*(PLOT_BMP_LINE_WIDTH/8);
	legend_ptr = PLOT_BMP_LEGEND + 134*3;

	//generate bottom border:
	for(x=streampos; x<PLOT_BMP_LINE_WIDTH/8; x++){
		if (x<24/8)
			buffer[datapos++] = pgm_read_byte(legend_ptr++);
		else
			buffer[datapos++] = 0xFF;
		
		len--;
		if (len == 0){
			if (x<PLOT_BMP_LINE_WIDTH/8-1)
				return 0;
			else 
				return datapos;
		}
	}

	return datapos;
}

