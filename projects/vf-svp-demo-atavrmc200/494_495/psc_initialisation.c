/**
* @file psc_initialisation.c
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

#include <avr/io.h>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
void PSC_Init(void)
{
	PSOC0 = (1<<POEN0B) | (1<<POEN0A) ;  /* connection I/O aux PSC waveform generator */
	PSOC1 = (1<<POEN1B) | (1<<POEN1A) ;
	PSOC2 = (1<<POEN2B) | (1<<POEN2A) ;		

 /*** Four Ramp Mode */
        PCNF0 = (1<<PMODE01) | (1<<PCLKSEL0) ;
        PCNF1 = (1<<PMODE11) | (1<<PCLKSEL1) ; /* fast clock input used */
        PCNF2 = (1<<PMODE21) | (1<<PCLKSEL2) ;

  /* DT0 = DT1 = 0 */
  /* OT0 = OT1 = 0x7FF */
  /* OT0 : OCRnRA
     DT0 : OCRnRB           NE PAS OUBLIER DE LANCER LA PLL !!!!!!!
     OT1 : OCRnSA
     DT1 : OCRnSB       */

  OCR0RB = 0x0000 ;
  OCR0SB = 0x0000 ;
  OCR1RB = 0x0000 ;
  OCR1SB = 0x0000 ;
  OCR2RB = 0x0000 ;
  OCR2SB = 0x0000 ;

  OCR0RA = 0x07FF ;
  OCR0SA = 0x07FF ;
  OCR1RA = 0x07FF ;
  OCR1SA = 0x07FF ;
  OCR2RA = 0x07FF ;
  OCR2SA = 0x07FF ;


  /* no divider on PSC input clock => f_PSC = 64 MHz                                             */
  /* le bit PRUN2 lance le fonctionnement des 3 PSC : synchro de PSC0 sur PSC2 et PSC1 sur PSC0  */
  /* PCCYCi = 1 : completes the entire waveform before halt operation                            */
        PCTL0 = (1<<PARUN0) | (1<<PCCYC0) ;
        PCTL1 = (1<<PARUN1) | (1<<PCCYC1) ;
        PCTL2 = (1<<PCCYC2) | (1<<PRUN2) ;				
}

void PSC0_Init (unsigned int dt0, unsigned int ot0, unsigned int dt1, unsigned int ot1)
{
   PSOC0 = (1<<POEN0B) | (1<<POEN0A) ;        /* connection I/O aux PSC waveform generator */
   OCR0SAH = HIGH(dt0);
   OCR0SAL = LOW(dt0);
   OCR0RAH = HIGH(ot0);
   OCR0RAL = LOW(ot0);
   OCR0SBH = HIGH(dt1);
   OCR0SBL = LOW(dt1);
   OCR0RBH = HIGH(ot1);
   OCR0RBL = LOW(ot1);
   //PCNF0 = (1<<PMODE01) | (1<<PCLKSEL0) ;
   PCNF0 = (1<<PMODE01) | (1<<PMODE00) | (1<<PCLKSEL0) ;  /* fast clock input used */
   PFRC0A = 0;
   PFRC0B = 0;
   PCTL0 = (1<<PARUN0) | (1<<PCCYC0);
}

void PSC1_Init (unsigned int dt0, unsigned int ot0, unsigned int dt1, unsigned int ot1)
{
   PSOC1 = (1<<POEN1B) | (1<<POEN1A) ;        /* connection I/O aux PSC waveform generator */
   OCR1SAH = HIGH(dt0);
   OCR1SAL = LOW(dt0);
   OCR1RAH = HIGH(ot0);
   OCR1RAL = LOW(ot0);
   OCR1SBH = HIGH(dt1);
   OCR1SBL = LOW(dt1);
   OCR1RBH = HIGH(ot1);
   OCR1RBL =  LOW(ot1);
   //PCNF1 = (1<<PMODE11) | (1<<PCLKSEL1);
   PCNF1 = (1<<PMODE11) | (1<<PMODE10) | (1<<PCLKSEL1);     /* fast clock input used */
   PFRC1A = 0;
   PFRC1B = 0;
   PCTL1 = (1<<PARUN1) | (1<<PCCYC1);
}

void PSC2_Init (unsigned int dt0, unsigned int ot0, unsigned int dt1, unsigned int ot1)
{
   PSOC2 = (1<<POEN2B) | (1<<POEN2A) ;        /* connection I/O aux PSC waveform generator */
   OCR2SAH = HIGH(dt0);
   OCR2SAL = LOW(dt0);
   OCR2RAH = HIGH(ot0);
   OCR2RAL = LOW(ot0);
   OCR2SBH = HIGH(dt1);
   OCR2SBL = LOW(dt1);
   OCR2RBH = HIGH(ot1);
   OCR2RBL = LOW(ot1);
   //PCNF2 = (1<<PMODE21) | (1<<PCLKSEL2);
   PCNF2 = (1<<PMODE21) | (1<<PMODE20) | (1<<PCLKSEL2);     /* fast clock input used */
   PFRC2A = 0;
   PFRC2B = 0;
   PCTL2 = (1<<PCCYC2) | (1<<PRUN2) ;
}




/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
