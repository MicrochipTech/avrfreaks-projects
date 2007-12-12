/*
 **Controlling an Espresso Machine** 
 * This project was undetaken to repair a faulty Delonghi Cafe Venezia Espresso Machine.
 * The Cafe Venezia is a "pump" machine, with "coffee" and "steam" modes.
 * Heat is controlled by two snap action disc thermostats, 95C for coffee, 125C for steam.
 * The thermostats are somewhat light duty for the 1100 watt boiler
 * Consequently, they fail after several years if use.
 * This project replaces thermostats with relays controlled by an AVR ATTiny24 . 
 * Boiler temperature is monitored by a type K thermocouple through an Analog Devices ADC595 thermocouple interface.
 * Power to the relays is interlocked through a 130C thermostat serving as a safety shutdown.
 * MCU and ADC595 are powered with 5V using a 9V "wall wart" through a 78L05 5V regulator.
 * The project was built with "Programmer's Notepad 2"
 * The MCU was programmed using the SPI serial bus.
 
 *AVR ATTiny 24 
 *Type K thermocople connects to an Analog Devices ADC595 thermocouple interface
 *The ADC595 outputs 10mV/degC and wires directly to PA0, pin 13.
 *Analog to Digital Conversions are triggered at a 2Hz rate by Timer1.
 *To ensure clean conversions, the ADC Noise Reduction feature is used.
 *
 *Outputs to Opto Isolators:
 * PB0 - Relay 1
 * PB1 - Relay 2
 *
 * Temperature thresholds:
 * 1: Off @ 97C
 * 1: On @ 95C
 * 2: Off @ 123C
 * 2: On @ 121C 
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define TIMER1_PRESCALE_1 1
#define TIMER1_PRESCALE_8 2
#define TIMER1_PRESCALE_64 3
#define TIMER1_PRESCALE_256 4
#define TIMER1_PRESCALE_1024 5

#define ON 1
#define OFF 0

#define COF_TEMP_OFF 200
#define COF_TEMP_ON COF_TEMP_OFF - 1

#define STEAM_TEMP_OFF 252
#define STEAM_TEMP_ON STEAM_TEMP_OFF - 1


// Test routine to toggle outputs
void
tLeds (void)
{
static uint8_t bState;


if (bState == OFF)
	{
		bState = ON;
		PORTB |= (1 << 0); // Turn on LED1
	}
	else
	{
		bState = OFF;
		PORTB &= ~(1 << 0); // Turn off LED1
	}
}

// this function is called when timer1 compare matches OCR1A
ISR ( TIM1_COMPA_vect ) 
{
   ADCSRA |= (1 << ADEN);  // Enable ADC
   ADCSRA |= (1 << ADIE);  // Enable ADC Interrupt
   MCUCR |= (1 << SM0); // Enter ADC Noise Reduction Mode, Conversion starts automatically when CPU Stops
}

// this function is called when the ADC conversion is complete
ISR(ADC_vect)
{
	
 static uint16_t adVal;	//16 bit storage for ADC result
   
 MCUCR &= ~(1 << SM0); 	// Disable ADC Noise Reduction Mode
 ADCSRA &= ~(1 << ADIE);  	// Disable ADC Interrupt
 ADCSRA &= ~(1 << ADEN);  	// Disable ADC
 
 adVal = (uint16_t)ADCL + ((uint16_t)ADCH << 8); 
 	
  if(adVal < COF_TEMP_ON)
      {
         PORTB |= (1 << 0); // Turn on Relay 1
      }
  if (adVal >= COF_TEMP_OFF)
		{
         PORTB &= ~(1 << 0); // Turn off Relay 1
		}

  if(adVal < STEAM_TEMP_ON)
      {
         PORTB |= (1 << 1); // Turn on Relay 2
      }
  if (adVal >= STEAM_TEMP_OFF)
		{
         PORTB &= ~(1 << 1); // Turn off relay 2
		}
}




void
ioinit (void)			/* Note [6] */
{
   DDRB |= (1 << 0); // Set PB 0  as output for Relay 1
   DDRB |= (1 << 1); // Set PB 1 as output for Relay 2

 
  /*
    the frequency of the interrupt overflow is determined by the 
    prescaler and overflow value.
    freq = clock_frequency / ( 2 * prescaler * overflow_val)
    where prescaler can be 1, 8, 64, 256, or 1024
    clock_freq is 1MHz
    and overflow_val is 16bit
    the overflow value is placed in OCR1A, the prescale is set in TCCR1B
    so for example:
	Desired ADC Rate = 2 Hz	
    desired freq = 2Hz
    clock freq = 1MHz
    1MHz / (2Hz * 2 * 8) = 31250
    OCR1A = 31250 and the prescale is 8
  */

  TCCR1B = (1 << WGM12) | TIMER1_PRESCALE_8;
  OCR1A = (uint16_t)31250;
  TIMSK1 |= 1 << OCIE1A;   // Output Compare Interrupt Enable (timer 1, OCR1A) 


  ADCSRA |= (1 << ADPS2) ;  // Set ADC prescaler to 16 - 62.5KHz sample rate @ 1MHz
 
  // No MUX values needed to be changed to use ADC0

  DIDR0 = (1 << ADC0D);	// Disable digital input on ADC0
   
  sei();   // Enable Global Interrupts

}

int
main (void)
{

    ioinit ();

    /* loop forever, the interrupts are doing the rest */

    for (;;)			/* Note sleep mode is defined in sleep.h */
        sleep_mode();
    return (0);
}
