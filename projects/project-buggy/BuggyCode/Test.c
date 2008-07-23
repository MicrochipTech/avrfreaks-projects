/* -----------------------------------------------------------------------
 * Title:    Buggy, a programmable interactive LED for kids
 * Author:   Kenneth Olsen <kenneth_olsen at juno.com>
 * Based on work by Alexander Weber <alexander.weber.0 at gmail.com>
 * http://www.instructables.com/id/Programmable-LED/
 * Date:     May 2008
 * Hardware: ATtiny13v
 * Software: WinAVR 20060421, AVR Studio 4.14, AVR Dude
 */

/* PHYSICALS - ATtiny24/44/84V
Pin Prt 	Device
1	VCC		+5v / +3v
2	
3	
4	
5	
6	PCINT7	switch
7	OCIA	piezo pos
8	PA5		Led	
9	PA4		Led 
10	PA3		photo diode cat	
11	ADC2 	Cds Read
12	PA1		ADC_Enable
13	ADC0	photo trans read
14	GND		GND
*/

#define F_CPU 1200000				// 1.2MHz, internal oscillator
#include <avr/io.h>
#include <avr/interrupt.h> 
#include <avr/sleep.h>
#include <stdlib.h> 

// Typedefs 
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0
#define ON 1
#define OFF 0


#define ledBit1 5
#define ledBit2 4
#define irBit   3
#define ADC_ENA_BIT 1
#define CdsRead 2
#define PtRead 0
#define switchBit 7		
#define speakerPos 6  	//ocia


// Global Varbs
volatile u8 tenths = 0;					// stores timer in tenths of second (up to 6.4 sec)
volatile u8 twelvths = 0;						// kepts track of 1/12 minute increments (upto 21 min?)
u16 Ticks256 = 0; 						// timer variable
volatile u8 lightState = 2;				// current light levels
volatile u8 lastState = 1;						// remembers what light level we were at
u16 ambientVis = 0;						// holds initial light reading
u16 ambientIr = 0;
u16 thresholdHiVis = 0;							// calculated offset for change
u16 thresholdLowVis = 0;
u16 thresholdHiIr = 0;							// calculated offset for change
u16 thresholdLowIr = 0;
volatile u8 readingVis = 0;						// holds last CdS reading
volatile u8 readingIr = 0;	
u8 song[12];							// array to store the led data
u16 amplitude[12];
u8 noteCount = 0;					// keep track of number of notes (upto 40)
volatile u8 ledColor = 2;
volatile u8 lightCount = 0;
u8 sound = ON;
u8 soundCounter = 0;
u16 keys[8] = {1966, 1745, 1559, 1470, 1309, 1166, 1041, 981};
u8 irAmp = 0;


// Function prototypes
void    digitalWrite(u8 port, u8 value);// writes to port (HIGH / LOW)
void    portMode(u8 port, u8 mode);		// set port mode (INPUT / OUTPUT)
u16		digitalRead(u8 port);			// reads port
void	binFlash(u16 value);			// flashes 2 byte number binary on(green) off(red)
void	led(u8 value);					// turns led off(0), red(1), or green(2)
u16 	get_adc(u8 channel);			// reads CdS cell
void 	setup(void); 					// configures ports and timer
void	record(void);					// records light sequence
void	flash(u8 value);				// flashes led x times
void	delay(u16 value);				// homemade delay function (in milliseconds)
void 	recordVis(void);
void 	recordIr(void);
void	playing(void);
void	shutDown(void);
void	checkAmbient(void);
void	blink(void);
u16		randNum(u16 n);
void 	ir(u8 value);
void	decFlash(u16 value);
u16		keyLookUp(u16 value);
void	chirp(void);


// Main loop
int main(void)
{
	setup(); 							// configure  ports & timer
	while(1){
		OCR1A = 0;  
		flash(2);
		chirp();
		while(1){
			decFlash(get_adc(CdsRead));
			decFlash(get_adc(PtRead));			
			}
	}
}


void chirp(void){

		if(sound){
			portMode(speakerPos, OUTPUT);
			for(u16 x = 500; x > 50; x = x - 20){
			OCR1A = x;
			delay(1);}
			for(u16 x = 50; x < 500; x = x + 5){
			OCR1A = x;
			delay(1);}
			portMode(speakerPos, INPUT);
		}
}



void shutDown(void) // Sleep
      	{	
 			sei();
			led(0);
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();
        	sleep_mode();
			sleep_disable();        	
			tenths = 0;
			twelvths = 0;
			sei();
			main();
		}


//PIN CHANGE TUTORIAL http://www.windmeadow.com/node/19
//ISR(INT0_vect) // Detected switch push
ISR(PCINT0_vect)
{

    if ((PCMSK0 & _BV(switchBit)) && !(PINA & _BV(switchBit))) { // button
		ledColor++;
		if(ledColor == 3){
			ledColor = 1;
			if(sound == ON){sound = OFF;}
			else{sound = ON;}  //beep if sound on
		}
		if(sound){
			portMode(speakerPos, OUTPUT);
			OCRA1 = 981;
			portMode(speakerPos, INPUT);
		}
		led(ledColor);
		uint16_t temp16 = 0;
		while ( !(PINA & _BV(switchBit)) && (temp16 <= 20) ) {
		delay(10);
		temp16++;
		}
		if (temp16 >= 20) {// held down for a long time, time to power down
			led(0);
			while (! (PINA & _BV(switchBit)) );
			delay(10);
			shutDown();
			} 
    OCRA1 = 0; //beep off
	main();
	} 
}


// Timer 0 Overflow Interrupt handler 
// tutorial at http://80.232.32.135/index.php?name=PNphpBB2&file=viewtopic&t=36895
ISR(TIM0_OVF_vect) 
{ 

	Ticks256++;  					// 256 ticks have gone by 
	if(Ticks256 == irAmp){portMode(irBit, INPUT);}
	if (Ticks256 == 59) 			// ~ 0.1s has past (see website for math)
	{ 
		portMode(irBit, OUTPUT);
		//OCR1A = get_adc(CdsRead);
		tenths++;					// increment counter
		Ticks256 = 0; 				// reset counter
		readingVis = get_adc(CdsRead);	// check CdS
		//OCR1A = readingVis;
		//see if levels above or below ambient value by threshold amount
		if ((readingVis >= thresholdLowVis) && (readingVis <= thresholdHiVis))
			{lightState = ledColor;}
		else{lightState = 0;}

		readingIr = get_adc(PtRead);	// check ir	photo transistor

		
		if(tenths == 50){
			tenths = 0;
			twelvths++;
			if (twelvths == 100){
				twelvths = 0;
				shutDown();
			}
		}
	} 

} 


// Flash led x times
void flash(u8 value){
	cli(); 	// disable interrupts just in case 
	for(int x=0; x <= value; x++){
		led(1);
		delay(50);
		led(2);
		delay(50);
	}
	led(lightState);
	sei(); 	// enable interrupts
}


// Return the 10bit value of the selected adc channel.
u16 get_adc(u8 channel) {

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

	return ADCH;					// most sig bits
	//return ADCW;					// return the value

}	


// Change LED state (0 off / 1 red / 2 green)
void led(u8 value)
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


void ir(u8 value)
{
	if (value == 0)					// 0 = ir off
	{digitalWrite(irBit, LOW);}
	if (value == 1)					// 1 = ir on
	{digitalWrite(irBit, HIGH);}

}


// Flashes 2 byte value in binary (green = 1, red = 0) for troubleshooting
void decFlash(u16 value)
{
	//cli(); // disable interrupts just in case 

	portMode(speakerPos, OUTPUT);
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
				ir(ON);
				OCR1A = keys[7];
				delay(delayTime);
				led(0);
				OCR1A = 0;
				ir(OFF);
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
				ir(ON);
				OCR1A = keys[0];
				delay(delayTime);
				led(0);
				ir(OFF);
				OCR1A = 0;
				delay(delayTime * 5);
			}
		}
		y = y / 10;
	}
	if(foundFirst == 0){
		led(1);
		OCR1A = keys[0];
		ir(ON);
		delay(delayTime);
		led(0);
		OCR1A=0;
		ir(OFF);
	}

	delay(delayTime * 10);				// pause before proceeding
	portMode(speakerPos, INPUT);
	//sei(); 						// enable interrupts 
}


// Sets portA bit HIGH (1) or LOW (0), leaves other PORTB bits unchanged.
void digitalWrite(u8 port, u8 value)
{
	if (value == 0)
		{PORTA &= ~(1 << port);}	// Set bit # 'position' low
	else
		{PORTA |= (1 << port);}		// Set bit # 'position' high
 }


// Gets portA bit
u16 digitalRead(u8 port)
{
	u16 temp = (PINB & (1 << port));
	return temp;
}


// Set portA bit as INPUT (0) or OUTPUT (1)
void portMode(u8 port, u8 mode)
{
	if (mode == 0)
		{DDRA &= ~(1 << port);}      // Set bit # 'position' low
	else
		{DDRA |= (1 << port);}       // Set bit # 'position' high
}

// delay function (approximate, _delay_ms(); interferes with timer
void delay(u16 value){
	value = value * 10;
	for(u16 x=0; x < value; x++)
	{
		u16 check = get_adc(CdsRead); // takes 25 cycles?
		check++; //just to avoid warning
	}
}


void checkAmbient(void){
	
	ambientVis = get_adc(CdsRead);			// get ambient light level
	if (ambientVis < 230){thresholdHiVis = ambientVis + 25;}
	else{thresholdHiVis = 255;}
	if(ambientVis > 25){thresholdLowVis = ambientVis - 25;}
	else{thresholdLowVis = 0;}
	
	ambientIr = get_adc(PtRead);			// get ambient light level
	if (ambientIr < 245){thresholdHiIr = ambientIr + 10;}
	else{thresholdHiIr = 1023;}
//	if(ambientIr > 10){thresholdLowIr = ambientIr - 10;}
//	else{thresholdLowIr = 0;}
}


// Configure device settings
void setup(void) 
{ 
 	cli(); 						// disable interrupts just in case 
	srand(get_adc(CdsRead));    // see random number generator
	portMode(ledBit1, OUTPUT);	//setup ports
	portMode(ledBit2, OUTPUT);
	//portMode(irBit, OUTPUT);
	portMode(ADC_ENA_BIT, OUTPUT);
	portMode(switchBit, INPUT);
	digitalWrite(switchBit, HIGH); // enables internal pull-up
	portMode(speakerPos, INPUT);  // off until needed
			
	portMode(CdsRead, INPUT); //(see section 16.13.5)
	digitalWrite(CdsRead, HIGH);

	// Timer/Counter Register
	TCNT0 = 0x00;   			// clear 8 bit Timer/Counter 

	//START_TIMERS; 
	TCCR0B |= (1 << CS01);	// 8bit clock select prescaler 8
 
	//// Timer/Counter Interrupt Mask
	TIMSK0 |= (1 << TOIE0);  // Timer/Counter0 Overflow Interrupt Enable
	//TIMSK0	|= (1<<OCIE0A);  //enable output compare interrupt for OCR1A

	//16 bit timer tutorials
	//http://winavr.scienceprog.com/avr-gcc-tutorial/program-16-bit-avr-timer-with-winavr.html
	//http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=50106
	
	TCCR1B |= (1 << WGM12); // 16bit CTC mode
	TCCR1B |= (1 << CS10);  // 16bit start timer, no prescaler
	TCCR1A |= (1 << COM1A0); // 16bit Output compare CdsRead A in toggle mode

	PCMSK0 |= (1<<switchBit);		// Pin Change Mask Register

   	// MCU Control Register				//Interrupt Sense Control
	 MCUCR = (0<<ISC01) | (0<<ISC00); 	// low level
	// MCUCR = (0<<ISC01) | (1<<ISC00); 	// logical change
 	// MCUCR = (1<<ISC01) | (0<<ISC00); 	// falling edge
  	// MCUCR = (1<<ISC01) | (1<<ISC00); 	// rising edge
  
	// General Interrupt Mask Register
	// GIMSK  |= (1<<INT0);		// External Interrupt Request 0 Enable (bit 6)
	GIMSK  |= (1<<PCIE0); 		// Pin Change Interrupt Enable (bit 5)

	// General Interrupt Flag Register
	//GIFR |= (1<<INTF);			// External Interrupt Flag 0
	GIFR |= (1<<PCIF0);			// Pin Change Interrupt Flag

	sei(); 						// enable interrupts 
} 


