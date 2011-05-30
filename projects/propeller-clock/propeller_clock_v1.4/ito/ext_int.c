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
#include "digital_clk.h"

void init_INT(void){
	EICRA = (1<<ISC20)|(1<<ISC21);	//choose rising edge
	EIMSK = (1<<INT2);				//set the mask for INT2
	EIFR = (1<<INTF2);
	//DDRD |=(1<<2);
	DDRD &=~(1<<2);	//set as input
	PORTD |=(1<<2); //activate internal pull up.
	sei();
}	

void INT2_task(short int pmode){
	
	switch(pmode){
		case ACLK:
					analog_clk_GET_LEDS_PONTEIROS(&analog_clock,train,cont_encoder);
					analog_clk_GET_LEDS_BACKGROUND(&analog_clock,train,cont_encoder,caract);
			break;
			
		case DCLK:
						digital_clock_generator(train,cont_encoder);
			break;
		case PICT:
					//update_train(convert_encod_2_radian(),train);
			break;
			
		default:
		
		
			break;
		
		
		
		
	}
	next_train(train);
	enable_send=0;
	set_xlat();
	clear_xlat();
	
}
