//=============================================================================
// Interrupt-driven PPM decoder.
//
// $Id: in_ppm.h,v 1.9.2.1 2006/07/20 20:08:42 SOY Exp $
//=============================================================================

#ifndef __IN_PPM_H_INCLUDED__
#define __IN_PPM_H_INCLUDED__



//=============================================================================
// Global variables

// Channel data variables
extern volatile int  channelData[MAX_CHANNELS];  // 16 bit per channel value in uS (732-2268)
extern char newDataFound;               // new data packet found flag

//=============================================================================
// Function prototypes

extern void inDecoderInit(void);    // initialize PPM decoder
inline void inDecoderPoll(void) {}  // polling is not used by PPM decoder
extern void inDecoderStop(void);    // stop PPM decoder interrupts


#endif  // __IN_PPM_H_INCLUDED__
