/*****************************************************************************
*Author: 	Jidan Al-Eryani,	jidan@gmx.net 
*Date: 	16.4.2007
*
* Basic control functions for 2 x DC-Motors
*
*
****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "motor.h"


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
		
	/**** Initialize the PWM for motor speed control ****/
	
	// enable 8 bit PWM, non-inverted
    TCCR1A =  (1<<WGM10); //Mode 5: Fast PWM 8-bit
    
    // timer1 running on 1*prescalar adn with clear timer/counter1 on Compare Match
    // PWM frequency = CK / [ N*(1+TOP)]. E.g. 8000000/(8*256)= 3906,25 Hz
    //E.g. 8000000/(1*256)= 31250 Hz	
    TCCR1B = (1<<CS10) | (1<<WGM12);
}

// initialization function for the quadrature encoder
void init_encoder(void){

	//set motor INputs
	DDRE |= (1<<M1_IN1);
	DDRE |= (1<<M1_IN2);
	DDRB |= (1<<M2_IN1);
	DDRB |= (1<<M2_IN2);	
	
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
	
	while(ASSR&0x07);           //wait until Timer0 is updated

	TIMSK |= (1<<TOIE0); //  enable Timer0 overflow interrupt
	
	
	/**** Initialize the interrupts for reading the channels from the encoder ****/
	//External interrupts 4 & 5
	EIMSK |= (1<<INT4);           // enable external intterupts
	//EIMSK |= (1<<INT5); 
	//EIMSK |= (1<<INT6); 
	//EIMSK |= (1<<INT7); 
	
	// For quadrature full performance use "trigger on any level change"
	//EICRB |= (1<<ISC40) | (1<<ISC50) | (1<<ISC60) | (1<<ISC70);
	
	//triger on rising edge channel 4
	EICRB |= (1<<ISC40) | (1<<ISC41);

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
	faststop_M1;
	faststop_M2;
}

void enable_m(void){
	enable_M1();
	enable_M2();
}


// Delay loop
void do_ms(unsigned int ms)
{
    unsigned int counter;
   
    while (ms) {
        counter = F_CPU / 5000;
       
        while (counter) {
            asm volatile("nop");
            counter--;
        }
        ms--;
    }
}
