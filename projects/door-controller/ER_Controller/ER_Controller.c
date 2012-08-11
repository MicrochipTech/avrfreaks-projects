/*
 * ER_Controller.c
 */ 

#include <avr/io.h>
#define F_CPU 1000000UL	//  8MHz÷CLK/8 = 1 MHz system clock
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include "timers.h"

/*
OUTPUT DEFINITIONS:		*/
#define OUTPUT_PORT		PORTB	// Output port for Door1/2 and Signals1/2
#define OUTPUT_DDR		DDRB	// Data Direction Port for output
#define DR1_OUT			PORTB0	// Door 1 output pin
#define DR2_OUT			PORTB1	// Door 2 output pin
#define SIG1_OUT_PIN	PORTB2	// Signal 1 output pin
#define SIG2_OUT_PIN	PORTB3	// Signal 2 output pin

/*						
INPUT DEFINITIONS:		*/
#define INPUT_PIN			PIND	// Input Port for Door 1 & Door 2
#define INPUT_PORT			PORTD	// Input Port for Door 1 & Door 2
#define INPUT_DDR			DDRD	// Data Direction for Input Ports
#define MODE_PIN			PIND0	// Mode selection pin
#define TOGGLE1_PIN			PIND1	// Toggle Mode pin for Door 1
#define TOGGLE2_PIN			PIND7	// Toggle Mode pin for Door 2
#define DR1_BUTTON			PIND2	// Door 1 Input Pin
#define DR2_BUTTON			PIND3	// Door 2 Input Pin
#define ACTIVE_MODE1_PIN	PIND4	// Active Mode Pin for Door 1
#define ACTIVE_MODE2_PIN	PIND5	// Active Mode Pin for Door 2
#define DS_PORT				PORTC	// Digital Switch Input Port
#define DS_DDR				DDRC	// Data Direction for DS_PORT
#define DS1_PIN				PORTC0	// DS1 Control Output Pin
#define DS2_PIN				PORTC1	// DS2 Control Output Pin

/*
CONSTANT DEFINITIONS:	*/
#define DIFF_DELAY		500UL	// Differential Delay Time between door retractions(ms)
#define SIG_DELAY		600UL	// Signal Delay Time before activating door signals(ms)
#define DEBOUNCE_TIME	255		// Switch Debounce Time
#define DEPENDENT		0x1		// Operating Mode Types
#define INDEPENDENT		0x0
#define ACTIVE_HIGH		0x1		// Output Mode Types
#define ACTIVE_LOW		0x0
#define true			0x1		// Boolean Types
#define false			0x0

const uint8_t inputPinMask = 0b10110011;

// Global Variables for the various configurations
volatile uint8_t outputMode1, outputMode2, inputReg = 0x0;
volatile boolean
		mode,
		dr1_toggleMode,
		dr2_toggleMode,
		retracting=false,
		dr1_toggled=false,
		dr2_toggled=false,
		dr1_active=false,
		dr2_active=false;

// Array holding the 4-bit timing selection values (in seconds)
const uint8_t PROGMEM timing_values_PGM[] = {
	1,	// 0
	2,	// 1
	5,	// 2
	10,	// 3
	15,	// 4
	20, // 5
	25,	// 6
	30,	// 7
	35,	// 8
	40, // 9
	45, // A
	50,	// B
	55,	// C
	60, // D
	90,	// E
	120,// F
};

#define getTimingValue(P) ( pgm_read_byte( timing_values_PGM + (P) ) )

void doorPinWrite(uint8_t doorPin, boolean active)
{
	byte mode;
	if(doorPin==DR1_OUT){
		if(active==true)
			mode = bit_get(outputMode1, BIT(0));
		else
			mode = !bit_get(outputMode1, BIT(0));
		bit_write( mode, OUTPUT_PORT, BIT(DR1_OUT) );
	} else {
		if(active==true)
			mode = bit_get(outputMode2, BIT(0));
		else
			mode = !bit_get(outputMode2, BIT(0));
		bit_write( mode, OUTPUT_PORT, BIT(DR2_OUT));
	}
	return;
}

uint8_t button_is_pressed(uint8_t PIN, uint8_t BIT)
{
	/* the button is pressed when BUTTON_BIT is clear */
	if (bit_is_clear(PIN, BIT))	{
		_delay_ms(DEBOUNCE_TIME);
		if (bit_is_clear(PIN, BIT)) return 1;
	}
	return 0;
}

unsigned long getHoldTime(uint8_t door)
{
	unsigned long time;
	if(door<2) {
		// Read DS1: set PC0 high, PC1 Low
		sbi(DS_PORT, DS1_PIN);
		loop_until_bit_is_set(DS_PORT, DS1_PIN);
		cbi(DS_PORT, DS2_PIN);		
		loop_until_bit_is_clear(DS_PORT, DS2_PIN);
	} else {
		// Read DS2: set PC0 Low, PC1 high
		sbi(DS_PORT, DS2_PIN);
		loop_until_bit_is_set(DS_PORT, DS2_PIN);
		cbi(DS_PORT, DS1_PIN);
		loop_until_bit_is_clear(DS_PORT, DS1_PIN);
	}
	uint8_t DSx_value = (PINC & 0x3c)>>2;
	time = getTimingValue(DSx_value)*1e3;
	return time;
}

void dependentRetract(void)
{
//	In toggle mode?
	if(dr1_toggleMode==true) {
		if(dr1_toggled==true) /*Outputs currently toggled, so toggle them off*/ {
			doorPinWrite(DR1_OUT,false); // Door 1 off
			doorPinWrite(DR2_OUT,false); // Door 2 off
			cbi(OUTPUT_PORT, SIG1_OUT_PIN);	// Turn off door signal outputs
			cbi(OUTPUT_PORT, SIG2_OUT_PIN);
			dr1_toggled = false;
			dr2_toggled = false;
		} 	else	/* Outputs not toggled, so toggle them now */ {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				doorPinWrite(DR1_OUT,true); // Door 1 Active
				dr1_toggled = true;
				_delay_ms(DIFF_DELAY); // Differential Delay
				wdt_reset(); // Reset the watchdog timer	
				doorPinWrite(DR2_OUT,true); // Door 2 Active
				dr2_toggled = true;
				_delay_ms(SIG_DELAY);	// Door Signal Delay
				sbi(OUTPUT_PORT, SIG1_OUT_PIN);	// Turn on door signal outputs
				sbi(OUTPUT_PORT, SIG2_OUT_PIN);
			}			
			while( bit_is_clear(INPUT_PIN, DR1_BUTTON) || bit_is_clear(INPUT_PIN, DR2_BUTTON) ) // Maintain while buttons are held
				{ wdt_reset(); /* Reset the watchdog timer */ }
		}
	} else /*Not in toggle mode*/ {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			retracting = true;
			doorPinWrite(DR1_OUT,true); // Door 1 Active
			_delay_ms(DIFF_DELAY); // Differential Delay
			doorPinWrite(DR2_OUT,true); // Door 2 Active
			_delay_ms(SIG_DELAY);	// Door Signal Delay
			sbi(OUTPUT_PORT, SIG1_OUT_PIN);	// Activate door signal outputs
			sbi(OUTPUT_PORT, SIG2_OUT_PIN);
		}
		door1_timer(true); // Initialize the Door 1 timer
		while( door1_timer(0) <= getHoldTime(1) ) // Hold Time Delay, value set from DS1
		{
			wdt_reset(); // Reset the watchdog timer
			if( bit_is_clear(INPUT_PIN, DR1_BUTTON) || bit_is_clear(INPUT_PIN, DR2_BUTTON) )	// Maintain active outputs when button is held
				door1_timer(true); // reset the timer 
			if(mode==INDEPENDENT) // If mode changed, break loop
				break;
		}
		doorPinWrite(DR1_OUT,false); // Door outputs off
		doorPinWrite(DR2_OUT,false);
		cbi(OUTPUT_PORT, SIG1_OUT_PIN);	// Deactivate door signal outputs
		cbi(OUTPUT_PORT, SIG2_OUT_PIN);
		retracting = false;
	}
}

void activateDoor1(boolean activate){
	if(activate==false){
		doorPinWrite(DR1_OUT,false);
		cbi(OUTPUT_PORT,SIG1_OUT_PIN);
	} else {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) // Ensure the following code completes before running an interrupt
		{
			doorPinWrite(DR1_OUT,true);
			_delay_ms(SIG_DELAY);
			sbi(OUTPUT_PORT,SIG1_OUT_PIN);
		}
	}
	wdt_reset(); // Reset the watchdog timer
}

void activateDoor2(boolean activate){
	if(activate==false){
		doorPinWrite(DR2_OUT,false);
		cbi(OUTPUT_PORT,SIG2_OUT_PIN);
	} else {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) // Ensure the following code completes before running an interrupt
		{
			doorPinWrite(DR2_OUT,true);
			_delay_ms(SIG_DELAY);
			sbi(OUTPUT_PORT,SIG2_OUT_PIN);
		}			
	}
	wdt_reset(); // Reset the watchdog timer
}

/*
Initialize the global variables */
static void getInputs(void)
{
	inputReg = INPUT_PIN & inputPinMask;
	
	if( bit_is_set(inputReg,0) ) { mode = DEPENDENT; }
	else { mode = INDEPENDENT; }
	
	if( bit_is_set(inputReg,TOGGLE1_PIN) ) { dr1_toggleMode = false; }
	else { dr1_toggleMode = true; }
	
	if( bit_is_set(inputReg,ACTIVE_MODE1_PIN) ) { outputMode1 = ACTIVE_HIGH; }
	else { outputMode1 = ACTIVE_LOW; }
	
	if( bit_is_set(inputReg,ACTIVE_MODE2_PIN) ) { outputMode2 = ACTIVE_HIGH; }
	else { outputMode2 = ACTIVE_LOW; }
		
	if( bit_is_set(inputReg,ACTIVE_MODE2_PIN) ) { outputMode2 = ACTIVE_HIGH; }
	else { outputMode2 = ACTIVE_LOW; }
		
	if( bit_is_set(inputReg,TOGGLE2_PIN) ) { dr2_toggleMode = false; }
	else { dr2_toggleMode = true; }
}

/*
Interrupt Handler For PCINT23:16 (PIND)
This routine changes the global variables and resynchronizes
the outputs to correspond with the changes	*/
ISR(PCINT2_vect)
{
	// The set bits in 'reg' corresponds to the changed pin inputs
	uint8_t reg = (INPUT_PIN^inputReg);
	
	if(  bit_is_set(reg, 0) ) // Operating Mode Changed
	{
		// Set the operating mode
		if( bit_get(INPUT_PIN, BIT(MODE_PIN) ) )
		mode = DEPENDENT;
		else
		mode = INDEPENDENT;
			
		// Change the input register value
		bit_write(mode, inputReg, BIT(0));
			
		// Turn off any active outputs for the new mode
		doorPinWrite(DR1_OUT,false);
		doorPinWrite(DR2_OUT,false);
		cbi(OUTPUT_PORT,SIG1_OUT_PIN);
		cbi(OUTPUT_PORT,SIG2_OUT_PIN);
		dr2_toggled = false;
		dr1_toggled = false;
		dr1_active = false;
		dr2_active = false;
		retracting = false;
		door1_timer(false);
		door2_timer(false);
	}
		
	if( bit_is_set(reg, 4) ) // Door 1 Active Mode Changed
	{
		// Set the Door 1 active mode
		if( bit_get(INPUT_PIN,BIT(ACTIVE_MODE1_PIN)) ) {
			outputMode1 = ACTIVE_HIGH;
			sbi(inputReg,ACTIVE_MODE1_PIN);
		} else {
			outputMode1 = ACTIVE_LOW;
			cbi(inputReg,ACTIVE_MODE1_PIN);
		}
			
		if( dr1_active || dr1_toggled || retracting) // If Door 1 is active anywhere ? Door 1 Output = Active Mode 1
			doorPinWrite(DR1_OUT,true);
		else										 // Otherwise ? Door 1 Output = !Active Mode
			doorPinWrite(DR1_OUT,false);
	}

	if( bit_is_set(reg, 5) ) // Door 2 Active Mode Changed
	{
		// Set the Door 2 active mode
		if( bit_get(INPUT_PIN,BIT(ACTIVE_MODE2_PIN)) ) {
			outputMode2 = ACTIVE_HIGH;
			sbi(inputReg, ACTIVE_MODE2_PIN);
		} else {
			outputMode2 = ACTIVE_LOW;
			cbi(inputReg, ACTIVE_MODE2_PIN);
		}
			
		if( dr2_active || dr2_toggled || retracting) // If Door 2 is active anywhere -> Door 2 Output = Active Mode 2
			doorPinWrite(DR2_OUT,true);
		else										 // Otherwise -> Door 2 Output = !Active Mode 2
			doorPinWrite(DR2_OUT,false);
	}
		
	if( bit_is_set(reg, 1) ) // Door 1 Toggle Mode Changed
	{
		// Set the door 1 toggle value
		if( bit_get(INPUT_PIN, BIT(TOGGLE1_PIN)) ) {
			dr1_toggleMode = false;
			sbi(inputReg,TOGGLE1_PIN);
		} else {
			dr1_toggleMode = true;
			cbi(inputReg,TOGGLE1_PIN);
		}
			
		if( dr1_toggled && dr1_toggleMode==false ) /* Toggle mode changed to off and output is toggled!*/
		{
			doorPinWrite(DR1_OUT,false);
			cbi(OUTPUT_PORT,SIG1_OUT_PIN);
			dr1_toggled = false;
			if(mode==DEPENDENT){
				doorPinWrite(DR2_OUT,false);
				cbi(OUTPUT_PORT,SIG2_OUT_PIN);
				dr2_toggled = false;
			}
		}
		bit_write( !bit_get(dr1_toggleMode,BIT(0)), inputReg, BIT(1)); /*Change the input register value*/
	}

	if( bit_is_set(reg, 7) )// Door 2 Toggle Mode Changed
	{
		// Set the door 2 toggle value
		if( bit_get(INPUT_PIN, BIT(TOGGLE2_PIN)) ) {
			dr2_toggleMode = false;
			sbi(inputReg,TOGGLE2_PIN);
		} else {
			dr2_toggleMode = true;
			cbi(inputReg,TOGGLE2_PIN);
		}
			
		if( dr2_toggled && dr2_toggleMode==false && mode == INDEPENDENT) /*Toggle mode changed to off and output is toggled!*/
		{
			doorPinWrite(DR2_OUT,false);
			cbi(OUTPUT_PORT,SIG2_OUT_PIN);
			dr2_toggled = false;
		}
		bit_write( !bit_get(dr2_toggleMode,BIT(0)), inputReg, BIT(7)); /*Change the input register value*/
	}
}

int main(void) {

//	INIT:
	wdt_reset();
	WDTCSR = (1<<WDCE)|(1<<WDE); // Set WDCE in WDTCSR to change the prescaler
	/* Set prescaler(time-out) value = 256K cycles (~2.0 s) */
	WDTCSR = (1<<WDP2) | (1<<WDP1) | (1<<WDP0);
	sbi(WDTCSR, WDIE);	// Watchdog Enabled

	sbi(TCCR0A, CS01);		// timer 0
	sbi(TCCR0A, CS00);
	sbi(TIMSK0, TOIE0); 	// enable timer 0 overflow interrupt

	INPUT_PIN = 0xff;	// Enable the Input Port internal pull-ups
	INPUT_DDR = 0x00;	// Initialize Input Port as inputs
	
 	OUTPUT_DDR = 0xff;
	DS_DDR = ( BIT(DS1_PIN) | BIT(DS2_PIN) );	// PC0 & PC1 set as outputs for DS switches
	
	sbi(PCICR,PCIE2);
	PCMSK2 = BIT(PCINT16)|BIT(PCINT17)|BIT(PCINT20)|BIT(PCINT21)|BIT(PCINT23)|BIT(PCINT22); // Enable Interrupts on inputs
	
	sei(); // Turn on interrupts
	getInputs();

//	MAIN LOOP:
	for(;;)	{
		
		//	Poll the Door 1 contact
		if( button_is_pressed(INPUT_PIN, DR1_BUTTON) && dr1_active==false ) {
			if ( mode == DEPENDENT )
			dependentRetract();
			else {
				if(dr1_toggleMode==true) {
					dr1_toggled = !dr1_toggled;
					activateDoor1(dr1_toggled);
				} else {
					dr1_active = true;
					activateDoor1(true);
					door1_timer(true);
				}
			}
		}
		
		//	Poll the Door 2 contact
		if( button_is_pressed(INPUT_PIN, DR2_BUTTON) && dr2_active==false ) {
			if ( mode == DEPENDENT )
				dependentRetract();
			else {
					if(dr2_toggleMode==true) {
						dr2_toggled = !dr2_toggled;
						activateDoor2(dr2_toggled);
					} else {
						dr2_active = true;
						activateDoor2(true);
						door2_timer(true);
					}
				}
		}
	
	//	Reprocess inputs if any have changed
 		if( inputReg != (INPUT_PIN & inputPinMask) ) {
			 getInputs();
 		}

	//	If either door is active, deactivate them when their timers expire
		if( dr1_active || dr2_active) {

			if( dr1_active && (door1_timer(false) >= getHoldTime(1)) ) {
				// Maintain Door 1 output while the button is held
				if(bit_is_clear(INPUT_PIN, DR1_BUTTON))
					door1_timer(true);
				else {				
					activateDoor1(false);
					dr1_active = false;
				}					
			}

			if( dr2_active && (door2_timer(false) >= getHoldTime(2)) ) {
				// Maintain Door 2 output while the button is held
				if(bit_is_clear(INPUT_PIN, DR2_BUTTON))
					door2_timer(true);
				else {
					activateDoor2(false);
					dr2_active = false;
				}					
			}
		}
		wdt_reset(); // Reset the watchdog timer
	}

}