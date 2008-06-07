#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "driver.h"


int main(void){
	uint16_t out[20];
	uint16_t in[20];
	uint8_t i,count;
	
	cli();
	PORT_OUT|=_BV(POWER);//power HCS
	_delay_ms(200);
	
	out[0]=0x9108;//manufacturer code
	out[1]=0x86F0;
	out[2]=0x58F2;
	out[3]=0x728B;
	out[4]=0x0000;//seed
	out[5]=0x0000;
	out[6]=0x0000;
	out[7]=0x0000;
	out[8]=0x00A0;
	out[9]=0x0710;
	out[10]=0x2071;//serial number word0
	out[11]=0x0821;//serial number word1
	out[12]=0x0005;//user
	out[13]=0x0006;
	out[14]=0x0007;
	out[15]=0x0008;
	out[16]=0x0000;//counter
	out[17]=0x0000;//reserved

	
	PORT_OUT|=_BV(CLK);  //clk high enter program mode
	_delay_ms(tps);
	PORT_OUT|=_BV(DATA);	//data high
	_delay_ms(tph1);
	PORT_OUT&=~_BV(DATA);//data low
	_delay_us(tph2);
	PORT_OUT&=~_BV(CLK);	//clk low
	_delay_ms(tpbw);     //bulk write wait time
	for(i=0;i<18;i++){ 	//send 18 words 
		send_word(out[i]);
	};
	for(i=0;i<18;i++){
		in[i]=get_word();
	};
	count=0;
	for(i=0;i<18;i++){	//verify data
		if(out[i]==in[i]) count++;
		else break;
	};
	if(count==18){
		DDRD|=_BV(PD7);
		PORTD|=_BV(PD7);
		_delay_ms(500);  //if succes led on for 0.5 seconds
		PORTD&=~_BV(PD7);//else blink 10 times
	}else{
		DDRD|=_BV(PD7);
		for(i=0;i<10;i++){
			PORTD|=_BV(PD7);
			_delay_ms(300);
			PORTD&=~_BV(PD7);
			_delay_ms(200);
		};
	};
return 0;
}

	
	
