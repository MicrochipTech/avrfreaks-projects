/**
* @file main.c
*
* Copyright (c) 2004 Atmel.
*
* @brief Ce fichier permet de commander un moteur asynchrone par une loi en U/f
*
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.6)
* @date 2006/03/08 17:02:28 (created on 06/04/2004)
* @author raubree (created by Emmanuel David)

*****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdlib.h>

#include "config.h"

#define MAX_THETA	120 // one quarter of the circle
#define MAX_THETAx2	240
#define MAX_THETAx3	360
#define MAX_THETAx4	480

#define MAX_PWM		2666
// 64MHz (PLL frequency) / 2666 / 2 = 12 kHz (PWM frequency)

#define DeadTime 	32     // 32 => temps mort = 0.5 µs
#define MAX_AMPLITUDE  ((MAX_PWM / 2) - DeadTime)

#define K_scal		32 // used for the angle integrator


volatile uint8_t   Flag_IT_timer0=0;
volatile uint8_t   Flag_IT_ADC=0 ;

int16_t           Omega_meas;
int16_t           Omega_ref = -160;
int16_t           Command = 0;
uint16_t           theta1=0, theta2=160, theta3=320 ;

uint16_t	amplitude , OmegaTe = 64 ;
uint8_t		direction = 0 ;
uint16_t	PWM0, PWM1, PWM2;

// uint16_t        DACoutput;

void ADC_Init(void);
void ADC_start_conv(void);
void DAC_Init(void);
void init(void);
void PSC_Init(unsigned int ot0,  unsigned int ot1);
void PSC0_Load (unsigned int dt0, unsigned int dt1);
void PSC1_Load (unsigned int dt0, unsigned int dt1);
void PSC2_Load (unsigned int dt0, unsigned int dt1);
void PSC_Load (unsigned int dt0a,  unsigned int dt1a,
               unsigned int dt0b,  unsigned int dt1b,
               unsigned int dt0c,  unsigned int dt1c);
int16_t    mc_control_speed_16b(int16_t speed_ref , int16_t speed_measure);
uint16_t    controlVF(uint16_t wTs);
int16_t    read_acquisition(void) ;
uint16_t    duty_cycle(uint16_t theta, uint16_t Vm) ;


/**
* @brief main
*/
int main(void)
{
	uint8_t	Counter_PE1 = 0;
	uint16_t Softcounter = 0 ;

	/* remove CKDIV8 fuse effect  */
	CLKPR = 0x80;
	CLKPR = 0x00;

	init();
	//  DAC_Init();
	ADC_Init();

	PSC_Init(0x00, MAX_PWM);


	while(1) {
		if (Flag_IT_timer0) {	/* 0.5 mS */
			if (Counter_PE1 != 0) {
				Counter_PE1 --;
			} else {
				Toggle_PE1();
				Counter_PE1 = 250;
			}

			Flag_IT_timer0 = 0;

			// generates speed reference steps in the software
			Softcounter++;
			if (Softcounter ==  2500) {
				Omega_ref = -320 ;
			} //-128;

			if (Softcounter == 5000) {
				Omega_ref= -160 ;
			}

			if (Softcounter == 7500) {
				Omega_ref= 160 ;
			}

			if (Softcounter == 10000) {
				Omega_ref= 320 ;
				Softcounter = 0 ;
			}
		}

		if (Flag_IT_ADC) {
			// get the measured speed from the ADC
			Omega_meas = read_acquisition();

			// compute the stator frequency (PI controller)
			// Command = mc_control_speed_16b(Omega_ref,Omega_meas);
			// Command = Omega_ref ; // command with the generated steps
			// Command = (25 * (256 + 18 - Omega_meas))/8; // command with the 0-10V input
			// command with the on board pot
			Command = ((512 - Omega_meas) * 16) / 10;

			// direction management: extract sign and absolute value
			if (Command > 0 ) {
				direction = 0 ;
			} else {
				direction = 1 ;
			}
			OmegaTe = abs(Command);

			//direction = 0 ;

			if (OmegaTe > K_scal) {
			// -------------------- V/f law ----------------------
				amplitude = controlVF(OmegaTe);

				if (amplitude > MAX_AMPLITUDE) {
					 amplitude = MAX_AMPLITUDE;
			       	}

			// -------------- natural PWN algorithm --------------
				PWM0 = duty_cycle(theta1,amplitude);
				PWM1 = duty_cycle(theta2,amplitude);
				PWM2 = duty_cycle(theta3,amplitude);
			} else {/* null speed */
				PWM0 = 0;
				PWM1 = 0;
				PWM2 = 0;
			}

			// ----- load the PSCs with the new duty cycles -----
			if (direction==0) {
				PSC_Load(PWM0, PWM0+DeadTime,
					       	PWM1, PWM1+DeadTime,
					       	PWM2, PWM2+DeadTime);
			} else {
				PSC_Load(PWM0, PWM0+DeadTime,
					       	PWM2, PWM2+DeadTime,
					       	PWM1, PWM1+DeadTime);
			}

			// 3 integrators for the evolution of the angles
			theta1 = ((uint32_t)K_scal*theta1 + OmegaTe) / K_scal ;
			theta2 = theta1 + 160 ;
			theta3 = theta1 + 320 ;

			if (theta1>=MAX_THETAx4) {
				theta1 -= MAX_THETAx4;
				Toggle_PC7();
			}
			if (theta2>=MAX_THETAx4)
				theta2 -= MAX_THETAx4;
			if (theta3>=MAX_THETAx4)
				theta3 -= MAX_THETAx4;

			 Flag_IT_ADC=0;
		}

		/* test the overcurrent input */
		if (( PIFR0 & _BV(PEV0A)) !=0 ) {
			/* fault on overcurrent */
			Set_PC7();
			Set_PC3();
			Clear_PE1();
			while (1) ;
		}
  	}
}

// interrupt vector for the sampling period (Ts=1 ms)
ISR(TIMER0_COMPA_vect)
{
	Flag_IT_timer0 = 1;
	ADC_start_conv();
}

// interrupt vector for the ADC (end of conversion)
ISR(ADC_vect)
{
	Flag_IT_ADC = 1;
}
