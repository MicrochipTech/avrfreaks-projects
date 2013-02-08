/*
 * stovectr.c
 *
 * Created: 12/28/2012 9:08:22 PM
 *  Author: sasquach
 */ 

#include <avr/io.h>
#include "config.h"
#include "stovectr.h"
#include "adc.h"
#include "stepper.h"
#ifdef DS1620_H_ENABLE
#include "ds1620.h"
#endif
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "spi.h"

uint8_t stove_mode;
uint8_t stove_target_temp;
uint8_t stove_target_temp_ref;
uint16_t stove_run_change_count_len;
uint16_t stove_run_change_count;
uint16_t stove_run_change_temp_count;
uint16_t stove_run_change_steps;
uint16_t stove_run_upper_limit;
uint16_t stove_run_lower_limit;
uint8_t stove_run_limit_result;
uint16_t stove_cooldown_count;
uint8_t stove_target_temp_lockout;
uint8_t stove_mode_start_first;
uint8_t stove_mode_room_temp_target_var;
uint8_t stove_mode_room_temp_target_test;
uint8_t EEMEM stove_mode_room_temp_target_ee = stove_mode_room_temp_target;

#ifndef DOWN_STAIRS_

uint16_t stove_control_buzzer_count_upstairs;
uint16_t stove_control_buzzer_count_downstairs;

#endif



void stove_control_init(void)
{
#ifndef DOWN_STAIRS_	
	stove_mode_room_temp_target_var =eeprom_read_byte(&stove_mode_room_temp_target_ee);
	stove_control_buzzer_PORT |= (_BV(stove_control_buzzer_PI));
	stove_control_buzzer_DDR |= _BV(stove_control_buzzer_PI);
#endif

	stove_control_led_PORT &= ~(_BV(stove_control_led_PI));
	stove_control_led_DDR |=_BV(stove_control_led_PI);
	
	STEPPER_POS=damper_shutdown_pos;  
	stove_target_temp = stove_start_temp;
	stove_run_change_count = 0;
	stove_run_upper_limit = damper_close_pos;
	stove_run_lower_limit = damper_open_pos;
	stove_run_limit_result = 0;
	stove_target_temp_lockout=0;
	stove_mode=0;
	stove_mode &= 0xF0;
	stove_mode |= MODE_NORM;
	stove_mode |= (4<<MODE_SRT);

	stove_control_reset(stove_control_reset_normal);
	stove_mode_start_first=2;
	STEPPER_POS=damper_shutdown_pos;
}

void stove_control_nite_mode(void)
{
	
}

void stove_control_reset(uint8_t stove_control_reset_mode)
{
#ifndef DOWN_STAIRS_	
	cli();
	eeprom_update_byte(&stove_mode_room_temp_target_ee,stove_mode_room_temp_target_var);
	sei();
	stove_control_buzzer_count_upstairs=0;
#endif
#ifdef DOWN_STAIRS_
	stove_mode_room_temp_target_var=stove_mode_room_temp_target;
#endif
	
	stove_mode_start_first=0;
	stove_run_upper_limit = damper_close_pos;
	stove_run_lower_limit = damper_open_pos;
	stove_target_temp_lockout=stove_start_temp;
	stove_target_temp=stove_run_hot_temp;
	stove_cooldown_count=0;
	stove_mode &= 0xF0;
	stove_mode |= MODE_NORM;

	if (stove_target_temp == 00 )
	{
		stove_target_temp=stove_run_hot_temp; 
	}
		
	if (stove_control_reset_mode == stove_control_reset_nite)
	{
		stove_target_temp_lockout=stove_run_cool_temp;
		stove_target_temp=stove_run_cool_temp;
		stove_mode &= 0xF0;
		stove_mode |= MODE_NITE;	
		stove_control_nite_mode();
	}
}

void stove_control_change_damper_pos(uint8_t i)
{
	stove_run_limit_result = 0;
	
	if (i == 1)
	{
		if (STEPPER_POS >= stove_run_change_steps)
		{
			STEPPER_POS -= stove_run_change_steps;
		}
		else
		{
			STEPPER_POS = 0;
		}
		
		if (STEPPER_POS < stove_run_lower_limit)
		{
			STEPPER_POS = stove_run_lower_limit;
			stove_run_limit_result = 1 ;
		}

	}
	else
	{
		if ((0xFFFF - STEPPER_POS) >= stove_run_change_steps)
		{
			STEPPER_POS += stove_run_change_steps;
		}
		else
		{
			STEPPER_POS = 0xFFFF;
		}
		
		if (STEPPER_POS > stove_run_upper_limit)
		{
			STEPPER_POS = stove_run_upper_limit;
			stove_run_limit_result = 2 ;
		}
		
	}
	
}

/*  function stove_control_fuzzy
*
*  test stove temp against target temp if different
*  see by how much n set delay n amount to adjust for
*  after the delay check if temp is moving the right direction if not
*  move the damper the set amount
*
*/
void stove_control_fuzzy(void)
{
		/*
		Test stove temp against target temp if equal do nothing
		if not see how far away from target 
		if    mask     then        delay  and amount of change
		*/
		if (ADC_VALUE1 != stove_target_temp)
		{
			if ((ADC_VALUE1 & (~stove_run_change_stage2_mask)) == (stove_target_temp & (~stove_run_change_stage2_mask)))
			{
				stove_run_change_count_len = stove_run_change_count_len_stage1;
				stove_run_change_steps = stove_run_change_steps_stage1;
			}
			else
			{
				if ((ADC_VALUE1 & (~stove_run_change_stage3_mask)) == (stove_target_temp & (~stove_run_change_stage3_mask)))
				{
					stove_run_change_count_len = stove_run_change_count_len_stage2;
					stove_run_change_steps = stove_run_change_steps_stage2;				
				}
				else
				{
					if ((ADC_VALUE1 & (~stove_run_change_stage4_mask)) == (stove_target_temp & (~stove_run_change_stage4_mask)))
					{
						stove_run_change_count_len = stove_run_change_count_len_stage3;
						stove_run_change_steps = stove_run_change_steps_stage3;
					}
					else
					{
						stove_run_change_count_len = stove_run_change_count_len_stage4;
						stove_run_change_steps = stove_run_change_steps_stage4;

					}
				}
			}
		}
		
		// delay for speed
		
		if ( stove_run_change_count <= stove_run_change_count_len )
		{
			stove_run_change_count++;
		}
		else
		{
			/*
			once delay is done figure out if the stove temp is moving in the right direction if it is do nothing
			else change damper position
			*/
			stove_run_change_count = 0;
			if (ADC_VALUE1 < stove_target_temp)
			{
				if (ADC_VALUE1 <= stove_target_temp_ref) 
				{
					stove_target_temp_ref=ADC_VALUE1;
					stove_control_change_damper_pos(open_damper);
				}
				else
				{
					stove_target_temp_ref=ADC_VALUE1;
				}
			}
			else
			{
				if (ADC_VALUE1 >= stove_target_temp_ref) 
				{
					stove_target_temp_ref=ADC_VALUE1;
					stove_control_change_damper_pos(close_damper);
				}
				else
				{
					stove_target_temp_ref=ADC_VALUE1;
				}
				
			}
		}	
}

/*	function stove_control_checks
*		set stove target temp vs room temp
*		check stove burn position and adjust accordingly
*		set nite mode if required
*/

void stove_control_checks(void)
{
	if (stove_target_temp!=stove_shutdown_temp)
	{
		if (stove_mode_start_first==0)
		{
			stove_mode_start_first=1;
			
			if (ADC_VALUE2 < stove_back_temp) 
			{
				stove_run_lower_limit = bottem_open_pos;
				stove_target_temp = stove_start_temp;
				stove_target_temp_lockout= stove_start_temp;
				STEPPER_POS=bottem_open_pos;
			}		
		}
		if (stove_target_temp > stove_target_temp_lockout)
		{
			stove_target_temp=stove_target_temp_lockout;
		}
		if (stove_target_temp == stove_start_temp)
		{
			if (ADC_VALUE2 > stove_back_temp)
			{
				stove_run_lower_limit = damper_open_pos;
				stove_target_temp = stove_run_hot_temp;
			}
			stove_cooldown_test(stove_run_hot_temp,stove_mode_warmup_count);
		}
		if (stove_target_temp == stove_run_hot_temp)
		{
			stove_cooldown_test(stove_run_med_temp,stove_mode_high_temp_lower_count);
			stove_target_temp_config(stove_run_hot_temp,stove_run_med_temp,stove_mode_high_temp_lower_count);		
		}
		if (stove_target_temp == stove_run_med_temp)
		{
			stove_cooldown_test(stove_run_cool_temp,stove_mode_med_temp_lower_count);		
			stove_target_temp_config(stove_run_hot_temp,stove_run_cool_temp,stove_mode_med_temp_lower_count);
		}
		if (stove_target_temp == stove_run_cool_temp)
		{
			stove_cooldown_test(stove_shutdown_temp,stove_mode_low_temp_lower_count);	
			stove_target_temp_config(stove_run_med_temp,stove_run_cool_temp,stove_mode_low_temp_lower_count);
			if(stove_cooldown_count >= (stove_mode_low_temp_lower_count/2))
			{
				stove_target_temp_lockout=stove_run_cool_temp;
			}
		}
		if (stove_target_temp == stove_shutdown_temp)
		{
			stove_run_upper_limit=damper_shutdown_pos;
			STEPPER_POS=damper_shutdown_pos;
		}
		if (stove_target_temp > stove_target_temp_lockout)
		{
			stove_target_temp=stove_target_temp_lockout;
		}
		if (stove_mode_start_first == 2)
		{
			vTaskDelay(1000);
			if (ADC_VALUE2 > stove_back_temp)
			{
				stove_mode_start_first=0;
			}
			else
			{
				stove_target_temp=stove_shutdown_temp;
				stove_target_temp_lockout=stove_shutdown_temp;
			}
		}		
	}		
}

void stove_cooldown_test(uint8_t stove_target_temp_test, uint16_t stove_cooldown_count_amount )
{
	if (STEPPER_POS != stove_run_lower_limit)
	{
		stove_cooldown_count=0;
	}
	if (ADC_VALUE1 >= stove_target_temp)
	{
		stove_cooldown_count=0;
	}
	else
	{
		if (stove_cooldown_count <= stove_cooldown_count_amount) 
		{
			stove_cooldown_count++;
		}
		else
		{
			stove_target_temp=stove_target_temp_test;
			stove_target_temp_lockout=stove_target_temp_test;
			stove_cooldown_count=0;
			
		}
	}
}

void stove_target_temp_config(uint8_t stove_target_temp_higher, uint8_t stove_target_temp_lower, uint16_t stove_run_change_count_len)
{   
	if (ds1620_temp == (stove_mode_room_temp_target_var*2))
	{
		stove_run_change_temp_count=0;
	}
	if (ds1620_temp >= ((stove_mode_room_temp_target_var+stove_mode_room_temp_over)*2))
	{
		if (stove_run_change_temp_count <= stove_run_change_count_len)
		{
			stove_run_change_temp_count++;
		}
		else 
		{
			stove_run_change_temp_count=0;
			stove_target_temp=stove_target_temp_lower;			
		}
	}
	if (ds1620_temp <=((stove_mode_room_temp_target_var-stove_mode_room_temp_under)*2))
	{
		if (stove_run_change_temp_count <= stove_run_change_count_len)
		{
			stove_run_change_temp_count++;
		}
		else
		{
			stove_run_change_temp_count=0;
			stove_target_temp=stove_target_temp_higher;
		}		
	}	
	
}

void stove_control_mode_check(void)
{
	switch (stove_target_temp)
	{
		case stove_start_temp:
			stove_mode &= 0x0f;
			stove_mode |= (MODE_SRT<<4);
		break;
		case stove_run_hot_temp :
			stove_mode &= 0x0f;
			stove_mode |= (MODE_HOT<<4);
		break;
		case stove_run_med_temp :
			stove_mode &= 0x0f;
			stove_mode |= (MODE_MED<<4);	
		break;
		case stove_run_cool_temp :
			stove_mode &= 0x0f;
			stove_mode |= (MODE_LOW<<4);
		break;
		case stove_shutdown_temp :
			stove_mode &= 0x0f;
			stove_mode |= (MODE_OFF<<4);
			stove_mode &= ~((1<<MODE_LED_bit) | (1<<MODE_BEEP_bit));
		break;
	}
	if ((stove_mode & 0x03) != MODE_NITE)
	{
		if (stove_target_temp_lockout == stove_start_temp)
		{
			stove_mode &= 0xFC;
			stove_mode |= MODE_NORM;
			stove_mode &= ~(1<<MODE_LED_bit);
			stove_mode &= ~(1<<MODE_BEEP_bit);
		}
		else
		{
			stove_mode &= 0xFC;
			stove_mode |= MODE_FEED;
			stove_mode |= (1<<MODE_LED_bit);
			stove_mode |= (1<<MODE_BEEP_bit);
						
		}
		if (stove_target_temp_lockout == stove_shutdown_temp)
		{
			stove_mode &= ~(1<<MODE_LED_bit);
			stove_mode &= ~(1<<MODE_BEEP_bit);
		}
	}
	else
	{
		stove_mode &= ~(1<<MODE_LED_bit);
		stove_mode &= ~(1<<MODE_BEEP_bit);
	}
}

void stove_control_led(void)
{
	if ((stove_mode & (1<<MODE_LED_bit)) == (1<<MODE_LED_bit))
	{
		stove_control_led_PORT |= _BV(stove_control_led_PI);
	}
	else
	{
		stove_control_led_PORT &= ~(_BV(stove_control_led_PI));
	}

#ifndef DOWN_STAIRS_
	stove_mode_buzzer();

	
#endif

}

#ifndef DOWN_STAIRS_

void stove_mode_buzzer(void)
{
	if ((stove_mode & (1<<MODE_BEEP_bit)) == (1<<MODE_BEEP_bit))
	{
		if (stove_control_buzzer_count_upstairs < stove_control_buzzer_short)
		{
			stove_control_buzzer_PORT &= ~(_BV(stove_control_buzzer_PI));
		}
		else
		{
			stove_control_buzzer_PORT |= _BV(stove_control_buzzer_PI);
		}
		stove_control_buzzer_count_upstairs++;
		if (stove_control_buzzer_count_upstairs == stove_control_buzzer_long)
		{
			stove_control_buzzer_count_upstairs=0;
		}
	}
	if 	((SPI_DATA_MISO[SPI_STOVE_MODE] & (1<<MODE_BEEP_bit)) == (1<<MODE_BEEP_bit))
	{
		if (stove_control_buzzer_count_downstairs < stove_control_buzzer_short)
		{
			stove_control_buzzer_PORT &= ~(_BV(stove_control_buzzer_PI));
		}
		else
		{
			stove_control_buzzer_PORT |= _BV(stove_control_buzzer_PI);
		}
		stove_control_buzzer_count_downstairs++;
		if (stove_control_buzzer_count_downstairs == stove_control_buzzer_long)
		{
			stove_control_buzzer_count_downstairs=0;
		}
	}
}

#endif


void stove_control(void)
{
	for ( ;; )
	{
		vTaskDelay(1000);
		stove_control_fuzzy(); // call fuzzy control for temp adjust
		stove_control_checks();
		stove_control_mode_check();
		stove_control_led();
	}
}