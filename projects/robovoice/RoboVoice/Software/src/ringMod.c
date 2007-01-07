/*! \file ringMod.c
 *	This file is an implementation of a ringmodulator.  In a ringmodulator, the
 *	incoming sound is mixed with a cosine-wave.
 *	The mixing function is simply a multiplication of the sound-samples with
 *	samples of the cosine-wave.
 *	The necessary cosine-wave is digitally generated.
 *
 *	The sine-wave generator generates a sine-wave and a cosine-wave at the same
 *	time:
 *
 *		s1[n] = alfa * sin[n * theta]
 *		s2[n] = beta * cos[n * theta]
 *
 *		s1[n+1] = (s1[n] + s2[n]) * cos(theta) + s2[n]
 *		s2[n+1] = (s1[n] + s2[n]) * cos(theta) - s1[n]
 *  (cos(theta) is precalculated and stored in RAM or ROM)
 *
 *	The fantastic thing about this generator is that it only needs two 
 *	additions, one subtraction and one multiplication!
 *
 *	This however limits the number of possible amplitudes alfa and beta.  Their 
 *	relationship is a follows:
 *		alfa = beta * cotg(theta/2)
 *
 *	The initial data for s1[n] and s2[n] are:
 *		s1[0]=0;
 *		s2[0]=beta;
 *  This is done in the initSineGen() function.  If you don't call this
 *	function, then the first period of the generateSin() will be all zeros.
 *
 *	So the amplitude of s2[n] is beta and alfa = beta * cotg(theta/2).
 *	If we choose theta small, then cotg(theta/2) becomes large, so alfa will
 *	be much larger than beta; possibly leading to overflow in s1[n] if beta is 
 *	too large.  On the other hand, if we choose beta small, then round-off 
 *	errors for s2[n] will lead to distortion in the generated sine- and cosine-
 *	waves.
 *
 *	For more info on Digital Sine-Cosine Generators, see Mitra, S.K., Digital
 *	Signal Processing, ISBN 0-07-232105-9.
 */
 
#include "define.h"

/*!Lookup table generated with Matlab-command: round(sin(2*pi*(0:(N-1))/N)*A)
 *where N=41 and A=127
 *A is the amplitude, it is chosen 127, because that is the largest 
 *possible amplitude that can be saved in one "signed char"-memorycell.
 *
 *The table is placed here instead of inside the fastGenerateSin-function,
 *because otherwise the complete buffer is copied to the stack each time we 
 *call this function.
 *
 *This buffer will be placed in the SRAM-memory.  There it occopies 43bytes of
 *the 512bytes.  Making the buffer "const" won't change that fact.  This is
 *because the AVR has a Harvard-architecture.  This means that data and program
 *memory are split.
 */
static const signed char buffer[]={
	 0,  19,  38,  56,  73,  88,  101, 112, 120, 125, 127, 126, 122,
	116, 107,  95,  81,  65,  47,   29,  10, -10, -29, -47, -65, -81,
	-95,-107,-116,-122,-126,-127,-125,-120,-112,-101, -88, -73, -56,
	-38, -19};


/*! For reasons of performance, we make the following variables constants. This
 *	and avoiding divisions, sped up the design by a factor of 6.4!
 *
 *The choice was made to make cos(theta) equal to 0.99.  In hardware this is
 *implemented using a fraction.  To work efficiently, the denominator of the 
 *fraction is fixed to 256.  We must find the numerator so that the fraction
 *most closely approximates 0.99.
 *
 *	numerator = 0.99 * 256 = 253.44 -> 253
 *
 *To limit the danger of overflow, or rounding errors, a two stage 
 *multiplication is done.  The numerator as well as the denominator are split 
 *into two factors.  For the denominator we take 256 = 16 * 16.  For the 
 *numerator 253 = cosT_Num1 * cosT_Num2.  It's best to choose these factors as 
 *close as possible to eachother.  Here the choice was made for 253 = 11 * 23.
 */
static const int cosT_Num1=11;
/*! The second part of the numerator.  
 *	The complete numerator = cosT_Num1 * cosT_Num2
 */
static const int cosT_Num2=23;
/*! We must also calculate the number of samples per period.  This is done by:
 *		N = 2 * pi / acos(cos(theta)) in radians
 *		  = 2 * pi / acos(253/256) in radians
 *		  = 41
 *	So it follows that when the sampling frequency is 32kHz,  The generated 
 *	frequency is 32kHz / 41 = 780Hz.
 *	
 */
static const byte N=41;
/*! The choice for beta is free, but keep in mind that the amplitudes of the
 *	related sine and cosine are determined by alfa = beta * cotg(theta/2).
 *	So watch out for overflow or rounding errors.
 *	Here alfa = beta * 7.02
 */
static const int beta=126;

/*!This struct saves the current status of the sinewave and the cosinewave.
 */ 
typedef struct{
	int s1;//!<Current value of the sinewave
	int s2;//!<Current value of the cosinewave
} RingMod;

//!The only instance of RingMod
static RingMod ring;

 
/*!Initialize the sine-wave generator.
 *\return OK when everything succeeded, else NOK
 */
ERROR initSineGen(){
	
	if(!beta)return NOK;
	
	ring.s1=0;
	ring.s2=beta;
	return OK;
}//initRingMod


/*!Generate a new sample of the sinewave everytime this function is called.  
 * This is an implementation of a digital sinewave converter.  After every
 * period the data is reset.  This prevents instability of the generator due to
 * rounding errors.
 * This function could be made faster by writing it in assembly.  This however
 * has the disadvantage that the code is no longer portable.
 *
 *\return	a sample of the sinewave
 */
int generateSin(){
	static byte n;
	int dummy, dummy1;
	
	if(++n>=N){
		ring.s1=0;
		ring.s2=beta;
		n=0;
	}else{
		dummy= ring.s1 + ring.s2;	// 	s1[n] + s2[n]
		dummy*=cosT_Num1;
		dummy/=16;
		dummy*=cosT_Num2;
		dummy/=16;					//	(s1[n] + s2[n]) * cos(theta)
		
		dummy1=dummy+ring.s2;
		ring.s2=dummy-ring.s1;	//s2[n+1] = (s1[n] + s2[n]) * cos(theta) - s1[n]
		ring.s1=dummy1;		//s1[n+1] =	(s1[n] + s2[n]) * cos(theta) + s2[n]
	}
	
	return ring.s2;
}//generateSin


/*!	Of course, if your not planning to change the sinewave-frequency during
 *	runtime, then it is faster to precalculate the values of the sinewave
 * 	and put them in a look-up-table.  
 *  You can reduce the size of the table by a factor of two if you choose the
 *	number of samples in the buffer even.  This is because:
 *		sin(alfa) = -sin(-alfa)
 *	You will need some extra logic to do this, so only do this for larger 
 *	tables.
 *  You can further reduce the size of the table by an extra factor of two if
 *	you make the size of the buffer divisable by four.
 *	This is because sin(180°-alfa) = sin(alfa)
 *
 *  In this implementation the reduction is not done, because the extra logic
 *	needed is larger than the datasize saved by leaving out half of the data.
 *
 *\return an integer containing the current value of the sinewave
 */	
int fastGenerateSin(){
	static byte counter=0;

	if(++counter>N-1)counter=0;
   
	return (int)buffer[counter];
}//fastGenerateSin


/*!Ringmodulator function, mixes the incoming soundsample with the sinewave.
 *\param sineWave	sample of the sinewave, generated by generateSin().
 *					amplitude = 127
 *\param sample		soundsample (ADCH of last AD-conversion)
 *\return			Soundsample, unsigned, can be sent to DAC.
 */
byte mixer(int sineWave, signed char sample){
	
	sineWave*= sample;
	sineWave/=256;
	
	//Conversion of 8-bit signed to 8-bit unsigned
	return sineWave+128;
}//mixer