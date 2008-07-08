/**
* @file space_vector_PWM.c
*
* Copyright (c) 2004 Atmel.
*
* @brief Ce fichier permet de
*
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.1.1.1)
* @date 2005/09/21 15:08:53 (created on 06/04/2004)
* @author jberthy (created by Emmanuel David)

*****************************************************************************/

#include "config.h"
#include "inavr.h"
#include "table_sin81.h"


#define K_scal          16
#define MAX_THETA	80
#define	MAX_PWM		2666  // 0x0A6A = 2666 => fréquence PWM = 12 kHz


Uint16    tau1, tau2 ;
Uint16    tau1bis, tau2bis;
Uint8     theta1 , theta2 ;
Uint16    Thetai = 0, Thetai_1 = 0;
extern Uint16    PWM0, PWM1, PWM2;   // 254 => temps mort = 4 µs
Uint8     Sector_number = 1;


void SVPWM(Uint16 amplitude, Uint16 OmegaTe) {

       Thetai_1 = (K_scal*Thetai_1 + OmegaTe) / K_scal ;

        if (Thetai_1 >= MAX_THETA) {
	      Thetai_1   -= MAX_THETA ;		
	      Sector_number = Sector_number + 1 ;
	      if (Sector_number > 6) {
	        Sector_number = 1 ;
	      }
	}
	       	
        theta2 = (unsigned char) Thetai_1 ;
	theta1 = (unsigned char) (MAX_THETA - Thetai_1) ;

        tau1 = (amplitude * tab_sin[theta1]) / 128 ;  // 128
        tau2 = (amplitude * tab_sin[theta2]) / 128 ;  // 128


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
