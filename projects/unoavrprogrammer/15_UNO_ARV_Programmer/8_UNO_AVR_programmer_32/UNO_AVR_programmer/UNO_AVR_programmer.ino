
#include "UNO_AVR_programmer.h"
#define Version "UNO_programmer_V1.5\r\n" 

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


  
