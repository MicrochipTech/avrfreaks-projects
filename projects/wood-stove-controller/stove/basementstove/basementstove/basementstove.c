/*
 * basementstove.c
 *
 * Created: 1/1/2013 2:14:01 PM
 *  Author: sasquach
 */ 

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <avr/eeprom.h>



#ifdef GCC_MEGA_AVR
	/* EEPROM routines used only with the WinAVR compiler. */
	#include <avr/eeprom.h> 
#endif

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"


#include "../basementstove/config.h" // this needs to be included  start
#include "stepper.h"
#include "adc.h"
#include "stovectr.h"
#include "spi.h"





 uint8_t ds1620_temp;
 uint8_t reset_count;



/*
 * The idle hook is used to scheduler co-routines.
 */

void vApplicationIdleHook( void );

uint8_t reset_value;
extern uint8_t reset_value;
void spi_overhead(void)
{
	for(;;)
	{
		ds1620_temp=SPI_DATA_MOSI[SPI_ds1620_temp];
		stove_mode_room_temp_target_var=SPI_DATA_MOSI[SPI_stove_mode_target_temp_var];
		switch (SPI_DATA_MOSI[SPI_basement_control])
		{
			case stove_control_reset_nite:
			stove_control_reset(stove_control_reset_nite);
			break;
			case stove_control_reset_normal:
			stove_control_reset(stove_control_reset_normal);
			break;			
		}
		SPI_DATA_MISO[SPI_LOOPBACK]=stove_mode_start_first; //SPI_DATA_MOSI[SPI_LOOPBACK];
		SPI_DATA_MISO[SPI_STOVE_MODE]=stove_mode;
		SPI_DATA_MISO[SPI_ADC_VALUE1]=ADC_VALUE1;
		SPI_DATA_MISO[SPI_ADC_VALUE2]=ADC_VALUE2;
		SPI_DATA_MISO[SPI_HIGH_STEPPER_POS]=(STEPPER_POS >> 8);
		SPI_DATA_MISO[SPI_LOW_STEPPER_POS]=(STEPPER_POS & 0XFF);
		SPI_DATA_MISO[SPI_HIGH_STEPPER_POS_CURRENT]=(STEPPER_POS_CURRENT >> 8);
		SPI_DATA_MISO[SPI_LOW_STEPPER_POS_CURRENT]=(STEPPER_POS_CURRENT & 0XFF);
		SPI_DATA_MISO[SPI_stove_target_temp]=stove_target_temp;
		SPI_DATA_MISO[SPI_stove_target_temp_lockout]=stove_target_temp_lockout;
		SPI_DATA_MISO[SPI_HIGH_stove_run_change_count]=(stove_run_change_count >> 8);
		SPI_DATA_MISO[SPI_LOW_stove_run_change_count]=(stove_run_change_count & 0XFF);
		SPI_DATA_MISO[SPI_HIGH_stove_cooldown_count]=(stove_cooldown_count >> 8);
		SPI_DATA_MISO[SPI_LOW_stove_cooldown_count]=(stove_cooldown_count & 0XFF);
		SPI_DATA_MISO[SPI_STEPPER_DISPLAY]=STEPPER_DISPLAY;
		vTaskDelay(100);
		if ((PINA & (_BV(PA5))) != (_BV(PA5)) )
		{
			stove_control_reset(stove_control_reset_normal);
			reset_count++;
			PORTA &= ~(_BV(PA4));
			if (reset_count > 10)
			{
				stove_control_reset(stove_control_reset_nite);
				PORTA |= _BV(PA4);
				vTaskDelay(100);
				PORTA &= ~(_BV(PA4));						
			}
		}
		else
		{
			reset_count=0;
		}
		if ((stove_mode & 0x03) == MODE_FEED)
		{
			PORTA |= _BV(PA4);
		}
		else
		{
			PORTA &= ~(_BV(PA4));
		}
	}	
}
short main( void )
{	
	reset_value = MCUCSR;
	PORTA |=_BV(PA5);
	DDRA &= ~(_BV(PA5));
	
	PORTA |= _BV(PA2);
	DDRA |= _BV(PA2);
	PORTA |= _BV(PA4);
	DDRA |= _BV(PA4);
	
//	DDRB = 0xFF;
//	PORTB = 0;
    adc_init();
	stepper_init();
	stove_control_init();
	spi_slave_init();
	
	

	xTaskCreate( stepper, "Stepper", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	xTaskCreate( adc, "ADC", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	xTaskCreate( stove_control, "STOVECTR", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	xTaskCreate( spi_slave, "SPIslave", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	xTaskCreate( spi_overhead, "SPIover", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	
	/* In this port, to use preemptive scheduler define configUSE_PREEMPTION 
	as 1 in portmacro.h.  To use the cooperative scheduler define 
	configUSE_PREEMPTION as 0. */
	vTaskStartScheduler();

	return 0;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	for( ;; )
	{
		asm volatile("nop\n\t");
	}
}