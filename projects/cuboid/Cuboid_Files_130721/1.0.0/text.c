/*

*/

#include "includes.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

	static const char font[480] PROGMEM = {
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b00100,
0b00100,
0b00100,
0b00000,
0b00100,
0b01010,
0b01010,
0b00000,
0b00000,
0b00000,
0b01010,
0b11111,
0b01010,
0b11111,
0b01010,
0b01111,
0b10100,
0b01110,
0b00101,
0b11110,
0b10001,
0b00010,
0b00100,
0b01000,
0b10001,
0b01000,
0b10100,
0b01101,
0b10010,
0b01101,
0b00100,
0b00100,
0b00000,
0b00000,
0b00000,
0b00010,
0b00100,
0b00100,
0b00100,
0b00010,
0b01000,
0b00100,
0b00100,
0b00100,
0b01000,
0b10101,
0b01110,
0b11111,
0b01110,
0b10101,
0b00100,
0b00100,
0b11111,
0b00100,
0b00100,
0b00000,
0b00000,
0b00000,
0b00100,
0b01000,
0b00000,
0b00000,
0b11111,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b01000,
0b00001,
0b00010,
0b00100,
0b01000,
0b10000,
0b01110,
0b10011,
0b10101,
0b11001,
0b01110,
0b00100,
0b01100,
0b00100,
0b00100,
0b01110,
0b01110,
0b10001,
0b00010,
0b01100,
0b11111,
0b01110,
0b10001,
0b00110,
0b10001,
0b01110,
0b00100,
0b01100,
0b11110,
0b00100,
0b00100,
0b11110,
0b10000,
0b11110,
0b00001,
0b11110,
0b01110,
0b10000,
0b11110,
0b10001,
0b01110,
0b11111,
0b00010,
0b00100,
0b01000,
0b10000,
0b01110,
0b10001,
0b01110,
0b10001,
0b01110,
0b01110,
0b10001,
0b01110,
0b00001,
0b01110,
0b00100,
0b00100,
0b00000,
0b00100,
0b00100,
0b00100,
0b00100,
0b00000,
0b00100,
0b01000,
0b00100,
0b01000,
0b10000,
0b01000,
0b00100,
0b00000,
0b11111,
0b00000,
0b11111,
0b00000,
0b00100,
0b00010,
0b00001,
0b00010,
0b00100,
0b01110,
0b10001,
0b00110,
0b00000,
0b00100,
0b01110,
0b10011,
0b10101,
0b10111,
0b01101,
0b00100,
0b01010,
0b11111,
0b10001,
0b10001,
0b11110,
0b10001,
0b11110,
0b10001,
0b11110,
0b01110,
0b10001,
0b10000,
0b10001,
0b01110,
0b11110,
0b10001,
0b10001,
0b10001,
0b11110,
0b11111,
0b10000,
0b11100,
0b10000,
0b11111,
0b11111,
0b10000,
0b11100,
0b10000,
0b10000,
0b01110,
0b10000,
0b10111,
0b10010,
0b01110,
0b10001,
0b10001,
0b11111,
0b10001,
0b10001,
0b01110,
0b00100,
0b00100,
0b00100,
0b01110,
0b00001,
0b00001,
0b00001,
0b10001,
0b01110,
0b10001,
0b10010,
0b11100,
0b10010,
0b10001,
0b10000,
0b10000,
0b10000,
0b10000,
0b11111,
0b10001,
0b11011,
0b10101,
0b10001,
0b10001,
0b10001,
0b11001,
0b10101,
0b10011,
0b10001,
0b01110,
0b10001,
0b10001,
0b10001,
0b01110,
0b11110,
0b10001,
0b11110,
0b10000,
0b10000,
0b01110,
0b10001,
0b10101,
0b10011,
0b01111,
0b11110,
0b10001,
0b11110,
0b10010,
0b10001,
0b01111,
0b10000,
0b01110,
0b00001,
0b11110,
0b11111,
0b00100,
0b00100,
0b00100,
0b00100,
0b10001,
0b10001,
0b10001,
0b10001,
0b01110,
0b10001,
0b10001,
0b10001,
0b01010,
0b00100,
0b10001,
0b10001,
0b10001,
0b10101,
0b01010,
0b10001,
0b01010,
0b00100,
0b01010,
0b10001,
0b10001,
0b01010,
0b00100,
0b00100,
0b00100,
0b11111,
0b00010,
0b00100,
0b01000,
0b11111,
0b01110,
0b01000,
0b01000,
0b01000,
0b01110,
0b10000,
0b01000,
0b00100,
0b00010,
0b00001,
0b01110,
0b00010,
0b00010,
0b00010,
0b01110,
0b00100,
0b01010,
0b10001,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b11111,
0b10000,
0b10000,
0b00000,
0b00000,
0b00000,
0b00100,
0b01010,
0b11111,
0b10001,
0b10001,
0b11110,
0b10001,
0b11110,
0b10001,
0b11110,
0b01110,
0b10001,
0b10000,
0b10001,
0b01110,
0b11110,
0b10001,
0b10001,
0b10001,
0b11110,
0b11111,
0b10000,
0b11100,
0b10000,
0b11111,
0b11111,
0b10000,
0b11100,
0b10000,
0b10000,
0b01110,
0b10000,
0b10111,
0b10010,
0b01110,
0b10001,
0b10001,
0b11111,
0b10001,
0b10001,
0b01110,
0b00100,
0b00100,
0b00100,
0b01110,
0b00001,
0b00001,
0b00001,
0b10001,
0b01110,
0b10001,
0b10010,
0b11100,
0b10010,
0b10001,
0b10000,
0b10000,
0b10000,
0b10000,
0b11111,
0b10001,
0b11011,
0b10101,
0b10001,
0b10001,
0b10001,
0b11001,
0b10101,
0b10011,
0b10001,
0b01110,
0b10001,
0b10001,
0b10001,
0b01110,
0b11110,
0b10001,
0b11110,
0b10000,
0b10000,
0b01110,
0b10001,
0b10101,
0b10011,
0b01111,
0b11110,
0b10001,
0b11110,
0b10010,
0b10001,
0b01111,
0b10000,
0b01110,
0b00001,
0b11110,
0b11111,
0b00100,
0b00100,
0b00100,
0b00100,
0b10001,
0b10001,
0b10001,
0b10001,
0b01110,
0b10001,
0b10001,
0b10001,
0b01010,
0b00100,
0b10001,
0b10001,
0b10001,
0b10101,
0b01010,
0b10001,
0b01010,
0b00100,
0b01010,
0b10001,
0b10001,
0b01010,
0b00100,
0b00100,
0b00100,
0b11111,
0b00010,
0b00100,
0b01000,
0b11111,
0b00110,
0b01000,
0b11000,
0b01000,
0b00110,
0b00100,
0b00100,
0b00100,
0b00100,
0b00100,
0b01100,
0b00010,
0b00011,
0b00010,
0b01100,
0b10100,
0b01010,
0b00000,
0b00000,
0b00000,
0b11111,
0b10001,
0b10001,
0b10001,
0b11111 };

	static PGM_P			text_ptr_p;
	static char				*text_ptr;

	static unsigned char 	row[5];
	static unsigned char 	row_contents=0;

	static char 			finished=TRUE;

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void load_char(char x);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

void text_set_P(PGM_P ptr)
{
	char tempchar;

	text_ptr=NULL;
	text_ptr_p = ptr;
	tempchar=pgm_read_byte(text_ptr_p);
	if(tempchar)
	{
		text_ptr_p++;
		load_char(tempchar);
		finished=FALSE;
	}
	else
		finished=TRUE;
}

void text_set(char* ptr)
{
	char tempchar;

	text_ptr_p=NULL;
	text_ptr = ptr;
	tempchar=*text_ptr;
	if(tempchar)
	{
		text_ptr++;
		load_char(tempchar);
		finished=FALSE;
	}
	else
		finished=TRUE;
}

//draw a vertical columb of text, at x, y, z, y==top pixel, font is 5 high
void text_draw_y(unsigned char x, unsigned char y, unsigned char z)
{
	char bit_on, bit_off;
	unsigned char i;
	
	x&=7;
	y&=3;
	z&=7;
	
	bit_on=0x80>>x;
	bit_off=~bit_on;
	
	i= (y<<3)+z;

	if(!finished)
	{
		if(row[0] & 0x20)
			led_frame_bitmap[i]|=bit_on;
		else
			led_frame_bitmap[i]&=bit_off;
		i+=8;
		if(row[1] & 0x20)
			led_frame_bitmap[i]|=bit_on;
		else
			led_frame_bitmap[i]&=bit_off;
		i+=8;
		if(row[2] & 0x20)
			led_frame_bitmap[i]|=bit_on;
		else
			led_frame_bitmap[i]&=bit_off;
		i+=8;
		if(row[3] & 0x20)
			led_frame_bitmap[i]|=bit_on;
		else
			led_frame_bitmap[i]&=bit_off;
		i+=8;
		if(row[4] & 0x20)
			led_frame_bitmap[i]|=bit_on;
		else
			led_frame_bitmap[i]&=bit_off;
		
		//shift rows
		row[0]<<=1;
		row[1]<<=1;
		row[2]<<=1;
		row[3]<<=1;
		row[4]<<=1;
		
		row_contents--;
		
		//rows empty??
		if(row_contents==0)
		{
			if(text_ptr_p)
				text_set_P(text_ptr_p);			//load next character if there is one
			else
				text_set(text_ptr);			//load next character if there is one
		};
	};
}

//draw a horizontal line of text in the x direction, at x, y, z, x==left pixel, font is 5x5 high
void text_draw_x(unsigned char x, unsigned char y, unsigned char z)
{
	unsigned char i;

	x&=3;
	y&=7;
	z&=7;

	i=(y<<3) + z;
	
	led_frame_bitmap[i] &=~(0x1F << (3-x));
	led_frame_bitmap[i] |= row[0] << (3-x);

	//shift rows
	row[0]=row[1];
	row[1]=row[2];
	row[2]=row[3];
	row[3]=row[4];
	row[4]=0;
	
	row_contents--;
	
	//rows empty??
	if(row_contents==0)
	{
		if(text_ptr_p)
			text_set_P(text_ptr_p);			//load next character if there is one
		else
			text_set(text_ptr);			//load next character if there is one
	};
}

char text_isfinished(void)
{
	return finished;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static void load_char(char x)
{
	int index;
	PGM_P font_ptr;

	//calculate index in font table
	x &=0x7F;
	x -=0x20;
	index = x + (x<<2);	//*5

	//point to font table
	font_ptr = &font[index];

	//load rows
	row[0] = pgm_read_byte(font_ptr++);
	row[1] = pgm_read_byte(font_ptr++);
	row[2] = pgm_read_byte(font_ptr++);
	row[3] = pgm_read_byte(font_ptr++);
	row[4] = pgm_read_byte(font_ptr);

	//row now contains 6 bits
	row_contents=6;
}
