

void Verify_Flash_Text (void);
void upload_text(void);


void Program_Flash_Text (void){
unsigned int add_in_flash;
unsigned char address_on_page;


text_started =0; endoftext = 3; counter = 0;
Rx_askii_char_old = '0';
r_pointer = 0;
w_pointer = 0;

sendString
("\r\nReduce baud rate to 4.8kB and press 'r'.\r\n");          				//Sending text requires baud rate reduction
_delay_ms(10);
UBRR0H = 1;
UBRR0L = 0xA0;
while (1){if (waitforkeypress() != 'r'); else break;}
sendString("Send file.");

UCSR0B |= (1<<RXCIE0);												//Activate UART interrupt
sei();																//Set global interrupt
T1OVFM = 1;
Timer_T1_sub_with_interrupt(T1_delay_25ms);							//Start Timer2 with interrupt

add_in_flash = FlashSZ - text_size; 								//ADDRESS in WORDS
address_on_page = 0;												//ADDRESS in WORDS
inititalise_UPDI_cmd_write(add_in_flash);

while(1){
UPDI_cmd_to_page_buffer();

store[r_pointer] = 0;												//clear the contents of the location in array store
add_in_flash += 1;
address_on_page += 1;

if(endoftext != 3)endoftext -= 1;									//Indicates that the timer has been shut down

if (address_on_page == PageSZ){										//PAGESZ in words
Write_page_to_NVM(add_in_flash - PageSZ);
address_on_page = 0;
inititalise_UPDI_cmd_write(add_in_flash);}

if(!(endoftext)) break;}											//Break when two '\0' chars have been appended to text stored in the array

cli();UCSR0B &= (~(1<<RXCIE0));									//download complete, disable UART Rx interrupt

while(1){if (isCharavailable(10)==1){receiveChar();}				//Clear last few characters of hex file
    else break;}

break_page_fill;
if(address_on_page){
Write_page_to_NVM(add_in_flash - address_on_page);}
	
address_in_flash -= 1;												//End of text

sendString("\r\nUnused data area:  ");
sendHex(10, 2*(FlashSZ - add_in_flash));
sendString(" bytes\r\n");


sendString
("\r\nRestore baud rate to 28.8kB and press 'r'.\r\n");
_delay_ms(10);
UBRR0L = 68;
UBRR0H = 0;
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

TCNT1 = 0;																	//Reset the timer. An interrupt is only required at the end of the file

if(!(counter%2)){															//Each location in array "store" holds two askii characters
store[w_pointer] = Rx_askii_char;
store[w_pointer] = store[w_pointer] << 8;}									//"w_pointer" gives address in array of the next free location
else{store[w_pointer] += Rx_askii_char;
inc_w_pointer;}
counter = (counter + 1);													//"counter" gives the number of characters downloaded
Rx_askii_char_old = Rx_askii_char;} 






void Verify_Flash_Text (void){
unsigned char  high_char, low_char;  				                       //Controls printing of hex file                         
int line_no;                                                                //Refers to the .hex file
int phys_address;                                                         	 //Address in flash memory
unsigned char string_counter = 1;
unsigned char line_length = 0;
unsigned int text_pair;

phys_address = ((FlashSZ - text_size) * 2) + flash_start;   
sendString("\r\nStart of flash ");sendHex(16,phys_address);newline();

sendHex(16,string_counter++);sendChar('\t');

 while ((low_char = read_flash(phys_address)) != 0xFF)
 {Timer_T2_sub(T2_delay_10ms);
 high_char = read_flash(phys_address) >> 8;
if(low_char)
	{line_length += 1;
	if((low_char == ' ') && (line_length > 90))
		{sendString("\r\n\t"); line_length = 0;}
	else sendChar(low_char);}
else {line_length = 0;newline();newline();
		sendHex(10,string_counter++);sendChar('\t');}

if(high_char)
	{line_length += 1;
	if((high_char == ' ') && (line_length > 90))
		{sendString("\r\n\t"); line_length = 0;}
	else sendChar(high_char);}
else {line_length = 0;newline();newline();
	sendHex(10,string_counter++);sendChar('\t');}

phys_address += 2;}}
  
