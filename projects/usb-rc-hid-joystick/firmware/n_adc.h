//=============================================================================
// Internal ADC interface (used in polled mode).
//
// $Id: in_adc.h,v 1.3.2.2 2006/07/20 20:08:42 SOY Exp $
//=============================================================================

#ifndef __IN_ADC_H_INCLUDED__
#define __IN_ADC_H_INCLUDED__

#if IN_ADC

//=============================================================================
// Global variables

// Channel data variables
extern int  channelData[MAX_CHANNELS];  // 16 bit per channel value in uS (732-2268)
extern char newDataFound;               // new data packet found flag

//=============================================================================
// Function prototypes

extern void inDecoderInit(void);    // initialize ADC interface
extern void inDecoderPoll(void);    // poll ADC interface for input data
extern void inDecoderStop(void);     // nothing to stop for ADC interface

#endif  // IN_ADC

#endif  // __IN_ADC_H_INCLUDED__
