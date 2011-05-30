/*
 * propeller.c
 *
 * Created: 27-05-2011 19:51:24
 *  Author: limapapy
 */ 
//#ifndef F_CPU
//#define F_CPU				20000000UL 
#define F_CPU				16000000UL 
//#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <util/delay.h>
#include "rs232.h"
#include "i2c_com.h"
#include "ext_int.h"
#include "spi_com.h"
#include "globals.h"
#include "analog_clk.h"
#include "image_process.h"


void init_pins(void);
void init_timer0(void);
void init_timer2(void);

int TMODE;
int main(void) 
{
	rgb_t col={200,200,0};
	serial_init(BAUD_PRESCALE);
	//init_INT();
//	init_timer0();
//	init_timer2();
	reset_values();
	DDRF=0xFF;
	DDRC=0xFF;
	//set_blank();
	SPI_MasterConfig();
	clear_train();
	//set_blank();
	clear_blank();
	//PORTB |=1<<PB0;
	//train[0]=col;
	//train[1]=col;
	//train[2]=col;
	//next_train(train);
	//_delay_ms(2000);
	//propeller_simul();

	
		
	clear_train();
train[31].r=255;
		
	
		
				train[30].b=200;
				train[29].b=200;
		
		next_train(train);
	//analog_clk_SET_ANGULOS_PONTEIROS(&analog_clock,time_clk);
	mode=DCLK;
    while(1)
    {
		//spiral(rand_color());
		//PORTC =0xFF;
		//PORTB |=1<<PB5;
		//_delay_ms(50);
		//PORTC = 0x00;
		//PORTB &=~(1<<PB5);
		//_delay_ms(50);
		
		
	//	if (enable_send)
	//		INT2_task(mode);

		
    }
}

ISR(USART0_RX_vect){
	unsigned char rec;
	command cmd;
	unsigned char *buf;
	rec = UDR0; // Fetch the received byte value into the variable "ByteReceived"
	UDR0 = rec; // Echo back the received byte back to the computer 
	
	USART_WORD[USART_WORD_INDEX]=rec;
	USART_WORD_INDEX=(USART_WORD_INDEX+1)%MAX_SIZE_WORD;
	cmd = decode_CMD(USART_WORD);
	switch(rec){
		case ENTER:
			switch (cmd){
			case SET_CLOCK:
				set_clock();
				set_clockRTC(time_clk.sec,time_clk.min,time_clk.hour);
				break;
			case GET_CLOCK:
				//serial_send_message(get_clock());
				break;
			
			case MODE:
				mode =get_mode(USART_WORD+CMD_SIZE+1);
				TMODE=mode;
				if(mode==-1){
					serial_send_message((unsigned char*)"Invalid mode \r\n");
				}else{
					serial_send_message((unsigned char*)"mode set \r\n");
				}
				break;
			case NUMBER_COLOR:
				analog_clock.nbr_color=get_color(USART_WORD+CMD_SIZE+1);
				break;
			case POINTER_COLOR:
				analog_clock.ptr_color=get_color(USART_WORD+CMD_SIZE+1);
				break;
			case HELP:
				buf=get_msg_ROM(HELP_MSG0);
				serial_send_message(buf);
				free(buf);
				
				buf=get_msg_ROM(HELP_MSG1);
				serial_send_message(buf);
				free(buf);
				buf=get_msg_ROM(HELP_MSG2);
				serial_send_message(buf);
				free(buf);
				buf=get_msg_ROM(HELP_MSG3);
				serial_send_message(buf);
				free(buf);
				buf=get_msg_ROM(HELP_MSG4);
				serial_send_message(buf);
				free(buf);
				break;
			case PATTERN:
			
				break;
			case INVALID_CMD:
				//buf=get_msg_ROM()
				serial_send_message((unsigned char*)"Invalid command\r\n");
			
			default:
			reset_values();
			break;
			}
			reset_values();
			break;	
		case BACK_SPACE:
			if (USART_WORD_INDEX>0)
			{
				USART_WORD_INDEX--;
				USART_WORD[USART_WORD_INDEX]='\0';
				USART_WORD_INDEX--;
				USART_WORD[USART_WORD_INDEX]='\0';
			}
			break;
		default:
		
		break;
		
		
	}
		
}

ISR(INT2_vect)
{

	cont_encoder=(cont_encoder+1)%360;
	enable_send=1;
}



void init_pins(void){
	//Set PIN 0 1 2 (MISO MOSI RESET) as inputs
	DDRC &= ~((1<<PC2)|(1<<PC1)|(1<PC0));	
	//enable pull-ups resistors
	//PORTC |=((1<<PC2)|(1<<PC1)|(1<PC0));
	PORTC |=(1<<PC2);
	
}
ISR(TIMER0_OVF_vect){
	static int cont_clk=0;
	if(cont_clk==50)
	{
		cont_clk=0;
		get_clock(&time_clk);
		analog_clk_SET_ANGULOS_PONTEIROS(&analog_clock,time_clk);
	}
	else
		cont_clk++;

}

ISR(TIMER2_COMP_vect){
	cont_encoder=(cont_encoder+1)%360;
	enable_send=1;
	
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
