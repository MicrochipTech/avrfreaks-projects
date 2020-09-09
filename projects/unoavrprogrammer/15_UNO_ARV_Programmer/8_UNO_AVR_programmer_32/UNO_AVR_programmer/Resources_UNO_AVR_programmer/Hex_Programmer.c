


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
      char_count = 9 + ((tempInt1) *2);								//Calculate line length in terms of individual characters
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

if ((counter > 8)&&(counter < char_count)){							//Continue to acquire, decode and store commands
if ((counter & 0x03) == 0x01){tempInt1 = Rx_Hex_char<<4;}			//Note: Final two chars at the end of every line are ignored
if ((counter & 0x03) == 0x02)  {tempInt1 += Rx_Hex_char;}
if ((counter & 0x03) == 0x03)  {tempInt2 = Rx_Hex_char<<4;}
if ((counter & 0x03) == 0x0)    {tempInt2+= Rx_Hex_char; 
                tempInt2=tempInt2<<8;tempInt1+=tempInt2;
local_pointer = w_pointer++;  
store[local_pointer] = tempInt1; cmd_counter++;}}

counter++;
w_pointer = w_pointer & 0b00111111; }								//Overwrites array after 32 entries





/*****************************************************************************************************************/
void Program_Flash_Hex (void){
char keypress;

while ((keypress = waitforkeypress()) != ':')						//Ignore characters before the first ':'
{if (keypress == 'x'){SW_reset;}}									//X pressed to escape

Initialise_variables_for_programming_flash;

UCSR0B |= (1<<RXCIE0); sei();										//Set UART Rx interrupt
(Atmel_config(Chip_erase_h, 0));									//Erase chip after first char of hex file has been received


new_record();														//Start reading first record which is being downloaded to array "store" 
start_new_code_block();												//Initialise new programming block (usually starts at address zero but not exclusivle so)
Program_record();													//Copy commands from array "store" to the page_buffer                            
      
    
while(1){   
new_record();														//Continue reading subsequent records
if (record_length==0)break;											//Escape when end of hex file is reached


if (Hex_address == HW_address){										//Normal code: Address read from hex file equals HW address and lines contains 8 commands
switch(short_record){
case 0: if (space_on_page == (PageSZ - line_offset))				//If starting new page
      {page_address = (Hex_address & PAmask);}						//get new page address
      break;

case 1: start_new_code_block();										//Short line with no break in file (often found in WinAVR hex files).
    short_record=0;break;}}
    
    
if(Hex_address != HW_address){										//Break in file
  if (section_break){												//Section break: always found when two hex files are combined into one                    
    if((Flash_flag) && (!(orphan)))
    {write_page_SUB(page_address);}									//Burn contents of the partially full page buffer to flash
    if(orphan) 
    write_page_SUB(page_address + PageSZ);}							//Burn outstanding commands to the next page in flash     
    
  if(page_break)													//In practice page breaks and short jumps are rarely if ever found                      
    {if((Flash_flag) && (!(orphan)))								//Burn contents of the partially filled page buffer to flash
    {write_page_SUB(page_address);}                           
    orphan = 0;}
    
  start_new_code_block();											//A new code block is always required where there is a break in the hex file.
  short_record=0;}
    
Program_record();}													//Continue filling page_buffer
    

cli();  
UCSR0B &= (~(1<<RXCIE0));											//download complete, disable UART Rx interrupt
LEDs_off;       
while(1){if (isCharavailable(1)==1)receiveChar();
    else break;}													//Clear last few characters of hex file
  
if((Flash_flag) && (!(orphan)))
{write_page_SUB(page_address);}										 //Burn final contents of page_buffer to flash
if(orphan) {write_page_SUB(page_address + PageSZ);}


if (op_code == 'P'){Prog_config_bytes;}
}
  





/***************************************************************************************************************************************************/
void Verify_Flash_Hex (void){


int  line_counter = 0, print_line = 0;								//Controls printing of hex file                         
int line_no;														//Refers to the .hex file
signed int phys_address;											//Address in flash memory
signed int prog_counter_mem;										//Initialise with size of .hex file used for programming
unsigned char print_out_mode = 0;									//Print out flash contents as hex or askii characters
char skip_lines[4];													//Enter number to limit the print out

sendString("\r\nPress 0 to verify flash or AOK\r\n");
if(waitforkeypress() != '0')return;

phys_address = 0;  read_ops=0; 
line_no = 0; prog_counter_mem = prog_counter; 

sendString("Integer(0-FF)?  ");										//0 prints no lines -1-, every line, -8- prints every eighth line etc... 
skip_lines[0] = '0';												//Acquire integer between 0 and FF
skip_lines[1] = waitforkeypress();
skip_lines[2] = '\0';
Timer_T1_sub(T1_delay_500mS); 
if (isCharavailable(1)){skip_lines[0] = skip_lines[1]; 
skip_lines[1] = receiveChar();} 
binUnwantedChars(); 
print_line = askiX2_to_hex(skip_lines);
sendHex (16,print_line); sendString("   ");

if (print_line == 0);												//hex file print out not required
else {sendString("1/2?\r\n");										//else -1- sends file as askii, -2- sends it as hex
print_out_mode =  waitforkeypress(); 
binUnwantedChars();     
newline();}


while(1){ if(!(prog_counter_mem))break;								//print out loop starts here, exit when finished
    
while(1) {															//Start reading the flash memory searching for the next hex command

Hex_cmd = Read_write_mem('L',phys_address, 0x0);              
Hex_cmd = (Hex_cmd<<8) + (Read_write_mem('H',phys_address, 0x0)); 
phys_address++;        
if (phys_address == FlashSZ)break;									//No more memory? Quit if yes
if (Hex_cmd != 0xFFFF) break;										//If the hex command is 0xFFFF remain in this loop otherwise exit.
LEDs_on;}

LEDs_off;
if (phys_address == FlashSZ)break;									//Exit when there is no more flash to read

if ((print_line == 0)  && (!(line_no%10)))
sendChar('*');														//Print out of hex file not required
																	//Print a -*- every tenth line of the file
if(print_line && (!(line_no%print_line)))							//Print out required: Print all lines or just a selection     
{newline(); sendHex (16, (phys_address-1)*2);   
sendString("   "); line_counter++;  
if(print_out_mode == '1'){send_as_askii;}							//Start with the address of the first command in the line
else sendHex (16, Hex_cmd);}										//Print first command in askii or hex
read_ops++;															//Value to be sent to PC for comparison with the hex filer size
prog_counter_mem--;													//"prog_counter_mem" decrements to zero when the end of the file is reached


for(int m=0; m<7; m++){												//Read the next seven locations in the flash memory   

Hex_cmd = Read_write_mem('L',phys_address, 0x0);        
Hex_cmd = (Hex_cmd<<8) + (Read_write_mem('H',phys_address, 0x0)); 
phys_address++; 
if(Hex_cmd == 0xFFFF)break;											//Read 0xFFFF: return to start of print out loop
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
newline(); 

sendString("\r\nHex_file_size:  ");
sendHex(10,cmd_counter); sendString("  d'loaded:  "); 
sendHex(10,prog_counter); sendString(" in:  "); 
sendHex(10,read_ops); sendString(" out\r\n");

Verify_config_bytes;
newline();
Last_Hex_Page = (phys_address & PAmask)*2;								//LAST PAGE used by Hex
}
