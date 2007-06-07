
/****************************************************************************
 Title  :   C Test program for the ADC FUNCTIONS library (test_adc.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      02/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/
/*
IMPORTANT:
It is my understanding that if you dont want to burn the AD Converter
never ever connect the ADC inputs to VCC. 
Also do not connect AREF or GND directly to the ADC inputs.
Use a resistor in series at least 10Kohm, otherwise you might burn the ADC as i did.
*/ 

#if !defined(AVRGCC_VERSION)

#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 400
#define AVRGCC_VERSION      400        /* AVRGCC version for including the correct files  */
#elif ((__GNUC__ * 100) + __GNUC_MINOR__) >= 303  &&  ((__GNUC__ * 100) + __GNUC_MINOR__) <400
#define AVRGCC_VERSION      303        /* AVRGCC version for including the correct files  */
#elif   #define AVRGCC_VERSION   200    
#endif

#endif /* #if !defined(AVRGCC_VERSION) */

#if AVRGCC_VERSION >= 400
#include "compatibility.h"
#endif

#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION    330        /* AVRGCC version for including the correct files  */
#endif

#if AVRGCC_VERSION >= 303
#include <avr/io.h>
#else
#include <io.h>
#endif

#include "lcd_io.h"
#include "adc.h"

/* Function prototypes */
void draw_adc_display(void);
void display_adc_readings(void);
void wait(unsigned long msec);

const unsigned int adc_1Vx10=2046;

/*######################################################################################################*/


void draw_adc_display(void)
{
  
  lcd_gotoxy(0,0);
  lcd_puts_P("CH0       ");
  lcd_puts_P("CH4");
  lcd_gotoxy(0,1);
  lcd_puts_P("CH1       ");
  lcd_puts_P("CH5");
  lcd_gotoxy(0,2);
  lcd_puts_P("CH2       ");
  lcd_puts_P("CH6");
  lcd_gotoxy(0,3);
  lcd_puts_P("CH3       ");
  lcd_puts_P("CH7"); 

return;
}
/*######################################################################################################*/

void display_adc_readings(void)
{
unsigned long adc_reading=0;   
  
    
  adc_reading=adc_get_channel(0); 
  adc_reading=(adc_reading*10000)/adc_1Vx10;
  lcd_gotoxy(4,0); lcd_puti(adc_reading,3);
  
  
  adc_reading=adc_get_channel(4); 
  adc_reading=(adc_reading*10000)/adc_1Vx10;
  lcd_gotoxy(15,0); lcd_puti(adc_reading,3);
  
  
  adc_reading=adc_get_channel(1); 
  adc_reading=(adc_reading*10000)/adc_1Vx10;
  lcd_gotoxy(4,1); lcd_puti(adc_reading,3);

  
  adc_reading=adc_get_channel(5); 
  adc_reading=(adc_reading*10000)/adc_1Vx10;
  lcd_gotoxy(15,1); lcd_puti(adc_reading,3);
  
  
  adc_reading=adc_get_channel(2); 
  adc_reading=(adc_reading*10000)/adc_1Vx10;
  lcd_gotoxy(4,2); lcd_puti(adc_reading,3);
  
  
  adc_reading=adc_get_channel(6); 
  adc_reading=(adc_reading*10000)/adc_1Vx10;
  lcd_gotoxy(15,2); lcd_puti(adc_reading,3);
 
  
  adc_reading=adc_get_channel(3); 
  adc_reading=(adc_reading*10000)/adc_1Vx10;
  lcd_gotoxy(4,3); lcd_puti(adc_reading,3);
  
 
  adc_reading=adc_get_channel(7); 
  adc_reading=(adc_reading*10000)/adc_1Vx10;
  lcd_gotoxy(15,3); lcd_puti(adc_reading,3);

return;
}
/*######################################################################################################*/

void main(void)
{

  lcd_init();
  draw_adc_display(); 

  while(1)
      { 
         
/* Another method for ADC_INT_DIS and ADC_INT_EN_SLEEP mode is shown below */
/* 
         unsigned int adc_result=0;
         
         adc_init(ADC_INT_DIS, 200, 0); 
         adc_result=adc_convert(0);
*/

/* After a adc_init() command using interrupt, at least one conversion will take place. */
/* If you use ADC_INT_DIS mode no conversion will be initiated */
#if  ADC_NO_INT_MODE_NEEDED == 1 
         /* Dont use ADC interrupt */
         adc_init(ADC_INT_DIS, 200, 0); 
         adc_convert(0);
#endif
#if  ADC_SLEEP_MODE_NEEDED == 1 
         /* Start conversion with sleep. for subsequent conversions use the adc_convert(x) function */         
         adc_init(ADC_INT_EN_SLEEP, 200, 1); 
         adc_convert(2);
#endif
#if  ADC_INT_MODE_NEEDED == 1 
         /* Start auto conversions in channel 3 using interrupt */
         adc_init(ADC_INT_EN, 200, 3); 
         /* switch to channel 4. The switch will be done after one complete conversion at channel 3 */
         /* the function will return after at least one conversion is perfomed at channel 4 */
         adc_select_channel(4);           
#endif
#if  ADC_SCAN_MODE_NEEDED == 1      
         /* Scan channels 5,6,7 */
         adc_init( ADC_INT_EN_SCAN, 200, (1<<ADC_CH5)|(1<<ADC_CH6)|(1<<ADC_CH7) ); 
#endif         
         display_adc_readings();               
      }
 
}
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/


