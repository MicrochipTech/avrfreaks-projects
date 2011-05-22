#include "pwm.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
volatile static uint8_t address = 0;
volatile static uint8_t buff;
volatile static uint8_t rgbw = 0;
volatile static uint8_t enga = 0;
volatile static uint8_t bite = 0;
volatile static uint8_t hinb = 0;
volatile static uint8_t lonb = 0;

int main(void){
	DDRC = 0x00;
	address = (PINC)&(0x3f);
	DDRD = 0x02;
	PORTD = 0x00;
	DDRB = 0xff;
	UBRR0H = 0;
	UBRR0L = 0x01;
	/* Enable receiver and interrupt*/
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
	pwm_setup();
	sei();
	while(1);
	return 0;
};

ISR(USART_RX_vect){
	buff = UDR0; 
	if     ((buff&0xC0)==0){
		if((buff)==address){
			enga = 1;
			bite = 0;
			hinb = 0;
			lonb = 0;
		}
		else if((buff)==0){
			enga = 1;
			bite = 0;
			hinb = 0;
			lonb = 0;
		}
		else{enga=0;};
	}
	else if(enga==1){
		if((buff&0xC0)==0x80){
			rgbw = ((buff&0x30)>>4);
			lonb = (buff&0x0f);
			enga = 0;
		}
		else if((buff&0xC0)==0xC0){
			rgbw = ((buff&0x30)>>4);
			hinb = ((buff&0x0f)<<4);
			bite = hinb+lonb;
			if     (rgbw ==0){red = bite;}
			else if(rgbw ==1){grn = bite;}
			else if(rgbw ==2){blu = bite;}
			else if(rgbw ==3){wht = bite;}
			pwm_write(red,grn,blu,wht);
			enga = 0;
		};
	}
	else;
};//end ISR
