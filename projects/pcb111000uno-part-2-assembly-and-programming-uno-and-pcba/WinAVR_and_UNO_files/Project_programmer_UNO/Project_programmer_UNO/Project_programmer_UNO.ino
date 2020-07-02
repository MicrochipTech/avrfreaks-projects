
/********ATMEGA programmer for flash, config bits and EEPROM. Runs on an ATMEGA328.


Used to program the EEPROM of either device but the flash of the PCB_A device only.
The UNO flash is programmed using the PCB_A bootloader.
The reason foor this is that the devices run under different configuration settings.


To make the program as easy as possible to use, the strings used for user prompts are 
stored in program memory space rather than the host EEPROM. 
Strings to be programmed to the target EEPROM are also temporarily stored in SRAM.
Use of an anything with less SRAM than the Atmega 328 to host this program is not therefore recommended.


The programmer runs under configuarion settings    Extended: 0xFF    High: 0xD7      Low: 0xE2
The important parameters are: Watch dog under program control, EEprom preserved through chip erase and 8HMz clock

Terminal port settings: 57600 Baud, 8 data bits, no parity, 1 stop bit and no handshaking.

ON_CHIP EEPROM reservation for Atmega 328
  0x3FD Default byte
  0x3FE User cal byte
  0x3FF User cal byte repeated
  0x3F9 0 if PCB_A has just been programed  
    
TARGET CHIP EEPROM reservations
0x0 Top of programmed text space  MSB
0x1 Top of programmed text space  LSB
0x2 Number of programmed data items
0x3 Bottom of application EEPROM space  MSB
0x4 Bottom of application EEPROM space  LSB   

A hex file that comes even near to filling the Atmega 328 program space must be split into 
two or more files which can be sent separately.  The LED flashes when the download is in progress.
As soon as it stops flashing the next file can be sent.

Note: it appears that the Arduino GUI polls its target device at regular intervals using the null character.
This interferres with PCB 111000_UNO when text files are being programmed to EEPROM.  The "EEPROM_programmer_sub.c" file
of this projects is therefore very slightly diffent from the eqivalent file used by "Project_programmmer_AVR". A line 
has been introduced to ensure that null characters received during a text file download are ignored.

*/


#include "Project_programmer_UNO.h"


int main (void){ 

unsigned int target_type = 0,  target_type_M;               //Device signature bytes      
unsigned char  op_code;
unsigned char keypress;
unsigned char OSCCAL_mini_OS;
int error_mag;

CLKPR = (1 << CLKPCE);
CLKPR = (1 << CLKPS0);                                      //Convert 16MHx xtal to 8MHx clock


setup_328_HW;                                               //see "Resources\ATMEGA_Programmer.h"

  
/*****************Power-up and make contact with target****************************/

while(1){
do{sendString("s  ");} 
while((isCharavailable(255) == 0));                           //User prompt 
if(receiveChar() == 's')break;}


Atmel_powerup_and_target_detect;                              //Leave target in programming mode                              

if (Atmel_config(read_fuse_bits_h, 0) == 0xFF) 
pcb_type = 1;                                                 //UNO pcb device
else pcb_type = 2;                                            //PC_A device

sendString ("\r\n\r\nATMEGA");
if (pcb_type == 1)sendString (" (UNO)");
else sendString (" (PCB_A)");

switch (target){
case 3281: sendString ("328"); break;
case 3282: sendString ("328P");break;
default: wdt_enable(WDTO_1S); while(1);break;}

PageSZ = 0x40; PAmask = 0x3FC0; FlashSZ=0x4000;                 //flash page size, gives address on page, flash size words
EE_top = 0x400-0x10;                                            //Last 16 bytes reseerved for system use
text_start = 0x5;                                               //First 5 bytes reserved for programmmer use

sendString(" detected\r\nPress -p- or -e- to program flash \
or EEPROM (or -x- to escape).");

while(1){
op_code = waitforkeypress();
switch (op_code){

case 'e': Prog_EEPROM(); break;

case 'd':                                                       //Delete contents of the EEPROM
sendString("\r\nReset EEPROM! D or AOK to escape");             //but leave cal data.
newline();
if(waitforkeypress() == 'D'){
sendString("10 sec wait");

for (int m = 0; m < EE_top; m++)  
{Read_write_mem('I', m, 0xFF);}
sendString(" Done\r\n");}
Exit_Programmer;break;

case 'p': 
case 'P': break;
case 'x':
case 'X': Exit_Programmer;break;
default: break;} 

if ((op_code == 'P') || (op_code == 'p')) break;} 

if (Atmel_config(read_fuse_bits_h, 0) == 0xFF) 
{sendString("\r\nCannot be used to program UNO flash\r\n");
Exit_Programmer;}

sendString("\r\nSend Program file (or x to escape).\r\n");

while ((keypress = waitforkeypress()) != ':')                     //Ignore characters before the first ':'
{if (keypress == 'x'){Exit_Programmer;}}                          //X pressed to escape

Initialise_variables_for_programming_flash;

UCSR0B |= (1<<RXCIE0); sei();                                     //Set UART Rx interrupt
(Atmel_config(Chip_erase_h, 0));                                  //Erase chip after first char of hex file has been received

Program_Flash();

/***Program target config space***/
Atmel_config(write_extended_fuse_bits_h,0xFD );                   //2.7V BOD
Atmel_config(write_fuse_bits_H_h,0xD0 );                          //WDT under prog control and Eeprom preserved, Reset vector 0x7000
Atmel_config(write_fuse_bits_h,0xE2 );                            //8MHz internal RC clock with 65mS SUT
Atmel_config(write_lock_bits_h,0xFF );                            //No  memory lock

Verify_Flash();  

sendString (Version);
sendString("Programming PCB_A");
sendString("\r\nConfig bytes: Fuses extended, \
high, low and lock\r\n");                                           //Print out fuse bytes, cal byte and file sizes
sendHex(16, Atmel_config(read_extended_fuse_bits_h, 0));
sendHex(16, Atmel_config(read_fuse_bits_H_h,0));  
sendHex(16, Atmel_config(read_fuse_bits_h, 0));
sendHex(16, Atmel_config(read_lock_bits_h, 0));


newline();
sendString("Hex_file_size:  ");
sendHex(10,cmd_counter); sendString("  d'loaded:  "); 
sendHex(10,prog_counter); sendString(" in:  "); 
sendHex(10,read_ops); sendString(" out\r\n");

if(pcb_type == 1)                                                   //UNO pcb: Note this version never programs the UNO flash
Read_write_mem('I', 0x3FC,0x80);                                    //Therefore this line is never executed
if(pcb_type == 2)                                                   //PCB_A
{Read_write_mem('I', 0x3F9, 0);                                     //Ensures that PCB_A jumps to mini-os immediately post programming
Read_write_mem('I', 0x3F1, 0);	                	                  //Only autocal PCB_A after programming flash.
Read_write_mem('I', 0x3F4, 0);                                      //I2C_V18 resets UNO when it first runs
}
Reset_H;                                                            //Set target device running          



/*************Auto cal mini-OS device*********************/
set_up_I2C;
waiting_for_I2C_master;  
OSCCAL_mini_OS = receive_byte_with_Ack();
error_mag = receive_byte_with_Ack() << 8;
error_mag += receive_byte_with_Nack();
clear_I2C_interrupt;
sendString("\r\nmini_OS OSCCAL user value   "); sendHex(10,OSCCAL_mini_OS);
sendString("calibration error  "); sendHex(10,error_mag);
/*********************************/

UCSR0B &= (~((1 << RXEN0) | (1<< TXEN0)));                        //Dissable UART
while(1);
return 1;}


/***************************************************************************************************************************************************/
ISR(USART_RX_vect){
  unsigned char Rx_Hex_char=0;
  unsigned char Rx_askii_char;
  int local_pointer;
    
Rx_askii_char = receiveChar();
switch (Rx_askii_char){
case '0':  Rx_Hex_char = 0x00; break;                                //Convert askii chars received from hex file to binary digits
case '1':  Rx_Hex_char = 0x01; break;
case '2':  Rx_Hex_char = 0x02; break;
case '3':  Rx_Hex_char = 0x03; break;
case '4':  Rx_Hex_char = 0x04; break;
case '5':  Rx_Hex_char = 0x05; break;
case '6':  Rx_Hex_char = 0x06; break;
case '7':  Rx_Hex_char = 0x07; break;
case '8':  Rx_Hex_char = 0x08; break;
case '9':  Rx_Hex_char = 0x09; break;
case 'A':  Rx_Hex_char = 0x0A; break;
case 'B':  Rx_Hex_char = 0x0B; break;
case 'C':  Rx_Hex_char = 0x0C; break;
case 'D':  Rx_Hex_char = 0x0D; break;
case 'E':  Rx_Hex_char = 0x0E; break;
case 'F':  Rx_Hex_char = 0x0F; break;
case ':':  counter = 0;  break;
default: break;}

switch (counter){
case 0x0:   break;                                              //Detect -:- at start of new line
case 0x1:   tempInt1 = Rx_Hex_char<<4;  break;                  //Acquire first digit 
case 0x2:   tempInt1 += Rx_Hex_char;                            //Acquire second digit and combine with first to obtain number of commands in line
      char_count = 9 + ((tempInt1) *2);                         //Calculate line length in terms of individual characters
      local_pointer = w_pointer++;                              //Update pointer to array "store"
      store[local_pointer] = tempInt1; break;                   //Save the number of commands in the line to the array  
case 0x3:   tempInt1 = Rx_Hex_char<<4;  break;                  //Next 4 digits give the address of the first command in the line
case 0x4: tempInt1 += Rx_Hex_char; 
      tempInt1=tempInt1<<8; break;                              //Acquire second digit and combine it with first 
case 0x5: tempInt1 += Rx_Hex_char<<4;  break;                   //Continue for third digit
case 0x6:   tempInt1 += Rx_Hex_char;                            //Acquire final digit and caculate address of next command 
      local_pointer = w_pointer++;                              //Update pointers to array "store"
      store[local_pointer] = tempInt1; break;                   //Save address of next command to array "store"
case 0x7:   break;                                              //chars 7 and 8 are not used
case 0x8:   break;
default:  break;}

if ((counter > 8)&&(counter < char_count)){                     //Continue to acquire, decode and store commands
if ((counter & 0x03) == 0x01){tempInt1 = Rx_Hex_char<<4;}       //Note: Final two chars at the end of every line are ignored
if ((counter & 0x03) == 0x02)  {tempInt1 += Rx_Hex_char;}
if ((counter & 0x03) == 0x03)  {tempInt2 = Rx_Hex_char<<4;}
if ((counter & 0x03) == 0x0)    {tempInt2+= Rx_Hex_char; 
                tempInt2=tempInt2<<8;tempInt1+=tempInt2;
local_pointer = w_pointer++;  
store[local_pointer] = tempInt1; cmd_counter++;}}

counter++;
w_pointer = w_pointer & 0b00011111; }                             //Overwrites array after 32 entries





/***************************************************************************************************************************************************/
void Program_Flash (void){

new_record();                                                 //Start reading first record which is being downloaded to array "store" 
start_new_code_block();                                       //Initialise new programming block (usually starts at address zero but not exclusivle so)
Program_record();                                             //Coppy commands from array "store" to the page_buffer                            
      
    
while(1){   
new_record();                                                 //Continue reading subsequent records
if (record_length==0)break;                                   //Escape when end of hex file is reached


if (Hex_address == HW_address){                              //Normal code: Address read from hex file equals HW address and lines contains 8 commands
switch(short_record){
case 0: if (space_on_page == (PageSZ - line_offset))          //If starting new page
      {page_address = (Hex_address & PAmask);}                //get new page address
      break;

case 1: start_new_code_block();                             //Short line with no break in file (often found in WinAVR hex files).
    short_record=0;break;}}
    
    
if(Hex_address != HW_address){                              //Break in file
  if (section_break){                                       //Section break: always found when two hex files are combined into one                    
    if((Flash_flag) && (!(orphan)))
    {write_page_SUB(page_address);}                         //Burn contents of the partially full page buffer to flash
    if(orphan) 
    write_page_SUB(page_address + PageSZ);}                 //Burn outstanding commands to the next page in flash     
    
  if(page_break)                                            //In practice page breaks and short jumps are rarely if ever found                      
    {if((Flash_flag) && (!(orphan)))                        //Burn contents of the partially filled page buffer to flash
    {write_page_SUB(page_address);}                           
    orphan = 0;}
    
  start_new_code_block();                                   //A new code block is always required where there is a break in the hex file.
  short_record=0;}
    
Program_record();}                                          //Continue filling page_buffer
    

cli();  
UCSR0B &= (~(1<<RXCIE0));                                   //download complete, disable UART Rx interrupt
LEDs_off;       
while(1){if (isCharavailable(1)==1)receiveChar();
    else break;}                                          //Clear last few characters of hex file
  
if((Flash_flag) && (!(orphan)))
{write_page_SUB(page_address);}                            //Burn final contents of page_buffer to flash
if(orphan) {write_page_SUB(page_address + PageSZ);}}


  

/***************************************************************************************************************************************************/
void Verify_Flash (void){


int  line_counter = 0, print_line = 0;                    //Controls printing of hex file                         
int line_no;                                              //Refers to the .hex file
signed int phys_address;                                  //Address in flash memory
signed int prog_counter_mem;                              //Initialised with size of .hex file used for programming
unsigned char print_out_mode = 0;                         //Print out flash contents as hex or askii characters
char skip_lines[4];                                       //Enter number to limit the print out

phys_address = 0;  read_ops=0; 
line_no = 0; prog_counter_mem = prog_counter; 

sendString("Integer(0-FF)?  ");                         //0 prints no lines -1-, every line, -8- prints every eighth line etc... 
skip_lines[0] = '0';                                    //Acquire integer between 0 and FF
skip_lines[1] = waitforkeypress();
skip_lines[2] = '\0';
timer_T1_sub(T1_delay_500mS); 
if (isCharavailable(1)){skip_lines[0] = skip_lines[1]; 
skip_lines[1] = receiveChar();} 
binUnwantedChars(); 
print_line = askiX2_to_hex(skip_lines);
sendHex (16,print_line); sendString("   ");

if (print_line == 0);                                   //hex file print out not required
else {sendString("1/2?\r\n");                           //else -1- sends file as askii, -2- sends it as hex
print_out_mode =  waitforkeypress(); 
binUnwantedChars();     
newline();}


while(1){ if(!(prog_counter_mem))break;               //print out loop starts here, exit when finished
    
while(1) {                                            //Start reading the flash memory searching for the next hex command

Hex_cmd = Read_write_mem('L',phys_address, 0x0);              
Hex_cmd = (Hex_cmd<<8) + (Read_write_mem('H',phys_address, 0x0)); 
phys_address++;        
if (phys_address == FlashSZ)break;                  //No more memory? Quit if yes
if (Hex_cmd != 0xFFFF) break;                       //If the hex command is 0xFFFF remain in this loop otherwise exit.
LEDs_on;}

LEDs_off;
if (phys_address == FlashSZ)break;                  //Exit when there is no more flash to read

if ((print_line == 0)  && (!(line_no%10)))
sendChar('*');                                      //Print out of hex file not required
                                                    //Print a -*- every tenth line of the file
if(print_line && (!(line_no%print_line)))           //Print out required: Print all lines or just a selection     
{newline(); sendHex (16, (phys_address-1)*2);   
sendString("   "); line_counter++;  
if(print_out_mode == '1'){send_as_askii;}           //Start with the address of the first command in the line
else sendHex (16, Hex_cmd);}                        //Print first command in askii or hex
read_ops++;                                         //Value to be sent to PC for comparison with the hex filer size
prog_counter_mem--;                                 //"prog_counter_mem" decrements to zero when the end of the file is reached


for(int m=0; m<7; m++){                             //Read the next seven locations in the flash memory   

Hex_cmd = Read_write_mem('L',phys_address, 0x0);        
Hex_cmd = (Hex_cmd<<8) + (Read_write_mem('H',phys_address, 0x0)); 
phys_address++; 
if(Hex_cmd == 0xFFFF)break;                         //Read 0xFFFF: return to start of print out loop
prog_counter_mem--;

if ((print_line) &&  (!(line_counter%2))) {LEDs_on;} else {LEDs_off;}              

if(print_line && (!(line_no%print_line)))
{if(print_out_mode == '1'){send_as_askii;} 
else sendHex (16, Hex_cmd);}
read_ops++;

if(phys_address==FlashSZ)break;}
if ((print_line)&&(!(line_no%print_line)) && (!(line_counter%8)))sendString("\r\n");
line_no++;
if (phys_address == FlashSZ)break;}

LEDs_off;
newline();newline(); }



/***********************************************************/
unsigned char receive_byte_with_Ack(void){
char byte;
TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);          //Set Ack enable and clear interrupt
while (!(TWCR & (1 << TWINT)));                           //Wait for interrupt
byte = TWDR;
return byte;}



/***********************************************************/
unsigned char receive_byte_with_Nack(void){
char byte;
TWCR = (1 << TWEN) | (1 << TWINT);                        //Set Ack enable and clear interrupt
while (!(TWCR & (1 << TWINT)));                           //Wait for interrupt
byte = TWDR;
return byte;}
