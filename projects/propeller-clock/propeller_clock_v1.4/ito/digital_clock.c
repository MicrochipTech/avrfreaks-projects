#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h> 
#include <inttypes.h>  
#include <avr/interrupt.h> 
#include <math.h> 
#include <util/twi.h> 
#include <util/delay.h> 
#include "digital_clock_vars.h"
#include "digital_clock_flash_data.h"
#include "digital_clock_funcs.h"


void init_timer0(void);
void init_timer2(void);

void main(void)

{
	//init_timer0();
	//init_timer2();
	SPI_MasterConfig();
	message_x=MSG_END;
	i2c_configure();
	shift_counter=SHIFT_N ;
	clock_digits[0]='2';
	clock_digits[1]='3';
	clock_digits[2]=':';
	clock_digits[3]='4';
	clock_digits[4]='7';
	clock_digits[5]=':';
	clock_digits[6]='5';
	clock_digits[7]='8';
	update_clk_window(clock_window);
	//sei();
	cont_encoder=0;
	
	while(1)
	{
	//gget_clock(clock_digits);
	//shift_msg_window();
	cont_encoder=(cont_encoder+1)%360;
	digital_clock_generator();
	
	}

}



ISR(TIMER0_OVF_vect){
	//cont_encoder++;
	//shift_msg_window();
	cont_encoder=(cont_encoder+1)%360;
	//update_clk_window(clock_window);
	digital_clock_generator();
	
}

ISR(TIMER2_COMP_vect){
	cont_encoder=(cont_encoder+1)%360;
	PORTC^=0X01;
	digital_clock_generator();
}

void init_timer0(void){
	//set prescaler F/1024
	TCCR0 = (1<<CS02) | (1<<CS00) ;
	TIMSK |=(1<<TOIE0);
	TCNT0 =5;
	
}

void init_timer2(void){
	//set prescaler F/8
	TCCR2=(1<<WGM21) | (1<<COM21) | (1<<COM20) |(1<<CS21);	
	TIMSK|=(1<<TOIE2);
	OCR2=184;
}	



