
/*
 * st7920.h
 *
 *  Created on: Apr 19, 2013
 *      Author: Peter Barnes
 *
 *      Use and distribute freely for non-commercial uses, but please
 *      include this file and my name if you distribute or post it. Enjoy!
 *
 */

#ifndef ST7920_H_
#define ST7920_H_

#include <stdlib.h>
#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// define pins here
#define DATA_PORT     PORTB
#define DATA_IN    PINB
#define COMMAND_PORT    PORTC
#define DATAPORT_DIR    DDRB
#define COMMAND_DIR        DDRC
#define RS     1;    // PC0  binary equivalent of pin number
#define RW    2;    // PC1
#define E    4;    // PC2

#define delay_time    80        // default settling time in uS after a command is sent

uint16_t packed_number[4];
uint8_t numbers[12][5] =     {                                    // a minimal 3x5 numeric font
                                {0x0e, 0x0a, 0x0a, 0x0a, 0x0e},    // 0
                                {0x04, 0x04, 0x04, 0x04, 0x04},    // 1
                                {0x0e, 0x02, 0x0e, 0x08, 0x0e}, // 2
                                {0x0e, 0x02, 0x0e, 0x02, 0x0e},    // 3
                                {0x0a, 0x0a, 0x0e, 0x02, 0x02}, // 4
                                {0x0e, 0x08, 0x0e, 0x02, 0x0e}, // 5
                                {0x0e, 0x08, 0x0e, 0x0a, 0x0e}, // 6
                                {0x0e, 0x02, 0x02, 0x02, 0x02},    // 7
                                {0x0e, 0x0a, 0x0e, 0x0a, 0x0e},    // 8
                                {0x0e, 0x0a, 0x0e, 0x02, 0x02}, // 9
                                {0x00, 0x00, 0x0e, 0x00, 0x00},    // -
                                {0x00, 0x00, 0x00, 0x00, 0x00}	// (space)
                            };

//void glcd_init();
void write_data(uint8_t data);
void test_ports(void);
void glcd_enable(void);
void e_lo(void);
void e_hi(void);
void rs_lo(void);
void rs_hi(void);
void rw_lo(void);
void rw_hi(void);
void toggle_E(void);

void cdisplay_clear(void);
void set_cgram_address(uint8_t x, uint8_t y);
void clcd_init();
void gotoxy(uint8_t x, uint8_t y);
void write_word(int x, int y, uint16_t value);
void glcd_fill(uint16_t value);
void write_wdata(uint16_t data);
uint16_t read_word(uint8_t type);

void set_pixel(uint8_t x, uint8_t y);
void clr_pixel(uint8_t x, uint8_t y);
void write_number_xy(uint8_t x, uint8_t y, uint8_t number);
void write_lnumber_xy(uint8_t x, uint8_t y, uint16_t number);
void write_char(int x, int y, uint8_t num_0, uint8_t num_1, uint8_t num_2, uint8_t num_3);
void draw_vertical_line(int x, int y, int length);
void draw_horiz_line(int x, int y, int length);
void write_char_xy(uint8_t x, uint8_t y, uint8_t number);
void write_gnumber_xy(uint8_t x, uint8_t y, uint16_t number);

int x = 0xffff;




void rs_hi()
{
    COMMAND_PORT |= RS;
    _delay_us(delay_time);
}

uint16_t read_word(uint8_t type)    // read data byte(type=1) or command register(type=0)
{
    uint8_t i, j = 0;
    uint16_t temp;
    DATAPORT_DIR = 0x00;    // set data port for input
    DATA_PORT = 0x00;
    e_lo();
    if (type == 0)
    {
        rs_lo();
    }
    else
    {
        rs_hi();
    }
    rw_hi();
    e_hi();            // toggle byte in
    i = DATA_IN;    // need I read this twice?
    e_lo();

    e_hi();
    i = DATA_IN;
    e_lo();
    _delay_ms(1);
    e_hi();
    j = DATA_IN;
    e_lo();

    temp = j;
    temp = temp | (i << 8);

    return(temp);    // return byte read
}

void write_word(int x, int y, uint16_t data)   // x=0-7 (8 words per line), y=0-63, starting from top left
{
    DATAPORT_DIR = 0xff;    // set data port for output
    e_lo();
    rs_lo();
    rw_lo();
    // convert coordinates to weirdly-arranged 128x64 screen (the ST7920 is mapped for 256x32 displays).
      if (y > 31)
      {
          y -= 32;    // because there are only 31 addressable lines in the ST7920
          x += 8;        // so we overflow x (7 visible bytes per line) to reach the bottom half
      }
    x |= 0x80;    // bit 7 signals that this is a data address write
    y |= 0x80;
    DATA_PORT = y;    // Set vertical DDRAM address
    toggle_E();
    DATA_PORT = x;    // Set horizontal DDRAM address
      toggle_E();
      rs_hi();    //   signal for WRITE
    DATA_PORT = (data >> 8);    // Set horizontal DDRAM address
      toggle_E();
    DATA_PORT = data;    // Set vertical DDRAM address
    toggle_E();
}

void rs_lo()
{
    COMMAND_PORT &= ~RS;    //
    _delay_us(delay_time);
}

void rw_hi()
{
    COMMAND_PORT |= RW;
    _delay_us(delay_time);
}

void rw_lo()
{
    COMMAND_PORT &= ~RW;
    _delay_us(delay_time);
}

void e_hi()
{
    COMMAND_PORT |= E;
    _delay_us(delay_time);
}

void e_lo()
{
    COMMAND_PORT &= ~E;
    _delay_us(delay_time);
}

void toggle_E()
{
    e_hi();
    e_lo();
}

void clcd_init()    // initialize as character display
{
    DATAPORT_DIR = 0xff;    // set data port for output
    e_lo();
    rs_lo();
    rw_lo();
    DATA_PORT = 0x30;    // set 8-bit interface
    toggle_E();
    _delay_ms(1);
    DATA_PORT = 0x30;    // set 8-bit interface, basic instruction set
    toggle_E();
    DATA_PORT = 0x0c;    // display ON, cursor and blink OFF
    toggle_E();
    DATA_PORT = 0x01;    // clear display, reset address
    toggle_E();
    DATA_PORT = 0x06;    // display ON, no cursor
    toggle_E();
}

void set_cgram_address(uint8_t x, uint8_t y)
{
    e_lo();
    rs_lo();
    rw_lo();
    DATAPORT_DIR = 0xff;    // set data port for output
    DATA_PORT = (0x40 | x);
    toggle_E();
    DATA_PORT = (0x40 | y);
    toggle_E();
}

void glcd_enable(void)        // start graphic mode
{
    DATAPORT_DIR = 0xff;
    e_lo();
    rs_lo();
    rw_lo();
    DATA_PORT = 0x30;    // set 8-bit interface
    toggle_E();
    _delay_ms(1);
    DATA_PORT = 0x36;    //  Repeat with graphics bit set to on.
    toggle_E();
    DATA_PORT = 0x0c;    // display ON, cursor and blink OFF
    toggle_E();
    DATA_PORT = 0x01;    // clear display, reset address
    toggle_E();
    DATA_PORT = 0x06;    // display ON, no cursor
    toggle_E();
}

void write_data(uint8_t data)
{
    DATAPORT_DIR = 0xff;    // set data port for output
    e_lo();
    rs_hi();
    rw_lo();
    DATA_PORT = data;    // write the byte to display at current address
    toggle_E();
}

void write_wdata(uint16_t data)        // write a word at the current position
{
    DATAPORT_DIR = 0xff;    // set data port for output
    e_lo();
    rs_hi();
    rw_lo();
    DATA_PORT = (data >> 8);    // write the byte to display at current address
    toggle_E();
    DATA_PORT = data;
    toggle_E();
}

void test_ports(void)
{
    int i;
    DATAPORT_DIR = 0xff;    // set data port for output
    COMMAND_DIR = 0xff;        // command port as well
    for (i = 0; i < 30; ++i)
    {
        e_lo();
        rs_lo();
        rw_lo();
        _delay_ms(500);
        e_hi();
        rs_hi();
        rw_hi();
        _delay_ms(500);
        DATA_PORT = 0x00;
        _delay_ms(500);
        DATA_PORT = 0xff;
        _delay_ms(500);
    }
}

void cdisplay_clear(void)
{
    DATAPORT_DIR = 0xff;    // set data port for output
    e_lo();
    rs_lo();
    rw_lo();
    DATA_PORT = 0x01;    // command to clear screen and reset address counter
    toggle_E();
}

void gotoxy(uint8_t x, uint8_t y)    // x=0-7, y=0-63  (for 128 x 64 display)
{
    DATAPORT_DIR = 0xff;    // set data port for output
    e_lo();
    rs_lo();
    rw_lo();
    // convert coordinates to weirdly-arranged 128x64 screen (the ST7920 is mapped for 256x32 displays).
      if (y > 31)
      {
          y -= 32;    // because there are only 31 addressable lines in the ST7920
          x += 8;        // so we overflow x (7 visible bytes per line) to reach the bottom half
      }
    x |= 0x80;    // bit 7 signals that this is a data address write
    y |= 0x80;
    DATA_PORT = y;    // Set vertical DDRAM address
    toggle_E();
    DATA_PORT = x;    // Set horizontal DDRAM address
      toggle_E();;
}

void glcd_fill(uint16_t value)    //  fill with (value; e.g., a 4-digit hex number). Zero clears the screen.
{
    uint8_t i, j;
    for (i = 0; i < 64; i++)
    {
        for (j = 0; j < 8; j++)
        {
            write_word(j, i, value);
        }
    }
}

void wait(void)        // wait for busy flag to clear
{
    int i = 0;
    while (i == 1)
    {
    //    i = read_busy_flag();
    }
}

void set_pixel(uint8_t x, uint8_t y)
{
    uint8_t x_wd, x_pixel;
    uint16_t temp;
    uint16_t dot = 0x8000;    // this will be rotated into it's correct position in the word
    x_wd = x / 16;        // find address of word with our pixel (x; 0-7)
    x_pixel = x - (x_wd * 16);        // get the modulo remainder; that's our pixel's position in the word
    gotoxy(x_wd, y);
    temp = read_word(1);    // read word from screen at that position
    temp = (temp | (dot >> x_pixel));    // convert x_pixel into a bit position, 0-16
    write_word(x_wd, y, temp);
}

void clr_pixel(uint8_t x, uint8_t y)
{
    uint8_t x_wd, x_pixel;
    uint16_t temp;
    uint16_t dot = 0x7fff;    // this will be rotated into it's correct position in the word
    x_wd = x / 16;        // find address of word with our pixel (x; 0-7)
    x_pixel = x - (x_wd * 16);        // get the modulo remainder; that's our pixel's position in the word
    gotoxy(x_wd, y);
    temp = read_word(1);    // read word from screen at that position
    temp = (temp & (dot >> x_pixel));    // convert x_pixel into a bit position, 0-16
    write_word(x_wd, y, temp);
}

void write_number_xy(uint8_t x, uint8_t y, uint8_t number)	// write single digit on word boundary (x=0-7)
{
    uint8_t i=0;
    uint16_t temp = 0;
    for (i = 0; i < 5; i++)
    {
        temp = numbers[number][i];
        temp = temp << 12;
        write_word(x, (y + i), temp);
    }
}

void write_lnumber_xy(uint8_t x, uint8_t y, uint16_t number)	//  write a 1-4 digit number on a word boundary (0-7)
{
    uint8_t j[4];    // the individual digits, from left to right
    if (number > 999)
    {
        j[0] = number / 1000;
        number = number - (1000 * j[0]);
    }
    else
    {
        j[0] = 0x0b;	// blank first zero if present
    }

    if (number > 99)
    {
        j[1] = number / 100;
        number = number - (100 * j[1]);
    }
    else
    {
        j[1] = 0;
    }

    if (number > 9)
    {
        j[2] = number / 10;
        number = number - (10 * j[2]);
    }
    else
    {
        j[2] = 0;
    }
    j[3] = number;        // at this point j[] has 4 separate decimal digits

    write_char(x, y, j[0], j[1], j[2], j[3]);

}

void write_char(int x, int y, uint8_t num_0, uint8_t num_1, uint8_t num_2, uint8_t num_3)	// write 4-digit number on word boundary
{
    int j;
    uint16_t temp = 0;
    uint16_t packed_number[5];
    for (j = 0; j < 5; j++)
    {
        temp = (numbers[num_0][j]);
        temp = temp << 4;
        temp |= (numbers[num_1][j]);
        temp = temp << 4;
        temp |= (numbers[num_2][j]);
        temp = temp << 4;
        temp |= (numbers[num_3][j]);
        packed_number[j] = temp;
    }
    for (j = 0; j < 5; j++)
    {
        write_word (x,y+j, packed_number[j]);
    }
}

void draw_vertical_line(int x, int y, int length)
{
	int i;
	for (i=0; i<length; i++)
	{
		set_pixel(x, y);
		x++;
	}
}

void draw_horiz_line(int x, int y, int length)
{
	int i;
	for (i=0; i<length; i++)
	{
		set_pixel(x, y);
		y++;
	}

}

void write_char_xy(uint8_t x, uint8_t y, uint8_t number)	// write a digit anywhere using set_pixel. Slower, but flexible for placement on the screen
{
	uint8_t h, i, j, l, m = x;
	uint8_t k = 0x08;
	h=0;
	for (i=0; i<5; i++)
	{
		l = (numbers[number][h]);
		for (j = 0; j < 3; j++)
		{
			if (l & k)
			{
				set_pixel(m, y);
			}
			m++;
			k /=2;
		}
		h++;
		y++;
		m=x;
		k=0x08;
	}
}

void write_gnumber_xy(uint8_t x, uint8_t y, uint16_t number)	//  write a 1-4 digit number on any x-y pixel
{
    uint8_t j[4];    // the individual digits, from left to right
    if (number > 999)
    {
        j[0] = number / 1000;
        number = number - (1000 * j[0]);
    }
    else
    {
        j[0] = 0x0b;	// blank first zero if present
    }

    if (number > 99)
    {
        j[1] = number / 100;
        number = number - (100 * j[1]);
    }
    else
    {
        j[1] = 0;
    }

    if (number > 9)
    {
        j[2] = number / 10;
        number = number - (10 * j[2]);
    }
    else
    {
        j[2] = 0;
    }
    j[3] = number;        // at this point j[] has 4 separate decimal digits

    write_char_xy(x, y, j[0]);
    x=x+4;
    write_char_xy(x, y, j[1]);
    x=x+4;
    write_char_xy(x, y, j[2]);
    x=x+4;
    write_char_xy(x, y, j[3]);
}

#endif /* ST7920_H_ */
