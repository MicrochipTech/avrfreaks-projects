



#define F_CPU 2000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>


#define display_tick	4000							//4000 2mS assuming a clock of 2MHz
#define comms_tick		200								//1mS per transaction of 10 bits
#define half_comms_tick	100		


#define wait_for_clock_tick \while (!(TCA0_SINGLE_INTFLAGS & TCA_SINGLE_CMP0EN_bm));\TCA0_SINGLE_INTFLAGS |= TCA_SINGLE_CMP0EN_bm;


#define inc_display_clock			TCA0_SINGLE_CMP0 += display_tick;
#define inc_comms_clock				TCA0_SINGLE_CMP0 += comms_tick;
#define inc_half_comms_clock		TCA0_SINGLE_CMP0 += half_comms_tick;


void Start_TCA0(void);									//Timer used to control display/comms multiplexing 
void Start_TCB0(int);									//Used to control display intensity
void Display_driver(void);								//Display multiplexer
void Char_definition(void);								//Defines a digit in terms of its segments
void comms_transaction(void);							//Transfer data or string
void Transmit_data_byte (char);							//Tx/Rx FW modules which communicate with UNO
char Receive_data_byte (void);



volatile char Tx_symbol, Rx_symbol, Rx_symbol_bkp;
volatile char transaction_type = 0;						//Data/string transfer to/from UNO
volatile char transaction_complete = 0;					//Set to 1 when a data transfer is complete
volatile int byte_counter = 0;							//Counts bytes sent to or received from UNO

volatile int cmp0_bkp;									//Used to merge comms transaction and display multiplexer

char display_buffer[15];
char temp_buffer[15];	
volatile int display_ptr;								//Points to next digit to be driven
long Long_Num_from_UNO, Long_Num_to_UNO;
float Float_Num_to_UNO, Float_Num_from_UNO;
char cr_keypress = 0;									//Set to 1 when user presses carriage return
unsigned char data_byte[4];								//32 bit numbers are split into 4 bytes for transmission
volatile int data_byte_ptr = 0;							//Points to next FPN/long byte to be transfered to the UNO

char * char_ptr;										//Used to convert float to four separate bytes
float * float_ptr;
char sign;
signed char expt;

volatile char busy_flag = 0;							//Data processing in progress: Do not poll UNO
int brightness_control;
 
 
 
#define display_buffer2temp \
null_bit_counter = 0; \
clear_temp_buffer;\
for(int m = 0; m <= 14; m++)\
{if(!(display_buffer[14-m]))null_bit_counter += 1; else break;}\
for(int m = 0; m <= 14-null_bit_counter; m++)\
temp_buffer[m] = display_buffer[14-null_bit_counter - m];



#define temp2display_buffer \
null_bit_counter = 0;\
for(int m = 0; m <= 14; m++)\
{if(!(temp_buffer[14-m]))null_bit_counter += 1; else break;}\
for(int m = 0; m <= 14-null_bit_counter; m++)\
display_buffer[m] = temp_buffer[14-null_bit_counter - m];



#define Combine_dp \
array_ptr = 0;\
if (temp_buffer[0] == '.'){temp_buffer[0] = '0' | 0x80;}\
else\
{for(int m = 0; m <= 14; m++)\
{if (temp_buffer[m] != '.' )continue;\
else array_ptr = m; break;}\
\
temp_buffer[array_ptr-1] |= 0x80;\
for (int m = array_ptr; m <=13; m++)\
temp_buffer[m] = temp_buffer[m+1];}



#define extract_dp \
array_ptr = 0;\
for(int m = 0; m <= 14; m++)\
{if (!(temp_buffer[m] & 0x80))continue;\
else temp_buffer[m] &= 0x7F;\
array_ptr = m; break;}\
for(int  m = 14; m >=array_ptr+2; m--)\
temp_buffer[m] = temp_buffer[m-1];\
temp_buffer[array_ptr + 1] = '.';



#define Remove_lagging_zeros \
array_ptr = 14;\
while ((temp_buffer[array_ptr] == 0) ||\
(temp_buffer[array_ptr] == '0'))\
{temp_buffer[array_ptr] = 0;\
array_ptr -= 1;}


#define Left_justify_number \
while(1){array_ptr = 0;\
for (int m = 14; m; m--)\
display_buffer[m] = display_buffer[m - 1];\
display_buffer[array_ptr++] = 0;\
if (display_buffer[7]) break;}


#define check_for_dp \
array_ptr = 14;\
while ((array_ptr)&&\
(!(display_buffer[array_ptr] & 0x80)))\
array_ptr -= 1;



#define Add_dp_if_missing \
if (!(array_ptr)){\
array_ptr = 0;\
for(int m = 0; m <= 14; m++)\
{if((display_buffer[m] == 'E') ||\
(display_buffer[m] == 'e'))array_ptr = m;}\
if(array_ptr)\
display_buffer[array_ptr + 1] |= 0x80;\
\
if (!(array_ptr))\
{array_ptr = 14;\
while ((array_ptr)&&\
(!(display_buffer[array_ptr] & 0x80)))\
array_ptr -= 1;\
if (!(array_ptr))\
display_buffer[0] |= 0x80;}}



#define Insert_sign \
if (sign == '-'){\
for(int  m = 14; m; m--){\
temp_buffer[m] = temp_buffer[m - 1];}\
temp_buffer[0] = '-';}





#include "../Resources/One_wire_transactions.c"
#include "../Resources/Display_header.h"
#include "../Resources/FPN_subroutines.c"

