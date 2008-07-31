#include "servo.h"
#include "global.h"

void Init_servo_out(void) 
{ 
	uint8_t i;

	SetBitMask8(DDRD, 0b10110000);  				// make pin PD4 (OC1B pin), pin PD5 (OC1A pin),
								   				// and PD7 (both 4017 resets) outputs 
	servo_channel_A_updated = 0;
	servo_channel_B_updated = 0;
	
	SetBit8(PORTD, 7);							// Reset 4017 			
	
	for (i=0; i<SERVO_OUTPUT_CHANNELS; i++) 
		{servo_out[i] = 3000;}  				// Put everyone at neutral (1.5 ms).

	//for (; RCout_ptr <= RCout_tail_ptr; RCout_ptr++) 
	//	{*RCout_ptr = 3000;}  				 	// faster than above for(;;) but uses more memory
	//RCout_ptr = RCout;

	SetBitMask8(TCCR1A, 0b01010000);				// set OC1A and OC1B to toggle on compare match 
	ClearBitMask8(TCCR1A, 0b10100000);			

	
	ClearBit8(PORTD, 7);							// Reset 4017

	for(i=0;i<SERVO_OUTPUT_CHANNELS/2-1; i++)
	{
		SetBitMask8(TCCR1A, 0b0001100);			// force compare on match, OC1A becomes and starts out as high
		SetBitMask8(TCCR1A, 0b0001100);			// toggle OC1A and OC2B again
	}
	
	SetBitMask8(TIMSK, 0b00011000);				// enable A compare on match interrupt
												// and B compare on match interrupt;				
	
	SetBitMask8(TCCR1B, 0b00000010);				// set clock input for timer1 to be Fcpu/8 							
	ClearBitMask8(TCCR1B, 0b00000101);
}


ISR(SIG_OUTPUT_COMPARE1A) 
{ 
	static uint16_t *servo_channel_A_ptr = servo_out;
	
	SetBit8(TCCR1A, 3);							// force compare on match.  This will toggle the OC1A pin.
												// since the pin was toggled to low when the OCR1A interrupt was set, this should set the pin to high to provide the next clock pulse on the 4017
	OCR1A += *servo_channel_A_ptr++;			// update the compare value for the next interrupt
	
	if(servo_channel_A_ptr > &servo_out[SERVO_OUTPUT_CHANNELS/2-1])
	{
		servo_channel_A_ptr -= SERVO_OUTPUT_CHANNELS/2;
		servo_channel_A_updated = 0;
	}
}


ISR(SIG_OUTPUT_COMPARE1B) 
{ 
	static uint16_t *servo_channel_B_ptr = &servo_out[SERVO_OUTPUT_CHANNELS/2];

	SetBit8(TCCR1A, 2);							// force compare on match.  This will toggle the OC1B pin.
												// since the pin was toggled to low when the OCR1B interrupt was set, this should set the pin to high to provide the next clock pulse on the 4017
	OCR1B += *servo_channel_B_ptr++;			// update the compare value for the next interrupt
	
	if(servo_channel_B_ptr > &servo_out[SERVO_OUTPUT_CHANNELS-1])
	{
		servo_channel_B_ptr = &servo_out[SERVO_OUTPUT_CHANNELS/2];
		servo_channel_B_updated = 1;
	}
}

