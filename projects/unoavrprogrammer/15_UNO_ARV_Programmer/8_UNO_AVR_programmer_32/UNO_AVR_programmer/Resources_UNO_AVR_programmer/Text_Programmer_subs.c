



void Program_Flash_Text (void){
w_pointer = 0; r_pointer = 0;
text_started =0; endoftext = 3; counter = 0;
Rx_askii_char_old = '0';

sendString
("\r\nReduce baud rate to 19.2kB and press 'r'.\r\n");          			//Sending text requires baud rate reduction
Timer_T0_sub(T0_delay_10ms);
UBRR0L = 51;
while (1){if (waitforkeypress() != 'r'); else break;}
sendString("Send file.");

UCSR0B |= (1<<RXCIE0);														//Activate UART interrupt
sei();																		//Set global interrupt

Timer_T2_sub_with_interrupt(7,0);											//Start Timer2 with interrupt

address_in_flash = FlashSZ;													//First character will be stored at 0x6FFF not 0x6FFE
write_address = PageSZ;														//Address on page buffer
while (1){
while (r_pointer == w_pointer);												//wait for w_pointer to be incremented
if(endoftext != 3)endoftext -= 1;											//Indicates that the timer has been shut down
write_address -= 1;
address_in_flash -= 1;
Load_page('L', write_address, store[r_pointer]); 
Load_page('H', write_address, store[r_pointer] >> 8);
store[r_pointer] = 0;														//clear the contents of the location in array store
inc_r_pointer;																//restore the value of "r_pointer" to that of "w_pointer"
if (write_address == 0){
sendChar('*');
write_page_SUB(address_in_flash);
write_address = PageSZ;}													//Restore address_in_flash
if(!(endoftext)) break;}													//Break when two '\0' chars have been appended to text stored in the array
write_page_SUB(address_in_flash - write_address);
address_in_flash += 1;	

sendChar('*');
cli();  
UCSR0B &= (~(1<<RXCIE0));

sendString
("\r\nRestore baud rate to 38.4kB and press 'r'.\r\n");
Timer_T0_sub(T0_delay_10ms);
UBRR0L = 25;
while (1){if (waitforkeypress() != 'r'); else break;}
Verify_Flash_Text();
}





void upload_text(void){
unsigned char Rx_askii_char;

Rx_askii_char = receiveChar();
if((Rx_askii_char) == '\0')return;											//Ignore polling from Boot loader
if((Rx_askii_char != '*') && (!(text_started)))return;					//Ignore header text occuring before a line of * characters
if((Rx_askii_char == '*')&&(!(text_started)))								//The number of * characters is not critical only one is required
{ text_started = 1;return;}
if((Rx_askii_char == '*')&&(text_started == 1))return;						//Ignore carriage returns occurring before the first text string
else {if (text_started == 1)text_started = 2;}
if((text_started == 2) && ((Rx_askii_char == '\r')
 || (Rx_askii_char == '\n')))return; else text_started = 3;				//Enter main area of text file containing strings to be written to flash

if ((Rx_askii_char == '\r') || (Rx_askii_char == '\n')){					//Check for '\r' and or '\n' and convert to a single '\0' character
if (Rx_askii_char_old == '\0') return;
else Rx_askii_char = '\0';}

TCNT2 = 0;																	//Reset the timer. An interrupt is only required at the end of the file

if(!(counter%2)){															//Each location in array "store" holds two askii characters
store[w_pointer] = Rx_askii_char;
store[w_pointer] = store[w_pointer] << 8;}									//"w_pointer" gives address in array of the next free location
else{store[w_pointer] += Rx_askii_char;
inc_w_pointer;}
counter = (counter + 1);													//"counter" gives the number of characters downloaded
Rx_askii_char_old = Rx_askii_char;} 






void Verify_Flash_Text (void){
unsigned char  high_char, low_char, print_line = 0;                         //Controls printing of hex file                         
int line_no;                                                                //Refers to the .hex file
int phys_address;                                                          //Address in flash memory
signed int prog_counter_mem;                                                //Initialise with size of .hex file used for programming
unsigned char string_counter = 1;
unsigned char line_length = 0;

phys_address = FlashSZ - 1;   
sendString("\r\nEnd of flash ");sendHex(16,phys_address);newline();

sendHex(16,string_counter++);sendChar('\t');
 while ((high_char = Read_write_mem('H',phys_address, 0x0)) != 0xFF)
 {Timer_T0_sub(T0_delay_10ms);
 low_char = Read_write_mem('L',phys_address, 0x0);
if(high_char)
	{line_length += 1;
	if((high_char == ' ') && (line_length > 90))
		{sendString("\r\n\t"); line_length = 0;}
	else sendChar(high_char);}
else {line_length = 0;newline();newline();
		sendHex(10,string_counter++);sendChar('\t');}

if(low_char)
	{line_length += 1;
	if((low_char == ' ') && (line_length > 90))
		{sendString("\r\n\t"); line_length = 0;}
	else sendChar(low_char);}
else {line_length = 0;newline();newline();
	sendHex(10,string_counter++);sendChar('\t');}

phys_address -= 1;}
if (op_code == 't')
{Last_Text_Page = (address_in_flash & PAmask)*2;
newline();
Unused_pages = (Last_Text_Page - Last_Hex_Page - PageSZ*2)/2/PageSZ;

if (Last_Text_Page > Last_Hex_Page) 
{sendString("\r\nUnused pages: "); sendHex(10,Unused_pages);
if (Unused_pages){sendString("  Bytes: ");
sendHex(10,(Unused_pages*PageSZ*2));}newline();newline();}}}
  
