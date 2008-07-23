/* -----------------------------------------------------------------------
 * Title:    Buggy, a programmable interactive LED for kids
 * Website:	 http://www.projectbuggy.blogspot.com/
 * Author:   Kenneth Olsen <kenneth_olsen at juno.com>
 * 			 Based on work by Alexander Weber <alexander.weber.0 at gmail.com>
 * 			 http://www.instructables.com/id/Programmable-LED/
 * Date:     June / July 2008
 * Hardware: ATtiny44v
 * Software: WinAVR 20060421, AVR Studio 4.14, AVR Dude
 */

/* PHYSICALS - ATtiny44V DIP-14
Pin Prt 	Device
1	VCC		+3v
2			open
3			open
4			open
5			open
6	PA7		(PCINT7) tactile normally-open button
7	PA6		(OCIA)piezo positive lead
8	PA5		Led	
9	PA4		Led 
10	PA3		IR photo diode cat	
11	PA2		(ADC2) 	Cds Read
12	PA1		ADC_Enable
13	PA0		(ADC0)	IR photo trans read
14	GND		GND
*/

#define F_CPU 1000000				// internal oscillator
#include <avr/io.h>
#include <avr/interrupt.h> 
#include <avr/sleep.h>
#include <stdlib.h> 
//#include <util/delay.h>			// used homemade delay

// Typedefs 
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

// Constants
#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0
#define ON 1
#define OFF 0

// Pins
#define IrRead 0
#define ADC_ENA_BIT 1
#define CdsRead 2
#define irBit   3
#define ledBit2 4
#define ledBit1 5
#define speakerPos 6  	
#define switchBit 7		

// Global Varbs
u8 maxNotes = 40;
volatile u8 tenths = 0;					// stores timer in tenths of second (up to 6.4 sec)
volatile u8 twelvths = 0;				// kepts track of 1/12 minute increments (upto 21 min?)
u16 Ticks256 = 0; 						// timer variable

u8 biColor = 1;							// 1 if red/green bicolor LED, 0 if one color
volatile u8 ledColor = 2;				// 2 = green / 1 = red
volatile u8 lightState = 2;				
volatile u8 lastState = 2;


u8 ambientVis = 0;
u8 ambientIr = 0;
u8 thresholdHiVis = 0;					// calculated offset for change
u8 thresholdLowVis = 0;
u8 thresholdHiIr = 0;					// calculated offset for change
volatile u8 readingVis = 0;				// holds last CdS reading
volatile u8 readingIr = 0;	
u8 song[40];							// array to store the led data
u16 amplitude[40];
u8 noteCount = 0;						// keep track of number of notes (upto 40)
u8 blinkTime = 0;
volatile u8 lightCount = 0;
u8 sound = ON;
u16 keys[9] = {0, 1966, 1745, 1559, 1470, 1309, 1166, 1041, 981};
volatile u8 irAmp = 0;
volatile u8 irCount = 0;


// Function prototypes (in order of appearance)
void 	recordIr(void);					// records ir light sequence
void 	recordVis(void);				// records visible light sequence
void	playing(void);					// plays recorded sequence
// TSRs are here ----------------------------------------------------
void	blink(void);					// eyes off for brief time
void	checkAmbient(void);				// recorde ambient vis / ir levels
void	chirp(void);					// emits short chirping noise
void	chirpDown(void);
void	decFlash(u16 value);			// flash 16 bit number in green / red (troubleshooting only)
void	delay(u16 value);				// homemade delay function (in milliseconds)
u16		digitalRead(u8 port);			// reads port
void    digitalWrite(u8 port, u8 value);// writes to port (HIGH / LOW)
void	flash(u8 value);				// blinks eyes x times
u16 	get_adc(u8 channel);			// reads CdS cell
void 	ir(u8 value);					// turn IR led on/off
u16		keyLookUpIr(u8 value);			// converts 8 bit ADC reading to 16 bit tone value
u16		keyLookUpVis(u8 value);			// converts 8 bit ADC reading to 16 bit tone value
void	led(u8 value);					// turns led off(0), red(1), or green(2)
void    portMode(u8 port, u8 mode);		// set port mode (INPUT / OUTPUT)
u16		randNum(u16 n);					// generate 16 bit random number
void 	setup(void); 					// configures ports and timers
void	shutDown(void);					// enter low-power mode (button or timeout)
void 	writeOCR1A(u16 i );				// writes timer high for PWM sound


// Main loop
int main(void)
{
	setup(); 							// configure  ports & timer
	//chirp();
	//delay(500);
	//chirpDown();
	while(1){
		writeOCR1A(0);  				// speaker off
		u16 curLight = 0;
		delay(10);
		checkAmbient();					// record ambient vis / ir levels
		lastState = lightState;
		while(lightState == lastState && readingIr < thresholdHiIr){
			readingVis = get_adc(CdsRead);	// check CdS
			curLight = keyLookUpVis(readingVis);
			if ((readingVis >= thresholdLowVis) && (readingVis <= thresholdHiVis))
				{lightState = ledColor;}
			else{lightState = 0;}
			readingIr = get_adc(IrRead);
			led(lightState);
			delay(10);
			}

		if(lightState != lastState){recordVis();}
		else{recordIr();}
		playing();
	}
}


void recordVis(void){

			u8 delayWait = 15;
			blinkTime = 0;
			//chirp();
			tenths = 0;
			twelvths = 0;
			lastState = lightState;
			u16 lastLight = keyLookUpVis(readingVis);
			u16 curLight = lastLight;
			led(lightState);
			noteCount = 0;
			while(noteCount < maxNotes){
				while(lightState == lastState && tenths < delayWait && curLight == lastLight){
				//while(lightState == lastState && tenths <= delayWait){
				//while(tenths <= delayWait && curLight == lastLight){
					readingVis = get_adc(CdsRead);			// check CdS
					curLight = keyLookUpVis(readingVis);
					if ((readingVis >= thresholdLowVis) && (readingVis <= thresholdHiVis))
						{lightState = ledColor;}
					else{lightState = 0;}					
					if(sound){writeOCR1A(curLight);}
					delay(50);								// keeps from taking too many recordings
				}				
				amplitude[noteCount] = lastLight;
				led(lightState);
				song[noteCount] = lastState << 6;
				song[noteCount] = song[noteCount] | tenths;
				noteCount++;
				if (tenths >= delayWait){playing();}
				tenths = 0;
				lastState = lightState;
				lastLight = curLight;
				blinkTime = 0;
			}
		playing();
		}


void recordIr(void){

			u8 delayWait = 20;
			chirpDown();
			//flash(1);
			blinkTime = 0;
			tenths = 0;
			twelvths = 0;
			u16 lastLight = keyLookUpIr(readingIr);
			u16 curLight = lastLight;
			led(lightState);
			noteCount = 0;
			while(noteCount < maxNotes){
				while(lightState == lastState && tenths < delayWait && curLight == lastLight){
					readingIr = get_adc(IrRead);					
					curLight = keyLookUpIr(readingIr);
					if ((readingIr <= thresholdHiIr))
						{lightState = ledColor;}
					else{lightState = 0;}
					//if(sound){writeOCR1A(curLight);}
					delay(50);
				}				
				amplitude[noteCount] = lastLight;
				led(lightState);
				song[noteCount] = lastState << 6;
				song[noteCount] = song[noteCount] | tenths;
				if (tenths >= delayWait){playing();}
				tenths = 0;
				noteCount++;
				lastState = lightState;
				lastLight = curLight;
				blinkTime = 0;
			}
		playing();
		}


void playing(void)
		{
			blinkTime = 0;
			portMode(irBit, OUTPUT);				// enable IR 
			flash(1);
			delay(100);
			u8 x = 0;
			noteCount--;
			while(x < noteCount){
				u8 tempLed = song[x] & 0b11000000;  // mask first two bits
				tempLed = tempLed >> 6;				// shift first two over
				u8 tempNote = song[x] & 0b00111111; // mask remaining bits
				if(sound){writeOCR1A(amplitude[x]);}
				irAmp = amplitude[x] / 180;
				led(tempLed);
				delay(tempNote*50);
				x++;			
				blinkTime = 0;
			}
			writeOCR1A(0);
			irAmp = 0;
			portMode(irBit, INPUT);					// disable IR
			//chirp();
			flash(1);
			main();
		}


// INTERUPT SERVICE ROUTINES ----------------------------------------------------------------

ISR(PCINT0_vect)  //PIN CHANGE TUTORIAL http://www.windmeadow.com/node/19
{

    if ((PCMSK0 & _BV(switchBit)) && !(PINA & _BV(switchBit))) { // button
		
		if(biColor == 1){ledColor++;}
		if(ledColor == 3){ledColor = 1;}
		led(ledColor);
		sound = (!sound);
		if(sound){writeOCR1A(981);}
		uint16_t temp16 = 0;
		while ( !(PINA & _BV(switchBit)) && (temp16 <= 20) ) {
		delay(10);
		temp16++;
		}
		if (temp16 >= 20) {				// held down for a long time, time to power down
			led(0);
			writeOCR1A(0);
			while (! (PINA & _BV(switchBit)) );
			delay(10);
			shutDown();
			} 
	OCRA1 = 0; 							// sound off
	main();
	} 
}


ISR(TIM0_OVF_vect) // Timer 0 Overflow Interrupt handler
{ 				   // tutorial at http://80.232.32.135/index.php?name=PNphpBB2&file=viewtopic&t=36895

	Ticks256++;  					// 256 ticks have gone by 
	irCount++;
	if(irCount < irAmp){ir(ON);}	// crude attempt at PWM for IR led
	else{ir(OFF);}	
	irCount++;
	if(irCount == 10){irCount = 0;}
	
	if (Ticks256 == 59) 			// ~ 0.1s has past (see website for math)
	{ 
		tenths++;					// increment counter
		Ticks256 = 0; 				// reset counter
		
		if(tenths == 50){
			blinkTime++;
			tenths = 0;
			twelvths++;
			if(blinkTime == 8){
				blinkTime = 0;
				blink();
				chirp();
				}
			if (twelvths == 100){
				twelvths = 0;
				shutDown();
			}
		}
	} 
} 

// END INTERUPT SERVICE ROUTINES --------------------------------------------------------------


void blink(void){			// turns eyes of for a short period

	led(0);
	delay(50);
	led(lightState);
}

void checkAmbient(void){  	// check ambient vis / ir levels

	u8 offSetVis = 0;
	ambientVis = get_adc(CdsRead);			// get ambient light level
	ambientIr = get_adc(IrRead);	
	if(ambientVis >= 100){offSetVis = ambientVis >> 2;}// divide by four
	else{offSetVis = 10;}				

	if (ambientVis <= 191){thresholdHiVis = ambientVis + offSetVis;}
	else{thresholdHiVis = 255;}
	
	if(ambientVis >= 25){thresholdLowVis = ambientVis - offSetVis;}
	else{thresholdLowVis = 0;}

	if (ambientIr < 245){thresholdHiIr = ambientIr + 10;}
	else{thresholdHiIr = 255;}

}


void chirp(void){  							// makes a quick audible chirp sound

		irAmp = 10;
		if(sound){
			for(u16 x = 500; x > 50; x = x - 20){
			writeOCR1A(x);
			delay(1);
			}
		}
		writeOCR1A(0);
		irAmp = 0;
}

void chirpDown(void){  							// makes a quick audible chirp sound

		irAmp = 10;
		if(sound){
			for(u16 x = 10; x < 500; x = x + 20){
				writeOCR1A(x);
				delay(2);
			}
		}
		writeOCR1A(0);
		irAmp = 0;
}



void decFlash(u16 value)					// Flashes 2 byte value in binary (green = 1, red = 0) 
{  											// (for troubleshooting)
	cli(); 									// disable interrupts just in case 
	u16 flashValue;
	u8 delayTime = 100;
	u8 foundFirst = 0;	
	int y = 10000;
	led(0);
	delay(delayTime);
	while(y > 0){
		if(value >= y){
			foundFirst = 1;
			flashValue = value / y;
			for(int x = 0; x < flashValue; x++)
			{
				led(2);
				delay(delayTime);
				led(0);
				delay(delayTime);
			}
			value = value - (y * flashValue);
			delay(delayTime * 5);
		}
		else
		{
			if(foundFirst == 1)
			{
				led(1);
				delay(delayTime);
				led(0);
				delay(delayTime * 5);
			}
		}
		y = y / 10;
	}
	if(foundFirst == 0){
		led(1);
		delay(delayTime);
		led(0);
	}
	delay(delayTime * 10);				// pause before proceeding
	sei(); 								// enable interrupts 
}


void delay(u16 value){  				// delay function (approximate, delay(); interferes with timer
	
	value = value * 10;
	for(u16 x=0; x < value; x++){
		u16 check = get_adc(CdsRead); 	// takes 25 cycles?
		check++; 						//just to avoid warning
	}

}


u16 digitalRead(u8 port)  // Gets portA bit
{
	u16 temp = (PINB & (1 << port));
	return temp;
}


void digitalWrite(u8 port, u8 value) // Sets portA bit HIGH (1) or LOW (0)
{
	if (value == 0)
		{PORTA &= ~(1 << port);}	// Set bit # 'position' low
	else
		{PORTA |= (1 << port);}		// Set bit # 'position' high
 }


void flash(u8 value){  				// Flash led x times
	cli(); 							// disable interrupts just in case 
	for(int x=0; x <= value; x++){
		led(1);
		delay(50);
		led(2);
		delay(50);
	}
	led(lightState);
	sei(); 							// enable interrupts
}


u16 get_adc(u8 channel) { // Return the 10bit value of the selected adc channel.
	
	PORTA |= (1 << ADC_ENA_BIT); 	// enable voltage for adc 	
	ADCSRA = 						
		(1 << ADEN) |				// enable ADC
		(1 << ADPS1) | (1 << ADPS0);// set prescaler to 8	
	ADCSRB = (1 << ADLAR);			// left adjust (see section 16.13.3.2)
	ADMUX = channel;				// select channel
	ADMUX |= (0 << REFS0);			// use internal reference
	ADMUX |= (1 << REFS1);
	ADCSRA |= (1 << ADSC);			// warm up the ADC, and 
	while (ADCSRA & (1 << ADSC)); 	// discard the first conversion
	ADCSRA |= (1 << ADSC);			// start single conversion
	while (ADCSRA & (1 << ADSC)); 	// wait until conversion is done
	PORTA &= ~(1 << ADC_ENA_BIT);	// disable voltage for adc
	return ADCH;					// return 8 bit value 
}


void ir(u8 value)
{
	if (value == 0)					// 0 = ir off
	{digitalWrite(irBit, LOW);}
	if (value == 1)					// 1 = ir on
	{digitalWrite(irBit, HIGH);}
}
	

u16 keyLookUpIr(u8 value){
	
	u16 key = 0;
	u8 x = value >> 5;
	if (x < 8 ){key = keys[x];}
	else{key = 0;}
	return key;
}


u16 keyLookUpVis(u8 value){
	
	u8 dark = 30;
	u8 range = 0;
	u8 x = 0;
	u8 span = 0;
	u16 key = 0;

	if (value <= ambientVis){
		range = ambientVis - dark;
		span = range / 8;
		if (value >= dark){x = 8 - ((value - dark) / span);}
		else {x = 8;}
	}
	else {
		range = 255 - ambientVis;
		span = range / 8;
		x = (value - ambientVis) / span;
	}
	if (x <= 8 ){key = keys[x];}
	else{key = 8;}
	return key;
}


void led(u8 value)  // Change LED state (0 off / 1 red / 2 green)
{
	if (value == 0)					// 0 = off
	{
		digitalWrite(ledBit1, LOW);
		digitalWrite(ledBit2, LOW);
	}
	if (value == 1)					// 1 = green
	{
		digitalWrite(ledBit1, HIGH);
		digitalWrite(ledBit2, LOW);
	}

	if (value == 2)					// 2 = red
	{
		digitalWrite(ledBit1, LOW);
		digitalWrite(ledBit2, HIGH);
	}
}


void portMode(u8 port, u8 mode) // Set portA bit as INPUT (0) or OUTPUT (1)
{
	if (mode == 0){DDRA &= ~(1 << port);}      // Set bit # 'position' low
	else{DDRA |= (1 << port);}       // Set bit # 'position' high
}


u16 randNum(u16 n)  // return a 16-bit random number between 0 and n
{
	int x = rand() / (RAND_MAX / n + 1);
	return x;
}


void setup(void)    					// Configure device settings
{ 
	cli(); 								// disable interrupts just in case 
	srand(get_adc(CdsRead));    		// see random number generator
	portMode(ledBit1, OUTPUT);
	portMode(ledBit2, OUTPUT);
	portMode(ADC_ENA_BIT, OUTPUT);
	portMode(switchBit, INPUT);
	digitalWrite(switchBit, HIGH); 		// enables internal pull-up
	portMode(speakerPos, OUTPUT);  		
	portMode(CdsRead, INPUT); 			//(see data sheet section 16.13.5)
	digitalWrite(CdsRead, HIGH);

	// Timer/Counter Register
	TCNT0 = 0x00;   					// clear 8 bit Timer/Counter 

	//START_TIMERS; 
	TCCR0B |= (1 << CS01);				// 8bit clock select prescaler 8
 
	//// Timer/Counter Interrupt Mask
	TIMSK0 |= (1 << TOIE0);  			// Timer/Counter0 Overflow Interrupt Enable
	//TIMSK0	|= (1<<OCIE0A); 		// enable output compare interrupt for OCR1A

	//16 bit timer tutorials
	//http://winavr.scienceprog.com/avr-gcc-tutorial/program-16-bit-avr-timer-with-winavr.html
	//http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=50106
	
	TCCR1B |= (1 << WGM12); 			// 16bit CTC mode
	TCCR1B |= (1 << CS10);  			// 16bit start timer, no prescaler
	TCCR1A |= (1 << COM1A0); 			// 16bit Output compare CdsRead A in toggle mode

	PCMSK0 |= (1<<switchBit);			// Pin Change Mask Register

   	// MCU Control Register				//Interrupt Sense Control
	 MCUCR = (0<<ISC01) | (0<<ISC00); 	// low level
	// MCUCR = (0<<ISC01) | (1<<ISC00); // logical change
 	// MCUCR = (1<<ISC01) | (0<<ISC00); // falling edge
  	// MCUCR = (1<<ISC01) | (1<<ISC00); // rising edge
  
	// General Interrupt Mask Register
	// GIMSK  |= (1<<INT0);				// External Interrupt Request 0 Enable (bit 6)
	GIMSK  |= (1<<PCIE0); 				// Pin Change Interrupt Enable (bit 5)

	// General Interrupt Flag Register
	//GIFR |= (1<<INTF);				// External Interrupt Flag 0
	GIFR |= (1<<PCIF0);					// Pin Change Interrupt Flag

	sei(); 								// enable interrupts 
} 


void shutDown(void) 		// Enter low-power mode (button off or time out)
{	
 	sei();
	led(0);
	ir(OFF);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
    sleep_mode();
	sleep_disable();        	
	tenths = 0;
	twelvths = 0;
	sei();
	main();
}


void writeOCR1A(u16 i )   	// Routine to set timer1 Top (see spec, Sect 12.3)
{
	u8 sreg;
	sreg = SREG;    		/* Save global interrupt flag */
    cli();                  /* Disable interrupts */
    OCR1A = i;              /* Set TCNT1 to i */
    SREG = sreg;    		/* Restore global interrupt flag */
}






