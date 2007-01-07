/*! \file reverb.c
 *	This file implements a reverb function.  A reverb is like an echo function
 *	with an infinite number of echos.
 */

#include "define.h"

/*! This factor determines the depth of the reverb.  This depth is used to set
 *	the size of two buffers that hold soundsamples.  So keep in mind that the
 *	maximum of this factor is limited by the amount of available SRAM.
 */
#define M	100

/*!This struct groups some constants needed for the reverb-function.
 */
typedef struct{
	byte rdPtr;//!<Pointer to the position where must be read
	byte wrPtr;//!<Pointer to the position whe sample must be written.
} strReverb;

//!Buffer containing the incoming samples that must be processed.
static signed char bufferX[M];
//!Buffer containing the processed samples.
static signed char bufferY[M];
//!The only instance of strReverb.
static strReverb reverb;


/*! Initialize reverberation structure.  The user must pass the gain of the
 *reverberation stage in this function.  The gain must be smaller than one 
 *otherwise the reverberation block is unstable.  So the gain is always a 
 *fraction.  Here the denominator is fixed to 256.  The numerator can be chosen
 *by the user.
 *e.g.  If a gain of 0.9 is required, then the numerator = 0.9 * 256 = 230.
 *
 *Because overflow must be avoided, we split the numerator up in two factors:
 * 23 and 10 (because 10*23=230).  The multiplication of the sample with the
 *gain is performed in four phases:
 *	first multiply by the first numerator
 *  then divide by 16
 *  then multiply by second numerator
 *  at last divide again by 16
 *
 *
 */
static const byte gNum1=23;
//!Second numerator of the gain
static const byte gNum2=10;

/*!Reverberation stage block.  Pass a signed sample into this block and get an
 * unsigned sample back.
 *
 * Transfer function:		y[n] = x[n-M] + gain * y[n-M]
 *
 *\param newSample	New sample, signed format
 *\return 			Unsigned output of this block
 */
byte performReverb(int newSample){

	int dummy;
	dummy=bufferY[reverb.rdPtr];		//dummy = y[n-M]
	dummy*=gNum1;
	dummy/=16;
	dummy*=gNum2;
	dummy/=16;							//dummy = gain * y[n-M]
	dummy+=bufferX[reverb.rdPtr];		//dummy = gain * y[n-M] + x[n-M]
	
	//Limit dynamic range of signal
	if(dummy>127){
		dummy=127;
	}
	if(dummy<-128){
		dummy=-128;
	}
	
	bufferY[reverb.wrPtr]=dummy;		//save y[n]
	bufferX[reverb.wrPtr]=newSample;	//save x[n]
	
	//update pointers
	if((++reverb.wrPtr)>=M){
		reverb.wrPtr=0;
	}
	if((++reverb.rdPtr)>=M){
		reverb.rdPtr=0;
	}
	
	//return result (adding 128 = conversion to unsigned)
	return dummy+128;
}//performReverb