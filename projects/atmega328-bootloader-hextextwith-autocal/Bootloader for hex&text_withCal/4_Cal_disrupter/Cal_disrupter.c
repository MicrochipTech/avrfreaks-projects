/*

/*Compile it using optimisation level s ONLY
Rx/Tx work at 57.6k
*/

/*EEPROM reservations
0x3FF	user cal if set
0x3FE	user cal if set
0x3FD	Default cal supplied by Atmel

*/





#include "Cal_disrupter.h"


int char_counter;


char string_counter(int);
void print_string_num(int, int);



int main (void){ 

char keypress;

setup_HW;
cal_device;																			//checks cal status and adopts user cal bytes if present (Optional)

OSCCAL += 15;
eeprom_write_byte((uint8_t*)0x3FE, OSCCAL);	 									//Save results to EEPROM for later use by "cal macro"
eeprom_write_byte((uint8_t*)0x3FF, OSCCAL);



char_counter = 0;																	//counts the number of characters in the text file (excludes \r & \n)
cal_device;																			//checks cal status and adopts user cal bytes if present (Optional)


while(1){																			//User prompt
do{sendString("w-Press W to disrupt the calibration/stuvwxyz  ");}while((isCharavailable(300) == 0));
keypress = receiveChar();
if (keypress == 'w') break;															//press "w" to continue with program execution
if(keypress == 's') {wdt_enable(WDTO_60MS); while(1);}}

OSCCAL += 15;

eeprom_write_byte((uint8_t*)0x3FE, OSCCAL);	 									//Save results to EEPROM for later use by "cal macro"
eeprom_write_byte((uint8_t*)0x3FF, OSCCAL);

while(1){
sendString("\r\nABCDEFGHIJKLMNOPQRSTUVWXYZ");
waitforkeypress();}


wdt_enable(WDTO_60MS); while(1);

return 1;}


