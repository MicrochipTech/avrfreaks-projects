/***************************************************************************/
/* File        : Ads7816p.c                                                */
/* Description : A/D Converter driver                                      */
/* Author      : Stefano Favillini                                         */
/* Data        : 28/05/2008                                                */
/* Version     : 1.0                                                       */
/***************************************************************************/  


#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "ios_def.h"  
#include "ad_tlc4541.h"  


/*-- Macro and defines --*/

#define CS_DISABLE    PORTC |=  _BV(PC6);  
#define CS_ENABLE     PORTC &= ~_BV(PC6);


/*--- Clock ---*/
#define nop()             {__asm__ __volatile__ ("nop");}
#define W250NS             nop();         // equivalent to 500 ns, when XTAL is 4MHz
#define W500NS             nop(); nop()
#define CLK_H              PORTC |= _BV(PC0)
#define CLK_L              PORTC &= ~_BV(PC0)


#define CLK()  {CLK_H ;  W250NS; CLK_L; W250NS;}


/*--------------------------------------------------------------
*
*						External functions
*
*---------------------------------------------------------------*/
/***********************************************
* Function   : Ads_getsample
* Description: init the harware
* Parameters : 
* Rets       : 
* Note       : 
************************************************/

void Ads_init(void)
{      
  /*-- set pin for TW serial data --*/
  TWCR = 1;  
  
 /*--- setting Port C as adc interface ---*/
  DDRC  |=  _BV(PC0);  //output
  DDRC  &= ~_BV(PC1);  //input
  DDRC  |=  _BV(PC6);  //output: CS enable
  PORTC |=  _BV(PC1);  
  CS_DISABLE
 
}    
/***********************************************
* Function   : Ads_reset
* Description: device reset after power on
* Parameters : 
* Rets       : 
* Note       : 
************************************************/
void Ads_reset(void)
{
   BYTE i;
   
   CS_ENABLE;  
   
   for(i = 0; i<4; i++)
     CLK();
	 
   CS_DISABLE;
}
/***********************************************
* Function   : Ads_getsample
* Description: data acquisition from adc
* Parameters : 
* Rets       : adc data conversion
* Note       : 
************************************************/
WORD Ads_getsample(void)  
{
  WORD result;  
  BYTE i;
  
    
   cli();
       
  /*--- clock low ---*/
    CLK_L;
	
  /*-- set CS --*/
    CS_ENABLE;   
  
  /*-- start adc reading --*/
  for(i = 0, result = 0; i < 24; i++)
  {
   
    if(i<16)
	{
      result <<=1;
      result |= (PINC & 0x02);
	}
    CLK();
	
  }    
   
  /*--- disable CS ---*/
  CS_DISABLE;    
    
  
  sei();
  return (result);
 
}  
/*****************************************************
* Function   : Ads_getsamplemean
* Description: calculate samples average value
* Parameters : samples 
* Rets       : avarage 
* Note       : 
******************************************************/
WORD Ads_getsamplemean(WORD nsample)   
{      
  DWORD result = 0L; 
  DWORD max=0L, min=0x0000FFFF;
  WORD val, i; 
  
  for(i = 0; i < nsample; i++)       
  {
    val = Ads_getsample(); 
   /*-- get out max and min values --*/
    if(val>max)
      max = (DWORD)val;
    if(val<min)
      min = (DWORD)val;
    result += (DWORD)val; 
  }  
  result -= (max + min);    
  
 return (WORD)(result / ((DWORD)(nsample-2L)));   
}


 
/*--------------------------------------------------------------
*
*						internal functions
*
*---------------------------------------------------------------*/ 


