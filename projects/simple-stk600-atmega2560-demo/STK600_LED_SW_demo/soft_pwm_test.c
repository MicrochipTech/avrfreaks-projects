#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <compat/ina90.h>

#define RESET &=~
#define SET   |=
#define TOGGLE ^=

#define LEFT 0x00
#define RIGHT 0x01
#define INCREASING 0x00
#define DECREASING 0x01

volatile uint8_t LED_mask = 0x01;			// Right-most LED
volatile uint8_t direction = LEFT;
volatile uint8_t brightness = INCREASING;
volatile uint8_t slider_value = 0x00;		// Off
volatile uint16_t slider_temp;				// Non-linear brightness algorithm
volatile uint8_t pulse_incr = 0x04;			// Slow pulse, range of 0x02 to 0x10
volatile uint8_t switch_temp;				// Temporary storage of switch port data

volatile unsigned long i;


ISR(TIMER0_OVF_vect)
{
      PORTC RESET LED_mask;  				// Turn on

}

ISR(TIMER0_COMPA_vect)
{
      PORTC SET LED_mask;   				// Turn off
}

ISR(PCINT2_vect)
{
	switch_temp = ~PINK;
	if (switch_temp != 0x00){					// Skip if button release
		pulse_incr = 0x10;					// Maximum step = 16
		for (i=0x80;i > 0x00; i = i >> 1){
			if (switch_temp & i) break;
			pulse_incr -= 2;
		}
		brightness = INCREASING;
		slider_value = 0x00;				// Start new cycle with LED off
	}
}

void timer0_init(void)
{
// NOTE: Assumes DIV8 fuse set for 1 MHz operation
// 8-bit counter means overflow interrupt every 61 Hz to turn on the LED
// Variable Compare_A interrupt in between OVF to turn off the LED
	OCR0A = slider_value;
  	TCCR0A = 0x03; 							// Fast PWM Mode
	TCCR0B = 0x03;							// Set prescaler to 64
	TIMSK0 |= (1<<OCIE0A) | (1<<TOIE0);		// Compare A and OVF Interupt enabled 
	TCNT0  = 0x00;
}

void LED_init(void)
{
	PORTC = 0xFF;							// All PORTC set (LED off)
	DDRC = 0xFF;							// All PORTC output
}

void SW_init(void)
{
	PORTK = 0xFF;							// All PORTK set
	DDRK = 0x00;							// All PORTK input
	PCICR = 1 << PCIE2;						// Enable pin change interrupt 16..23
	PCMSK2 = 0xFF;							// Enable mask for PCINT
}

void power_init(void)
{
	PRR0 = 0xCF;							// Only Timer0 enabled
	PRR1 = 0x3F;							// All disabled
	SMCR = 0x01;							// Enable Idle mode
}

int main(void)
{
	timer0_init();

	LED_init();
	SW_init();
	power_init();

	sei();

	while(1)
	{

		_SLEEP();
		
		if(brightness == INCREASING)
		{
			if(slider_value < 240)				// pulse_incr can = 16 without overflow
			{
				slider_value += pulse_incr;
				slider_temp = (1 + slider_value + (slider_value * slider_value / 2)) >> 7;
				OCR0A = slider_temp;
			}
			else								// slider_value >= 240
				brightness = DECREASING;
		}
		else
		{
			if(slider_value > 0) 
			{
				slider_value -= pulse_incr;
				slider_temp = (1 + slider_value + (slider_value * slider_value / 2)) >> 7;
				OCR0A = slider_temp;
			}
			else {								// slider_value = 0
				brightness = INCREASING;
				if(direction == LEFT) 
				{
					LED_mask = LED_mask << 1;
					if (LED_mask == 0x00) {LED_mask = 0x40; direction = RIGHT;}
				}
				else 							// direction = RIGHT
				{
					LED_mask = LED_mask >> 1;
					if (LED_mask == 0x00) {LED_mask = 0x02; direction = LEFT;}
				}
			}
		}
	}

	return 0;
}
