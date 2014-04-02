#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/ina90.h>

#define RESET &=~
#define SET   |=
#define TOGGLE ^=
#define TRUE  1
#define FALSE 0

#define LEFT 0x00
#define RIGHT 0x01
#define INCREASING 0x00
#define DECREASING 0x01

// Global variables
volatile uint8_t LED_mask = 0x01;			// Right-most LED
volatile uint8_t direction = LEFT;
volatile uint8_t brightness = INCREASING;
volatile uint8_t slider_value = 0x00;		// Off
volatile uint16_t slider_temp;				// Non-linear brightness algorithm
volatile uint8_t pulse_incr = 0x04;			// Slow pulse, range of 0x02 to 0x10
volatile uint8_t switch_temp;				// Temporary storage of switch port data
volatile uint8_t enable_update;				// Flag that it is OK to modify COMPA to adjust PWM duty cycle

volatile uint8_t i;							// For switch ISR

// Function prototypes
void timer0_init(void);
void timer1_init(void);
void LED_init(void);
void SW_init(void);
void power_init(void);
uint8_t log_scaled(uint8_t slider);

int main(void)
{

	timer0_init();
	timer1_init();

	LED_init();
	SW_init();
	power_init();

	sei();

	while(1)
	{

		_SLEEP();

		if(enable_update)
		{
			enable_update = FALSE;
			if(brightness == INCREASING)
			{
				if(slider_value < (256 - pulse_incr))				// pulse_incr can = 16 without overflow
				{
					enable_update = FALSE;
					slider_value += pulse_incr;
					//				slider_temp = (1 + slider_value + (slider_value * slider_value / 2)) >> 7;
					slider_temp = log_scaled(slider_value);
					OCR0A = slider_temp;
				}
				else								// slider_value >= 240
				{
					brightness = DECREASING;
				}
			}
			else
			{
				if(slider_value >= pulse_incr)
				{
					enable_update = FALSE;
					slider_value -= pulse_incr;
					//				slider_temp = (1 + slider_value + (slider_value * slider_value / 2)) >> 7;
					slider_temp = log_scaled(slider_value);
					OCR0A = slider_temp;
				}
				else								// slider_value = 0
				{
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
	}

	return 0;
}


ISR(TIMER0_OVF_vect)
{
      PORTC RESET LED_mask;  				// Turn on

}

ISR(TIMER0_COMPA_vect)
{
      PORTC SET LED_mask;   				// Turn off
	  enable_update = TRUE;
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
  	TCCR0A = (1 << WGM01) | (1 << WGM00); 	// Fast PWM Mode
	TCCR0B = (1 << CS01) | (1 << CS00);		// Set prescaler to 64
	TIMSK0 |= (1<<OCIE0A) | (1<<TOIE0);		// Compare A and OVF Interupt enabled 
	TCNT0  = 0x00;
}

void timer1_init(void)
{
// Used for timing functions
	TCCR1A = 0x00;
	TCCR1B = (1 << CS10);							// Normal operation, no clk prescaling
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
	PRR0 = (1 << PRTWI) | (1 << PRTIM2) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC);		// Only Timer0 and Timer1 enabled
	PRR1 = 0x3F;							// All disabled
	SMCR = (1 << SE);							// Enable Idle mode
}

uint8_t log_scaled(uint8_t slider)
{
/*
for slider = 0 to 255
log_scaled = offset + ((slider-base)/mult)

i     offset      base  mult
0=    0     0     /4
1=    31    128   /2
2=    63    192   /1
3=    95    224   *2
4=    127   240   *4

5=    159   248   *8
6=    191   252   *16
7=    223   254   *32
8=    255   255   *64

except that the leading_ones loop can end at 4
so offset can be uint8_t
and the last 8 points can just be an array
[159,167,175,183,191,207,223,255]
which can be indexed into by temp ?
*/

/*
      uint16_t offset = 0;
      uint8_t temp = slider;
      uint8_t base, leading_ones, scaled, dividend, temp2;

      for (leading_ones=0;leading_ones<=8;leading_ones++){
	  		temp2 = ~temp;
            if(temp2 >= 0x80){
                  break;}
            else{
                  temp = temp<<1;}
      }

      if (leading_ones>0){
            offset = leading_ones*32 - 1;}

      base = ~(0xFF >> leading_ones);
 
      if (leading_ones>=2){
            scaled=(slider-base)<<(leading_ones-2);
      }
      else{
            dividend=2>>leading_ones;
            scaled=(slider-base)>>dividend;
      }
      scaled+=offset;
*/
	  uint8_t scaled;

	  static uint8_t last32[32] = {
	   96, 98, 99,101,102,104,106,107,
	  109,111,113,115,118,120,123,125,
	  128,131,134,138,141,145,150,154,
	  160,166,173,181,192,205,224,255};

	  scaled = slider / 4;				// first 128

	  if(slider >= 128){				// next 64
	  		scaled = (slider / 2) - 33;
	  }

	  if(slider >= 192){				// next 32
	  		scaled = slider - 129;
	  }

	  if(slider >=224){					// last 32
	  		scaled = last32[slider - 224];
	  }

      return scaled;
}


