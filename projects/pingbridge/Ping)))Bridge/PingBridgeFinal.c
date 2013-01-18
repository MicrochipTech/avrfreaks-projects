/*
 * PingBridge.c
 *
 * Created: 12/8/2012 9:23:20 PM
 *  Author: Daniel Mack
 * 
 * Processor: ATMEGA88PA
 * Speed: 3.69 MHz 
 * Compiles with avr-gcc
 * 
 * SPI : Uses SPI in slave mode
 * 
 * Function:
 * Sits inbetween at the Parallax Ping ))) sonar and another micro or computer.
 * Returns a raw 16 bit number  to indicate distance. This leaves the  master
 * micro free to do other task . Can do about 55 measurements per second, 
 * depending on distance. Measurements per second is limited by the speed 
 * of sound. 
 * 
 *  Distance  per unit count is found by the following formula:
 * (Fclk^-1)*V*4
 * Fclk = AVR clock speed
 * V = velocity of sound (about 340 meters per second)
 *  4 = timer prescale/2 (round trip, sound leaves the  Ping)) and then comes back)
 * 
 * 7885 timer gives about 2.9 metes at standard presure and temperature
 * 
 * Connections:
 * PORTB PIN0 from AVR connects to the SIG pin of the Ping))).
 *
 * SPI connects to second micro/computer  
 * 
 */ 


// includes
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// defines

/* timeout at about 9.5 feet, set time out to 2148.5 per MHz,.
 * You may decrease the value to lower the range, you may also try
 * to increase to get a little more range but 2148.5 was choses based
 * on the Ping))) 's datasheet max range/own timeout.
 */
#define TIMEOUT 7885
#define low(x) ((x) & 0x00FF)
#define high(x) (((x)>>8)& 0x00FF)

// function prototypes
uint16_t time_stamp(uint8_t);

// global variables
volatile uint8_t edgeDetect = 0;
volatile uint8_t conversionComplete = 0;
volatile uint16_t distance = 0;

int main(){
	// initialize hardware
	// Ports
	DDRB = (1<<PB0) | (1<<PB4); // MISO and PB0
	// spi
	SPCR = (1<<SPIE) | (1<<SPE);	// interrupt, f/4
	SPDR = 0;
	// timer counter 1
	OCR1A = TIMEOUT;
	uint16_t risingEdge = 0;
	uint16_t fallingEdge = 0;
	
	while(1){
		// trigger the sonar with a 5 micro second pulse
		DDRB |= (1<<PB0);
		cli();
		// atmega88pa pins can be flipped by writing to them
		// hence the two writes and not one write and one
		// clear
		PINB |= (_BV(PB0));
		_delay_us(5);
		PINB |= (_BV(PB0));
		sei();
		//Switch PB0 to input to receive pulse from Ping)))
		DDRB &=~(1<<PB0);
		// Start Timer
		// set timer up to capture incoming pulse
		// rising edge, ctc, OCR1A as top, f/8
		TCCR1B = (1<<ICES1) | (1<<WGM12) | (1<<CS11);
		// clear pending interrupts
		TIFR1 |= (1<<ICF1) | (1<<OCF1B) | (1<<OCF1A);
		// input capture compA interrupt enabled
		TIMSK1 = (1<<ICIE1) | (1<<OCIE1A);
		
		// look for rising edge, timestamp it then look for falling edge, timestamp it.
		// Do math to find distance. Distance is just a number of counts not normal units
		// ie inches or cm.
		while(conversionComplete == 0){
			if(edgeDetect == 1)
				risingEdge = time_stamp(0);
			else if(edgeDetect == 3){
				cli();
				fallingEdge = time_stamp(1);
				distance = fallingEdge - risingEdge;
				SPDR = low(distance);
				conversionComplete = 1;
				sei();
			}
		}
		// delay 200 microseconds before next reading 
		// as instructed by Ping))) datasheet
		// stop and reset timer
		conversionComplete = 0;
		TCCR1B = 0x00;
		TCNT1 = 0x0000;
		TIMSK1 = 0x00;
		_delay_us(200);
	}
}

// functions

// returns timer value at rising edge, sets timer to 
// look for falling edge and returns timer value
// at falling edge
uint16_t time_stamp(uint8_t c){
	switch(c){
		case 0:
			edgeDetect++;
			TCCR1B &= ~_BV(ICES1);
			break;
		case 1:
			edgeDetect = 0;
			break;
	}
	return(ICR1);
}

// ISR
ISR(SPI_STC_vect){
	SPDR = high(distance);
}

// reinitialize after timeout
ISR(TIMER1_COMPA_vect){
	conversionComplete = 1;
	edgeDetect = 0;
	SPDR = 0;
	distance = 0;
}

// lets main loop know rising or falling edge has
// been detected
ISR(TIMER1_CAPT_vect){
	edgeDetect++;
}
