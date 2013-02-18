/***************************************************************************
* 
* File              : touch-vXX.c
*
* Author			: Kurt E. Clothier
* Date				: June 15, 2012
* Modified			: July 8, 2012
*
* Description       : Controller for touch lamp
*
* Compiler			: AVR-GCC
*
* Hardware			: ATTINY25; Calibrated Internal Oscillator with CKDIV8
*					:	- Could easily be ported to ATtiny13 (1K flash)
*
* Fuse Settings		: DEFAULT (lfuse:w0x62:m hfuse:w0xdf:m efuse:w0xff:m)
*
****************************************************************************/
#define VERSION	0x20	// Firmware Version 2.0
//	- Updated Output Light Levels
//	- Removed Sleep Command from Initialization
//	- Disabled Timer0 and OC0A during Initiialization
//  - Removed Status flag, Switch case checks OCR0A
//	- Removed Timer 0 Power Save (Timer is Disabled in Sleep Mode)
//	- Added Medium Light Level
//	- Added more comments

/********** Program Overiew ****************** 
- Program Flow
	1. Initialize AVR Chip
	2. Go to sleep
	3. Wait for Pin Change Interrupt
	4. Check LED State
		4a. If OFF, Set Low
		4b. Else If LOW, Set MED
		4c. Else If MED, Set HIGH
		4d. Else If HIGH, Turn OFF & Sleep
	5. Repeat from Step 3
- Additonal Information
	> Reduce Clock Frequency to 31.25kHz
	> Use 8 Bit Timer 0 for PWM output (122Hz)
	> 122 Hz, PWM, clear OC0A on Match
	> Touch Sensor Controls Program Flow
	> Use Pin Change Hardware Interrupt (PB2)
	> Three Output Levels + Off
**********************************************/
 
//-------------------------
//		Includes
//-------------------------
#include <custom/avr.h>		// Custom AVR Header File
#include <avr/sleep.h>		// Supplied AVR Sleep Macros

//-------------------------
//		Defines
//-------------------------

/********** LED Current Percentages ********** 
Circuitry defines maximum LED current as 76mA
8 bit counter maximum = 255, OCR0A sets level
	10% = 7.6mA		OCR0A = 26
	25% = 19mA		OCR0A = 64
	50% = 38mA		OCR0A = 128
	75% = 57mA		OCR0A = 191
	90% = 68.4mA	OCR0A = 230
**********************************************/

#define LED_OFF		0		// LED Output is off
#define LOW_LIGHT	25		// Low Intensity LED Output
#define MED_LIGHT	100		// Medium Intensity LED Output
#define HIGH_LIGHT	175		// High Intesity LED Output

// Hardware Definitions
#define LED 	_BV(PB0)	// Pin 5 - OC0A
#define NA1		_BV(PB1)	// Pin 6 - OC0B
#define TOUCH	_BV(PB2)	// Pin 7 - INT0
#define NA3		_BV(PB3)	// Pin 2 - XTAL1
#define NA4		_BV(PB4)	// Pin 3 - XTAL2
#define RESET	_BV(PB5)	// Pin 1 - !RESET

#define LED_PORT	PORTB	// Port for the LED output
#define LED_DDR		DDRB	// Direction Register for LED

#define TOUCH_PORT	PORTB	// Port for the Touch Sensor Input
#define TOUCH_DDR	DDRB	// Direction Register for Sensor
#define TOUCH_PIN	PINB	// Pin for Touch Sensor Input


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
  
    // Set System Clock Prescaler
  CLKPR = _BV(CLKPCE);	// Enable Prescaler Change
  CLKPR = _BV(CLKPS3);	// Prescaler = 256, F_osc = 8MHz / 256 = 31.25kHz
  
  // Set up Timer 0 for 122 Hz LED Pulse -> 31.25kHz / (1 * 256)
  PRR &= ~_BV(PRTIM0);								// Disable Timer Power Save
  TCCR0A = _BV(WGM00) | _BV(WGM01);				// PWM Mode, outputs Off
  TCCR0B = _BV(CS00);								// Prescaler = 1
  OCR0A  = LED_OFF;									// Start with LEDs Off 
  
  // Enable Pin Change Interrupt (INT0 only wakes Power Down Sleep Mode on Low Level)
  PCMSK = _BV(PCINT2);		// Pin Change Interrupt on PB2
  GIMSK = _BV(PCIE);		// Enable Pin Change Interrupt
  
  // Enable Sleep Mode for Power Down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);	// Set Sleep Mode: Power Down
  sleep_enable();						// Enable Sleep Mode

  sei();								// Enable Interrupts	


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
  
  if(TOUCH_PIN & TOUCH){		// if touched...

	switch (OCR0A){					// Check LED State and Mode
	
	case LOW_LIGHT:
	  
	  OCR0A = MED_LIGHT;				// Set Output to MEDIUM
	  break;
	  
	case MED_LIGHT:
	  
	  OCR0A = HIGH_LIGHT;				// Set Output to HIGH
	  break;
	
	case HIGH_LIGHT:
	
	  TCCR0A &= ~_BV(COM0A1);			// Disable OC0A Output
	  OCR0A  = LED_OFF;					// Set Output to OFF
	  sleep_enable();					// Enable Sleep Mode
	  break;
	
	default:	// State = OFF, Mode = don't care
	
	  OCR0A = LOW_LIGHT;				// Set Output to LOW
	  TCCR0A |= _BV(COM0A1);			// Enable OC0A Output
	  break;
	}
	
	LOOP_UNTIL_BIT_LO(TOUCH_PIN, TOUCH);	// Delay until not touching (see avr.h)
  }
}




