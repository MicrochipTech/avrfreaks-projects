/*! \file spi.c \brief SPI interface driver. */
//*****************************************************************************
//
// File Name	: 'spi.c'
// Title		: SPI interface driver
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/22/2000
// Revised		: 06/06/2002
// Version		: 0.6
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include "global.h"
#include "93eeprom.h"
#include "mmc/serial.h"
#include "delay.h"

#define		delay	_delay_us(10)

void clock()
{
	delay;
	ee93_sck_h;
	delay;
	ee93_sck_l;
	delay;
}
/////////////////////////////////////////////////////////////////////////////

u08 read_chip()
{
	u08 x = 0 , chip = 0;
	ee93_cs_h;
	ee93_mosi_h;
	clock();
	clock();
	ee93_mosi_l;
	clock();
	
	while( 1 )
	{
		
		x++;
		ee93_sck_h;
		delay;
		if(bit_is_clear(ee93_pin,ee93_miso))
		{
			break;
		}
		ee93_sck_l;
		delay;
		if(x > 12)
			break;
		
	}
	switch(x)
	{
		case 7:
			chip = chip46;
		break;

		case 8:
			chip = chip56;
		break;
		case 9:
			chip = chip66;
		break;
		case 11:
			chip = chip86;
		break;
		default:
			chip = 0;
		break;


	}
	ee93_mosi_l;
	ee93_sck_l;
	ee93_cs_l;
	ee93_chip = chip;
	delay_ms(100);
	return chip;


}
/////////////////////////////////////////////////////////////////////////////
u08	ee93_erase(void)
{

	unsigned long loop = 0;
	ee93_we();
	ee93_cs_h;			// select chip
	ee93_mosi_h;		//
	clock();			// send 1
	ee93_mosi_l;
	clock();			// send 0
	clock();			// send 0
	ee93_mosi_h;		//
	clock();			// send 1
	ee93_mosi_l;		//
	for(s08 x = ee93_chip ;  x>1; x--)
		clock();		// all send 0
	ee93_cs_l;

	delay;
	ee93_cs_h;
	while(bit_is_clear(ee93_pin,ee93_miso))
	{
		loop++;
		if(loop == 200000)
			return 0;
	}
	ee93_cs_l;	

	return 1;
}
/////////////////////////////////////////////////////////////////////////////
u08	ee93_read(u16 address)
{
	u08 data;
	ee93_cs_h;
	ee93_mosi_h;
	clock();
	clock();
	ee93_mosi_l;
	clock();

	for(s08 x = ee93_chip-1 ;  x>=0; x--)
	{
		if(bit_is_set(address,x))
			ee93_mosi_h;
		else
			ee93_mosi_l;
		if(x != 0)
			clock();		
		else
		{
			delay;
			ee93_sck_h;
			delay;
			if(bit_is_set(ee93_pin,ee93_miso))
				ee93_error = 1;
			ee93_sck_l;
			delay;
		}
	}
	data = ee93_r_byte();
	ee93_cs_l;

	return data;

}
/////////////////////////////////////////////////////////////////////////////
u08 ee93_write(u16 address,u08 datawrite)
{
	unsigned long loop=0;
	ee93_cs_h;
	ee93_mosi_h;
	clock();
	ee93_mosi_l;
	clock();
	ee93_mosi_h;
	clock();
	for(s08 x = ee93_chip-1 ;  x>=0; x--)
	{
		if(bit_is_set(address,x))
			ee93_mosi_h;
		else
			ee93_mosi_l;
		clock();		
	}
	ee93_w_byte(datawrite);
	ee93_cs_l;

	delay;
	ee93_cs_h;
	while(bit_is_clear(ee93_pin,ee93_miso))
	{
		loop++;
		if(loop == 200000)
			return 0;
	}
	ee93_cs_l;	

	return 1;
	


}
/////////////////////////////////////////////////////////////////////////////
void	ee93_wd(void)	//	disable write
{
	ee93_cs_h;			// select chip
	ee93_mosi_h;		// mosi H
	clock();
	ee93_mosi_l;
	clock();
	clock();
	for(s08 x = ee93_chip ;  x>0; x--)
		clock();
	ee93_cs_l;

}
/////////////////////////////////////////////////////////////////////////////
void	ee93_we(void)	//	enable write
{
	ee93_cs_h;			// select chip
	ee93_mosi_h;		// send 1
	clock();
	ee93_mosi_l;		
	clock();			// send 0
	clock();			// send 0
	ee93_mosi_h;		
	for(s08 x = ee93_chip ;  x>0; x--)
		clock();		// send 1
	ee93_cs_l;


}
/////////////////////////////////////////////////////////////////////////////
void eeprom_93_init()
{

	sbi(ee93_ddr,ee93_sck);
	sbi(ee93_ddr,ee93_mosi);
	cbi(ee93_ddr,ee93_miso);
	sbi(ee93_ddr,ee93_cs);
	
	cbi(ee93_port,ee93_cs);
	cbi(ee93_port,ee93_mosi);
	cbi(ee93_port,ee93_sck);
	sbi(ee93_port,ee93_miso);
	ee93_error = 0;
	read_chip();
}
/////////////////////////////////////////////////////////////////////////////

void ee93_w_byte(u08 datain)
{
	for(s08 x = 7; x >= 0 ; x--)
	{
		if(bit_is_set(datain,x))
			sbi(ee93_port,ee93_mosi);
		else
			cbi(ee93_port,ee93_mosi);
		clock();
	}
}
/////////////////////////////////////////////////////////////////////////////
u08 ee93_r_byte(void)
{
	u08 data = 0;
	ee93_mosi_l;
	for(s08 x = 7; x >= 0 ; x--)
	{
		delay;
		ee93_sck_h;
		delay;
		if(bit_is_set(ee93_pin,ee93_miso))
			sbi(data,x);
		else
			cbi(data,x);
		ee93_sck_l;
		delay;
		
	}
	return data;

}
