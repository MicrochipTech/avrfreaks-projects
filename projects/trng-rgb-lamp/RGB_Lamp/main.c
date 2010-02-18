/* 
TRNG RGB Lamp
Schematic and the deets can be found at petemills.blogspot.com
By Pete Mills 
February 2010
ATTiny2313 @ 8MHz internal oscilator
*/


#include <avr/io.h>					// defines things like "PORTB" and "TCCR0" etc
#include <util/delay.h>				// delay functions


// variables

uint8_t red_cur = 0;				// current red variable
uint8_t green_cur = 0;				// current green variable
uint8_t blue_cur = 0;				// current blue variable
uint8_t red_new = 0;				// target red variable
uint8_t green_new = 0;				// target green variable
uint8_t blue_new = 0;				// target blue variable

uint8_t v1 = 0;						// random bit 1 to XOR for unbiasing
uint8_t v2 = 0;						// random bit 2 to XOR for unbiasing

uint8_t rand = 0;					// 8-bit unbiased random number

uint8_t i = 0;						// loop counter vars
//uint8_t j = 0;
//int8_t k = 0;						


// function prototypes

void setup(void);
uint8_t get_rand();


// functions

void setup(void){

// port config

DDRD |= (1<<0);							// set PD1 to "1" for output
PORTD &= ~(1<<0);							// set the output low

DDRD &= ~(1<<1);							// set PD1 to "0" for input

DDRB |= ((1<<2) | (1<<3) | (1<<4));		// set portB bits 2,3,4 to 1 for output - PWM
PORTD &= ~((1<<2) | (1<<3) | (1<<4));		// set the outputs low


// PWM config

/*
OCR0A is for red
OCR1AL is for green
OCR1BL is for blue
*/

// Timer/Counter0: channel:A clear on compare match, Fast PWM, TOP = 0xff 

TCCR0A |= ((1<<COM0A1) | (1<<WGM01) | (1<<WGM00));
TCCR0B |= (1<<CS00);									// internal clock as source, no prescale

		
// Timer/Counter1: Channel:A/B clear on compare match, Fast PWM, 8-bit, TOP = 0x00ff

TCCR1A |= ((1<<COM1A1) | (1<<COM1B1) | (1<<WGM10));
TCCR1B |= ((1<<WGM12) | (1<<CS10));					// finish up WGM bits, internal clock as source, no prescale
}



uint8_t get_rand(){

i = 0;                     			// initalize loop counter
   
   while(i < 7){

   v1 = PIND & (1<<1);        		// take two samples from the white noise generator
   v2 = PIND & (1<<1);
   
      if(v1 ^ v2){            		// here we mean if two consecutive samples are dissimilar
        
      rand = (rand << 1 ) | v1; 	// add new random bit to byte rand
	  
	  i++;                 			// we have a new bit shifted in so increment counter
            
      }
   }
   
return rand;

} 





int main(void){

setup();

	while(1){
	
	red_new = get_rand();					// get new random RGB values
	green_new = get_rand();
	blue_new = get_rand();
	
		while((red_cur + green_cur + blue_cur) != (red_new + green_new + blue_new)){
		
			if(red_cur > red_new){			// take a step towards the new color value
			--red_cur;
			}
			
			if(red_cur < red_new){
			++red_cur;
			}
			
			
			if(green_cur > green_new){		// and for green
			--green_cur;
			}
			
			if(green_cur < green_new){
			++green_cur;
			}
			
			
			if(blue_cur > blue_new){		// and for blue
			--blue_cur;
			}
			
			if(blue_cur < blue_new){
			++blue_cur;
			}
			
		OCR0A = red_cur;					// set PWM registers for RGB value
		OCR1AL = green_cur;
		OCR1BL = blue_cur;
		
		/*
		This is the fade "time"
		The maximum difference in PWM value
		can be is 255 so 255*_delay_ms(39) = 9945mS or ~10S.  
		*/
		
		_delay_ms(250);						// original 250Ms
			
		}
		
	//PORTD |= (1<<0);						// debug
	
	for(i=0; i<45; i++){					// how long to display a "settled" RGB value; 45S here
	_delay_ms(1000);						
	}
	
	//PORTD &= ~(1<<0);						// debug
	
	}

}




