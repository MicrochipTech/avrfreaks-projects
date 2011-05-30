/*
 * rs232.h
 *
 * Created: 27-05-2011 20:06:19
 *  Author: limapapy
 */ 


#ifndef RS232_H_
#define RS232_H_

#include "globals.h"

//#define FOSC				16000000UL //clock speed
#define FOSC				20000000UL //clock speed
#define BAUD				19200
#define BAUD_PRESCALE		FOSC/16/BAUD-1		//to fill the baud rate register 
#define UNVALID				"UNVALID DATA"
#define CMD_SIZE			4
#define ENTER				0x0D
#define BACK_SPACE			0x08


typedef enum{
	INVALID_CMD=-1,
	SET_CLOCK=0,
	GET_CLOCK=1,
	SEND_PIC=2,
	DISP_PIC=3,
	MODE=4,
	NUMBER_COLOR=5,
	POINTER_COLOR=6,
	PATTERN=7,
	HELP=8
	
}command;

extern unsigned int hours;
extern unsigned int minuts;
extern unsigned int seconds;
extern unsigned char USART_WORD[];
extern unsigned USART_WORD_INDEX;


void serial_init(unsigned int ubrr);
void serial_send(unsigned char data);
unsigned char serial_receive(void);
void set_clock(void);
void serial_send_message(unsigned char *msg);
command decode_CMD(unsigned char *cmd);
void reset_values(void);
int get_mode(unsigned char *mode);
rgb_t get_color(unsigned char* color);
unsigned char *get_msg_ROM(int index);

#endif /* RS232_H_ */