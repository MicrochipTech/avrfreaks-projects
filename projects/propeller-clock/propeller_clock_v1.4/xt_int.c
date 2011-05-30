/*
 * ext_int.c
 *
 * Created: 06-06-2011 22:57:08
 *  Author: limapapy
 */ 
#include <avr/interrupt.h>
#include "globals.h"
#include "analog_clk.h"
#include "image_process.h"
#include "spi_com.h"

void init_INT(void){
	EICRA = (1<<ISC20)|(1<<ISC21);	//choose rising edge
	EIMSK = (1<<INT2);				//set the mask for INT2
	EIFR = (1<<INTF2);
	DDRD &=~((1<<PD2) | (1<<PD4));	//set as input
	//PORTD |=((1<<PD2) | (1<<PD4)); //activate internal pull up.
	//if problem comment the line abouve.
	sei();
}	

void INT2_task(short int pmode){
	switch(pmode){
		case ACLK:
					analog_clk_GET_LEDS_PONTEIROS(&analog_clock,train,encoder360);
					analog_clk_GET_LEDS_BACKGROUND(&analog_clock,train,encoder360,caract);
			break;
			
		case DCLK:
			digital_clock_generator();
			break;
		case PICT:
#ifdef ROM
					update_train(encoder360,train);
#endif // ROM
#ifdef RAM
					update_train_im(0/*para mudar*/,train_im);
#endif // RAM
			break;
			
		default:
		
		
			break;
		
		
		
		
	}
#ifdef ROM
	next_train(train);
#endif // ROM
#ifdef RAM
	if(pmode==PICT){		
		next_train_im(train_im);	
	}else{
		next_train(train);
	}		
#endif // RAM		

enable_send=0;	
}