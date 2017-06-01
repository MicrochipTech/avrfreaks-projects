/*
 * SP_model_platform_control.c
 *
 * Created: 5/11/2017 5:18:57 PM
 *  Author: Big Daddy
 */ 

/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */

#define _USER_BOARD_

#include "project.h"

#include <asf.h>
#include <avr32/io.h>



#define DELAY(cnt,vrbl) vrbl=cnt;while(vrbl--){__asm__ __volatile__ ("nop");}
	
int main (void)
{
	volatile U8 pwm_rate = 0;
	U8 Dirflag='L';
//	U8 Old = 1;
	board_init();
	delay_init(BOARD_OSC0_HZ);

	/* Enable global Interrupts for ADC,Scomm etc */
	Enable_global_interrupt();
	
	// clear screen
	printf("\f\033[2J");
	while (1)
	{

		delay_ms(50);
		LED_TOGGLE(GRN_LED);
		
		pwm_rate = gn_DriveFlag*10;
		set_pwm_duty_cycle(MTR3, pwm_rate);

// 		if (gn_DirFlag == 0) {LED_OFF(RED_LED); gpio_set_pin_high(DIR3); Dirflag = 'L';}
// 		if (gn_DirFlag == 1) {LED_ON(RED_LED);  gpio_set_pin_low(DIR3);  Dirflag = 'R';}
// // 		if (gn_DriveFlag)
// 		{
// 			if (pwm_rate >= gn_DriveFlag*10 ) {	UpDn = -1; }
// 			if (pwm_rate == 0 ) {	UpDn =  1; }
// 			pwm_rate = pwm_rate + UpDn;
// 		}
// 		else {pwm_rate = 0;}
			
//		printf("                          \r%0d%c %3d%%  %d %d", gn_DriveFlag, Dirflag, pwm_rate, gn_PosDir[3], gn_PosCnt[3]);

	}

	// Insert application code here, after the board has been initialized.
//TTD separate ADC code out	
//TTD test actual PWM motor driving
}


