/**
* @file psc_initialisation2.c
*
* Copyright (c) 2004 Atmel.
*
* @brief Ce fichier permet de
*
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.3)
* @date 2006/02/02 16:30:54 (created on 06/04/2004)
* @author raubree (created by Emmanuel David)

*****************************************************************************/

#include <avr/io.h>
#include "lib_mcu/pll/pll_drv.h"

/**********************************************************************************/
/*                   PSC Initialization                                           */
/***********************************************************************************/
/*!
 * This function initializes the PSCs :
   *    - centered aligned mode,
   *    - fast clock input (the PWM clock (64 MHz) comes from the PLL)
*/
/**********************************************************************************/
void PSC_Init(unsigned int ot0,  unsigned int ot1)
{
   Start_pll_64_mega();   // start the PLL at 64 MHz
   Wait_pll_ready();

   OCR0RA = ot0;
   OCR0RB = ot1;
   PCNF0 = (1<<PMODE01) | (1<<PMODE00) | (1<<PCLKSEL0) ;  /* fast clock input used */
   PFRC0A = (1<<PFLTE0A)|(0<<PRFM0A3)|(1<<PRFM0A2)|(1<<PRFM0A1)|(1<<PRFM0A0);
//   PFRC0A = 0;
   PFRC0B = 0;
   PCTL0 = (1<<PARUN0) | (1<<PCCYC0);

   OCR1RA = ot0;
   OCR1RB = ot1;
   PCNF1 = (1<<PMODE11) | (1<<PMODE10) | (1<<PCLKSEL1);   /* fast clock input used */
   PFRC1A = 0;
   PFRC1B = 0;
   PCTL1 = (1<<PARUN1) | (1<<PCCYC1);

   OCR2RA = ot0;
   OCR2RB = ot1;
   PCNF2 = (1<<PMODE21) | (1<<PMODE20) | (1<<PCLKSEL2);   /* fast clock input used */
   PFRC2A = 0;
   PFRC2B = 0;
   PCTL2 = (1<<PCCYC2) | (1<<PRUN2) ;

   // connect the PSC waveform generator outputs to the port outputs
   PSOC0 = (1<<POEN0B) | (1<<POEN0A) ;
   PSOC1 = (1<<POEN1B) | (1<<POEN1A) ;
   PSOC2 = (1<<POEN2B) | (1<<POEN2A) ;

}

/**********************************************************************************/
/*                                  PSC0                                          */
/**********************************************************************************/
/**
* @brief PSC0_Load: load the OCR0SA et OCR0SB registers with new values
*/
/**********************************************************************************/
void PSC0_Load (unsigned int dt0,  unsigned int dt1)
{
   PCNF0 = (1<<PLOCK0)|(1<<PMODE01) | (1<<PMODE00) | (1<<PCLKSEL0) ;
   OCR0SA = dt0;
   OCR0SB = dt1;
   PCNF0 = (1<<PMODE01) | (1<<PMODE00) | (1<<PCLKSEL0) ;
}

/**********************************************************************************/
/*                                  PSC1                                          */
/**********************************************************************************/
/**
* @brief PSC1_Load: load the OCR1SA et OCR1SB registers with new values
*/
/**********************************************************************************/
void PSC1_Load (unsigned int dt0, unsigned int dt1)
{
   PCNF1 = (1<<PLOCK1)|(1<<PMODE11) | (1<<PMODE10) | (1<<PCLKSEL1);
   OCR1SA = dt0;
   OCR1SB = dt1;
   PCNF1 = (1<<PMODE11) | (1<<PMODE10) | (1<<PCLKSEL1);
}

/**********************************************************************************/
/*                                  PSC2                                          */
/**********************************************************************************/
/**
* @brief PSC2_Load: load the OCR2SA et OCR2SB registers with new values
*/
/**********************************************************************************/
void PSC2_Load (unsigned int dt0, unsigned int dt1)
{
   PCNF2 = (1<<PLOCK2)|(1<<PMODE21) | (1<<PMODE20) | (1<<PCLKSEL2);
   OCR2SA = dt0;
   OCR2SB = dt1;
   PCNF2 = (1<<PMODE21) | (1<<PMODE20) | (1<<PCLKSEL2);
}


/**********************************************************************************/
/**********************************************************************************/
/*                                  All PSC                                       */
/**********************************************************************************/
/**
* @brief PSC_Load: load the OCR0SA et OCR0SB registers with new values
*/
/**********************************************************************************/
void PSC_Load (unsigned int dt0a,  unsigned int dt1a,
               unsigned int dt0b,  unsigned int dt1b,
               unsigned int dt0c,  unsigned int dt1c)
{
   PCNF0 = (1<<PLOCK0)|(1<<PMODE01) | (1<<PMODE00) | (1<<PCLKSEL0);
   PCNF1 = (1<<PLOCK1)|(1<<PMODE11) | (1<<PMODE10) | (1<<PCLKSEL1);
   PCNF2 = (1<<PLOCK2)|(1<<PMODE21) | (1<<PMODE20) | (1<<PCLKSEL2);

   OCR0SA = dt0a;
   OCR0SB = dt1a;

   OCR1SA = dt0b;
   OCR1SB = dt1b;

   OCR2SA = dt0c;
   OCR2SB = dt1c;

   PCNF0 = (1<<PMODE01) | (1<<PMODE00) | (1<<PCLKSEL0);
   PCNF1 = (1<<PMODE11) | (1<<PMODE10) | (1<<PCLKSEL1);
   PCNF2 = (1<<PMODE21) | (1<<PMODE20) | (1<<PCLKSEL2);
}


