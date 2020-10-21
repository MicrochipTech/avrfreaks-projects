
/*
Enables UNO to program the following Atmega devices

40 pin Atmega 644 device with 64k of flash
28 pin Atmega 328 family with 4k to 32k of flash
20 pin ATtiny 84 family with 2K to 8K of flash
14 pin ATtiny 268 family with 2K to 8K of flash
All these devices appear to be of a similar vintage and the default configuration bytes should work for them all
as follows:
8Mhz internal clock; WDT under program control; EEPROM preserved at chip erase; Reset pin; reset to address zero.
No effort has been made to set the Brown out detector (BOD) or boot space.  BOD is of course essential for battery powered devices.

Two other devices are included: the Atmega32 and ATtiny 26.  
These appear to be of a previous vintage and the default configuration bytes will not work as described above.

Note ATtiny 84A family devices are essentially the same as ATtiny 84 devices and have the same signature bytes
The same appears to be true for the ATtiny 268 and 268A devices.
Members of the Atmega 328 and 328P families appear to be completely interchangeable but do have different configuration bytes.

VERSION 2


Version 2.0 is the same as version 1.5 but with the following additional features:
It can detect and program the ATtiny 84 and 268 devices.

If at the user prompt "Press -p- to program flash, -e- for EEPROM, -r- to run target or -x- to escape."
-t- is pressed the programmer puts a 65.536 mS square wave on its PB5 (SCK) pin.  This is already conected to the 
target which can use it to calibrate its internal RC clock in place of a watch crystal. 


*/






#include "UNO_AVR_programmer.h"
#define Version "UNO_programmer_V2.0\r\n" 

int main (void){ 

setup_328_HW;                                                     //see "Resources\ATMEGA_Programmer.h"

Reset_L;                                                          //Put target in reset state to dissable UART

while(1){
do{sendString("s  ");} 
while((isCharavailable(255) == 0));                               //User prompt 
if(receiveChar() == 's')break;}

Atmel_powerup_and_target_detect;                                  //Leave target in programming mode                              


sendString(" detected.\r\nPress -p- to program flash, \
-e- for EEPROM, -r- to run target or -x- to escape.");

while(1){
op_code = waitforkeypress();
switch (op_code){

case 'r': Exit_programming_mode; break;                      //Wait for UNO reset
case 'R': Verify_Flash_Text();  SW_reset; break;
case 'e': Prog_EEPROM(); SW_reset; break;
case 't': set_cal_clock();break;

case 'd':                                                       //Delete contents of the EEPROM
sendString("\r\nReset EEPROM! D or AOK to escape");             //but leave cal data.
newline();
if(waitforkeypress() == 'D'){
sendString("10 sec wait");
for (int m = 0; m <= EE_top; m++)  
{Read_write_mem('I', m, 0xFF);}                                 //Write 0xFF to all EEPROM loactions bar the top 3
sendString(" Done\r\n");}
SW_reset;break;

case 'x': SW_reset; break;
default: break;} 

if ((op_code == 'p')||(op_code == 'P')) break;} 
sendString("\r\nSend hex file (or x to escape).\r\n");

Program_Flash_Hex();
Verify_Flash_Hex();

sendString("\r\nText_file? y or n\r\n");
if (waitforkeypress() == 'y')
{op_code = 't';                                                 //Required by UART ISR
Program_Flash_Text();}


sendString (Version);
newline();

Read_write_mem('I', EE_size - 4, \
(Atmel_config(signature_bit_2_h, signature_bit_2_l)));          //Define target type on target device
Read_write_mem('I', EE_size - 5, \
(Atmel_config(signature_bit_3_h, signature_bit_3_l)));       


Exit_programming_mode;                                                  //Wait for UNO reset
return 1;}





/***************************************************************************************************************************************************/
ISR(USART_RX_vect){
switch (op_code){
case 't': upload_text();break;
case 'p':
case 'P': upload_hex(); break;}}


/***************************************************************************************************************************************************/
ISR(TIMER2_OVF_vect) {                                          //Timer2 times out and halts at the end of the text file
if(text_started == 3)                                           //Ignore timeouts occurring before start of file download
  {endoftext -= 1; TCCR2B = 0; TIMSK2 &= (~(1 << TOIE2));       //Shut timer down
  inc_w_pointer; store[w_pointer] = 0;                          //Append two '\0' chars to the end of the text
  inc_w_pointer; store[w_pointer] = 0; }}



/***************************************************************************************************************************************************/
void set_cal_clock(void){

sendString("\r\n\r\nSquare wave with 65.536mS period on PB5\r\n");
UCSR0B &= (~((1 << RXEN0) | (1<< TXEN0)));
initialise_IO;
Set_LED_ports;
LEDs_off;
DDRB |= 1 << DDB5;
PORTB &= (~(1 << PORTB5));                                       //Output low
TCNT0 = 0;
TCCR0B = (1 << CS02) | (1 << CS00);                             //7.8125 KHz clock counts to 256 in 32.768mS                    

Reset_H;

while(1){
while(!(TIFR0 & (1<<TOV0)));
TIFR0 |= (1<<TOV0);
PORTB ^= (1 << PORTB5);}}
