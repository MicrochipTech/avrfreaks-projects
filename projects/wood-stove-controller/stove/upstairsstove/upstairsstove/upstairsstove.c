/*
 * upstairsstove.c
 *
 * Created: 1/1/2013 11:02:14 AM
 *  Author: sasquach
 */ 


#include <stdlib.h>
#include <avr/sfr_defs.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include "../upstairsstove/config.h"

#ifdef GCC_MEGA_AVR
	/* EEPROM routines used only with the WinAVR compiler. */
	#include <avr/eeprom.h> 
#endif

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"



#include "ds1620.h"
#include "hd44780.h"
#include "stepper.h"
#include "adc.h"
#include "stovectr.h"
#include "keypad.h"
#include "spi.h"








/*
 * The idle hook is used to scheduler co-routines.
 */
void vApplicationIdleHook( void );

uint8_t reset_value;
extern uint8_t reset_value;

short main( void )
{	
	reset_value = MCUCSR;
	PORTA |= _BV(PA4);
	DDRA &= ~(_BV(PA4));
	PORTA |= _BV(PA5);
	DDRA |= _BV(PA5);
//	DDRB = 0xFF;
//	PORTB = 0;
    adc_init();
	stepper_init();
	stove_control_init();
	lcd_init();
	ds1620_init();
	keypad_init();
	menu_init();
	spi_master_init();
	

	xTaskCreate( ds1620_read, "ds1620", configMINIMAL_STACK_SIZE,NULL,2,NULL );
	xTaskCreate( lcd_task, "LCD", configMINIMAL_STACK_SIZE,NULL,2,NULL );
	xTaskCreate( stepper, "Stepper", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	xTaskCreate( adc, "ADC", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	xTaskCreate( stove_control, "STOVECTR", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	xTaskCreate( keypad, "KeyPad", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	xTaskCreate( menu, "Menu", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	xTaskCreate( spi_master, "SPI", configMINIMAL_STACK_SIZE,NULL,3,NULL );
	
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