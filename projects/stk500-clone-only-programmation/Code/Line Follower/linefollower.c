/******************************************************************************
* Line Follower
*
* Author: 	Jidan Al-Eryani,	jidan@gmx.net 
* Date: 	18.4.2007
*
* Purpose: 
* Makes MAMoRo follow a black line drawn on a white surface
* see "9.1 The Line Follower" in thesis
*
****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define SW2_PIN PF0
#define LED_PIN PA0

#define SENSR_PIN PF5
#define SENSL_PIN PF7

#define XTAL 8000000

/******************Define motor pins***********/
#define M1_EN PB7 //#define OC1C_PIN PB7
#define M1_IN1 PE2
#define M1_IN2 PE3
#define M1_A PE4
#define M1_B PE5

#define M2_EN PB6
#define M2_IN1 PB5
#define M2_IN2 PB4
#define M2_A PE6
#define M2_B PE7


/****** Define basic functions************/

//Directions
#define forward_M1 PORTE |= (1<<M1_IN1); PORTE &= ~(1<<M1_IN2);
#define backward_M1 PORTE &= ~(1<<M1_IN1); PORTE |= (1<<M1_IN2);
#define faststop_M1 PORTE &= ~(1<<M1_IN1); PORTE &= ~(1<<M1_IN2);

#define backward_M2 PORTB |= (1<<M2_IN1); PORTB &= ~(1<<M2_IN2);
#define forward_M2 PORTB &= ~(1<<M2_IN1); PORTB |= (1<<M2_IN2);
#define faststop_M2 PORTB &= ~(1<<M2_IN1); PORTB &= ~(1<<M2_IN2);

/****** variables***** */
uint16_t adc_result_r, adc_result_l;
int adc_difference;


/***function prototypes***/

void init_mototrs(void);

void move_m(int speed_m1, int speed_m2);
void turn_m(signed int degree);

void disable_m(void);
void enable_m(void);

void enable_M1(void);
void enable_M2(void);
void disable_M1(void);
void disable_M2(void);

void wait_ms(unsigned int ms);
uint16_t readADC(uint8_t channel); 


int main(void)
{    
	int stop=1;

	
	// Set MCU's  pin for ADC as input
	DDRF &= ~( (1<<SENSR_PIN) | (1<<SENSL_PIN) );
	PORTF |= (1<<SENSR_PIN) | (1<<SENSL_PIN);
	
	//define switche 2 to turn the motors ON & OFF
	DDRF &= ~(1<<SW2_PIN);
	PORTF |= (1<<SW2_PIN);
	
	//define MCU pins for LED
	DDRA |= (1<<LED_PIN);
	PORTA |= (1<<LED_PIN);
	
	init_mototrs();
	
	wait_ms(2000);

	for(;;){
		adc_result_r= readADC(SENSR_PIN);
		adc_result_l= readADC(SENSL_PIN);
		
		adc_difference = adc_result_r - adc_result_l;

		if ( !(PINF & (1<<SW2_PIN)) ){
			wait_ms(300);
			disable_m();
			if (stop==1)
				stop=0;
			else
				stop=1;
		}
		

		if (adc_difference > 50){
			PORTA |= (1<<LED_PIN);
		}
		else
		if (adc_difference  < -50){
			PORTA |= (1<<LED_PIN);
		}
		else{
			PORTA &= ~(1<<LED_PIN);			
		}
		
		if (stop==0) {
			if (adc_difference > 50){
				move_m(60,-60);
			}
			else
			if (adc_difference  < -50){
				move_m(-60,60);
			}
			else{
				move_m(20,20);			
			}
		}
	
	}
                 
}

///////////////////////////////// Function definations ///////////////

void init_mototrs(void){
	/****  Initialize the MCU pins ****/
	// set motor ENable pins
	DDRB |= (1<<M1_EN);
	disable_M1();
	DDRB |= (1<<M2_EN);
	disable_M2();
	
	//set motor INputs
	DDRE |= (1<<M1_IN1);
	DDRE |= (1<<M1_IN2);
	DDRB |= (1<<M2_IN1);
	DDRB |= (1<<M2_IN2);	
	faststop_M1;
	faststop_M2;
	
	//set quadrature encoder channels
	DDRE &= ~(1<<M1_A);
	DDRE &= ~(1<<M1_B);
	DDRE &= ~(1<<M2_A);
	DDRE &= ~(1<<M2_B);
	
	
	/****  Initialize the Timer ****/
	// Not necesserly. Works also if OC0 pin is configured as input
	//DDRB |= (1<<OC0_PIN);
	
	TIMSK &=~((1<<TOIE0)|(1<<OCIE0)); // disable Timer0 interrupts
	ASSR |= (1<<AS0); // set external quartz crstal(32,768 KHz) as clock source
	TCNT0 = 0x00; //preset Timer0 register to 0
	TCCR0 |= (1<<CS00); // set prescaler = 1. 128 overflow interrupts per second ( OF=CK/(Prescalar*256) )
	
	while(ASSR&0x07);           //wait_ms until Timer0 is updated

	TIMSK |= (1<<TOIE0); //  enable Timer0 overflow interrupt	
	
	/**** Initialize the PWM for motor speed control ****/
	
	// enable 8 bit PWM, non-inverted
    TCCR1A =  (1<<WGM10); //Mode 5: Fast PWM 8-bit
    
    // timer1 running on 1/8 MCU clock with clear timer/counter1 on Compare Match
    // PWM frequency = CK / [ N*(1+TOP)]. E.g. 8000000/(8*256)= 3906,25 Hz
    TCCR1B = (1<<CS11) | (1<<WGM12);
}



void move_m(int speed_m1, int speed_m2){
	disable_m();
	
	if (speed_m1>0){
		forward_M1;
	}
	else{
		speed_m1 = -speed_m1;
		backward_M1;
		}
	
	if (speed_m2>0){
		forward_M2;
	}
	else{
		speed_m2 = -speed_m2;
		backward_M2;
		}

	OCR1CL = speed_m1;
	OCR1BL = speed_m2;
	
	enable_m();
}

void enable_M1(void){
	TCCR1A |=  (1<<COM1C1);
}

void enable_M2(void){
	TCCR1A |=  (1<<COM1B1);
}

void disable_M1(void){
	TCCR1A &=  ~(1<<COM1C1);
	PORTB &= ~(1<<M1_EN);
}
	
void disable_M2(void){
	TCCR1A &=  ~(1<<COM1B1);
	PORTB &= ~(1<<M2_EN);
}

void disable_m(void){
	disable_M1();
	disable_M2();
}

void enable_m(void){
	enable_M1();
	enable_M2();
}


//read from the ADC
uint16_t readADC(uint8_t channel) {
	uint8_t c;
	uint16_t result = 0;
	
	// Activate ADC and set prescalar to 64
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1);

	// set the channel which the the ADC will read
	ADMUX = channel;
	
	// Referece voltage: AVCC (3.3V)
	ADMUX |= (1<<REFS0);
	
	// Initilize the ADC and do a  Dummy
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	
	//Convert the analgue signal 3 times and calculte the average value
	for(c=0; c<3; c++) {
		// start the conversion
		ADCSRA |= (1<<ADSC);
		// wait for the ADC result
		while(ADCSRA & (1<<ADSC));
		
		result += ADCW;
	}
	
	// deactivate the ADC
	ADCSRA &= ~(1<<ADEN);
	
	result /= 3;
	
	return result;
}
	
// Delay loop
void wait_ms(unsigned int ms)
{
    unsigned int counter;
   
    while (ms) {
        counter = XTAL / 5000;
       
        while (counter) {
            asm volatile("nop");
            counter--;
        }
        ms--;
    }
}
