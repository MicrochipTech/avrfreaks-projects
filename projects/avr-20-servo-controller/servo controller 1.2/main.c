//  This program controls 20 servos using two 4017 decade counters.
//  This was tested on a mega16 and mega32.  Remember to change AVR studio 
//  under configuration options if using a different avr.
//  Note that the two compile warnings are ok and do not mean anything is wrong,
//  the two variables are indeed used.  


//  PD5 -> CD4017 #1 clock (servos 0-9)
//  PD4 -> CD4017 #2 clock (servos 10-19)
//  PD7 -> CD4017 #1 and CD4017 #2 resets (resets are tied together and tied to PD7)


#define F_CPU 						16000000UL

#include "servo.h"
#include "global.h"

int main( void )
{
	Init_servo_out();				// initialize timer1 and program variables
	sei();

	while(1)
	{

		if(servo_channel_A_updated == 0)	// If all 10 servos were updated on the first 4017 and we
		{									// rapped around back to the first servo then it's time
											// to update servos 0 to 9.
			servo_channel_A_updated = 1;
											
			servo_out[0] = 3000;  			// set the first servo to neutral (1.5ms when
									    	// using a 16 MHz crystal).  Remember a servo update rate
											// is only about 50 Hz so only update the array
											// with a new value roughly every 20ms (when servo_channel_A_updated 
											// equals zero). No point in going any faster.
			// set any number of outputs 
			//servo_out[2] = 3000;
			//servo_out[3] = 3000;
			//servo_out[4] = 3000;
			//servo_out[5] = 3000;
			//servo_out[6] = 3000;
			//servo_out[7] = 3000;
			//servo_out[8] = 3000;
			//servo_out[9] = 3000;
			
		}
		
		if(servo_channel_B_updated == 0)	
		{								
			servo_channel_B_updated = 1;
			
			//set any number of outputs 
			servo_out[10] = 3000;
			//servo_out[11] = 3000;
			//servo_out[12] = 3000;
			//servo_out[13] = 3000;
			//servo_out[14] = 3000;
			//servo_out[15] = 3000;
			//servo_out[16] = 3000;
			//servo_out[17] = 3000;
			//servo_out[18] = 3000;
			//servo_out[19] = 3000;	
		}
	}
		
	return 0;
}

 


 

 
