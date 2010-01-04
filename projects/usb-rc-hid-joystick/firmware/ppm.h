//=============================================================================
// Interrupt-driven PPM decoder.
//
// $Id: in_ppm.h,v 1.9.2.1 2006/07/20 20:08:42 SOY Exp $
//=============================================================================

#ifndef __IN_PPM_H_INCLUDED__
#define __IN_PPM_H_INCLUDED__

#if IN_PPM

//=============================================================================
// Global variables

// Channel data variables
extern int  channelData[MAX_CHANNELS];  // 16 bit per channel value in uS (732-2268)
extern char newDataFound;               // new data packet found flag

//=============================================================================
// Function prototypes

extern void inDecoderInit(void);    // initialize PPM decoder
extern void inDecoderPoll(void);  // polling is not used by PPM decoder
extern void inDecoderStop(void);    // stop PPM decoder interrupts

#endif  // IN_PPM

#endif  // __IN_PPM_H_INCLUDED__
