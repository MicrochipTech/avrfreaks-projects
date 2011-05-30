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

unsigned char tcl[3];
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
		serial_send_message((unsigned char*)"\r\npropeller>>Invalid Clock Format");
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
	}else if (strncasecmp((char*)cmd,"SPIC",4)==0){
		return SHOW_PICTURE;
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
		return INV;
	}
	
}	

rgb_t *get_color(unsigned char* color){
	unsigned char col[3];
	unsigned char i;
	rgb_t *tmp_color=(rgb_t*)malloc(sizeof(rgb_t));
	
	for (i=0;i<6;i+=2){
		if ((color[i]>='A' && color[i]<='F') || (color[i]>='a' && color[i]<='f')){
			col[i>>1]=((0x0F & color[i])+9)<<4;
			tcl[i>>1]=col[i>>1];
		}else if(color[i]>='0' && color[i]<='9'){
			col[i>>1]=((color[i] & 0x0F)<<4);			
			tcl[i>>1]=col[i>>1];
		}else{
			free(tmp_color);
			return NULL;
		}
		
		if ((color[i+1]>='A' && color[i+1]<='F') || (color[i+1]>='a' && color[i+1]<='f')){
			col[i>>1]|=((0x0F & color[i+1])+9);
			tcl[i>>1]=col[i>>1];
		} else if(color[i+1]>='0' && color[i+1]<='9'){
			col[i>>1]|=(color[i+1] & 0x0F);	
			tcl[i>>1]=col[i>>1];		
		}else{
			free(tmp_color);
			return NULL;
		}
	}
	tmp_color->r=col[0];
	tmp_color->g=col[1];
	tmp_color->b=col[2];
	return tmp_color;
}



unsigned char *get_msg_ROM(int index){
	unsigned char *msg;
	msg=(unsigned char*)malloc(80*sizeof(unsigned char));
	memset(msg,0,80);
	strcpy_P((char*)msg,(PGM_P)pgm_read_word(&(messages[index])));
	return msg;
}

void serial_send_num(short int num){
	short int num2=num,ten=10;
	short int i=0;
	unsigned char res,*nums;
	while(num2>0){
		mod_div(&num2,&ten,&num2,&res);
		i++;
	}
	nums=(unsigned char*)malloc((i+1)*sizeof(unsigned char));
	num2=num;
	nums[i]=0;
	while(num2>0){
		mod_div(&num2,&ten,&num2,(short int *)&res);
		nums[i]=res |0x30;
	}
	serial_send_message(nums);
}	



void serial_int(command cmd){
	
	
}