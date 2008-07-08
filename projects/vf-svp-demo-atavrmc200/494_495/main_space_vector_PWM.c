/**
* @file main.c
*
* Copyright (c) 2004 Atmel.
*
* @brief This module provide services to show a simple program for  AT90PWM3 Only
* @version 1.0 (CVS revision : 1.7)
* @date 2006/03/08 17:02:35
* @author raubree
*****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdlib.h>

#include "config.h"

#define	MAX_PWM		2666
// 64MHz (PLL frequency) / 2666 / 2 = 12 kHz (PWM frequency)

#define MAX_THETA	80 // one sixth of the circle
#define K_scal          16 // used for the angle integrator


volatile uint8_t Flag_IT_timer0=0, Flag_IT_ADC=0 ;
uint16_t        Softcounter = 0 ;

int16_t           Omega_meas;
int16_t           Omega_ref = 100;
int16_t           Command = 0;

uint16_t        amplitude , OmegaTe = 64 ;
uint8_t         direction = 0 ;
uint16_t        PWM0, PWM1, PWM2;

#define      DeadTime  32     // 32 => temps mort = 0.5 µs
#define      MAX_AMPLITUDE  ((MAX_PWM / 2) - DeadTime)


void   init(void);
void   ADC_Init(void);
void   ADC_start_conv(void);
void   DAC_Init(void);
void   PSC_Init(unsigned int ot0,  unsigned int ot1);
void   PSC0_Load (unsigned int dt0, unsigned int dt1);
void   PSC1_Load (unsigned int dt0, unsigned int dt1);
void   PSC2_Load (unsigned int dt0, unsigned int dt1);
void PSC_Load (unsigned int dt0a,  unsigned int dt1a,
               unsigned int dt0b,  unsigned int dt1b,
               unsigned int dt0c,  unsigned int dt1c);
int16_t    mc_control_speed_16b(int16_t speed_ref , int16_t speed_measure);
uint16_t controlVF(uint16_t Omega);
void SVPWM(uint16_t amp, uint16_t Omega);
int16_t    read_acquisition(void) ;


void main(void)
{

  /* remove CKDIV8 fuse effect  */
  CLKPR = 0x80;
  CLKPR = 0x00;

  init();
//  DAC_Init(); /* do not init DAC if you want to use PC7 as /IO */
  ADC_Init();

  PSC_Init(0x00, MAX_PWM);

  while(1)
  {
      if (Flag_IT_timer0)
       {

        ADC_start_conv();
        Flag_IT_timer0=0;
        // generates speed reference steps in the software
/*        Softcounter += 1 ;
        if (Softcounter ==  2500) {
          Omega_ref = -400 ;
         } //-128; }
        if (Softcounter == 5000)
         {
           Omega_ref= -200 ;
         }
        if (Softcounter == 7500)
         {
           Omega_ref= 200 ;
         }
        if (Softcounter == 10000)
         {
           Omega_ref= 400 ;
           Softcounter = 0 ;
         }*/

       }
      if (Flag_IT_ADC)
       {

        // get the measured speed from the ADC
         Omega_meas = ((uint32_t)63 * Omega_meas + read_acquisition() )/ 64;
//         Omega_meas = read_acquisition();

         // compute the stator frequency (PI controller)
//         Command = mc_control_speed_16b(Omega_ref,Omega_meas); // for use in closed loop
//         Command = Omega_ref ; //  // command with the generated steps
         Command = ((512 - Omega_meas)*20) / 10; // command with the on board pot
//         Command = (25 * (256 + 18 - Omega_meas))/8; // command with a pot on the 0-10V input


         // direction management : extract sign and absolute value
         if (Command > (int16_t)(0) ) {
           direction = 0 ;
           OmegaTe = Command;
         }
          else {
            direction = 1 ;
            OmegaTe = (~Command) + 1;
         }


         // ------------------------ V/f law --------------------------
         amplitude = controlVF(OmegaTe);
         if (amplitude > MAX_AMPLITUDE) { amplitude = MAX_AMPLITUDE ; }


         // ------------ - space vector PWN algorithm -----------------
         SVPWM(amplitude, OmegaTe) ;

         // -------- load the PSCs with the new duty cycles -----------
         if (direction==0)
         {
            PSC_Load (PWM0, PWM0+DeadTime, PWM1, PWM1+DeadTime, PWM2, PWM2+DeadTime);
         }
         else
         {
            PSC_Load (PWM0, PWM0+DeadTime, PWM2, PWM2+DeadTime, PWM1, PWM1+DeadTime);
         }


    	Flag_IT_ADC=0;
      }   /* end of Flag_IT_ADC */
      /* test the overcurrent input */
      if (( PIFR0 & (1<<PEV0A)) !=0 )
      {
        /* fault on overcurrent */
        Set_PC7();
        Set_PC3();
        Clear_PE1();
        while (1) ; /* infinite loop */
      }

   }     /* end of while(1) */
}         /* end of main */

// interrupt vector for the sampling period (Ts=1 ms)
ISR(TIMER0_COMPA_vect)
{
	Flag_IT_timer0 = 1;
}

// interrupt vector for the ADC (end of conversion)
ISR(ADC_vect)
{
	Flag_IT_ADC = 1;
}

