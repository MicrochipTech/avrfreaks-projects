
#include "UART_definitions.h"

void upload_hex(void){
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
case 0x0:   break;													//Detect -:- at start of new line
case 0x1:   tempInt1 = Rx_Hex_char<<4;  break;						//Acquire first digit 
case 0x2:   tempInt1 += Rx_Hex_char;								//Acquire second digit and combine with first to obtain number of commands in line
      char_count = 9 + ((tempInt1) *2);							//Calculate line length in terms of individual characters
      local_pointer = w_pointer++;									//Update pointer to array "store"
      store[local_pointer] = tempInt1; break;						//Save the number of commands in the line to the array  
case 0x3:   tempInt1 = Rx_Hex_char<<4;  break;						//Next 4 digits give the address of the first command in the line
case 0x4: tempInt1 += Rx_Hex_char; 
      tempInt1=tempInt1<<8; break;									//Acquire second digit and combine it with first 
case 0x5: tempInt1 += Rx_Hex_char<<4;  break;						//Continue for third digit
case 0x6:   tempInt1 += Rx_Hex_char;								//Acquire final digit and caculate address of next command 
      local_pointer = w_pointer++;									//Update pointers to array "store"
      store[local_pointer] = tempInt1; break;						//Save address of next command to array "store"
case 0x7:   break;													//chars 7 and 8 are not used
case 0x8:   break;
default:  break;}

if ((counter > 8)&&(counter < char_count)){						//Continue to acquire, decode and store commands
if ((counter & 0x03) == 0x01){tempInt1 = Rx_Hex_char<<4;}			//Note: Final two chars at the end of every line are ignored
if ((counter & 0x03) == 0x02)  {tempInt1 += Rx_Hex_char;}
if ((counter & 0x03) == 0x03)  {tempInt2 = Rx_Hex_char<<4;}
if ((counter & 0x03) == 0x0)    {tempInt2+= Rx_Hex_char; 
                tempInt2=tempInt2<<8;tempInt1+=tempInt2;
local_pointer = w_pointer++;  
store[local_pointer] = tempInt1; cmd_counter++;}}

counter++;
w_pointer = w_pointer & 0b00111111; }								//Overwrites array after 128 entries





/*****************************************************************************************************************/
void Program_Flash_Hex (void){										//Simplified version of subroutine used for UNO_AVR_Programmer
char keypress;

Initialise_variables_for_programming_flash;

UCSR0B |= (1<<RXCIE0); sei();										//Set UART Rx interrupt


new_record();														//Start reading first record which is being downloaded to array "store" 
start_new_code_block();												//Initialise new programming block (usually starts at address zero but not exclusivle so)
inititalise_UPDI_cmd_write(page_address);							//Prepare to copy up to 32 commands from array "store" to the page_buffer
Program_record();													                            
      
    
while(1){   
new_record();														//Continue reading subsequent records
if (record_length==0)break;											//Escape when end of hex file is reached


switch(short_record){
case 0: if (space_on_page == (PageSZ - line_offset))				//If starting new page
      {page_address = (Hex_address & PAmask);						//get new page address
      if(!(Hex_address%PageSZ))									//Start of new record is also start of new page
	  {inititalise_UPDI_cmd_write(page_address);					//Re-initialise UPDI for pages that start at the beginning of a record
	  	  }}break;

case 1: start_new_code_block();										//Short line with no break in file (often found in WinAVR hex files).
    short_record=0;break;}
    
Program_record();}	



cli();UCSR0B &= (~(1<<RXCIE0));									//download complete, disable UART Rx interrupt

while(1){if (isCharavailable(10)==1){receiveChar();}				//Clear last few characters of hex file
    else break;}



																	//Add cmd 0x0000 at end of file where record contains an odd number of bytes.
if(odd_line_length%2) {
store[w_pointer] = 0x0000;
w_pointer += 1;
UPDI_cmd_to_page_buffer();
prog_counter += 1;
Flash_flag = 1;
if ((add_last_cmd + 1)%PageSZ){orphan = 1;}}
 


if(record_type_old)													//Hex file cotains discontinity	and records can span two pages											
{ 	

if((!(Flash_flag)) && (orphan))									//Nothing to save to flash. The last record is a short one.
{break_page_fill;}													//Exit UPDI repeat comand early since there is no data to occupy a page


if((Flash_flag) && (orphan)){										//Data to save to flash. Last line occupies 2 pages
break_page_fill;
write_page_SUB(page_address + 0x20);}								//Write the final partially full page


if((Flash_flag) && (!(orphan))){									//Data to save to flash
break_page_fill;
write_page_SUB(page_address);}}										//Last line only occupies 1 page



if(!(record_type_old)){											//Hex file has no discontinuities 
if(Flash_flag) {													//Hex file ends mid-page
break_page_fill;													//Exit proramming early
write_page_SUB(page_address);}										//Store page to flash									
}																	//Hex file ends at a page boundary
}																	//nothing more to store






  





/***************************************************************************************************************************************************/
