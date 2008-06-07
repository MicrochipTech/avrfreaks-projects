#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "driver.h"


uint8_t port_init(void){
	PORT_DIR|=_BV(DATA)|_BV(CLK)|_BV(POWER);  	//all output
	PORT_OUT&=~_BV(DATA)&~_BV(CLK)&_BV(POWER);	//all low
return 0;
}

void send_bit(uint8_t b){
	PORT_OUT|=_BV(CLK);  //clk high
	if(b) PORT_OUT|=_BV(DATA); //data high
	else PORT_OUT&=~_BV(DATA);//data low
	_delay_us(tclk);
	PORT_OUT&=~_BV(CLK); //clk low
	_delay_us(tclk);
}

void send_word(uint16_t word){
	uint8_t i;
	
	for(i=0;i<16;i++){
		send_bit((word&_BV(i))>>i);
	};
	PORT_OUT|=_BV(CLK);  //clk high
	PORT_DIR&=~_BV(DATA);//data in
	_delay_ms(twc);	//wait write cycle
	//while(!((PORT_IN&DATA)>>DATA)) {_delay_us(200);} //wait for an ack
	//while((PORT_IN&DATA)>>DATA); 
	PORT_OUT&=~_BV(CLK); //clk low
	PORT_DIR|=_BV(DATA);//data out
	PORT_OUT&=~_BV(DATA); //data low
	_delay_us(120); //hold time
}

uint16_t get_word(void){
	uint16_t data=0;
	uint8_t i;
	
	PORT_DIR&=~_BV(DATA); //data in
	for(i=0;i<16;i++){
		PORT_OUT|=_BV(CLK);  //clk high
		_delay_us(tclk);
		PORT_OUT&=~_BV(CLK); //clk low
		data>>=1;
		if((PORT_IN&_BV(DATA))>>DATA) data|=0x8000;
		_delay_us(tclk);
	};
return data;
}
