/*
 * lcd1.c
 *
 * Created: 12/24/2012 2:28:24 PM
 *  Author: sasquach
 */ 
//#include <stdint.h>
#include <stdlib.h>
#include "config.h"
#include <util/delay.h>
#include <hd44780.h>
#include <ds1620.h>
#include <stepper.h>
#include <stovectr.h>
#include <adc.h>
#include <avr/io.h>
#include <keypad.h>
#include "spi.h"

uint8_t lcd_display_mode;

static void lcd_delay(void)
{
	uint8_t i;
	for ( i = 0 ; i < LCD_SHORT_DELAY_50us ; i++ )
	{
		asm volatile("nop\n\t");
		asm volatile("nop\n\t");
	}		 
}
static void lcd_putc(uint8_t lcd_bts)
{
	uint8_t temp;
	temp = lcd_bts >> 4;
	temp &= 0x0F;
	temp = temp << LCD_DATA_NIB;
	LCD_PORT &= ~(_BV(LCD_DATA0_PIN) | _BV (LCD_DATA1_PIN) | _BV (LCD_DATA2_PIN) | _BV (LCD_DATA3_PIN));
	LCD_PORT |= temp;
	lcd_delay();
	LCD_PORT &= ~_BV(LCD_E_PIN);
	lcd_delay();
	LCD_PORT |= _BV(LCD_E_PIN);

	temp = lcd_bts;
	temp &= 0x0F;
	temp = temp << LCD_DATA_NIB;
	LCD_PORT &= ~(_BV(LCD_DATA0_PIN) | _BV (LCD_DATA1_PIN) | _BV (LCD_DATA2_PIN) | _BV (LCD_DATA3_PIN));
	LCD_PORT |= temp;
	lcd_delay();
	LCD_PORT &= ~_BV(LCD_E_PIN);
	lcd_delay();
	LCD_PORT |= _BV(LCD_E_PIN);
	
}
char data[10];
void lcd_putv(uint16_t lcd_varible, uint8_t lcd_numtype, uint8_t lcd_digits )
{
			uint8_t x=lcd_digits;
			
			itoa(lcd_varible, data,lcd_numtype);
			for (uint8_t i=0; data[i] != 0x00 ; i++)
			{
					lcd_digits--;
			}
			
			if (lcd_digits > x)
			{
				lcd_digits=0;
			}
						
			for (uint8_t i=0; i<lcd_digits; i++)
			{
				lcd_putc(' ');
			}			
			for (uint8_t i=0; data[i] != 0x00; i++)
			{
				lcd_putc(data[i]);
			}

}
void lcd_init (void)
{
    for (uint16_t i=0; i < 1000; i++)
	{
		lcd_delay();
	}
	//vTaskDelay(100);
	LCD_PORT &= ~(_BV(LCD_DATA0_PIN) | _BV (LCD_DATA1_PIN) | _BV (LCD_DATA2_PIN) | _BV (LCD_DATA3_PIN) | _BV (LCD_RS_PIN) | _BV (LCD_E_PIN) | _BV(LCD_LED_PIN));
	LCD_DDR  |= (_BV(LCD_DATA0_PIN) | _BV (LCD_DATA1_PIN) | _BV (LCD_DATA2_PIN) | _BV (LCD_DATA3_PIN) | _BV (LCD_RS_PIN) | _BV (LCD_E_PIN) | _BV(LCD_LED_PIN));
	for (uint16_t i=0 ; i < 3; i++)
	{
		LCD_PORT |= _BV(LCD_E_PIN);
		lcd_delay();
		LCD_PORT &= ~LCD_DATA_MASK;
		LCD_PORT |= LCD_FUNC_RESET << LCD_DATA_NIB;
		LCD_PORT &= ~_BV(LCD_E_PIN);
		//vTaskDelay(10);
		for (uint8_t t=0; t < 200; t++)
		{
			lcd_delay();	
		}
	}	
	LCD_PORT |= _BV(LCD_E_PIN);
	lcd_delay();
	LCD_PORT &= ~LCD_DATA_MASK;
	LCD_PORT |= LCD_FUNC_INIT_4bit << LCD_DATA_NIB;
	LCD_PORT &= ~_BV(LCD_E_PIN);
	lcd_delay();
	LCD_PORT |= _BV(LCD_E_PIN);
	lcd_delay();
	lcd_putc(LCD_FUNC_INIT);
	lcd_putc((LCD_SHOW | LCD_SHOW_ON));
	lcd_putc((LCD_MODE | LCD_MODE_RIGHT));
	lcd_putc(LCD_CLR);

	
}


void lcd_task(void)
{

	for ( ;; )
	{
		SPI_DATA_MOSI[SPI_ds1620_temp]=(ds1620_temp/2);
		SPI_DATA_MOSI[SPI_stove_mode_target_temp_var]=stove_mode_room_temp_target_var;
		if (ADC_VALUE3 > LCD_LIGHT_SENCE)
		{
			LCD_PORT |= _BV(LCD_LED_PIN);
		}
		else
		{
			LCD_PORT &= ~_BV(LCD_LED_PIN);
		}
		LCD_PORT &= ~_BV(LCD_RS_PIN);
		lcd_putc(LCD_HOME);
		LCD_PORT |= _BV(LCD_RS_PIN);		
		vTaskDelay(3);
		if ((lcd_display_mode & 0xf0)== 0x10)
		{
			if ((lcd_display_mode & 0x0f)==LCD_MODE_DIAG)
			{
				lcd_display_diag();
			}
			else
			{		
				lcd_display_normal();
			}
		}
		else
		{
			lcd_display_mode |= 0x10;
			LCD_PORT &= ~_BV(LCD_RS_PIN);
			lcd_putc(LCD_CLR);
			LCD_PORT |= _BV(LCD_RS_PIN);
		}				
	}
}


void lcd_display_normal(void)
{
	lcd_display_stove_mode(stove_mode);
	lcd_putc(' ');
	lcd_putv((ADC_VALUE1*4),10,4);
	lcd_putc(' ');
	lcd_putc('R');
	lcd_putc(' ');	
	lcd_putv((ds1620_temp/2),10,2);
	lcd_putc('.');
	if ((ds1620_temp & 0x01) == 1)
	{
		lcd_putc('5');
	}
	else
	{
		lcd_putc('0');
	}
	lcd_display_stove_mode(SPI_DATA_MISO[SPI_STOVE_MODE]);
	lcd_putc(' ');
	lcd_putv(((SPI_DATA_MISO[SPI_ADC_VALUE1])*4),10,4);
	lcd_putc(' ');
	lcd_putc('T');
	lcd_putv(stove_mode_room_temp_target_var,10,3);
	lcd_putc('.');
	lcd_putc('0');
	lcd_display_stepper_pos(STEPPER_DISPLAY,ADC_VALUE1,stove_target_temp);
	lcd_display_stepper_pos(SPI_DATA_MISO[SPI_STEPPER_DISPLAY],SPI_DATA_MISO[SPI_ADC_VALUE1],SPI_DATA_MISO[SPI_stove_target_temp]);
}


void lcd_display_stepper_pos(uint8_t pos_count,uint8_t current_temp,uint8_t target_temp)
{
	lcd_putc('O');
	SPI_DATA_MOSI[SPI_STEPPER_DISPLAY]=pos_count;
	for (uint8_t i; i<5; i++)
	{
		if (i==pos_count)
		{
			lcd_display_stepper_dir(current_temp,target_temp);
		}
		else
		{
			lcd_putc('-');
		}
	}

	lcd_putc('+');

	for (uint8_t i; i<12; i++)
	{
		if (i==(pos_count-6))
		{
			lcd_display_stepper_dir(current_temp,target_temp);
		}
		else
		{
			lcd_putc('-');
		}
	}
	lcd_putc('C');
}

void lcd_display_stepper_dir(uint8_t current_temp, uint8_t target_temp)
{
	if (current_temp == target_temp)
	{
		lcd_putc('=');
	}
	else
	{
		if (current_temp < target_temp)
		{
			lcd_putc(0x7f);
		}
		else
		{
			lcd_putc(0x7e);	
		}
	}
}

void lcd_display_stove_mode(uint8_t stove_mode_var)
{
	switch ((stove_mode_var & 0x03))
	{
		case MODE_NORM:
		lcd_putc('N');
		lcd_putc('O');
		lcd_putc('R');
		lcd_putc('M');
		lcd_putc(' ');
		break;
		case MODE_NITE:
		lcd_putc('N');
		lcd_putc('I');
		lcd_putc('T');
		lcd_putc('E');
		lcd_putc(' ');
		break;
		case MODE_FEED:
		lcd_putc('F');
		lcd_putc('E');
		lcd_putc('E');
		lcd_putc('D');
		lcd_putc(' ');
		break;
		default:
		lcd_putc('N');
		lcd_putc('O');
		lcd_putc(' ');
		lcd_putc(' ');
		lcd_putc('C');
		break;

	}
	

	switch (((stove_mode_var & 0xF0)>>4))
	{
		case MODE_SRT:
		lcd_putc('S');
		lcd_putc('R');
		lcd_putc('T');
		break;
		case MODE_HOT:
		lcd_putc('H');
		lcd_putc('O');
		lcd_putc('T');
		break;
		case MODE_MED:
		lcd_putc('M');
		lcd_putc('E');
		lcd_putc('D');
		break;
		case MODE_LOW:
		lcd_putc('L');
		lcd_putc('O');
		lcd_putc('W');
		break;
		case MODE_OFF:
		lcd_putc('O');
		lcd_putc('F');
		lcd_putc('F');
		break;
		default:
		lcd_putc('O');
		lcd_putc('M');
		lcd_putc('M');
		break;
	}		
}

void lcd_display_diag(void)
{
		lcd_putv(stove_mode_room_temp_target_var,16,2);
		lcd_putv((ds1620_temp/2),16,2);
		lcd_putv(stove_mode,16,2);
		lcd_putv(ADC_VALUE1,16,2);
		lcd_putv(ADC_VALUE2,16,2);
		lcd_putv(ADC_VALUE3,16,2);
		lcd_putv(STEPPER_POS,16,4);
		lcd_putv(STEPPER_POS_CURRENT,16,4);
		lcd_putv(stove_target_temp,16,2);
		lcd_putv(stove_target_temp_lockout,16,2);
		lcd_putv(stove_run_change_count,16,4);
		lcd_putv(stove_cooldown_count,16,4);
		lcd_putc(' ');
		lcd_putc(' ');
		lcd_putc(' ');
		lcd_putc(' ');
		lcd_putc(' ');
		lcd_putc(' ');
		lcd_putc(' ');
		lcd_putc(' ');
		for (uint8_t i=0;i<16;i++)
		{
			lcd_putv(SPI_DATA_MISO[i],16,2);
		}

		lcd_putv(SPI_DATA_MISO[SPI_STOVE_MODE],16,2);
		lcd_putv(stove_mode,16,2);
		lcd_putv(SPI_DATA_MISO[SPI_LOOPBACK],16,2);	
}