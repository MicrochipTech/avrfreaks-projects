/*
 Programming AVR devices with a UPDI interface
 Arduino sketch to be downloaded to a UNO
 Open a terminal program before running the shetch.
 Set terminal to 28800Hz, 8 data bits, 1 stop bits, no parity and no handshaking
 User prompt R R R..... is generated
 Press 'r' and then respond to the on-screen instructions
 If the "compiled binary file" is exported (see Shetch drop down menue)
 it can be copied to an Atmega328 using the UNO AVR Programmer.
 The only difference is that the baud rate must be reduced to 19.2K
 (Note: The internal RC clock may also need calibrating but this can usually
 be avoided if several devices are available.)

 This Sketch has only been tested with ATtiny 806/1606 both excellent devices for the
 newcommer: They are available in a SOIC package that can be hand soldered and they have 
 hardware I2C and UART modules rather the the USI that is software driven. 
 Test projects have been written for the ATtiny1606 using Atmel Studio 7.
 
Note:  It is hoped that any newcommer persuing this project will find both Arduino and  
Studio 7 equally invaluable when attempting to build projects in the easiest and most flexible way.
Programmers note pad is also highly recommended for viewing C-files that Arduino can't open easily.  
 */
 
//Select device type and text size here
//text bytes must be a multiple of 256 characters
#define device_type 1606
#define text_bytes  2048

//#define device_type 806
//#define text_bytes  1024




#include "Resources_UNO_UPDI_Programmer_V1a.h"
char mode = 0;
int main (void){ 
char kepress;
setup_328_HW;                                           //see "Resources\ATMEGA_Programmer.h"
P_counter = 0;                                          //Used to check parity of data received from UPDI
User_prompt;

sendString("\r\nPress 'a' to program target or AOK to run taget code\r\n");
if(waitforkeypress() == 'a') 

{sendString("\r\nUNO_UPDI_Programer_V1a (also works on an Atmega 328 clocked at 8MHz)\r\n\r\n");
sendString ("Press any key to start\r\n\r\n");
waitforkeypress();

/********************************Programmer target connection sequence************************************/
Timer_T1_sub_with_interrupt(T1_delay_100ms);
sei();
contact_target;
configure_updi;                                       //Increases UPDI clock to 16MHz and reduce guard band to 2 cycles
print_out_SIB;
if(UPDI_timeout)
{sendString("Power Cycle Required\r\n");
while(1);}
else{cli();TCCR1B = 0;
TIMSK1 &= (~(1 << TOIE1));}


initialise_NVM_programming;
sendString("Programming fuses\r\n");
write_fuse (WDTCFG, 0x0);                             //Default value: WDT under program control
write_fuse (BODCFG,0x0);                              //Default value: BOD dissabled
write_fuse (OSCCFG, 0x7E);                            //Default value: 20MHz internal clock
write_fuse (SYSCFG0, 0xF7);                           //UPDI enabled, EEPROM preserved at chip erase
write_fuse (SYSCFG1, 0xFD);                           //16mS SUT
write_fuse (APPEND, data_blocks);                      //For Attiny 1606:2Kbyte data partition for text
write_fuse (BOOTEND, 0x0);                            //Default value: No boot partition



sendString("\r\nProgram flash with hex? -y- or AOK");
if (waitforkeypress() == 'y')
{ mode = 'H';
  sendString("\r\nSend file  ");
  Program_Flash_Hex();
}
Verify_Flash_Hex();
sendString("\r\nFuses:\tWDT, BOD, OSC,SYS0\tSYS1, APPEND, BOOTEND\r\n\t");
read_out_fuses();

sendString("\r\n\r\nProgram flash with text? -y- or AOK");
if (waitforkeypress() == 'y')
{ mode = 'T';
  Program_Flash_Text();}




sendString("\r\n\r\nUnlocking device");
write_fuse (0x128A, 0xC5);                              //Device unlocked
sendString("\r\n-y- to Lock device or AOK");
if (waitforkeypress() == 'y')
{write_fuse (0x128A, 0x0);
sendString("\r\nDevice locked\r\n");}
else sendString("\r\nDevice unlocked\r\n");

UPDI_reset;

sendString("Sesion terminated\r\n\r\n");
Dissable_UPDI_sesion;}

/************Code in this section is test code.  It is not part of the UPDI programmer******************
******I*****Its purpose is to relay signals from the UNO UART port to the ATtiny UART ****************
******************so that the operation of the progammer can be tested easily*************************/


sendString("\r\nRun trial application? -y- or AOK (POR may be required)\r\n");
if(waitforkeypress() == 'y'){

//inter pcb baud rate is 9.6K:  requires delay of 104uS 
//UNO receives from ATtiny 1606 on C5 (UNO pin A5)
//UNO transmits to ATtiny 1606 on C4 (UNO pin A4)

DDRC |= (1 << DDC4);
PORTC |= (1 << PORTC4);                                     //UNO UART TX port is output high
while(1){

while((PINC & (1 << PINC5)) && (!(UCSR0A & (1 << RXC0))));  //wait for data from PC or ATtiny1606

if(!(PINC & (1 << PINC5))){                                 //Start bit detected
data_byte_Rx = 0;
_delay_us(52);                                              //Half clock delay
for(int n = 0; n <= 7; n++){                                //Receive 8 data bits
_delay_us(104);                                             //Full clock delay
if (PINC & (1 << PINC5))Rx_bit = 1;                         //Sample bit
else Rx_bit = 0;
if(Rx_bit){data_byte_Rx |= (1 << n);}}                      //Update byte
_delay_us(104);                                             //Stop bit delay
sendChar(data_byte_Rx);}


if(UCSR0A & (1 << RXC0)){                                   //Detect byte from PC
data_byte_Tx  = UDR0;                                       //Read the byte
PORTC &= (~(1 << PORTC4));                                  //Generate start bit        
_delay_us(104);
for(int n = 0; n <= 7; n++){
Tx_bit = (data_byte_Tx & (1 << n));
if(Tx_bit){PORTC |= (1 << PORTC4);}                         //8 data bits
else {PORTC &= (~(1 << PORTC4));}
_delay_us(104);}
PORTC |= (1 << PORTC4);                                     //Stop bit
_delay_us(104);}
}}

/**********************************End to test code section*********************************************/

SW_reset;
return 1;}





/***************************************************************************************************************************************************/
ISR(USART_RX_vect){if (mode =='H')upload_hex();
                    if (mode =='T')upload_text();}
                    
ISR(TIMER1_OVF_vect) {
  if(!(T1OVFM)){UPDI_timeout = 1;TCCR1B = 0;}
  
  if(T1OVFM == 1){
    if(text_started == 3){T1OVFM = 0;
    endoftext -= 1; 
  TCCR1B = 0; TIMSK1 &= (~(1 << TOIE1));       
  inc_w_pointer; store[w_pointer] = 0;                         
  inc_w_pointer; store[w_pointer] = 0;}}}


/***********************************************************************************************/

/*
Routines used to drive the UART interface when progamming config bytes and for other non-time
critical operations.  Use of subroutines rather than code definitions make efficient use of program
memory but is not so good on speed.
 */


void UART_Tx(unsigned int data_byte_T){               //starts Hi Z
  unsigned char parity = 0;
  DDRC |= (1 << DDC0);                                //start bit
  clock_delay_T;

for (int n = 0; n <= 7; n++){
  if (data_byte_T & (1 << n))
  {DDRC &= (~(1 << DDC0)); parity += 1;}
 else {DDRC |= (1 << DDC0);}clock_delay_T;}           //clock data out

if (parity%2){DDRC &= (~(1 << DDC0));} 
else {DDRC |= (1 << DDC0);}
  clock_delay_T;                                      //clock parity bit
  DDRC &= (~(1 << DDC0));                             //initiate stop bit 
  clock_delay_T;                                      //Stop bits
  clock_delay_T;}




  
/************************************************************************************************/
unsigned char UART_Rx(void){
  unsigned char data_byte_R = 0;
    char parity = 0;
                          
  while (PINC & (1 << PINC0));                        //wait for start bit
  clock_delay_R;                                      //The full delay is required 
  for (int n= 0; n <= 7; n++){clock_delay_R;
    if (PINC & (1 << PINC0))
    {data_byte_R |= (1 << n); parity += 1;}}
    clock_delay_R;

 if ((PINC & (1 << PINC0)) && (parity%2)); 
 else P_counter++;                                      //Parity often received in error?????
 
 clock_delay_R;                                         //stop bit
 clock_delay_R;                                         //stop bit  
 return data_byte_R;}
