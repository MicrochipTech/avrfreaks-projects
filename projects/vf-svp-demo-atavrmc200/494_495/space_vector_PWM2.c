/**
* @file space_vector_PWM.c
*
* Copyright (c) 2004 Atmel.
*
* @brief Ce fichier permet de
*
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.4)
* @date 2006/03/08 17:03:07 (created on 06/04/2004)
* @author raubree (created by Emmanuel David)

*****************************************************************************/

#include <avr/io.h>

#include "table_sin81.h"

#define K_scal          32
#define MAX_THETA	80
#define	MAX_PWM		2666
// 64MHz (PLL frequency) / 2666 / 2 = 12 kHz (PWM frequency)

uint16_t    tau1, tau2 ;
uint16_t    tau1bis, tau2bis;
uint8_t     theta1 , theta2 ;
uint16_t    Thetai = 0, Thetai_1 = 0;
extern    uint16_t  PWM0, PWM1, PWM2;
uint8_t     Sector_number = 1;

void SVPWM(uint16_t amp, uint16_t Omega) {

       Thetai_1 = ((uint32_t)K_scal*Thetai_1 + Omega) / K_scal ;

        if (Thetai_1 >= MAX_THETA) {
	      Thetai_1   -= MAX_THETA ;		
	      Sector_number = Sector_number + 1 ;
	      if (Sector_number > 6) {
	        Sector_number = 1 ;
	      }
	}
	       	
        theta2 = (unsigned char) Thetai_1 ;
	theta1 = (unsigned char) (MAX_THETA - Thetai_1) ;

        tau1 = ((uint32_t)amp * tab_sin[theta1]) / 128 ;  // 128
        tau2 = ((uint32_t)amp * tab_sin[theta2]) / 128 ;  // 128


        switch (Sector_number) {
	     	case 1 :
		      PWM0 = (unsigned short int) (MAX_PWM/2 - tau1 - tau2) ;
		      PWM1 = (unsigned short int) (MAX_PWM/2 + tau1 - tau2) ;
		      PWM2 = (unsigned short int) (MAX_PWM/2 + tau1 + tau2) ; break ;
		case 2 :
		      PWM0 = (unsigned short int) (MAX_PWM/2 - tau1 + tau2) ;
		      PWM1 = (unsigned short int) (MAX_PWM/2 - tau1 - tau2) ;
		      PWM2 = (unsigned short int) (MAX_PWM/2 + tau1 + tau2) ; break ;
		case 3 :
		      PWM0 = (unsigned short int) (MAX_PWM/2 + tau1 + tau2) ;
		      PWM1 = (unsigned short int) (MAX_PWM/2 - tau1 - tau2) ;
		      PWM2 = (unsigned short int) (MAX_PWM/2 + tau1 - tau2) ; break ;
		case 4 :
		      PWM0 = (unsigned short int) (MAX_PWM/2 + tau1 + tau2) ;
		      PWM1 = (unsigned short int) (MAX_PWM/2 - tau1 + tau2) ;
		      PWM2 = (unsigned short int) (MAX_PWM/2 - tau1 - tau2) ; break ;
		case 5 :
		      PWM0 = (unsigned short int) (MAX_PWM/2 + tau1 - tau2) ;
		      PWM1 = (unsigned short int) (MAX_PWM/2 + tau1 + tau2) ;
		      PWM2 = (unsigned short int) (MAX_PWM/2 - tau1 - tau2) ; break ;
		case 6 :
		      PWM0 = (unsigned short int) (MAX_PWM/2 - tau1 - tau2) ;
		      PWM1 = (unsigned short int) (MAX_PWM/2 + tau1 + tau2) ;
		      PWM2 = (unsigned short int) (MAX_PWM/2 - tau1 + tau2) ; break ;	
		default :
		      PWM0 = (unsigned short int) (MAX_PWM/2) ;
		      PWM1 = (unsigned short int) (MAX_PWM/2) ;
		      PWM2 = (unsigned short int) (MAX_PWM/2) ; break ;
      	}
}
