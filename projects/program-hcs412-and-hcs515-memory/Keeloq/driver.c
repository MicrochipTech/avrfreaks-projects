//comenzi pentru clock si data

#define F_CPU 8000000UL // 8MHz
#include <avr/io.h>
#include <util/delay.h>
#include "driver.h"

extern uint8_t port_init(void){
	PORT_DIR|=_BV(DATA)|_BV(CLK); //all output 
	PORT_OUT&=~_BV(DATA)&~_BV(CLK); //all low	
	PCMSK0|=_BV(PCINT1);    //interrupt on pin change PB1
return 0;
}

//--------------------------------------------------------
// Name: request_command
// Desc: send request command
// Rets: ret 1 on success, 0 on error
// Date: 2008/05/19
// Modf:
//	
extern uint8_t request_command(void){
	uint16_t count=0;
	uint8_t status=1;
	
	PORT_OUT|=_BV(CLK); //clk high
	PORT_DIR&=~_BV(DATA); //data in
	_delay_ms(10);
	while(!((PORT_IN&_BV(DATA))>>DATA)){
		count++;
		if(count<MAX_IT) _delay_us(5);
		else{
			status=0;
			break;
		};
	};
	PORT_OUT&=~_BV(CLK); //clk low
	PORT_DIR|=_BV(DATA); //data output 
	PORT_OUT&=~_BV(DATA); //low		
	_delay_us(100);  //time to first command bit
return status;
}

//--------------------------------------------------------
// Name: get_ack
// Desc: get acknoledge after programm or erase
// Rets: ret 1 on success, 0 on error
// Date: 2008/05/19
// Modf:
//	
extern uint8_t get_ack(void){
	uint16_t count=0;
	uint8_t status=1;
	
	PORT_OUT|=_BV(CLK);  //clk high
	PORT_DIR&=~_BV(DATA); //data in
	_delay_ms(1);
	while(!((PORT_IN&_BV(DATA))>>DATA)){
		count++;
		if(count<MAX_IT) _delay_us(10);
		else{
			status=0;
			break;
		};
	};
	_delay_us(50);
	PORT_OUT&=~_BV(CLK);
	_delay_us(100);
	if(count<3000) status=0; // under 30mS error
return status;
}

//--------------------------------------------------------
// Name: get_ack2 
// Desc: get acknoledge after sending the learn command
// Rets: ret 1 on success, 0 on error
// Date: 2008/05/19
// Modf:
//	
extern uint8_t get_ack2(void){
	uint8_t count=0;
	uint8_t status=1;
	
	PORT_DIR&=~_BV(DATA); //data in
	PORT_OUT|=_BV(CLK);  //clk high	
	_delay_us(1);
	while(!((PORT_IN&_BV(DATA))>>DATA)){
		count++;
		if(count<100) _delay_us(1);
		else{
			status=0;
			break;
		};
	};
	_delay_us(50);
	PORT_OUT&=~_BV(CLK);
	_delay_us(50);
return status;
}	

//--------------------------------------------------------
// Name: send_octet
// Desc: send one byte to the device
// Rets: nothing
// Date: 2008/05/19
// Modf:
//		
extern void send_octet(uint8_t data){
	uint8_t i;
	
	PORT_DIR|=_BV(DATA); //data output 		
	for(i=0;i<8;i++){
		send_bit((data&_BV(i))>>i);
	};
	_delay_us(50); //command last bit to data first bit
}

extern void send_bit(uint8_t b){
	PORT_OUT|=_BV(CLK);  //clk high
	if(b) PORT_OUT|=_BV(DATA);
	else PORT_OUT&=~_BV(DATA);
	_delay_us(50);
	PORT_OUT&=~_BV(CLK); //clk low
	_delay_us(50);
}

//--------------------------------------------------------
// Name: get_bytes
// Desc: receive 'len' bytes from device
// Rets: nothing
// Date: 2008/05/19
// Modf:
//	
extern void get_bytes(uint8_t *buf,uint8_t len){
	uint8_t i,j;
	uint8_t data=0;
	
	PORT_DIR&=~_BV(DATA); //data in		
	for(j=0;j<len;j++){	
		for(i=0;i<8;i++){
			PORT_OUT|=_BV(CLK);  //clk high
			_delay_us(50);
			PORT_OUT&=~_BV(CLK); //clk low
			data>>=1;
			if((PORT_IN&_BV(DATA))>>DATA) data|=0x80;
			_delay_us(50);
		};
		*buf++=data;
	};
}
		
