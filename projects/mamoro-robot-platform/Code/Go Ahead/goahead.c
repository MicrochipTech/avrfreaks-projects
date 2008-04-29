/*****************************************************************************
* Go Ahead
*
*Author: 	Jidan Al-Eryani,	jidan@gmx.net 
* Date: 	18.4.2007
*
*
* Purpose: 
* Simple program to test the interaction between the microcontroller and FPGA.
* This Program makes the robot platform move forward (hence the name of program = Go Ahead)
* and turn randomly right or left around an obstacle if one is detected.
* The sensors are two infrared sensors and the motors encoders.
*
*
****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"
#include "motor.h"
#include "usart.h"
#include "goahead.h"


unsigned short adc_sense_r, adc_sense_l;
volatile unsigned short  pulses_M1A = 0;
volatile unsigned char obstacle = 0;
unsigned short frw_times = 0;

/*Interrupt service routine (ISR)*/
ISR(INT4_vect){	
	pulses_M1A++;
}

ISR(TIMER0_OVF_vect){

	if (pulses_M1A < 10 && ( (PINE & (1<<M1_IN1)) | (PINE & (1<<M1_IN2)) | (PINB & (1<<M2_IN1)) | (PINB & (1<<M2_IN2)))  ){
		obstacle = 1;
	}else{
		obstacle = 0;;
	}
	
	pulses_M1A = 0;
	
	//Generate a pulse (128 times per second).  At each pulse, the FPGA reads and processes the sensor's signals
	PORTC &= ~(1<<FPGA_MCU_CLK);
	PORTC |= (1<<FPGA_MCU_CLK);
}


void goahead(void)
{    
	unsigned char stop = 1;
	
	//initialize
	init_goahead();
	do_ms(100);	//wait for the FPGA to initialize properly
	
	//enable interrupts
	sei();
		
	while(1){
	
		if ( !(PINF & (1<<SW2_PIN)) ){stop = 1; disable_m();}
		if ( !(PINF & (1<<SW3_PIN)) ) stop = 0;

	
		adc_sense_l = readADC(SENSL_PIN);
		adc_sense_r = readADC(SENSR_PIN);
		
		/* Debug *//*
		if ( !(PINF & (1<<SW3_PIN)) )
			PORTC |= (1<<FPGA_IRSENSE);
		else
			PORTC &= ~(1<<FPGA_IRSENSE);
			
		if ( !(PINF & (1<<SW2_PIN)) )
			PORTC |= (1<<FPGA_OBSTACLE);
		else
			PORTC &= ~(1<<FPGA_OBSTACLE);*/

		
		//LED debug
		if (adc_sense_l > 500 || adc_sense_r > 500)
			PORTC |= (1<<FPGA_IRSENSE);
		else
			PORTC &= ~(1<<FPGA_IRSENSE);
			
		if (obstacle == 1)
			PORTC |= (1<<FPGA_OBSTACLE);
		else
			PORTC &= ~(1<<FPGA_OBSTACLE);
			
		
		
		/*if ( !(PINC & (1<<FPGA_COMMAND2)) ) // Turn around (left/right) COMMAND2=0; COMMAND1=x
			PORTA |= (1<<LED_PIN);
		else
			PORTA &= ~(1<<LED_PIN);	*/

				
		//motor turned ON/OFF through switch
		if( stop==1) continue; 
		
		//read control actions from FPGA
		if ( (PINC & (1<<FPGA_COMMAND2)) && (PINC & (1<<FPGA_COMMAND1)) ){ // stop: COMMAND2=1; COMMAND1=1
			PORTA &= ~(1<<LED_PIN);
			disable_m();
		}
		if ( (PINC & (1<<FPGA_COMMAND2)) && (!(PINC & (1<<FPGA_COMMAND1))) ){ // move forward: COMMAND2=1; COMMAND1=0
			PORTA &= ~(1<<LED_PIN);
			move_m(40, 40);	
		}
		if ( !(PINC & (1<<FPGA_COMMAND2)) ){ // Turn around (left/right) COMMAND2=0; COMMAND1=x
			PORTA |= (1<<LED_PIN);
			if ( (PINC & (1<<FPGA_COMMAND1)) )
				go_around(1);
			else
				go_around(0);
		}
		
	}
                 
}

void go_around(unsigned char turn){
	//move backward
	move_m(-100, -100);
	do_ms(300);
	
	//turn left/right
	if ( turn==0)
		move_m(-100, 100);
	else
		move_m(100, -100);
		
	do_ms(300);
	
}

//initialize function
void init_goahead(void){
	//set sensors pins as input with High-Z
	DDRF &= ~( (1<<SENSR_PIN) | (1<<SENSL_PIN) );
	PORTF |= (1<<SENSR_PIN) | (1<<SENSL_PIN);
	
	//set pin for indicator LED
	DDRA |= (1<<LED_PIN);
	PORTA &= ~(1<<LED_PIN);
	
	//switches pins
	DDRF &= ~( (1<<SW2_PIN) | (1<<SW3_PIN));
	PORTF |= (1<<SW2_PIN) | (1<<SW3_PIN);
	
	//FPGA pins
	DDRC |= (1<<FPGA_IRSENSE) | (1<<FPGA_OBSTACLE) | (1<<FPGA_MCU_CLK);
	PORTC &=  ~( (1<<FPGA_IRSENSE) | (1<<FPGA_OBSTACLE) | (1<<FPGA_MCU_CLK) ); //initial state is low
	DDRC &= ~( (1<<FPGA_COMMAND1) | (1<<FPGA_COMMAND2) );
	PORTC |= (1<<FPGA_COMMAND1) | (1<<FPGA_COMMAND2); //initial state is high
	
	//initializations functions
	init_mototrs();
	init_encoder();
	
	//for debugging
	usart_init(0);
}
