/*
 * rs232.c
 *
 * Created: 27-05-2011 20:12:06
 *  Author: limapapy
 * TXCn check the transmitter has complete.
 * RXCn check if data on receiver.
 */ 
#include <stdio.h>
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "globals.h"
#include "rs232.h"

char tcl[3];

//initialize serial interface
void serial_init(unsigned int ubrr){
	//set the baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)(ubrr);
	//enable transmitter and receiver
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	//set frame format 8 bit for data 1 stop bit
	UCSR0C = (3<<UCSZ00);	
	UCSR0B |=(1<<RXCIE0); //enable usart receiver interrupt
	
	sei();	//enabling global interrupt
	
}

//send a character through serial port
void serial_send(unsigned char data){
	
	/*wait until the buffer is empty*/
	while (!(UCSR0A & (1<<UDRE0)));
	
	//when is ready put the data in the register to be sent
	UDR0 = data;
	
}

//receive a character from serial port
unsigned char serial_receive(void){
	//wait for data 
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void serial_send_message(unsigned char *msg){
	int i;
	int len=strlen((char *)msg);
	for (i=0;i<len;i++)
	{
		serial_send(msg[i]);
	}
	
}

void set_clock(void){
	unsigned char data[6];
	unsigned int i;
	int number=0;
	for (i=0;i<6;i++)
	{
		data[i]=USART_WORD[i+CMD_SIZE+1];
		if (data[i]<0x30 || data[i]>0x39 )
		{
			number=1;
			break;
		}
	}
	if (number==1){
		serial_send(0x0D);
		serial_send_message((unsigned char*)"Invalid Clock Format");
		return;
	}
	time_clk.hour=((data[0] & 0x0F)<<4)|(data[1] & 0x0F );
	time_clk.min=((data[2] & 0x0F)<<4)|(data[3] & 0x0F );
	time_clk.sec=((data[4] & 0x0F)<<4)|(data[5] & 0x0F );
	return;
}

command decode_CMD(unsigned char *cmd){
	if (strncasecmp((char*)cmd,"SCLK",4)==0){
		return SET_CLOCK;
	} 
	else if(strncasecmp((char*)cmd,"GCLK",4)==0){
		return GET_CLOCK;
	}
	else if (strncasecmp((char*)cmd,"SPIC",4)==0){
		return SEND_PIC;
	}else if (strncasecmp((char*)cmd,"DPIC",4)==0){
		return DISP_PIC;
	}else if (strncasecmp((char*)cmd,"MODE",4)==0){
		return MODE;
	}
	else if (strncasecmp((char*)cmd,"NCLR",4)==0){
		return NUMBER_COLOR;
	}
	else if (strncasecmp((char*)cmd,"PCLR",4)==0){
		return POINTER_COLOR;
	}
	else if (strncasecmp((char*)cmd,"HELP",4)==0){
		return HELP;
	}
	else if (strncasecmp((char*)cmd,"PATT",4)==0){
		return PATTERN;
	}
	else{
		return INVALID_CMD;
	}
		
	
}

void reset_values(void){
	memset(USART_WORD,0,MAX_SIZE_WORD);
	USART_WORD_INDEX=0;
	time_clk.hour=0;
	time_clk.min=0;
	time_clk.sec=0;
}


int get_mode(unsigned char *mode){
	if (strncasecmp((char*)mode,"ACLK",4)==0){
		return ACLK;
	} 
	else if(strncasecmp((char*)mode,"DCLK",4)==0){
		return DCLK;
	}
	else if(strncasecmp((char*)mode,"PICT",4)==0){
		return PICT;
	}
	else{
		return -1;
	}
	
}	

rgb_t get_color(unsigned char* color){
	unsigned char col[3];
	unsigned char i;
	rgb_t tmp_color;
	for (i=0;i<6;i+=2){
		if ((color[i]>=0x41 && color[i]<=0x46) || (color[i]>=0x61 && color[i]<=0x66)){
			col[i>>1]=((0x0F & color[i])+9)<<4;
		}else{
			col[i>>1]=((color[i] & 0x0F)<<4);			
		}
		
		if ((color[i+1]>=0x41 && color[i+1]<=0x46) || (color[i+1]>=0x61 && color[i+1]<=0x66)){
			col[i>>1]|=((0x0F & color[i+1])+9);
		}else{
			col[i>>1]|=(color[i+1] & 0x0F);			
		}
	}
	
	tcl[0]=col[0];
	tcl[1]=col[1];
	tcl[2]=col[2];
	tmp_color.r=col[0]>>6;
	tmp_color.g=col[1]>>6;
	tmp_color.b=col[2]>>6;
	return tmp_color;
}



unsigned char *get_msg_ROM(int index){
	unsigned char *msg;
	msg=(unsigned char*)malloc(80*sizeof(unsigned char));
	memset(msg,0,80);
	strcpy_P((char*)msg,(PGM_P)pgm_read_word(&(messages[index])));
	return msg;
}	


void serial_int(command cmd){
	
	
}