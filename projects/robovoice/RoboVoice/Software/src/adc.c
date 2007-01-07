/*! \file adc.c
 *
 *	Here the ADC is controlled.  The ADC is performs differential measurements
 *	using channel 0 and 1.  In this way, the DC-offset of the soundsamples can
 *	easily be eliminated.
 *
 *	An interrupt occurs every time an AD-conversion completes.  The IS-routine
 *	then performs some soundprocessing on the sample and sends it to the DAC.
 */

#include "define.h"

/*! Interrupt on AD-conversion completed.
 */
ISR(ADC_vect){
	byte result;
	extern volatile bool timer1_trigger;
	
	result=ADCH;
	
	if((result!=0x80) && (result!=0x7F)){
		#ifdef RINGMOD
			PORTD=mixer(fastGenerateSin(),result);
		#endif
		#ifdef SAWTOOTHVIBRATO
			PORTD=performSawtoothVibrato(result);
		#endif
		#ifdef SQUAREVIBRATO
			PORTD=performSquareVibrato(result);
		#endif
		#ifdef SINEWAVEVIBRATO
			PORTD=performSinewaveVibrato(result);
		#endif
		#ifdef REVERB
			PORTD=performReverb(result);
		#endif
		#ifdef NO_EFFECT
			PORTD=result+128;
		#endif
		//PORTD=(fastGenerateSin()/2)+128;
		//PORTD=(generateSin()/2)+128;
	}
	
	//reset trigger
	timer1_trigger=false;
}//ISR

/*! Initialize the AD-converter
 *  Differential measurement is performed with ADC0 as positive and ADC1 as
 *	negative input channel.  The result is left adjusted, we only need the 
 *	highest eight bits.  AVCC (5 volts) is used as a reference.
 *
 *	The ADC-complete-interrupt is enabled.
 */	
void init_ADC(){
	/*Reference = AVCC
	 *Left adjust result
	 *Differential ADC: ADC0 = +input, ADC1= -input 
	 */
	 ADMUX=(1<<REFS0)|(1<<ADLAR)|(1<<MUX3)|(1<<MUX0);

	/*Enable ADC
	 *enable interrupt on AD-conversion complete
	 *ADC-clock = Fosc/2
	 */
	ADCSRA=(1<<ADEN)|(1<<ADIE);

}//init_ADC