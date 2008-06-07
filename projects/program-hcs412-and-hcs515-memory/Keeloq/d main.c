/* program memory of HCS515
	use serial for debug
	
*/
#define F_CPU 8000000UL // 8MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdlib.h>
#include "driver.h"
#include "usart.h"


//prototypes
uint8_t prog_mem(void);
uint8_t activate_learn(void);
uint8_t read_learn(void);
uint8_t erase_all(void);


volatile uint8_t status;  // status=1 if valid transmission
uint8_t buffer[10];

//--------------------------------------------------------
// Name: interrupt routine PCINT0
// Desc: on PB1 change, sends acknoledge and read 10 bytes(read transmission)
// Rets: 0 on error, 1 on valid transmission
// Date: 2008/05/19
// Modf:
//	
ISR(PCINT0_vect){
	cli();
	if((PORT_IN&_BV(DATA))>>DATA){
		_delay_us(400); // tcla micro request ack 300uS (min.50uS max. 1mS)
		PORT_OUT|=_BV(CLK); //clk high (acknoledge)
		_delay_us(50);   //wait for decoder ack (max. 4uS)
		if(!((PORT_IN&_BV(DATA))>>DATA)){
			status=1;
			_delay_us(50);
			PORT_OUT&=~_BV(CLK); //clk low
			_delay_us(200); //time to first bit
			get_bytes(buffer,10);
		}else{
			status=0;
			PORT_OUT&=~_BV(CLK); //clk low
		};
	};
	sei();
}

//--------------------------------------------------------
// Name: read_learn
// Desc: read status byte
// Rets: 0 on error, 1 on success
// Date: 2008/05/20
// Modf:
//	
uint8_t read_learn(void){
	uint16_t timeout=0;
	uint8_t status=0;

	PORT_DIR&=~_BV(DATA); //data in
	usart_puts_P("Waiting for a transmitter.\n");
	do{
		_delay_ms(1);	//loop until HCS515 responds  
		timeout++;		//or until ~32 sec.expires
		if((PORT_IN&_BV(DATA))>>DATA) break;
	}while(timeout<MAX_IT);
	if(!((PORT_IN&_BV(DATA))>>DATA)){
		usart_puts_P("No transmission received.\n");
		return status;
	};
	_delay_ms(100);
	if((PORT_IN&_BV(DATA))>>DATA){
		usart_puts_P("Error on first transmission.\n");
		return status;
	}else{
		usart_puts_P("First transmission received. 30 seconds remaining!\n");
		status=1;
	};
/*	_delay_us(100);  // tcla min.50uS
	PORT_OUT|=_BV(CLK); //clk high
	_delay_us(100);
	if(!((PORT_IN&_BV(DATA))>>DATA)){
		PORT_OUT&=~_BV(CLK);//clk low
		_delay_us(50);
		get_bytes(buffer,1);
	}else{
		usart_puts_P("No response to CLK high.\n");
		return status;
	};
	if(buffer[0]==0){
		usart_puts_P("First transmission received. 30 seconds remaining!\n");
		status=1;
	}else{
		usart_puts_p("status byte not ok. status= ");
		usart_puthex_byte(buffer[0]);
		usart_puts_P("\n");
		return status;
	};*/
	if(status){		//wait for second transmission
		status=0;
		do{
			_delay_ms(1);	//loop until HCS515 responds  
			timeout++;		//or until ~32 sec.expires
			if((PORT_IN&_BV(DATA))>>DATA) break;
		}while(timeout<MAX_IT);
		if(!((PORT_IN&_BV(DATA))>>DATA)){
			usart_puts_P("No second transmission received within 30 seconds.\n");
			return status;
		};		
		_delay_us(100);  // tcla min.50uS
		PORT_OUT|=_BV(CLK); //clk high
		_delay_us(100);
		if(!((PORT_IN&_BV(DATA))>>DATA)){
			PORT_OUT&=~_BV(CLK);//clk low
			_delay_us(50);
			get_bytes(buffer,10);
			status=1;
		}else{
			usart_puts_P("No response to CLK high.\n");
			return status;
		};
		usart_puts_P("Transmitter learned. Decoded message: ");
		for(uint8_t i=0;i<10;i++){
			usart_puthex_byte(buffer[i]);
			usart_puts_P(" ");
		};
	}
return status;
}

	
	
//--------------------------------------------------------
// Name: activate_learn
// Desc: activate learn command
// Rets: 0 on error, 1 on success
// Date: 2008/05/19
// Modf:
//	
uint8_t activate_learn(void){
	uint8_t buffer[3];
	uint8_t status=0;
	
	buffer[0]=0xD2; //learn command
	buffer[1]=0x3C; //two dummy bytes
	buffer[2]=0xAE;
	
	PCICR&=~_BV(PCIE0);
	if(request_command()){
		for(uint8_t i=0;i<3;i++){
			send_octet(buffer[i]);
		};
		status=get_ack2();
		if(status){
			usart_puts_P("learn mode entered.\n");
		}else usart_puts_P("not ack\n");
	}else usart_puts_P("no response to request\n");
return status;
}

//--------------------------------------------------------
// Name: erase_all
// Desc: erase all transmitters in the decoder
// Rets: 0 on error, 1 on success
// Date: 2008/05/19
// Modf:
//	
uint8_t erase_all(void){
	uint8_t buffer[3];
	uint8_t status=0;
	
	buffer[0]=0xC3; //erase all command
	buffer[1]=0x00; //one dummy byte
	buffer[2]=0xAE;
	
	PCICR&=~_BV(PCIE0); //disable interrupt
	if(request_command()){
		for(uint8_t i=0;i<3;i++){
			send_octet(buffer[i]);
		};
		status=get_ack();
		if(status){
			usart_puts_P("all transmitters erased.\n");
		}else usart_puts_P("not ack\n");
	}else usart_puts_P("no response to request\n");
	PCICR|=_BV(PCIE0);
return status;
}
	

//--------------------------------------------------------
// Name: prog_mem
// Desc: programm memory with config byte am manufacturer code
// Rets: puts usart messages
// Date: 2008/05/19
// Modf:
//	
uint8_t prog_mem(void){
	uint8_t i;
	uint8_t status=0;
	uint8_t buffer[20];
	
	buffer[0]=0xB4; //command program
	buffer[1]=0x00; //config byte
	buffer[2]=0xE2; //manufacturer code LSB 00 E2 AF D1 BC 59 43 21 3A
	buffer[3]=0xAF;
	buffer[4]=0xD1;
	buffer[5]=0xBC;
	buffer[6]=0x59;
	buffer[7]=0x43;
	buffer[8]=0x21;
	buffer[9]=0x3A;

	PCICR&=~_BV(PCIE0);
	if(request_command()){
		//usart_puts_P("programming\n");
		for(i=0;i<10;i++){
			send_octet(buffer[i]);
		};
		status=get_ack();
		if(status){
			usart_puts_P("program success.\n");
		}else usart_puts_P("not ack\n");
	}else usart_puts_P("no response to request\n");
	PCICR|=_BV(PCIE0);
return status;
}


int main(void){
	
	cli();
	usart_init(103);
	port_init();
	set_sleep_mode(0);
	sei();
	PCICR|=_BV(PCIE0); //enable change on pin interrupt
	DDRD|=_BV(PD7);
	DDRD&=~_BV(PD6);
	PORTD|=_BV(PD6);	//pull-up resistor
	usart_puts_P("Power on.\n");
	_delay_ms(500);
	//prog_mem();
	
	status=0;
	while(1){
		PORT_DIR&=~_BV(DATA);   //data in
		if(PIND&_BV(PD6)){
			if(activate_learn()){
				read_learn();
				PCICR|=_BV(PCIE0);
			};
		};
		if(status){
			status=0;
			usart_puts_P("Data from decoder: ");
			for(uint8_t i=0;i<10;i++){
				usart_puthex_byte(buffer[i]);
			};
			usart_puts_P("\n");
			if(buffer[0]==0x88){
				if(PIND&_BV(PD7)) PORTD&=~_BV(PD7);
				else PORTD|=_BV(PD7);
			};
		};
		usart_puts_P("going to sleep.\n");
		_delay_ms(50);
		sleep_mode();
	};
}
