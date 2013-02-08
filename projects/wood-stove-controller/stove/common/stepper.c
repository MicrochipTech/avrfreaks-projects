/*
 * stepper.c
 *
 * Created: 12/27/2012 3:00:17 PM
 *  Author: sasquach
 */ 

#include <stdint.h>
#include <math.h>
#include "config.h"
#include <stepper.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>


uint8_t STEPPER_CYCLE[4];
uint16_t STEPPER_POS;
uint16_t STEPPER_POS_CURRENT;
uint8_t  STEPPER_INIT_DONE;
uint16_t STEPPER_POS_INTERNAL;
uint8_t STEPPER_DISPLAY;

void stepper_init (void)
{
	STEPPER_PORT &= ~(0x0f << STEPPER_SHIFT);
	STEPPER_STOP_PORT |= _BV(STEPPER_STOP);
	STEPPER_DDR |= (0x0f << STEPPER_SHIFT);
	STEPPER_STOP_DDR |= _BV(STEPPER_STOP);
	
	STEPPER_CYCLE[0] = STEPPER_CYCLE1;
	STEPPER_CYCLE[1] = STEPPER_CYCLE2;
	STEPPER_CYCLE[2] = STEPPER_CYCLE3;
	STEPPER_CYCLE[3] = STEPPER_CYCLE4;
	STEPPER_POS_INTERNAL=STEPPER_POS_START;
	STEPPER_POS=STEPPER_POS_START;
	STEPPER_POS_CURRENT=STEPPER_POS_CURRENT_START;
	STEPPER_INIT_DONE=0;

}

void stepper_pos_calc(void)
{ 
	if (STEPPER_POS_CURRENT >= damper_open_pos)
	{
		STEPPER_DISPLAY = ((STEPPER_POS_CURRENT-damper_open_pos)/((damper_close_pos-damper_open_pos)/11));
		STEPPER_DISPLAY += 6;
	}
	else
	{
		STEPPER_DISPLAY = ((STEPPER_POS_CURRENT-bottem_open_pos)/((damper_open_pos-bottem_open_pos)/5));
	}
	if (STEPPER_POS_CURRENT > damper_close_pos)
	{
		STEPPER_DISPLAY=0x11;	
	}
	if (STEPPER_POS_CURRENT < bottem_open_pos)
	{
		STEPPER_DISPLAY=0;
	}
}


void stepper(void)
{
	uint16_t STEPPER_STEP = 0;
	uint8_t  STEPPER_STOP_VAR;
	
	for ( ;; )
	{
		stepper_pos_calc();	
		STEPPER_STOP_VAR = STEPPER_STOP_PIN & _BV(STEPPER_STOP);
		STEPPER_STOP_VAR >>= STEPPER_STOP;
		if (STEPPER_STOP_VAR != 0x01 )
		{
			if ((STEPPER_POS_CURRENT & 0xff00) != (STEPPER_POS_STOP & 0xFF00))
			{
				STEPPER_POS_CURRENT = STEPPER_POS_STOP;
				STEPPER_POS_INTERNAL= STEPPER_POS_STOP;
				STEPPER_INIT_DONE = 1;
			}							
		}
		if (STEPPER_INIT_DONE == 1)
		{
			STEPPER_POS_INTERNAL=STEPPER_POS;

		}
		if (STEPPER_POS_INTERNAL == STEPPER_POS_CURRENT)
		{
			STEPPER_PORT &= ~(0x0f << STEPPER_SHIFT);	
		}
		else
		{
			if (STEPPER_POS_INTERNAL > STEPPER_POS_CURRENT)
			{
				STEPPER_POS_CURRENT++;
				STEPPER_STEP = STEPPER_POS_CURRENT;
				STEPPER_STEP &= 0b11;
                STEPPER_PORT &= ~(0x0f << STEPPER_SHIFT);
				STEPPER_PORT |= (STEPPER_CYCLE[STEPPER_STEP] << STEPPER_SHIFT);
			}
			else
			{
				STEPPER_POS_CURRENT--;
				STEPPER_STEP = STEPPER_POS_CURRENT;
				STEPPER_STEP &= 0b11;
				STEPPER_PORT &= ~(0x0f << STEPPER_SHIFT);
				STEPPER_PORT |= (STEPPER_CYCLE[STEPPER_STEP] << STEPPER_SHIFT);				
			}
		}

	vTaskDelay (STEPPER_SPEED);
		
	}
}
