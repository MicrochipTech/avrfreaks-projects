
#define F_CPU 3333333
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)


#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#define set_flash_sz \
switch (device_type){\
	case 806: FlashSZ = 0x1000; break;\
case 1606: FlashSZ = 0x2000; break;}


unsigned int FlashSZ;													//Size of ATtiny 1606 flash in 16 bit words
unsigned int flash_start = 0x8000;										//Program start address in 8 bit bytes
unsigned int text_start_address;


void Timer_TCA(char, unsigned int);

void USART0_init(void);
void char_to_PC(char);
void string_to_PC(char *str);

char isCharavailable (int);
char char_from_PC(void);
char waitforkeypress(void);

void reverse(char *, int);
long longToStr(long , char *, int );
signed char ftoaL(float, char *);							//Local version of the float to askii routine

unsigned char string_counter(void);
void print_string_num(int);
long Num_from_KBD(char*);
char get_next_char(void);

char User_response;

char Int_array[8];									//Used to obtain integer string from keyboard
char Float_array[12], power_array[12];				//Used to obtain float string from keyboard
char expt_array[4];									//Used by ftoaL to hold exponent											
signed char expt;
int string_length;
long  Int_num;
float Float_num, power;
	
	
/************************Used by Text_reader.c*********************/	
unsigned char line_length = 0;
char next_char;
int char_counter;
volatile char text_charL, text_charH, next_char_ptr;
volatile char * txt_ptr;






#define setup_HW \
USART0_init();\
set_flash_sz;

#define SW_reset \
CCP = 0xD8;WDT.CTRLA = 0x03; while(1);

#define TCA_10s		7, 32552
#define TCA_5s		6, 65104	
#define TCA_1s		5, 52083
#define TCA_500ms	5, 26042
#define TCA_100ms	5, 5208
#define TCA_10ms	5, 52


#define User_prompt \
while(1){\
	do{string_to_PC("R?    ");}	 while((isCharavailable(80) == 0));\
	User_response = char_from_PC();\
if((User_response == 'R') || (User_response == 'r'))break;}


