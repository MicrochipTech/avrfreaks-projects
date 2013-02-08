/*
 * keypad.c
 *
 * Created: 12/31/2012 2:52:04 PM
 *  Author: sasquach
 */ 

#include <avr/io.h>
#include <keypad.h>
#include <stovectr.h>
#include <hd44780.h>
#include "spi.h"

uint8_t keypad_out;
uint8_t keypad_temp;
uint16_t keypad_count;
uint8_t keypad_multi;
uint8_t menu_key_last;

void keypad_init(void)
{
	KEYPAD_PORT |= (_BV(KEYPAD_PIN0) | _BV(KEYPAD_PIN1) | _BV(KEYPAD_PIN2) | _BV(KEYPAD_PIN3));
	KEYPAD_DDR &= ~(_BV(KEYPAD_PIN0) | _BV(KEYPAD_PIN1) | _BV(KEYPAD_PIN2) | _BV(KEYPAD_PIN3));
	keypad_temp=0;
	keypad_count=0;
}

void keypad(void)
{
	for(;;)
	{
		if ((~KEYPAD_PIN & KEYPAD_MASK) != 0 )
		{
			if ((~KEYPAD_PIN & KEYPAD_MASK) == keypad_temp)
			{
				if (keypad_count>(KEYPAD_DEBOUNCE_COUNT/2))
				{
					keypad_out=keypad_temp;
					if (keypad_count> (KEYPAD_AUTO_KEY_COUNT/2))
					{
						keypad_out=0;
						keypad_count=0;
						keypad_multi++;
					}
				}
				keypad_count++;				
			}
			else
			{
				keypad_temp= ~KEYPAD_PIN & KEYPAD_MASK;
				keypad_count=0;
			}
		}
		else
		{
			keypad_temp=0;
			keypad_multi=0;
			keypad_count=0;
			keypad_out=0;
		}
		vTaskDelay(2);
	}
}
void menu_init(void)
{
	
}
void menu(void)
{
	for (;;)
	{
		if (keypad_out != 0)
		{
			if (keypad_out != menu_key_last)
			{
				if ((keypad_out & (1<<KEYPAD_RST)) == (1<<KEYPAD_RST))
				{
					if (keypad_multi!=0)
					{
						if (keypad_multi>=2)
						{
							stove_control_reset(stove_control_reset_nite);
						}
					}
					else
					{
						stove_control_reset(stove_control_reset_normal);
					}
				}
				if ((keypad_out & (1<<KEYPAD_RST_BASEMENT)) == (1<<KEYPAD_RST_BASEMENT))
				{
					if (keypad_multi!=0)
					{
						if (keypad_multi>=2)
						{
							SPI_DATA_MOSI[SPI_basement_control] = stove_control_reset_nite;
						}
					}
					else
					{
						SPI_DATA_MOSI[SPI_basement_control] = stove_control_reset_normal;
					}
				}
				else
				{
					SPI_DATA_MOSI[SPI_basement_control] = 0xFF;
				}
				if ((keypad_out & ((1<<KEYPAD_UP) | (1<<KEYPAD_DOWN))) == ((1<<KEYPAD_UP) | (1<<KEYPAD_DOWN)))
					{
						if ((lcd_display_mode & 0x0f) == LCD_MODE_DIAG)
						{
							lcd_display_mode=LCD_MODE_NORMAL;
						}
						else
						{
							lcd_display_mode=LCD_MODE_DIAG;
						}
					}
				else
				{
					if ((keypad_out & (1<<KEYPAD_UP)) == (1<<KEYPAD_UP))
					{
						stove_mode_room_temp_target_var++;
					}
					if ((keypad_out & (1<<KEYPAD_DOWN)) == (1<<KEYPAD_DOWN))
					{
						stove_mode_room_temp_target_var--;
					}
				}									
			}
		}				
	menu_key_last=keypad_out;
	vTaskDelay(2);		
	}
}