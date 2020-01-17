
/*Proj_6G_message_from_the_OS

Loaded with Hex_txt_programmer but requires PCB_A_Mini_OS_I2C_V16_1_CC to function.
*********************************************************************/





#include "Message_from_the_OS_header_file.h"



int main (void){
char receive_byte, num_bytes=0;

set_up_switched_inputs;

waiting_for_I2C_master;									//Initiate comuninations: master requests data
send_byte_with_Ack(num_bytes);							//Slave sends payload size
send_byte_with_Nack('H');								//Slave sends mode
clear_I2C_interrupt;									//Complete transaction

waiting_for_I2C_master;									//Initiate comuninations:master to send string

do{receive_byte = receive_byte_with_Ack();				//print out string as characters are received
if(receive_byte)Char_to_PC(receive_byte);}
while(receive_byte);									//Detect '\0' character used to terminate a string
receive_byte = receive_byte_with_Nack();				//Detect second '\0' char
clear_I2C_interrupt;
Char_to_PC('\r');Char_to_PC('\n');


while(switch_2_down);
wdt_enable(WDTO_60MS); while(1);}	



/***********************************************************/
void send_byte_with_Ack(char byte){
TWDR = byte;											//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);		//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}



/***********************************************************/
void send_byte_with_Nack(char byte){
TWDR = byte;											//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEN);						//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}



/***********************************************************/
char receive_byte_with_Ack(void){
char byte;
TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);		//Set Ack enable and clear interrupt
while (!(TWCR & (1 << TWINT)));						//Wait for interrupt
byte = TWDR;
return byte;}



/***********************************************************/
char receive_byte_with_Nack(void){
char byte;
TWCR = (1 << TWEN) | (1 << TWINT);						//Set Ack enable and clear interrupt
while (!(TWCR & (1 << TWINT)));						//Wait for interrupt
byte = TWDR;
return byte;}



/***********************************************************/
void Char_to_PC(char data){
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = data;}
