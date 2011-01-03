// 16x16 pixels screen output commands
// for nerdkits with ATmega168
// hexorgstas@gmail.com

#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include "draw.h"

void SetPixel(uint8_t *image, int8_t x, int8_t y, uint8_t color)
{
	if ((x>=1) && (x<=16) && (y>=1) && (y<=16) && (color <= 7))
	{
		uint8_t byte, bit;
		int8_t offset;
		bit = (16-x)*3;
		byte = (y-1)*6 + (bit/8);
		offset = 5-(bit % 8);
		if (offset >= 0)
		{
			image[byte] = ((image[byte] & ~(0x07<<offset)) | (color << offset));
			return;
		}
		if (offset == -2)
		{
			image[byte] = ((image[byte] & 0xFE) | ((color & 0x04)>>2));
			image[byte+1] = ((image[byte+1] & 0x3F) | ((color & 0x03)<<6));
			return;
		}
		if (offset == -1)
		{
			image[byte] = ((image[byte] & 0xFC) | ((color & 0x06)>>1));
			image[byte+1] = ((image[byte+1] & 0x7F) | ((color & 0x01)<<7));
			return;
		}
	}
}

uint8_t GetPixel(uint8_t *image, int8_t x, int8_t y)
{
	if ((x>=1) && (x<=16) && (y>=1) && (y<=16))
	{
		uint8_t byte, bit;
		int8_t offset;
		bit = (16-x)*3;
		byte = (y-1)*6 + (bit/8);
		offset = 5-(bit % 8);
		if (offset >= 0)
			return ((image[byte] & (0x07<<offset))>>offset);
		if (offset == -2)
			return ((image[byte] & 0x01)<<2) | ((image[byte+1] & 0xC0)>>6);
		if (offset == -1)
			return ((image[byte] & 0x03)<<1) | ((image[byte+1] & 0x80)>>7);
		return 0;
	}
	else return 0;
}

void BitBlt(uint8_t *src, int8_t src_x, int8_t src_y, uint8_t width, uint8_t height, uint8_t *desc, int8_t desc_x, int8_t desc_y)
{
	uint8_t cx, cy;
	for (cy = 0; cy < height; cy++)
		for (cx = 0; cx < width; cx++)
			{
				uint8_t color;
				color = GetPixel(src, (src_x+cx), (src_y+cy));
				SetPixel(desc, (desc_x+cx), (desc_y+cy), color);
			}
}

void DrawLine(uint8_t *image, int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t color)
{
	int8_t steep = (abs(y1 - y0) > abs(x1 - x0));
	int8_t tx0, tx1, ty0, ty1;
	tx0 = x0;
	tx1 = x1;
	ty0 = y0;
	ty1 = y1;
	
	if (steep)
	{
		tx0 = y0; ty0 = x0;
		tx1 = y1; ty1 = x1;
	}
	if (x0 > x1)
	{
		tx0 = x1; tx1 = x0;
		ty0 = y1; ty1 = y0;
	}
	
	int8_t deltax = tx1 - tx0;
	int8_t deltay = abs(ty1 - ty0);
	int8_t error = deltax / 2;
	int8_t ystep;
	int8_t y = ty0;
	int8_t x;
	if (ty0 < ty1)
		ystep = 1;
	else
		ystep = -1;
	for (x = tx0; x <= tx1; x++)
	{
		if (steep)
			SetPixel(image, y, x, color);
		else
			SetPixel(image, x, y, color);
		error -= deltay;
		if (error < 0)
		{
			y += ystep;
			error += deltax;
		}
	}	
}

void FillRect(uint8_t *image, int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t color)
{
	uint8_t x, y;
	for (y = y0; y<=y1; y++)
		for (x = x0; x<=x1; x++)
			SetPixel(image, x, y, color); 
}

void FloodFill(uint8_t *image, uint8_t color)
{
	/*	Color = 0x00, pattern: 000000000000
	**  Color = 0x01, pattern: 249249249249
	**  Color = 0x02, pattern: 492492492492
	**  Color = 0x03, pattern: 6db6db6db6db
	**  Color = 0x04, pattern: 924924924924
	**  Color = 0x05, pattern: b6db6db6db6d
	**  Color = 0x06, pattern: db6db6db6db6
	**  Color = 0x07, pattern: ffffffffffff
	*/
	FillRect(image, 1, 1, 16, 16, color);
}

void PrintChar(uint8_t *image, int8_t x, int8_t y, uint8_t character, uint8_t color)
{
	if ((character <= 61) && ((x<=16) && (y<=16)))
	{
		uint8_t byte, data, bit;
		
		for (byte = 0; byte < 8; byte++)
		{
			data = eeprom_read_byte((uint8_t *)(character*8+byte));
			for (bit = 0; bit < 8; bit++)
				{	
					if (data & (1<<(7-bit)))
					SetPixel(image, (x+bit), (y+byte), color); 
				}
		}
	}
}

