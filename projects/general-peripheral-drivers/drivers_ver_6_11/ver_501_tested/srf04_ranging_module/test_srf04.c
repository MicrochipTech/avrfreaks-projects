
/****************************************************************************
 Title  :   C  test file for the SRF04 FUNCTIONS library (test_srf04.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      20/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/

#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION       330        /* AVRGCC version for including the correct files  */
#endif

#if AVRGCC_VERSION == 330
#include <avr/io.h>
#else
#include <io.h>
#endif

#include "lcd_io.h"
#include "srf04.h"


#define AVERAGING_FLT_SAMPLES   8  /* each measurement is taking 67 ms and max samples is 8 */

void main(void)
{
unsigned int  range=0;
unsigned int  range_[AVERAGING_FLT_SAMPLES];
unsigned char x1=0, x2=0, matching_samples=0, matching_samples_pos=0;

/* LCD init */
lcd_init();
lcd_clrscr();
lcd_gotoxy(0,0); lcd_puts_P("RANGE (m) = ");

/* ALL SRF04 UNITS WILL BE INTIALIZED! */
srf04_init();


while(1)
    {

/*-----------------------------------------------------------------------------------------------------*/
/*                            MEASUREMENT TAKING AND DIGITAL FILTER                                    */
/*-----------------------------------------------------------------------------------------------------*/
       /* TAKE AVERAGING_FLT_SAMPLES NUMBER OF MEASUREMENTS */
       for(x1=0; x1<AVERAGING_FLT_SAMPLES; x1++)
         {
#if SRF04_UNITS_USED == 1
            range_[x1] = srf04_ping();
#elif SRF04_UNITS_USED > 1
            range_[x1] = srf04_ping(SRF04_0);
#endif
            
         }

       /* REJECTION FILTER */
       for(x1=0,matching_samples_pos=0; x1<AVERAGING_FLT_SAMPLES; x1++)
         {
            for(x2=0, matching_samples=0; x2<AVERAGING_FLT_SAMPLES; x2++)
              {
                 if( range_[x1]<=((range_[x2]*105)/100) && range_[x1]>=((range_[x2]*95)/100) ) 
                  {
                     matching_samples++;
                  }
              }
            if(matching_samples>=(AVERAGING_FLT_SAMPLES/2))
             { 
                matching_samples_pos |= (1<<x1);
             }
            
         }

       /* AVERAGING FILTER */
       for(x1=0, x2=0, range=0; x1<AVERAGING_FLT_SAMPLES; x1++)
         {     
             if(matching_samples_pos & (1<<x1)) { range+=range_[x1]; x2++; }
         }
       if(x2==0) { continue; }
       range=range/x2;
/*-----------------------------------------------------------------------------------------------------*/
/*                         END OF MEASUREMENT TAKING AND DIGITAL FILTER                                */
/*-----------------------------------------------------------------------------------------------------*/

       /* DISPLAY TO LCD */
       lcd_gotoxy(12,0); lcd_put_i(range,2,4);

       


    }

return;
}
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

       

