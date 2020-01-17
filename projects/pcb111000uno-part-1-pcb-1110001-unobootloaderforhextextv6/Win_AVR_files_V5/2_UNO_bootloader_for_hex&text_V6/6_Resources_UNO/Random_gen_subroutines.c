
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>


void I2C_Tx(char, char, char*);
void send_byte_with_Ack(char);
void send_byte_with_Nack(char);


#define waiting_for_I2C_master \
TWCR = (1 << TWEA) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));\
TWDR;


/************************************************************************************/
void I2C_Tx_2_integers(unsigned int s1, unsigned int s2){			
char num_bytes=4; char mode=1; char s[4];
for (int m = 0;  m < 4; m++){
switch (m){
case 0: s[m] = s1; break; 											//Send S1 lower byte
case 1: s[m] = s1 >> 8; break; 									//Send S1 higher byte									
case 2: s[m] = s2; 	break;											//Send S1 lower byte
case 3: s[m] = s2 >> 8; break;}}									//Send S1 higher byte
I2C_Tx(num_bytes,mode, s);}



/************************************************************************************/
void I2C_Tx(char num_bytes, char mode, char s[]){
waiting_for_I2C_master;
send_byte_with_Ack(num_bytes);
send_byte_with_Ack(mode);
for (int m = 0;  m < num_bytes; m++){
if (m==num_bytes-1){send_byte_with_Nack(s[m]);}
else {send_byte_with_Ack(s[m]);}}
TWCR = (1 << TWINT);}



/************************************************************************************/
void send_byte_with_Ack(char byte){
TWDR = byte;													//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);				//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}



/************************************************************************************/
void send_byte_with_Nack(char byte){
TWDR = byte;													//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEN);								//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}
