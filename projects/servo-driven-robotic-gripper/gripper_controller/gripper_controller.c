#include <avr/io.h>
#include <avr\interrupt.h>
#define F_CPU 9600000
#include <util/delay.h>

#include "gripper_controller.h"

#define ARM 	PB0
#define HAND 	PB1
#define LED		PB2
#define Flow	PB3
#define Rate	PB4

#define ServoMax	168 // 2.2 ms / 13.02 US = 168
#define ServoMin	54	// .7 ms / 13.02 US = 54
#define PeriodMax	168 // 2.2 ms / 13.02 US = 168
#define PeriodMin	76	// 1 ms / 13.02 US = 76
#define SERVO_CLIP(x) (x<ServoMin ? ServoMin : (x>ServoMax ? ServoMax : x))
#define PERIOD_CLIP(x) (x<PeriodMin ? PeriodMin : (x>PeriodMax ? PeriodMax : x))
#define ten_ms	60	// 50 ms

// Global variables
unsigned char duty1, duty2;				 // Duty cycle for each servo
unsigned char pwm_duty1, pwm_duty2; // Duty counters for each servo
unsigned char Prescaler64;					 // 832us counter
unsigned char timer20ms, timer100ms; // 20ms, 100ms counter
unsigned char adc_flag;

void step_to(unsigned char value, unsigned char num);
  
int main (void)
{
  // Flow value
  unsigned char a_Dist, pwm_last;  		// vars to hold the current distance reading and the pwm_last one too
  unsigned char des_pwm1, des_pwm2;	// vars to hold our desired position for each servo
  unsigned char gripper_flag = 0;
  
  //MCUCR	= ( 1<< PUD) | (1 << SE ) | (1 << SM1) ;  	// PULLUP DISABLE, Sleep Enable Power-down
  // The CKDIV8 Fuse determines the initial value of the CLKPS bits. 
	CLKPR  = 0x80;
	CLKPR  = 0x00; 						// Set devide by 1 Hi Speed 9.6 MC
  
  DDRB = _BV(LED);					// LED is output
  
  Init_Servo();							// initialize servos and associated pins
  Init_ADC();								// Init A2D and associated pins
	
	duty1 = duty2 = ServoMin;	// set both servos to max left travel
	Prescaler64=timer20ms=timer100ms=adc_flag = 1;	// Init counter vars
	sei();										// enable interrupts 	
	
	_delay_loop_2(1000);			// wait a short time before starting
	
	PORTB |= _BV(LED); 				// turn on LED
	
	
//
// The main endless loop looks for a flay called adc_flag that it set every 20ms in the Timer0 ISR
// Otherwire, it checks the currently ordered position of the arm servo to know if we are close enough 
// to open the gripper and initiate a pickup
//	
	while (1) 								// infinite loop
	{
		PORTB ^= _BV(LED);			// XOR LEd (if it's on turn it off and vice versa)
		// Read our current distance measurement
		if(adc_flag == 1)
		{
			adc_flag = gripper_flag = 0; 				// reset our adc flag
			a_Dist = ADC_read(2); 							// 0 = B5, 1 = B2, 2 = B4, 3 = B3

			// Now that we have the distance, set the desired_pwm of the arm
			des_pwm1 = a_Dist * 2;							// scale the distance since our Vref = Vcc = 5V
			if(des_pwm1 > pwm_last)             // and the sensor only goes up to 2.4V approx
				duty1 = SERVO_CLIP(duty1 + (des_pwm1 - pwm_last) / 2); // move about have the requested distance
			else if(des_pwm1 < pwm_last)
				duty1 = SERVO_CLIP(duty1 - (pwm_last - des_pwm1) / 2);
			pwm_last = duty1;										// store the current requested distance for the next itteration
		}		
			
		if(gripper_flag != 1)
		{
			if(duty1 > 145)
			{
				duty2 = ServoMin + 70;
				gripper_flag = 1;				// Gripper is open, we need to close it before raising the arm
			}
			else
				duty2 = ServoMin + 50;
		}	
		
		if(gripper_flag == 1)
		{
			// lower arm all the way
			step_to(ServoMax+5, 1);
			//close gripper
			step_to(ServoMin+25, 2);
			// raise arm all the way
			step_to(ServoMin+2, 1);
			//open gripper
			step_to(ServoMin+90, 2);
			
			_delay_ms( 4000 );
			gripper_flag = 0;
		}
			
		_delay_loop_2( 32000 ); 	//wait a bit before doing anything again	//64 * a_Dist); //65535);
	}

}


/*! \brief Initialize ADC and perform dummy conversion.
 *
 * \note Function only called once and can be inlined (forced)
 */
void Init_ADC( void )
{
	DIDR0 = _BV(Flow)|_BV(Rate); // Turn off digital IO on PB3 and 4 since they are analog in
	
  // VCC voltager ref, ADC2 (PB4) as default input
  ADMUX = 2;
  // Enable ADC, ADC prescaler /64
  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1);

  // Start conversion and wait for it to complete (result not used)
  ADCSRA |= (1<<ADSC);            // Start convertion
  while (ADCSRA & (1<<ADSC));     // Wait for conversion to complete
  ADCSRA = ADCSRA;                // Clear ADC interrupt flag

}

/*! \brief Reads ADC input
 */
unsigned char ADC_read(char input)
{
    // Hold ADC value
    unsigned char a_Value;		// 8-bit unsigned var (0 - 255)

    // VCC ref and ADCx
    //ADMUX = (0<<REFS0) | (0x07&input);
    ADMUX = _BV(ADLAR) | (0x07&input);	// Left shift value and mask off channel selection
    
    ADCSRA |= (1 << ADIF) | (1<<ADSC);  // Clear ADIF and start single conversion
    
    while(!(ADCSRA & (1 << ADIF)));			// wait for conversion done, ADIF flag active
    // read out ADCL register
    //a_Value = ADCL;
    // read out ADCH register
    //a_Value += (ADCH << 8);
		a_Value = ADCH;			// get 8-bit conversion value from the high byte of ADC register
    return a_Value;
}

void Init_Servo(void)
{
  DDRB   |= _BV(ARM) | _BV(HAND);	// Turn our servo pins on as output
  TCCR0B = ( 1 << CS00) ;
  TCCR0A	= ( 1 << WGM01 ); 			// Timer mode 2 Clear Timer on Compare Match (CTC) mode,
	OCR0A		= 124; 									// This is 9600 baud/8 -1
	TIMSK0  = ( 1 << OCIE0A ); 			// Enable the Timer interupt.
}

// Timer compare A is configured to run every 13us
// 64 of these * 25 = 20ms
SIGNAL(SIG_OUTPUT_COMPARE0A)
{	
  // check if we need to clear arm servo pin
  if(pwm_duty1 == 0)
  {
  	PORTB &= ~_BV(ARM);		// clear the pin we have completed the duty cycle
  }
  else 
  	pwm_duty1--;					// otherwise decrement our pulse width counter
  	
  // check if we need to hand servo pin
  if(pwm_duty2 == 0)
  {
  	PORTB &= ~_BV(HAND);	// clear the pin we have completed the duty cycle
  }
  else 
  	pwm_duty2--;					// otherwise decrement our pulse width counter
  
  // now check our 20ms timer
  if(Prescaler64 == 0)
  {
  	// reset Prescaler64 and check 20ms
  	Prescaler64 = 64;		// 1/FCPU = tick time. This point = ticktime * compareA * 64 or 833us
  	if(timer100ms == 0)
  	{
  		timer100ms = ten_ms;
  		adc_flag = 1;
  	}
  	else
  		timer100ms--;
  	
  	if(timer20ms == 0)
  	{
  		// ok, we have reached 20ms
  		// turn on the port pin, reset the duty, and reset the timer20ms
  		timer20ms = 25;			// 25 * 833us = 20ms which is our servo framerate
  		pwm_duty1 = duty1;
  		pwm_duty2 = duty2;
  		PORTB |= _BV(ARM) | _BV(HAND);
  	}
  	else
  		timer20ms--;
  }
  else
  	Prescaler64--;
}


// this function takes the end value and the pwm vaiable and commands the servo 1 step at a time towards the end value
// 
void step_to(unsigned char value, unsigned char num)
{
	// is this duty1 or duty2?
	if(num == 1)
	{
		if(value > duty1)
		{	while(duty1 != value)	// run this routine to pick up the block
			{
				duty1++;							// = SERVO_CLIP(duty1 - 1);
				PORTB ^= _BV(LED);			// XOR LEd (if it's on turn it off and vice versa)
				_delay_ms( 30 );
			}
		}
		else
		{	while(duty1 != value)	// run this routine to pick up the block
			{
				duty1--;							// = SERVO_CLIP(duty1 - 1);
				PORTB ^= _BV(LED);			// XOR LEd (if it's on turn it off and vice versa)
				_delay_ms( 30 );
			}
		}
	}
	else
	{
		if(value > duty2)
		{	while(duty2 != value)	// run this routine to pick up the block
			{
				duty2++;							// = SERVO_CLIP(duty1 - 1);
				PORTB ^= _BV(LED);			// XOR LEd (if it's on turn it off and vice versa)
				_delay_ms( 30 );
			}
		}
		else
		{	while(duty2 != value)	// run this routine to pick up the block
			{
				duty2--;							// = SERVO_CLIP(duty1 - 1);
				PORTB ^= _BV(LED);			// XOR LEd (if it's on turn it off and vice versa)
				_delay_ms( 30 );
			}
		}
	}
}
