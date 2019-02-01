



/*
Basic 8 digit 7 segment display controller which offers two modes of operation
Mode 1: Display numbers
Mode 2: Illuminate individual segments.
Illumination of the decimal points and colons is not supported.
Three settings are offered for control of the display brightness


Operation is based on a multiplexer that updates each digit of the display 
every 16mS using a 2mS Timer0 interrupt.
A pcb test routine is also included that dissables the multiplexer and 
illuminates 1 digit at a time as a fault finding aide.



The display has eight digits numbered 1 to 8 from right to left.
Each digit is composed of 7 segments a,b,c,d,e,f and g.
Data used to drive the display is stored in array display_buf[8].

For numerical entry 8 keypresses (0 to 9) are stored in "display_buf"
starting with location zero that is displayed in digit 1. 

For segment entry the first 7 bits of each location of array "display_buf"
are used to control one segment.  
"The bits in display_buf[0]" control segment 'a'.
If bit zero is 1, segment 'a' of digit 1 will be illuminated.
If bit two is 1, segment 'a' of digit 3 will be illuminated.
Similarly the bits of display_buf[3] control segment 'b'. 
etc.
*/


/*

The following fuse bytes are required:
extended_fuse byte: 0xFF	(left in its default state)
Fuse byte high:		0xD7	(WDT under control of program and EEPROM prserved through chip erase)
Fuse byte:			0xE2	(8MHz internal RC clock selected)
The device was programmed using the serial port
When running, this program assumes a baud rate of 57.6k, no handshaking or parity, 8 data bits and 1 stop bit

It happens occaisionally that the default calibration of the internal 8MHz is not good enough to enable
communication with a PC.  If this happens a program to caliobrate the clock can be found by doing a search for
"Using the Atmega 328 internal RC clock" posted under projects on Jan 10, 2019 by osbornema.

*/





#include "Display_driver_Header_file.h"

int main (void){

char cal_factor=0;
char watch_dog_reset=0;


setup_HW;


/*******User prompt: Only privided post programming or POR*******************/
if((watch_dog_reset == 0)){
while(1){do{sendString("C  ");}while((isCharavailable(100) == 0)); 
if(receiveChar() == 'C')break;}}

/*******************************Main menue************************************/
sendString("\r\nPress\r\n1 for numeric keypresses\r\n2 to illuminate\
 segments\r\n3 to change intensity\r\n4 to run a pcb test routine\r\n");


/******************************Start multiplexer******************************/		
sei();
T0_interupt_cnt = 0;	
TIMSK0 |= (1 << TOIE0);										
switch(eeprom_read_byte((uint8_t*)0x3FB)){
case 0xFF: timer_T0_sub_with_interrupt(T0_delay_2ms); break;			//High intensity: Each of the 8 digits is active for 2mS in 16ms
case 0xFE: timer_T0_sub_with_interrupt(T0_delay_500us); break;			//Normal intensity:  Each digit is refreshed every 16mS but only for 500uS
case 0xFD: timer_T0_sub_with_interrupt(T0_delay_125us); break;			//Low intensity: Each digit is refreshed for 125uS every 16mS
default: timer_T0_sub_with_interrupt(T0_delay_2ms); break;}
	

/****************************Wait for User response***************************/	
while(1){
switch(waitforkeypress()){
case '1': numeric_entry(); break;
case '2': segment_illumination(); break;
case '3': intensity_control(); break;
case '4': pcb_test(); break;
default: sendChar('?');continue; break;}}}								//End of "main" routine





/*********************Subroutine to control display intensity********************/
void intensity_control(void){

sendString("\r\nPress 1, 2 or 3 for high, normal or low\r\n\
intensity and z to escape\r\n");
mode = 1;for (int m = 0; m < 8; m++){display_buf[m]= m+'0';}			//Set display to 1,2,3,4,5,6,7,8 

while(1){
switch(waitforkeypress()){
case '1': eeprom_write_byte((uint8_t*)(0x3FB), 0xFF);break;			//Intensity is controlled by the T0 ISR
case '2': eeprom_write_byte((uint8_t*)(0x3FB), 0xFE);break;			//This reads EEPROM location 0x3FB to determine required intensity		
case '3': eeprom_write_byte((uint8_t*)(0x3FB), 0xFD);break;
case 'z': SW_reset;
default: eeprom_write_byte((uint8_t*)(0x3FB), 0xFE);break;}
T0_interupt_cnt = 0;}
SW_reset;}



/*********************Subroutine enables numeric entry*************************/
void numeric_entry(void){ 
char keypress;
mode = 1;																//Subroutine "Resources_CA\Display_subroutines\Display_driver()"
sendString ("\r\n-z- when display is full to exit");					//uses mode setting to distinguish between numeric entry
while(1){																//and segment entry.
clear_display_buffer;
for (int m = 0; m < 8; m++){	
	keypress = waitforkeypress();
		if(keypress == 'z'){SW_reset;}
			if((keypress < '0') || (keypress > '9')) m--;
			else display_buf[m] = keypress;}							//Numeric characters stored in "display_buf" will be displayed.
if (waitforkeypress() == 'z'){SW_reset;}}}




/*********************Subroutine enables segment entry*************************/
void segment_illumination(void){
mode = 2;
sendString("\r\nEnter number 1-8 or z to exit at ? prompt \r\n\
then letters a-g or z for next number\r\n");
while(1){
sendString("?");
if ((digit_num = waitforkeypress()) != 'z') digit_num -= '1';			//digit_num varies between 0 and 7 not 1 and 8 as entered.
else {SW_reset;}
if ((digit_num < 0) || (digit_num > 7))continue;
while ((letter = waitforkeypress()) != 'z'){
	switch (letter){
	case 'A': case 'a': letter = 0; break;								//display_buf[0] controlles immunination of segment 'a' in each digit
	case 'B': case 'b': letter = 1; break;								//display_buf[1] controlles immunination of segment 'b' in each digit
	case 'C': case 'c': letter = 2; break;
	case 'D': case 'd': letter = 3; break;
	case 'E': case 'e': letter = 4; break;
	case 'F': case 'f': letter = 5; break;
	case 'G': case 'g': letter = 6; break;
	default: letter = 8; continue;	}

									//Illegal entry! skip next three lines of code and return to top of while(1) loop.

if(display_buf[letter] & (1 << digit_num))
display_buf[letter] &= (~(1 << digit_num));							//Toggles segments on and off
else display_buf[letter] |= (1 << digit_num);}}}						//Illuminates segments					






/***************Single digit illumination: provided for HW test purposes****************/
void pcb_test(void){

char test_num, test_digit;
TIMSK0 &= (~(1 << TOIE0));												//Disable Multiplexer
	
sendString("\r\nPress number 0-9 then digit 0-7; z to exit");
		
		
		while(1){
		test_num = waitforkeypress();reset_digits; reset_segments;
		switch(test_num){
		case '0': zero; break;
		case '1': one; break;
		case '2': two; break;
		case '3': three; break;
		case '4': four; break;
		case '5': five; break;
		case '6': six; break;
		case '7': seven; break;
		case '8': eight; break;
		case '9': nine; break;
		case 'z': SW_reset;}
		test_digit = waitforkeypress();
		switch(test_digit){
		case '0': digit_0; break;
		case '1': digit_1; break;
		case '2': digit_2; break;
		case '3': digit_3; break;
		case '4': digit_4; break;
		case '5': digit_5; break;
		case '6': digit_6; break;
		case '7': digit_7; break;}}	}
	




	
