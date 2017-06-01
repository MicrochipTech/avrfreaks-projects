/*
 * TC_PWM.c
 *
 * Created: 5/20/2017 8:09:49 PM
 *  Author: Big Daddy
 */ 

#define _TC_PWM_

#include "project.h"
#include <asf.h>
#include <board.h>
#include <conf_board.h>

void TC_PWM_Init(void)
{ 	
	// TC0 module setup table for pin and function	 
	static const gpio_map_t TC0_GPIO_MAP =
	{
		{ PWM0_PIN , PWM0_FUNCTION },
		{ PWM1_PIN , PWM1_FUNCTION },
		{ PWM2_PIN , PWM2_FUNCTION },
		{ PWM3_PIN , PWM3_FUNCTION },
		{ PWM4_PIN , PWM4_FUNCTION },
		{ PWM5_PIN , PWM5_FUNCTION }
	};
	
	// enable clock for module
	sysclk_enable_peripheral_clock(tc0);
	
	// Assign GPIO to TC0.
	gpio_enable_module(TC0_GPIO_MAP, sizeof(TC0_GPIO_MAP) / sizeof(TC0_GPIO_MAP[0]));
	 
	// set clock PBA and PBC
	// enable clocks for counter timers
//	pm_enable_module(AVR32_TC0_CLK_PBC);
//	pm_enable_module(AVR32_TC1_CLK_PBA);

	
	//  set up PWMs in CTCs
	tc_select_external_clock(tc0 ,0 ,TC_CH0_EXT_CLK0_SRC_NO_CLK);
	tc_select_external_clock(tc0 ,1 ,TC_CH0_EXT_CLK0_SRC_NO_CLK);
	tc_select_external_clock(tc0 ,2 ,TC_CH0_EXT_CLK0_SRC_NO_CLK);

	init_tc_PWM_channel(TC0_0);
	init_tc_PWM_channel(TC0_1);
	init_tc_PWM_channel(TC0_2);

	// Set the compare trigger.
	tc_write_rc(tc0, TC0_0, PWM_PERIOD_VALUE ); // Set RC value
	tc_write_rc(tc0, TC0_1, PWM_PERIOD_VALUE ); // Set RC value
	tc_write_rc(tc0, TC0_2, PWM_PERIOD_VALUE ); // Set RC value
	
	// init motors to off
	set_pwm_duty_cycle(MTR0, 0);
	set_pwm_duty_cycle(MTR1, 0);
	set_pwm_duty_cycle(MTR2, 0);
	set_pwm_duty_cycle(MTR3, 0);
	set_pwm_duty_cycle(MTR4, 0);
	set_pwm_duty_cycle(MTR5, 0);
 	
 	//  gpio outputs: Motor direction control
//  	gpio_configure_pin(DIR0, GPIO_DIR_OUTPUT);
//  	gpio_configure_pin(DIR1, GPIO_DIR_OUTPUT);
//  	gpio_configure_pin(DIR2, GPIO_DIR_OUTPUT);
//  	gpio_configure_pin(DIR3, GPIO_DIR_OUTPUT);
//  	gpio_configure_pin(DIR4, GPIO_DIR_OUTPUT);
//  	gpio_configure_pin(DIR5, GPIO_DIR_OUTPUT);
 	
 	gpio_configure_pin(DIR0, GPIO_DIR_OUTPUT|GPIO_INIT_LOW);
 	gpio_configure_pin(DIR1, GPIO_DIR_OUTPUT|GPIO_INIT_LOW);
 	gpio_configure_pin(DIR2, GPIO_DIR_OUTPUT|GPIO_INIT_LOW);
 	gpio_configure_pin(DIR3, GPIO_DIR_OUTPUT|GPIO_INIT_LOW);
 	gpio_configure_pin(DIR4, GPIO_DIR_OUTPUT|GPIO_INIT_LOW);
 	gpio_configure_pin(DIR5, GPIO_DIR_OUTPUT|GPIO_INIT_LOW);
 	
	// Gentlemen, start your engines
	tc_start(tc0, TC0_0);
	tc_start(tc0, TC0_1);
	tc_start(tc0, TC0_2);

}


void init_tc_PWM_channel(unsigned int channel)
{
	// Options for waveform generation.
	tc_waveform_opt_t waveform_opt =
	{
		.channel  = channel,                      // Channel selection.

		.bswtrg   = TC_EVT_EFFECT_NOOP,           // Software trigger effect on TIOB.
		.beevt    = TC_EVT_EFFECT_NOOP,           // External event effect on TIOB.
		.bcpc     = TC_EVT_EFFECT_CLEAR,          // RC compare effect on TIOB.
		.bcpb     = TC_EVT_EFFECT_SET,            // RB compare effect on TIOB.

		.aswtrg   = TC_EVT_EFFECT_NOOP,           // Software trigger effect on TIOA.
		.aeevt    = TC_EVT_EFFECT_NOOP,           // External event effect on TIOA.
		.acpc     = TC_EVT_EFFECT_CLEAR,          // RC compare effect on TIOA.
		.acpa     = TC_EVT_EFFECT_SET,            // RA compare effect on TIOA.

		.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,      // Waveform selection: Up mode with automatic trigger on RC compare.
		.enetrg   = false,                        // External event trigger enable.
		.eevt     = TC_EXT_EVENT_SEL_XC0_OUTPUT,  // External event selection.
		.eevtedg  = TC_SEL_NO_EDGE,               // External event edge selection.
		.cpcdis   = false,                        // Counter disable when RC compare.
		.cpcstop  = false,                        // Counter clock stopped with RC compare.

		.burst    = TC_BURST_NOT_GATED,           // Burst signal selection.
		.clki     = TC_CLOCK_RISING_EDGE,         // Clock inversion.
		.tcclks   = TC_CLOCK_SOURCE_TC2           // Internal source clock 2, connected to fPBA / 2.
	};

	// Initialize the timer/counter waveform.
	tc_init_waveform(tc0, &waveform_opt);
}

void set_pwm_duty_cycle(U8 pwm, U8 dutycycle)
{
	if (pwm > 5) {return;}
	if (dutycycle > 100) {dutycycle = 100;}// bounds check

	U16 counter_value = (U16)PWM_PERIOD_VALUE - (U16)( ( (U32)PWM_PERIOD_VALUE * (U32)dutycycle ) / (U32)100) + 1;
		
	switch (pwm)
	{
	case 0:  // TC0_1B
		tc_write_rb(tc0, TC0_1, counter_value );
		break;
	case 1:  // TC0_2A
		tc_write_ra(tc0, TC0_2, counter_value );
		break;
	case 2:  // TC0_0B
		tc_write_rb(tc0, TC0_0, counter_value );
		break;
	case 3:  // TC0_1A
		tc_write_ra(tc0, TC0_1, counter_value );
		break;
	case 4:  // TC0_2B
		tc_write_rb(tc0, TC0_2, counter_value );
		break;
	case 5:  // TC0_0A
		tc_write_ra(tc0, TC0_0, counter_value );
		break;
	default:  // do nothing
		break;
	}
}