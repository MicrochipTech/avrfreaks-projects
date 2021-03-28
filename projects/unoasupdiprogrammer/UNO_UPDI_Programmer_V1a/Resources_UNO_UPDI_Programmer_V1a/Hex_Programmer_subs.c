/*
This code is closely based on that used for the UNO AVR programmer however it 
can only be used to down load signle hex files and is therefore rather simpler/ 

Where strings are to be stored AVR hex files consist of two sections.  
The first contains the C code and the second contains the strings.
The second follows the first with no unused address space between them
However the code section does not necessarily end with a comlete record
and the string section always starts on a new record.
Code records therefore always start with addresses 0000, 0x0010,  0x0020, 0x0030 ......
String recods can start with any address.
The hex file is stored in pages and the break between code and strings does not
necessarily occur at a page boundary. 

Some of the original complexity therefore remains.

In addition it was observed that hex files generated with AVR studio for
devices using the UPDI can end in lines of odd length. This occurs for
srings with odd number of characters.  Preiously the compiler added an addditional
null character at the end of the hex file to ensue that all lines have even length
i.e. a whole number of 16 bit words to write to flash.
Without this precaution there was no termination for the final string which 
continues printing indefinitly.
*/



/**********************************************************************************************************************/
void new_record(void);
void start_new_code_block(void);
void start_new_code_block(void);
void Program_record(void);
void copy_cmd_to_page_buffer(void);
void get_next_hex_cmd(void);
void write_page_SUB(int);

void inititalise_UPDI_cmd_write(int);
void UPDI_cmd_to_page_buffer(void);
void Write_page_to_NVM(int);


/**********************************************************************************************************/
void new_record(void){int Local_r_pointer;								//Start of new record from the hex file
while (r_pointer == w_pointer);											//Wait for new entry in array "store"
Local_r_pointer = r_pointer;
record_length =  store[Local_r_pointer]; 								//Obtain the length of the new record (in bytes)
inc_r_pointer;  														//Increment the read pointer

Count_down = record_length/2; 											//Initialise to record length (in commands) 
if (record_length_old < Std_record_length)
short_record=1;															//Check for "record length" (<16 bytes)

odd_line_length = record_length_old;									//Save length of previous record in bytes

record_length_old = record_length;										//Save length of this record in bytes

while (r_pointer == w_pointer);											//Wait for next entry in array "store"
Local_r_pointer = r_pointer;
Hex_address  =  (store[Local_r_pointer]); 								//Get the address of the first command in the new record
inc_r_pointer;  														//Increment the read pointer
Hex_address  = Hex_address/2;											//Convert address from byte to word value

add_last_cmd = Hex_address + (record_length/2) - 1;					//addresss of last command in the record						

record_type_old = record_type;
if(Hex_address%half_SRL) record_type = 1;								//Record may span two pages 
else record_type = 0;													//Record never occupies more than one page					


if (Hex_address == HW_address)orphan = 0;								//New record follows on immediately from the old
else{if (Hex_address == 0);}}											//Start of hex file: address is zero



/**********************************************************************************************************/
void start_new_code_block(void){
HW_address = Hex_address;												//Initialise HW_address
page_address = (Hex_address & PAmask);									//Obtain page address
write_address = Hex_address - page_address;								//Initialise write_address: zero to Page size -1
page_offset = write_address;											//"page_offset" is the initial value of "write_address"											
line_offset = Hex_address & 0x0007;										//Not all lines start with addresses that are multiples of 8
space_on_page = (PageSZ - page_offset);									//Initialise "space_on_page"
}





/**********************************************************************************************************/
void Program_record(void){
while(Count_down){ 														//Initially contains the number of commands in the line
Count_down--;															//Decrement "count_down" each time a command is written to the page buffer
Flash_flag = 1;  														//Indicates that page buffer contains commands that will need burning to flash 
UPDI_cmd_to_page_buffer();
write_address++;
HW_address++;
space_on_page--;
prog_counter += 1;       
if (write_address == PageSZ){											//If page_buffrer is now full:
Write_page_to_NVM(page_address);
Flash_flag = 0;															//Buffer now contains no data to burn to flash
write_address = 0;														//"while loop" continues if there is a line offset
space_on_page = PageSZ;
page_offset = line_offset;
if (line_offset) {orphan = 1;											//One or more commands in current record will be on next page (except for a short line))
inititalise_UPDI_cmd_write(page_address + 0x20);}						//Initialise UPDI for next page
}}}


/**********************************************************************************************************/
void copy_cmd_to_page_buffer(void){
get_next_hex_cmd();}


/**********************************************************************************************************/
void get_next_hex_cmd(void){int Local_r_pointer;
while (r_pointer == w_pointer);
Local_r_pointer = r_pointer;
Hex_cmd = store[Local_r_pointer];
inc_r_pointer;}



/**********************************************************************************************************/
void write_page_SUB(signed int page_address){ 
Write_page_to_NVM(page_address);
Flash_flag = 0;}





