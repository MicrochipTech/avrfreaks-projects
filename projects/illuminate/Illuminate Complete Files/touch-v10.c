/***************************************************************************
* 
* File              : touch-vXX.c
*
* Author			: Kurt E. Clothier
* Date				: June 15, 2012
* Modified			: June 22, 2012
*
* Description       : Controller for touch lamp
*
* Compiler			: AVR-GCC
*
* Hardware			: ATTINY25; Calibrated Internal Oscillator with CKDIV8
*
* Fuse Settings		: DEFAULT (lfuse:w0x62:m hfuse:w0xdf:m efuse:w0xff:m)
*
****************************************************************************/
#define VERSION	0x10	// Firmware Version 1.0 - Basic Functionality
 
//-------------------------
//		Includes
//-------------------------
#include <custom/avr.h>
#include <avr/sleep.h>

//-------------------------
//		Defines
//-------------------------
// Constants
#define LOW_LIGHT	25		// Low Intensity LED Output - 25/255 = 9.8%
#define HIGH_LIGHT	150		// High Intesity LED Output - 150/255 = 58.8%

// Hardware Definitions
#define LED 	_BV(PB0)	// Pin 5 - OC0A
#define NA1		_BV(PB1)	// Pin 6 - OC0B
#define TOUCH	_BV(PB2)	// Pin 7 - INT0
#define NA3		_BV(PB3)	// Pin 2 - XTAL1
#define NA4		_BV(PB4)	// Pin 3 - XTAL2
#define RESET	_BV(PB5)	// Pin 1 - !RESET

#define LED_PORT	PORTB
#define LED_DDR		DDRB

#define TOUCH_PORT	PORTB
#define TOUCH_DDR	DDRB
#define TOUCH_PIN	PINB

// Status Flag Bits
#define LED_MODE	0x10	// 0 = LOW, 1 = HIGH
#define LED_STATE	0x20	// 0 = OFF, 1 = ON

//-------------------------
//      Global Variables
//-------------------------
static volatile uint8_t stat_flag = 0x00;


/**************************************************************************
	Main
***************************************************************************/

int main (void) 
{

  /****************************
	Initialize ATtiny25 Device
  ****************************/
  
  cli();	// Turn off interrupts
  
  // Power Reduction Register
  PRR = 
	_BV(PRTIM0) |	// Timer0 - 118.2uA
	_BV(PRTIM1) |	// Timer1 - 153.0uA
	_BV(PRUSI)  |	// USI	  - 92.2 uA
	_BV(PRADC);		// ADC	  - 333.3uA
	
  // Set I/O Ports
  DDRB = LED;		// Set as Output
  PORTB = 0x00;		// Outputs - Low; Inputs - Pulls Ups Disabled
  
  // Set up Timer 0 for 60 Hz LED Pulse -> 1MHz / (64 * 256)
  PRR &= ~_BV(PRTIM0);								// Disable Timer Power Save
  TCCR0A = _BV(WGM00) | _BV(WGM01) | _BV(COM0A1);	// PWM, clear OC0A on Match
  TCCR0B = _BV(CS01) | _BV(CS01);					// Prescaler = 64
  OCR0A  = LOW_LIGHT;								// Duty Cycle = 10% = (25/255) 
  
  // Enable Pin Change Interrupt (INT0 only wakes Power Down Sleep Mode on Low Level)
  PCMSK = _BV(PCINT2);		// Pin Change Interrupt on PB2
  GIMSK = _BV(PCIE);		// Enable Pin Change Interrupt
  
  // Enable Sleep Mode for Power Down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);	// Set Sleep Mode: Power Down
  sleep_enable();						// Enable Sleep Mode
  sleep_bod_disable();					// Disable BOD
  sei();								// Enable Interrupts
  sleep_cpu();							// Got to Sleep
  sleep_disable(); 						// Disable Sleep on Wakeup


  /****************************
	Enter Main Program Loop
  ****************************/
  for(;;)
  {
    if (MCUCR & _BV(SE)){	// If Sleep is Enabled
	  cli();				// Disable Interrupts
      sleep_bod_disable();	// Disable BOD
      sei();				// Enable Interrupts
      sleep_cpu();			// Go to Sleep
      sleep_disable();		// Disable Sleep on Wakeup
    }
  }
}

/**************************************************************************
	INTERRUPT HANDLERS
***************************************************************************/

ISR(PCINT0_vect)	// Pin Change Interrupt Request 0 - Touch Sensor
{

  sleep_disable();	// Disable Sleep on Wakeup
  
  if(TOUCH_PIN & TOUCH){					// if touched...
//    LOOP_UNTIL_BIT_LO(TOUCH_PIN, TOUCH);	// Delay until not touching
	switch (stat_flag & 0x30){				// Check LED State and Mode
	
	case 0x20:	// State = ON, Mode = LOW
	  
	  SET_BIT(stat_flag, LED_MODE);		// Set mode to HIGH
	  OCR0A = HIGH_LIGHT;				// Set Output to HIGH
	  break;
	
	case 0x30:	// State = ON, Mode = HIGH
	
	  CLEAR_BIT(stat_flag, LED_STATE);	// Set state to OFF
	  PRR &= ~_BV(PRTIM0);				// Enable Timer0 Power Save
	  TCCR0A &= ~_BV(COM0A1);			// Disable OC0A Output
	  sleep_enable();					// Enable Sleep Mode
	  break;
	
	default:	// State = OFF, Mode = don't care
	
	  SET_BIT(stat_flag, LED_STATE);	// Set state to ON
	  CLEAR_BIT(stat_flag, LED_MODE);	// Set mode to LOW
	  OCR0A = LOW_LIGHT;				// Set Output to LOW
	  PRR &= ~_BV(PRTIM0);				// Disable Timer0 Power Save
	  TCCR0A |= _BV(COM0A1);			// Enable OC0A Output
	  break;
	
	}
	LOOP_UNTIL_BIT_LO(TOUCH_PIN, TOUCH);	// Delay until not touching
  }
}




