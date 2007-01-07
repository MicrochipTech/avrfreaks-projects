/*! \file vibrato.c
 *	This file implements a vibrato function.  The vibrato function delays 
 *  soundsamples.  The delay is not constant, but it is a periodic function.
 *  This function has a peak-to-peak value (the depth of the delay).  This depth
 *  determines the maximum possible delay.  The amount of delay in seconds
 *  varies between zero seconds and the depth.
 *  The vibrato function also has a fixed frequency.  This frequency determines
 *  the rate of change of the delay.
 *
 *	All the vibrator-functions that are here coded work using the same 
 *	principle.  First the incoming sample is saved to the buffer.  Then the
 *	function calculates which one of the samples out of the buffer must be sent
 *	to the DAC.  So here the value of the samples is not changed.  They are only
 *	output in a different order than they came in.
 */

#include "define.h"

/*! The DEPTH determines the maximum possible delay that can be attained by the
 *	delayfunctions.  As in the case of the reverb-function.  The maximum delay
 *	is here also limited by the amount of available SRAM.
 */
#define DEPTH	160

//! Struct that holds data needed by some vibrato-functions.
typedef struct{
	byte delay;//!<Current value of the delay
	byte wrPtr;//!<Position where incoming sample must be written
} strVibrato;

//!Buffer that holds all the soundsamples.
static signed char buffer[DEPTH];
//!The only instance of strVibrato
static strVibrato vibrato;


/*! Initialize vibrato structure.  
 *\return OK when success, else NOK
 */
ERROR initSawtoothVibrato(){
	/*An increasing writepointer is used. This means that the oldest data can 
	 *always be found at vibrato.wrptr+1.  The newest data can be found at
	 *vibrato.wrptr-1.
	 */
	vibrato.wrPtr=0;
	vibrato.delay=DEPTH-2; 
	
	return OK;
}//initReverb


/*!Sawtooth Vibrato stage block.  In this function the delay function is a 
 *sawtooth with a falling slope.  This means that the value of this function
 *decreases linearly with time until it reaches zero.  At that point it is set
 *to its maximum.
 *
 *\param newSample	New sample, signed format
 *\return 			Unsigned output of this block
 */
byte performSawtoothVibrato(signed char newSample){

	int index=0;

	//save incoming sample to our buffer
	buffer[vibrato.wrPtr]=newSample;
	
	//Calculate the index of the sample that must be returned.
	index=(int)vibrato.wrPtr+1-(int)vibrato.delay;
	
	//Make sure the index falls within our buffer
	while(index>=DEPTH)index-=DEPTH;
	while(index<0)index+=DEPTH;
	
	//update pointers
	if((++vibrato.wrPtr)>=DEPTH){
		vibrato.wrPtr=0;
	}
	if((--vibrato.delay)<1){
		vibrato.delay=DEPTH;
	}
	
	//return result (adding 128 = conversion to unsigned)
	return buffer[index]+128;
}//performSawtoothVibrato


/*!Square wave Vibrato stage block.  In this function the delay function is a 
 *square wave with 50% duty cycle.  In the first half of the square wave, the 
 *most recent sample in the buffer is returned.  In the second half, the oldest 
 *sample in the buffer is returned.
 *
 *The disadvantage of this soundeffect is that it depends a lot on the sampling
 *frequency.  If you keep the amplitude of the delay constant in time, but 
 *change the sampling frequency, then the effect is totally different.
 *My experience shows that it gives the strongest effects at a sampling
 *frequency of 8kHz.
 *
 *\param newSample	New sample, signed format
 *\return 			Unsigned output of this block
 */
byte performSquareVibrato(signed char newSample){
	const int LIMIT=40;
	static int counter=0;
	int index;
	
	//save incoming sample to our buffer
	buffer[vibrato.wrPtr]=newSample;
	
	//update frequency counter of the squarewave
	if(counter++<LIMIT){
		//most recent sample in the buffer
		index=(int)vibrato.wrPtr;
	}else{
		//oldest sample in the buffer
		index=(int)vibrato.wrPtr+1;
		if(counter>=2*LIMIT-1)counter=0;
	}
	
	//Make sure the index falls within our buffer
	while(index>=DEPTH)index-=DEPTH;
	while(index<0)index+=DEPTH;
	
	//update write pointer
	if((++vibrato.wrPtr)>=DEPTH){
		vibrato.wrPtr=0;
	}
	
	//return result (adding 128 = conversion to unsigned)
	return buffer[index]+128;
}//performSquareVibrato


/*! Sinewave vibrato stage block.  The delay function is a sinewave with an
 *amplitude of 64 and an offset of 128.  So the value of the delay changes
 *between zero and 128.  We need the offset, because our delay can never be
 *negative.  If the index was negative, this would mean that it points to a
 *soundsample of the future.
 *\param 	newSample	a soundsample from the ADC, 8bit-signed format
 *\return	an unsigned sample that can be sent to the ADC.
 */
byte performSinewaveVibrato(signed char newSample){

	int index=0;
	static byte prescaler=0;
	const byte PRESCALE=150;
	
	//save incoming sample to our buffer
	buffer[vibrato.wrPtr]=newSample;
	
	//Calculate the index of the sample that must be returned.
	index=(int)vibrato.wrPtr+1-(int)vibrato.delay;
	
	//Make sure the index falls within our buffer
	while(index>=DEPTH)index-=DEPTH;
	while(index<0)index+=DEPTH;
	
	//update pointers
	if((++vibrato.wrPtr)>=DEPTH){
		vibrato.wrPtr=0;
	}

	/*Calculate new delay index.
	 *Every time this function is called, the prescaler increases.  When it
	 *reaches the threshold, a new delay value is calculated.
	 *This prescaler was introduced because with the generateSin-function it's
	 *not possible to generate sinewaves with a very low frequency.
	 *The advantage of using a prescaler is that it works very fast.  It's no
	 *longer needed to call the slow generateSin()-function every time the
	 *performSinewaveVibrato(...) function is called.  The disadvantage is that
	 *we the sinewave is now more of a staircase sinewave.
	 */
	if(++prescaler>=PRESCALE){
		prescaler=0;
		vibrato.delay=128-generateSin()/2;
	}
	
	//return result (adding 128 = conversion to unsigned)
	return buffer[index]+128;
}//performSinewaveVibrato