//=============================================================================
// Internal ADC interface (used in polled mode).
//
// $Id: in_adc.c,v 1.3.2.3 2006/08/10 16:04:29 SOY Exp $
//=============================================================================

#include "common.h"

#if IN_ADC

// For 4-channel controllers we provide some fixed values for unused channels
//#define IN_ADC_USE_ONLY_4_CHANNELS     0
//#define IN_ADC_USE_CH5_VALUE           1000
//#define IN_ADC_USE_CH6_VALUE           1350
//#define IN_ADC_USE_CH7_VALUE           1650
//#define IN_ADC_USE_CH8_VALUE           2000

//=============================================================================
// Global variables

// Channel data variables
int  channelData[MAX_CHANNELS]; // 16 bit per channel value in uS (732-2268)
char newDataFound;              // new data packet found flag

//-----------------------------------------------------------------------------
//
// Initialize ADC interface
//
void inDecoderInit(void)
{
    // configure ADC
    REG_ADMUX  = (0<<REFS1)|(1<<REFS0);                         // AREF=AVCC
    REG_ADCSR  = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);    // lowest frequency
}

//
// Poll ADC interface for input data
//
void inDecoderPoll(void)
{
    int i;
    for (i = 0; i < MAX_CHANNELS; i++)
    {
        REG_ADMUX &= ~0x1f;             // ADMUX mask
        REG_ADMUX |= i;                 // select ADC channel
        REG_ADCSR |= (1<<ADSC);         // start conversion
        while (REG_ADCSR & (1<<ADSC)) ; // wait for result (fast, not a problem for USB driver)
        int adc = ADC;                  // read the result
        int data = adc + (1500-1024/2); // convert to uS
        channelData[i] = data;          // save to data buffer
    }

#if IN_ADC_USE_ONLY_4_CHANNELS
    // some useful fixed values (e.g. for throttle or gyro sensitivity)
    channelData[4] = IN_ADC_USE_CH5_VALUE;
    channelData[5] = IN_ADC_USE_CH6_VALUE;
    channelData[6] = IN_ADC_USE_CH7_VALUE;
    channelData[7] = IN_ADC_USE_CH8_VALUE;
#endif

    // set new data flag
    newDataFound = MAX_CHANNELS;
}


void inDecoderStop(void){}
#endif  // IN_ADC
