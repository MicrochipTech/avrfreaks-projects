/*
 * ds1620.c
 *
 * Created: 12/23/2012 9:13:35 AM
 *  Author: sasquach
 */ 
#include <stdint.h>
#include <math.h>
#include <ds1620.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>


uint16_t ds1620_temp;

void ds_1620_write_byte(uint8_t ds1620_data_var1)
{
	for ( uint8_t ds1620_bit_count = 0; ds1620_bit_count < 8 ; ds1620_bit_count++ )
		{
			vTaskDelay(ds1620_clk_delay);
			ds1620_PORT &= ~_BV(ds1620_clk);
			if ((ds1620_data_var1 & 1<<(ds1620_bit_count)) == 1<<(ds1620_bit_count))
			{
				ds1620_PORT |= _BV(ds1620_data);
			}
			else
			{
				ds1620_PORT &= ~_BV(ds1620_data);
			}
			vTaskDelay(ds1620_clk_delay);
			ds1620_PORT |=_BV(ds1620_clk);
		}
}

void ds1620_init(void)
{
		ds1620_DDR |= _BV(ds1620_clk) | _BV(ds1620_data) | _BV(ds1620_rst);
		ds1620_PORT &= ~_BV(ds1620_clk);
		ds1620_PORT |= _BV(ds1620_rst);

}

void ds1620_read(void)
{
	uint16_t ds1620_data_var;
	uint8_t ds1620_bit_count;
	uint16_t ds1620_data_bit;
	uint8_t testa = 0;
	for ( ;; )
	{
		//ds1620_PORT &= ~_BV(ds1620_rst2);
		vTaskDelay(ds1620_clk_delay);
		ds1620_DDR |= _BV(ds1620_clk) | _BV(ds1620_data) | _BV(ds1620_rst);
		ds1620_PORT &= ~_BV(ds1620_clk);
		ds1620_PORT &= ~_BV(ds1620_rst);
		vTaskDelay(ds1620_clk_delay);
		ds1620_PORT |= _BV(ds1620_rst);
		vTaskDelay(ds1620_clk_delay);
				
	//	ds_1620_write_byte(0x0c);
	//	vTaskDelay(20);
	//	ds_1620_write_byte(0x10);
	//	vTaskDelay(100);
		ds_1620_write_byte(0xEE);
		vTaskDelay(ds1620_clk_delay);
		ds1620_PORT &= ~_BV(ds1620_rst);
		vTaskDelay(ds1620_clk_delay);	
		ds1620_PORT |= _BV(ds1620_rst);
		ds_1620_write_byte(0xAA);
		vTaskDelay(ds1620_clk_delay);
		ds1620_PORT &= ~_BV(ds1620_data);
		ds1620_DDR &= ~_BV(ds1620_data);
		vTaskDelay(ds1620_clk_delay);
		ds1620_data_var = 0;
		testa = 0;
		for ( ds1620_bit_count = 0; ds1620_bit_count < 9 ; ds1620_bit_count++ )
		{
			testa++;
			ds1620_PORT &= ~_BV(ds1620_clk);
			vTaskDelay(ds1620_clk_delay);
			ds1620_data_bit = ds1620_PIN;
			ds1620_data_bit &= _BV(ds1620_data);
			ds1620_data_bit >>= ds1620_data;
			ds1620_data_bit <<= ds1620_bit_count;
			ds1620_data_var |= ds1620_data_bit;
			ds1620_PORT |=_BV(ds1620_clk);
			vTaskDelay(ds1620_clk_delay);	
		}
		
		ds1620_temp = ds1620_data_var;
	}	
};