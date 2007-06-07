
/****************************************************************************
 Title  :   C  test file for the SRF08 FUNCTIONS library (test_srf08.c)
 Author:    Chris efstathiou hendrix@vivodinet.gr
 Date:      7/Apr/2007
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/

#if !defined(AVRGCC_VERSION)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) > 303
#define AVRGCC_VERSION      304        /* AVRGCC version for including the correct files  */
#elif ((__GNUC__ * 100) + __GNUC_MINOR__) >= 300  &&  ((__GNUC__ * 100) + __GNUC_MINOR__) <= 303
#define AVRGCC_VERSION      303        /* AVRGCC version for including the correct files  */
#else 
#define AVRGCC_VERSION   300    
#endif
#endif


#if AVRGCC_VERSION >= 303
#include <avr/io.h>
#else
#include <io.h>
#endif

#include "lcd_io.h"
#include "srf08.h"
#include "i2c.h"

#define AVERAGING_FILTER_SAMPLES   3




void main(void)
{
unsigned int range=0;
unsigned char counter1=0, gain=0;

/*
    To change the SRF08 i2c address use the below given algorithm.
    Delay (1E6 microseconds=1 second) is there to ensure that SRF08 is out of reset and operational.
    Make sure that the SRF08 unit that you wish to change the address of,
    is selected before issuing the srf08_change_i2c_address(SRF08_UNIT_X) command.
    If you are not sure about the address then make sure that the only device connected to the I2C bus
    is a single SRF08 unit and use the "srf08_select_unit(SRF08_BROADCAST_ADDRESS)" command
    instead of the "srf08_select_unit(SRF08_UNIT_X)" command.
    The "srf08_select_unit(SRF08_BROADCAST_ADDRESS)" transmits to all SRF08 units. 
    After the address change the SRF08_UNIT_X is selected and active.
    Also remember to disable the algorithm afterwards (by commenting it or erasing it),
    recompile and flash the device again otherwise in every reset the address of the
    particular SRF08 unit will be changed (except if you want for this to happen!).

#define START_UP_DELAY  ( (1000000*(F_CPU/60000))/100 )
unsigned long delay=0;
delay=START_UP_DELAY;
while(delay) delay--; 
srf08_select_unit(SRF08_UNIT_0);
srf08_change_i2c_address(SRF08_UNIT_1); 
delay=START_UP_DELAY;
while(delay) delay--; 
*/


/*
   By default SRF_UNIT_0 is selected so the below command is not needed actually.
   All commands after the "srf08_select_unit(SRF08_UNIT_0);" command refer to that unit only! 
*/

srf08_select_unit(SRF08_UNIT_0);   
gain=SRF08_MAX_GAIN;
srf08_set_gain(gain);
srf08_set_range(SRF08_MAX_RANGE);   /* Set range to 11008 mm */ 

lcd_clrscr();
lcd_gotoxy(0,0); lcd_puts_P("RANGE (E1)  = ");
lcd_gotoxy(0,1); lcd_puts_P("RANGE (E2)  = ");
lcd_gotoxy(0,2); lcd_puts_P("RANGE (E5)  = ");
lcd_gotoxy(0,3); lcd_puts_P("LIGHT sensor= ");

while(1)
    {
       
       /* AVERAGING FILTER */
       for(counter1=0,range=0; counter1<AVERAGING_FILTER_SAMPLES; counter1++)
         {
            range+=srf08_ping(SRF08_CENTIMETERS);
         } 
       range /= AVERAGING_FILTER_SAMPLES;

       /* AUTOMATIC GAIN CONTROL (SLOW ATTACK, FAST RELEASE) */
       if(srf08_read_register(SRF08_ECHO_5) > 0 )
        { 
           if(gain>=5) { srf08_set_gain(gain-=5); } else { srf08_set_gain(gain=0); }  
        } 
       else if(srf08_read_register(SRF08_ECHO_2)<=0 && gain<31) { srf08_set_gain(++gain); } 

       /* DISPLAY TO LCD */
       lcd_gotoxy(14,0); lcd_puti(range,2);
       lcd_gotoxy(14,1); lcd_put_i(srf08_read_register(SRF08_ECHO_2), 2,4);
       lcd_gotoxy(14,2); lcd_put_i(srf08_read_register(SRF08_ECHO_5), 2,4);
       lcd_gotoxy(14,3); lcd_put_i(srf08_read_register(SRF08_LIGHT),2,4);


    }

return;
}
/*#################################################################################################*/
       

