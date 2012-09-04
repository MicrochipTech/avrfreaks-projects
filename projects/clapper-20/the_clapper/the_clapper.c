/*
	The Clapper 2.0
	Like the original clapper, this is designed to trigger a relay when a clapping sequence is heard. But unlike the original 
	clapper, the clapping sequence is programmable via a button and is stored in non-volatile memory so that the sequence is 
	not lost on reset. The programming button also doubles as a toggle switch when held for a programmed amount of time. The 
	microphone sensitivity is adjustable via a pot connected to ADC channel 1.

	Parts of this code originally by:
    Steve Hoefer http://grathio.com
    Version 0.1.10.20.10
    Licensed under Creative Commons Attribution-Noncommercial-Share Alike 3.0
    http://creativecommons.org/licenses/by-nc-sa/3.0/us/
 
 */ 

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/portpins.h>
#include <avr/eeprom.h>
#include "defines.h"

#define BUTTON_PIN_PORT PINB
#define BUTTON_PORT		PORTB
#define BUTTON_DDR		DDRB
#define PROGRAM_BTN_PIN PINB0

#define OUTPUTS_PORT	PORTD
#define OUTPUTS_DDR		DDRD
#define RELAY_PIN		PD0
#define RED_LED_PIN		PD1
#define GRN_LED_PIN		PD2

// Tuning constants
const int rejectValue = 25;			// Fails if an individual clap is off by this percentage
const int averageRejectValue = 15;	// Fails if the average timing of the claps is off by this percent
const int clapFadeTime = 100;		// milliseconds we allow a clap to fade before we listen for another one. (Debounce)
const int maximumClaps = 10;		// Maximum number of claps we listen for.
const int clappingComplete = 1000;	// Longest time to wait for a clap before we assume that it's finished.
const int holdTriggerTime = 600;	// Time the button is held before triggering the output

// Variables.
int threshold;
int secretCode[] = {100, 50, 50, 0, 0, 0, 0, 0, 0, 0 };  // Default setup
int clapReadings[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint16_t clapSensorValue = 0;		// Last reading of the clap sensor.
boolean programmingFlag = false;	// Flag so we remember the programming button setting at the end of the cycle.

void listenToClaps(void);
void triggerRelay(void);
boolean checkClaps(void);
void writeToEEPROM(void);
void readFromEEPROM(void);
uint16_t analogRead(void);
long getThreshold(void);

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

// Timer0 overflow interrupt
ISR(TIMER0_OVF_vect) {
//	wdt_reset();
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;
	
	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}
	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;
}

unsigned long millis(void) {
	unsigned long mil;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mil = timer0_millis;
	}
	return mil;
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

unsigned long micros(void) {
	unsigned long m;
	uint8_t t;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		m = timer0_overflow_count;
		#if defined(TCNT0)
		t = TCNT0;
		#elif defined(TCNT0L)
		t = TCNT0L;
		#else
		#error TIMER 0 not defined
		#endif

		
		#ifdef TIFR0
		if ((TIFR0 & _BV(TOV0)) && (t < 255))
		m++;
		#else
		if ((TIFR & _BV(TOV0)) && (t < 255))
		m++;
		#endif
	}	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

void delay(unsigned long ms) {
	uint16_t start = (uint16_t)micros();

	while (ms > 0) {
		if (((uint16_t)micros() - start) >= 1000) {
			ms--;
			start += 1000;
		}
	}
}

int main(void) {
	
	OUTPUTS_DDR = (1<<RELAY_PIN)|(1<<RED_LED_PIN)|(1<<GRN_LED_PIN);
	OUTPUTS_PORT = (1<<GRN_LED_PIN);
	
	BUTTON_PORT = bit(PROGRAM_BTN_PIN); // Internal pull-ups on inputs
	BUTTON_DDR = bit(PROGRAM_BTN_PIN);
	
	DIDR0 = bit(ADC0D)|bit(ADC1D); // Disable PC0 & PC1 digital inputs
	
	cbi(PRR,PRADC);	// Turn on ADC
	ADCSRA |= bit(ADPS0)|bit(ADPS1)|bit(ADPS2); // ADC Speed = ck/128
	ADMUX |= bit(REFS0); // AVcc Reference, Channel = ADC0
	sbi(ADCSRA,ADEN); // Enable ADC
	
	TCCR0A |= bit(CS01)|bit(CS00); // Initialize timer0 with a prescaler of 64
	sbi(TIMSK0, TOIE0); 	// enable timer 0 overflow interrupt
	
	readFromEEPROM(); // Read the previously stored sequence, if it exists
	
	sei(); // enable global interrupts
	
	boolean btnReleased = true; // when button is held, allows only one relay trigger until the button is released
	
    while(1) {
		
		// Get the analog values
		threshold = getThreshold(); // Set the sensor threshold
		clapSensorValue = analogRead(); // Listen for a clap
  
		// See if the programming button is pressed
		if ( bit_is_clear(BUTTON_PIN_PORT, PROGRAM_BTN_PIN) ) {
			programmingFlag = true;	// Set the programming flag
			// Loop while the button is held
			unsigned int start = millis();
			while( bit_is_clear(BUTTON_PIN_PORT, PROGRAM_BTN_PIN) ) {
				// See if the button has been held longer than the set hold time
				if( btnReleased && ((millis()-start) >= holdTriggerTime) ) {
					programmingFlag = false;	// Remove the programming flag
					btnReleased=false;			// Change the button released flag
					triggerRelay();				// Trigger the relay
				}
			}
		} else btnReleased = true; /* Else, the button has been released so change the flag */
		
		// Turn on the Red LED if we're programming a new clap
		if ( programmingFlag )
			sbi(OUTPUTS_PORT,RED_LED_PIN);	
		else
			cbi(OUTPUTS_PORT,RED_LED_PIN);
  
		// Listen to claps if analog reading is greater than threshold
		if ( clapSensorValue >= threshold )
			listenToClaps();
	}

}

// Records the timing of claps.
void listenToClaps(void) {

	// Reset the clap array
	for (int i=0;i<maximumClaps;i++)
		clapReadings[i]=0;
	
	int currentClapNumber=0; // Incrementer for the array.
	unsigned int start = millis(); // Reference for when this clap started.
	unsigned int now = millis();
	
	cbi(OUTPUTS_PORT, GRN_LED_PIN); // we blink the LED for a bit as a visual indicator of the clap.
	
	if(programmingFlag==true)
		cbi(OUTPUTS_PORT, RED_LED_PIN); // and the red one too if we're programming a new clap.
		
	delay(clapFadeTime); // wait for this peak to fade before we listen to the next one.
	sbi(OUTPUTS_PORT, GRN_LED_PIN);
	
	if (programmingFlag==true)
		sbi(OUTPUTS_PORT, RED_LED_PIN);

	do {
		//listen for the next clap or wait for it to timeout.
		clapSensorValue = analogRead();

		if (clapSensorValue >=threshold) //got another clap...
		{
			//record the delay time.
			now = millis();
			clapReadings[currentClapNumber] = now-start;
			currentClapNumber ++; // increment the counter
			start=now;
			// and reset our timer for the next clap
			cbi(OUTPUTS_PORT, GRN_LED_PIN);

			if (programmingFlag==true)
				cbi(OUTPUTS_PORT, RED_LED_PIN); // and the red one too if we're programming a new clap.
			
			delay(clapFadeTime); // again, a little delay to let the clap decay.
			sbi(OUTPUTS_PORT, GRN_LED_PIN);
			if (programmingFlag==true)
				sbi(OUTPUTS_PORT, RED_LED_PIN);
		}
		
		now=millis();
	
		//did we timeout or run out of claps?
		} while ((now-start < clappingComplete) && (currentClapNumber < maximumClaps));

		// We've got our clap recorded, lets see if it's valid
		if (programmingFlag==false) {
			if (checkClaps() == true)
				triggerRelay();
			else {
				cbi(OUTPUTS_PORT, GRN_LED_PIN); // We didn't unlock, so blink the red LED as visual feedback.
				for (int i=0;i<4;i++){
					sbi(OUTPUTS_PORT, RED_LED_PIN);
					delay(100);
					cbi(OUTPUTS_PORT, RED_LED_PIN);
					delay(100);
				}
				sbi(OUTPUTS_PORT, GRN_LED_PIN);
			}
		} else {
			checkClaps();		// We're in programming mode so check the claps to capture the data
			writeToEEPROM();	// Write the new values to EEPROM
			
			// Blink the LED's alternately to show that program is complete.
			cbi(OUTPUTS_PORT, RED_LED_PIN);
			sbi(OUTPUTS_PORT, GRN_LED_PIN);
			for (int i=0;i<3;i++) {
				delay(100);
				sbi(OUTPUTS_PORT, RED_LED_PIN);
				cbi(OUTPUTS_PORT, GRN_LED_PIN);
				delay(100);
				cbi(OUTPUTS_PORT, RED_LED_PIN);
				sbi(OUTPUTS_PORT, GRN_LED_PIN);      
			}
		}
}

void triggerRelay(void){

	sbi(OUTPUTS_PORT, GRN_LED_PIN); // Turn on the Green LED
	
	// toggle the relay output
	uint8_t val = bit_get( OUTPUTS_PORT, bit(RELAY_PIN) ); 
	bit_write( !val, OUTPUTS_PORT, bit(RELAY_PIN) );
	
	// Blink the green LED a few times for more visual feedback.
	for (int i=0; i < 4; i++){
		cbi(OUTPUTS_PORT, GRN_LED_PIN);
		delay(75);
		sbi(OUTPUTS_PORT, GRN_LED_PIN);
		delay(75);
	}
	
}

// Sees if our clap matches the programmed sequence
boolean checkClaps(void){
 
  // Check if the number of claps
  unsigned int currentClapCount = 0;
  unsigned int secretClapCount = 0;
  unsigned int maxClapInterval = 0;   // We use this later to normalize the times.
  
  for (int i=0;i<maximumClaps;i++)  {
    if (clapReadings[i] > 0)
		currentClapCount++;
    if (secretCode[i] > 0)
		secretClapCount++;
    if (clapReadings[i] > maxClapInterval) 	// collect normalization data while we're looping.
		maxClapInterval = clapReadings[i];
  }
  
  // If we're recording a new clap, save the info and get out of here.
  if (programmingFlag==true) {
	  
      for (int i=0;i<maximumClaps;i++) // normalize the times
		secretCode[i]= map(clapReadings[i],0, maxClapInterval, 0, 100); 

      // And flash the lights in the recorded pattern to let us know it's been programmed.
      cbi(OUTPUTS_PORT, GRN_LED_PIN);
      cbi(OUTPUTS_PORT, RED_LED_PIN);
      delay(450);
      sbi(OUTPUTS_PORT, GRN_LED_PIN);
      sbi(OUTPUTS_PORT, RED_LED_PIN);
      delay(50);
      for (int j=0; j<maximumClaps ; j++) {
        cbi(OUTPUTS_PORT, GRN_LED_PIN);
        cbi(OUTPUTS_PORT, RED_LED_PIN);  
        // only turn it on if there's a delay
        if (secretCode[j] > 0) {                                   
          delay( map(secretCode[j],0, 100, 0, maxClapInterval) ); // Expand the time back out to what it was. Roughly. 
          sbi(OUTPUTS_PORT, GRN_LED_PIN);
          sbi(OUTPUTS_PORT, RED_LED_PIN);
        }
        delay(50);
      }
	  programmingFlag = false;
	  return false; 	// We don't flip the relay when we are recording a new clap.
  }
  
  if (currentClapCount != secretClapCount)
	return false; 
  
  //  Compare the relative intervals between our claps
  int totaltimeDifferences=0;
  int timeDiff=0;
  // Normalize the times
  for (int i=0;i<maximumClaps;i++) {
    clapReadings[i]= map(clapReadings[i],0, maxClapInterval, 0, 100);      
    timeDiff = abs(clapReadings[i]-secretCode[i]);
    if (timeDiff > rejectValue) // Individual value too far out of whack
      return false;
    totaltimeDifferences += timeDiff;
  }
  // Check the accuracy
  if (totaltimeDifferences/secretClapCount>averageRejectValue)
    return false; 

  return true;
}

const byte EEPROM_ID = 0x99;	// used to identify if valid data in EEPROM
const int ID_ADDR = 0;			// the EEPROM address used to store the ID
const int CODE_ADDR = 1;		// the starting EEPROM address used to store the clapping sequence data

void writeToEEPROM(void) {
	eeprom_busy_wait(); // Wait till EEPROM is ready
	eeprom_write_byte((unsigned char *) ID_ADDR, EEPROM_ID);
	int ADDR_COUNTER = CODE_ADDR;	// counter to increment the address location
	for(int i=0;i<maximumClaps;i++) { 
		eeprom_write_word(ADDR_COUNTER, secretCode[i]);
		ADDR_COUNTER += 2;
	}
}

void readFromEEPROM(void) {
	byte id = eeprom_read_byte(ID_ADDR);
	
	// If ID doesn't match, store the default sequence
	if( id != EEPROM_ID) {
		int secretCode[] = {100, 50, 50, 0, 0, 0, 0, 0, 0, 0 };
		writeToEEPROM();
	} else { // Else, load the previously saved sequence
		int ADDR_COUNTER = CODE_ADDR;
		for(int i=0;i<maximumClaps;i++) {
			secretCode[i] = eeprom_read_word(ADDR_COUNTER);
			ADDR_COUNTER += 2;
		}
	}
}

uint16_t analogRead(void) {
	uint8_t low, high;
	sbi(ADCSRA,ADSC); // Start Conversion
	while( bit_is_clear(ADCSRA,ADIF) ); // Wait for conversion to finish
	low = ADCL;
	high = ADCH;
	return (high << 8) | low; // Return the combined bytes
}

long getThreshold(void) {
	sbi(ADMUX, MUX0); // Select channel ADC1
	long val;
	uint16_t analogValue = analogRead();
	val = map(analogValue, 0, 1023, 50, 1000);
	cbi(ADMUX, MUX0);
	return val;
}