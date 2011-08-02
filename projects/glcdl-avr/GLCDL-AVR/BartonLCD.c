/*
    This file is part of GLCDL-AVR - Copyright 2004 Lorenz Christophe
    christophelorenz magicathere gmail.com
    https://sites.google.com/site/donutscience

    GLCDL-AVR is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    GLCDL-AVR is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    This project is under LPGL, and you are free to donate if you find it usefull.
    https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=W6HV9YP7GK4S8&lc=BE&item_name=Donut%20Science&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted
    
*/
#include <stdlib.h>
#include <avr\io.h>
#include <avr\interrupt.h>
#include <util\delay.h>

#include "i2cmaster.h"
#include "LcdLowLevel.h"
#include "LcdBasicDraw.h"

uint8_t frameBuffer[BUFFER_SIZE];

void long_delay_ms(uint16_t t)
{
	t=t/16;
	while(t-->0)
	{
		_delay_ms(16);
	}
}

void long_delay_s(uint8_t t)
{
	while(t-->0)
	{
		long_delay_ms(1000);
	}
}

void LCD_clearFrameBuffer()
{
	uint16_t cnt=0;
	while(cnt<BUFFER_SIZE) frameBuffer[cnt++]=0; 
}


void LCD_setPixelXY(uint8_t x,uint8_t y,uint8_t mode)
{
	uint8_t bank;
	if ((x>=BUFFER_WIDTH)||(y>=BUFFER_HEIGHT)) return;
	bank=y>>3;
	y=y-(bank<<3);
	if (mode==LCD_SET)
		frameBuffer[bank*BUFFER_WIDTH+x]|=1<<y;
	else
	if (mode==LCD_CLEAR)
		frameBuffer[bank*BUFFER_WIDTH+x]&=(uint8_t)~(1<<y);
	else 
	//if (mode==LCD_XOR)
		frameBuffer[bank*BUFFER_WIDTH+x]^=(1<<y);
}





// RESET is active low
#define STE_RESET_PIN PINC2
#define STE_RESET_PORT PORTC
#define STE_RESET_DDR DDRC

#define DevSTE2004 (0x78)

// DC high=write to ram    DC low=command
#define DC (64)
// CO high=command    CO low=stream
#define CO (128)

void i2c_write_check(uint8_t v)
{
	if (i2c_write(v)!=0)
		while(1)
		{
			PORTB=0xFF;
			long_delay_ms(200);
			PORTB=0;
			long_delay_ms(200);
		}
}

#define FLIP_Y

// values for MX MY
// Since those are overwritten on page select, it is good to set them ok each time.
#ifdef FLIP_Y
	#define PageSelectorMXMY (_BV(5)+_BV(3))
	#define YBANKSTART (2)
	#define XSTART (6)
#else
	#define PageSelectorMXMY (_BV(5))
	#define XSTART (6)
#endif


void command(uint8_t command)
{
	i2c_write_check(0); // more commands follow, command mode (CO=0 DC=0)
	i2c_write_check(command); // page 1, more commands follow, command mode (CO=0 DC=0)
}

void setFunction(uint8_t page,uint8_t cmd)
{
	command(PageSelectorMXMY+page);
	command(cmd);
}

void prepare_write(uint8_t x,uint8_t y)
{
	i2c_start_wait(DevSTE2004+I2C_WRITE);     // set device address and write mode
	i2c_write_check(0);
	if (y>=2) y++; // skip bank 3, because it is not displayed...
	i2c_write_check(_BV(6)+(YBANKSTART+y)); // set bank (Y)
	i2c_write_check(0);
	i2c_write_check(_BV(7)+(XSTART+x)); // set X
	i2c_stop();                
}

void start_write()
{
 	i2c_start_wait(DevSTE2004+I2C_WRITE);     // set device address and write mode
	i2c_write_check(DC); // stream of bytes (CO=0 DC=1)
}

#define end_write() i2c_stop()

void LCD_displayFrameBuffer()
{
	uint8_t bank,x;
	uint16_t p;
	bank=0;
	while(bank<5)
	{
		p=BUFFER_WIDTH*bank;
		prepare_write(0,bank);
		_delay_ms(1);
		start_write();
		x=0;	while(x++<BUFFER_WIDTH) i2c_write_check(frameBuffer[p++]);
		end_write();                
		_delay_ms(1);
		bank++;
	}
}

/*
void makeTestPattern()
{
	uint8_t t;
	prepare_write(0,0);
	start_write();
	t=0;	while(t++<96) i2c_write_check(0xF0);
	end_write();                
	
	prepare_write(0,1);
	start_write();
	t=0;	while(t++<96) i2c_write_check(0x55);
	end_write();                
	
	prepare_write(0,2);
	start_write();
	t=0;	while(t++<96) i2c_write_check(0xFF);
	end_write();                
	
	prepare_write(0,3);
	start_write();
	t=0;	while(t++<96) i2c_write_check(0x55);
	end_write();                
	
	prepare_write(0,4);
	start_write();
	t=0;	while(t++<96) i2c_write_check(0x0F);
	end_write();                
}
*/


void LCD_Reset()
{
	uint8_t t;

	STE_RESET_PORT&=~_BV(STE_RESET_PIN); 	// set RESET to LOW to reset lcd.
	long_delay_ms(100);
	STE_RESET_PORT|=_BV(STE_RESET_PIN);		// set RESET to high to activate lcd.
	long_delay_ms(100);

	i2c_start_wait(DevSTE2004+I2C_WRITE);     // set device address and write mode
	PORTB=(uint8_t)~0xF1;
	setFunction(0,_BV(3)|_BV(2)); // lcd normal mode
	setFunction(1,_BV(0)); // checkerboard
	_delay_ms(10);
	_delay_ms(10);
	setFunction(1,_BV(3)+_BV(2)); // DO=1 LSB first
	setFunction(0,_BV(2)+2+1); // PRS VLDC 10.62v
	setFunction(0,_BV(4)+2); // Charge pump X4
	setFunction(1,_BV(4)+4); // Bias ratio 3
	
#ifdef FLIP_Y
	setFunction(0,_BV(1)); // scroll to adjust 1 line when in Y flip
#endif

	i2c_write_check(CO); // NO commands follow, command mode (CO=0 DC=0)
	i2c_write_check(PageSelectorMXMY);  // page 0
	i2c_stop();                

	i2c_start_wait(DevSTE2004+I2C_READ);     // set device address and write mode
	t=i2c_readNak();
	i2c_stop();   
	 
//	PORTB=~t; // output status to port B

	i2c_start_wait(DevSTE2004+I2C_WRITE);     // set device address and write mode
	setFunction(0,_BV(6)+0x2); // set bank (Y)
	setFunction(0,_BV(7)+0x0); // set X
	i2c_write_check(CO); // NO commands follow, command mode (CO=0 DC=0)
	i2c_write_check(PageSelectorMXMY);  // page 0
	i2c_stop();                

//	makeTestPattern();
}

void LCD_Init()
{
	STE_RESET_DDR|=_BV(STE_RESET_PIN);	// RESET pin output
	LCD_Reset();
}	


void LCD_Invert(uint8_t inverted)
{
	
}




