/****************************************************************************
 Title  :   C  include file for the ADC FUNCTIONS library (adc.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      03/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/
/*
     
IMPORTANT:
It is my understanding that if you dont want to burn the AD Converter
never ever connect the ADC inputs to VCC. 
Also do not connect AREF or VCC directly to the ADC inputs.
Use a resistor in series at least 1 Kohm, otherwise you might burn the ADC as i did.
This is particulary true when "ADC_NOISE_REDUCTION" == 1

When "ADC_NOISE_REDUCTION" is set to 1 adc noise is greatly reduced by allowing only the 
current adc channel pin to be an input. All other pins are set as outputs and at 0 state.
This way i am creating a ground shield near the active adc pin.
I REALLY DONT KNOW IF THE AVR PINS ARE OVERCURRENT PROTECTED IF YOU APPLY 5 VOLTS TO A GROUNDED PIN.
DURING TESTING IT SHOWS THAT THERE IS NO PROBLEM BUT I AM NOT SURE IF THIS APPLIES IN THE LONG RUN ALSO.
So always use a resistor in series at least 1 Kohm.
You can make a lowpass filter by using a resistor in series followed by a capacitor
connected directly to the adc pin and the AGND.
A good network would be a 8,2 Kohm resistor and 1 nf capacitor which makes a lowpass filter
with a 6db point at ~20 Khz (VOLTAGE). 
ALSO WHEN MEGA128 IS USED CHECK THE JTAG FUSE!
IF THE JTAG INTERFACE IS ENABLED CHANNELS 4,5,7 WILL MALFUNCTION! 
The same applies in other devices where the JTAG interface shares the adc pins.
*/ 

#ifndef ADC_H
#define ADC_H
/*##############################################################################################*/
/*                             START OF CONFIGURATION BLOCK                                     */
/*##############################################################################################*/

#ifndef  F_CPU
#define  F_CPU                   3686400
#endif
#define  ADC_NOISE_REDUCTION     0          /* 1=Software adc noide reduction on */
#define  ADC_DYNAMIC_CLOCK       0          /* 0 = No change in adc clock during run time. */
#define  ADC_CLOCK_KHZ           200        /* ADC clock in Khz, Valid only if ADC_DYNAMIC_CLOCK = 0 */

/* WAYS TO REDUCE CODE SIZE BY NOT COMPILING UNWANTED FUNCTIONS OR CODE PORTIONS */
#define  ADC_INT_MODE_NEEDED     1          /* 0 = NO AUTORELOAD USING INTERRUPT  */ 
#define  ADC_SCAN_MODE_NEEDED    1          /* 0 = NO SCAN MODE USING INTERRUPT   */ 
#define  ADC_SLEEP_MODE_NEEDED   1          /* 0 = NO SLEEP MODE USING INTERRUPT  */ 
#define  ADC_NO_INT_MODE_NEEDED  1          /* 0 = NON INTERRUPT MODE DISABLED    */ 

/*##############################################################################################*/
/*                              END OF CONFIGURATION BLOCK                                      */
/*##############################################################################################*/

/* MODE commands used in adc_init()  */
#define ADC_INT_DIS       0
#define ADC_INT_EN        1
#define ADC_INT_EN_SCAN   2
#define ADC_INT_EN_SLEEP  3

#define ADC_CH0           0
#define ADC_CH1           1
#define ADC_CH2           2
#define ADC_CH3           3
#define ADC_CH4           4
#define ADC_CH5           5
#define ADC_CH6           6
#define ADC_CH7           7

/*
   The first parameter is adc mode, which is one of the predefined modes earlier in this file
   The second argument is the ADC clock in KHZ.         
   If you use fixed adc clock then the second parameter doesn't matter. it can be any value(say 0).
   The third argument is the channel to be used. Example: adc_init(ADC_INT_EN, 200, 0);  
   When ADC_INT_EN_SCAN mode is selected the third argument is the scan mask. 
   Example adc_init(ADC_INT_EN_SCAN, 100, (1<<ADC_CH0)|(1<<ADC_CH7)); means scan channels 0 & 7 
   SCAN MASK EXAMPLES:
   1) to scan channels 0,1,2,3 and 4 the mask would be Ob00011111=0X1F 
   2) To scan channels 0, 2 and 7 the mask would be 0b10000101=0X85
   The channels need not to be continuous in order.
   When ADC_INT_DIS mode is selected the third argument does not used and is set at 0.
   You can also use this mode to completely disable the ADC. By giving a adc_init(ADC_INT_DIS, x, 0)
   no conversion will be initiated and the ADC interrupt is disabled. 
   
   MODE DESCRIPTION: ADC_INT_DIS      =  ONE SHOT OPERATION NO INTERRUPT USE
                     ADC_INT_EN_SLEEP =  ONE SHOT OPERATION WITH INTERRUPT USE AND MCU PUT IN SLEEP
                     ADC_INT_EN       =  CONTINUOUS OPERATION WITH INTERRUPT USE
                     ADC_INT_EN_SCAN  =  CONTINUOUS MULTI CHANNEL SCANNING OPERATION WITH INTERRUPT USE

IMPORTANT: After 2 days of fault finding i decided to let all other bits except the first 3 in ADMUX
           intact so you can modify them as you please. The driver won't mess with those bits that
           set varius things like the reference source etc.
           Also no reinitialization will be performed until at least one conversion completes
           in the previus adc mode.
*/
extern void          adc_init(unsigned char mode, unsigned int adc_clock, unsigned char channel);

/* 
   You have to init the adc using the ADC_INT_DIS or ADC_INT_EN_SLEEP mode first in order to use
   the below function. ONE SHOT OPERATION!!!
*/
extern unsigned int  adc_convert(unsigned char channel);

/*
   If you are in ADC_INT_EN mode only then use the below function to set the adc channel 
   of the next conversion. 
*/ 
extern void          adc_select_channel(unsigned char channel);

/* 
   Get a required channel measurement result. The result will be always valid (correct).
   When in "ADC_INT_EN_SCAN" mode the result will be valid after and only after
   at least one complete channel scan. The function will wait untill that happens.
   When in "ADC_INT_EN" mode the result is valid after at least one measurement has been taken
   and if you change channel using the "adc_select_channel()" command, after at least one measurement 
   at the new set channel has been taken. The function will wait untill that happens.

 */
extern unsigned int  adc_get_channel(unsigned char channel);

#endif  /* ADC_H */
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

