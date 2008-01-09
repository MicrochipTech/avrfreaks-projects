
/* ---- Standard header files --------------------------------------------- */   

#include <avr/io.h>




/**********************************************************************
 *	Calculates the best fitting UBRR based on measuring the duration
 *  of a bit coming out of the UART.
 *
 *	NB:  While it gives the best match, the timing may still not
 *  be close enough for reliable communication.  Usually no more
 *  than a 2% error can be tolerated.
 *
 *  This calculates the "double speed" UBRR.  (Use U2X)
 *
 *  Send a "U" as the synch character.
 *
 *  Returns the value of the UBRR, or zero if there is an error
 *
 **********************************************************************/

 static uint16_t autobaud(void)
{
#define RXD0_INPUT (PINE)	//valid for an M128
#define RXD0_BIT	(PE0)
	
	uint16_t a;	
	uint16_t b;

	uint16_t OuterLoop = 400;//322;
	uint16_t InnerLoop = 0;


	//since we could be starting in the middle of a character coming from the host,
	//with the pin high or low, we need to wait for a full cycle

	//UART has reverse logic:
	//RXD0_BIT Set == UART zero
	//RXD0_BIT Clear == UART "1"



	//at idle, the RX bit is SET: check for it to clear with a time-out
	//give it about 10 seconds: at about 25ms per inner loop?
	do
	{
		while ((--InnerLoop) && (bit_is_set(RXD0_INPUT, RXD0_BIT))); 
		
	} while ((--OuterLoop) && (bit_is_set(RXD0_INPUT, RXD0_BIT))); 
	
	if (!OuterLoop)	//if the loop timed out, then return an ERROR (zero)
	{
		return (0);
	}
		

	TCNT1 = 0;		//clear the count
	loop_until_bit_is_set(RXD0_INPUT, RXD0_BIT);	//UART bit might start clear
	loop_until_bit_is_clear(RXD0_INPUT, RXD0_BIT);	//bit set == UART ZERO

	// bit just went low (== UART "1")
	TCCR1B = 1;		//start timer at 1:1 prescale
	loop_until_bit_is_set(RXD0_INPUT, RXD0_BIT);
	TCCR1B = 0;		//stop the timer
	a = TCNT1;		

	TCNT1 = 0;		//clear the count
	loop_until_bit_is_clear(RXD0_INPUT, RXD0_BIT);

	// bit just went low ("1")
	TCCR1B = 1;		//start timer at 1:1 prescale
	loop_until_bit_is_set(RXD0_INPUT, RXD0_BIT);
	TCCR1B = 0;		//stop the timer

	b = TCNT1;		

	//take the shortest as the single bit time
	if (b<a)
	{
		a=b;	//now a is the smaller value
	}



	//At least 5 bits of a 10 bit frame have been received.
	//Now wait for the rest of the bits to pass.  Give it
	//8 bit times just to be sure

	//set the timer to 1/8 the normal clock
	//set the counter to 1 bit width below the top
	//start the timer and wait for it to overflow
	
	TCNT1 = (0xFFFF - a); 	//technically you need to add 1 more  //+ 1;
	TIFR &= (1<<TOV1);		//clear the timer 1 overflow flag
	TCCR1B = 2;		//start timer at 1:8 prescale, "Normal" Mode
	while (bit_is_clear(TIFR, TOV1));	//wait for the overflow flag to trigger
	TCCR1B = 0;		//stop the timer




	//check the last bit of the shift, If set, it WOULD need rounding up
	//but since we need to decrement the value to get the UBRR, then
	//if the bit is set, no decrement is necessary

	if (a & (1<<2))
	{
		return (a>>3);	//round down effect of shift handles the decrement
	}
	else
	{
		return ((a>>3)-1);	//no rounding effect, so needs a decrement
	}

	//****************************************************************
	// if you want to use regular speed (not U2X) then use this code:
	//if (a & (1<<3))
	//{
	//	return (a>>4);	//round down effect of shift handles the decrement
	//}
	//else
	//{
	//	return ((a>>4)-1);	//no rounding effect, so needs a decrement
	//}
	//*****************************************************************

}

