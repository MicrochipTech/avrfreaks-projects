


char isCharavailable (int);
char waitforkeypress(void);
long Int_from_KBD(void);
char decimal_digit (char);
char wait_for_return_key(void);
float Float_from_KBD(void);

void int_string_to_display(void);
void float_string_to_display(void);

float float_num_from_eepom(char);

/**********************************************************************************************/
char isCharavailable (int m){int n = 0;										//For m = 1 waits a maximun of 7.8mS
while (!(Serial.available())){n++;											//before returning zero
if (n>4000) {m--;n = 0;}if (m == 0)return 0;}								//Returns 1 immediately that a char is received
return 1;}



char waitforkeypress(void){
while (!(Serial.available()));
return Serial.read();}


/***************************************************************************************************************************************/
char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}


/***************************************************************************************************************************************/
char wait_for_return_key(void){  											//Returns keypress
char keypress,temp;															//but converts \r\n, \r or \n to \r
while (!(Serial.available())); 
keypress = Serial.read();
if((keypress == '\r') || (keypress == '\n')){
if (isCharavailable(1)){temp = Serial.read();}keypress = '\r';}
return keypress;}





/***************************************************************************************************************************************/
long Int_from_KBD(void){                                   					//Acquires an integer string from the keyboard and returns the binary equivalent
char keypress;
long I_number;
char SREG_BKP;

SREG_BKP = SREG;
sei();

cr_keypress = 0;                                                    		//Set to one when carriage return keypress terminates the string
for(int n = 0; n<=8; n++) data_buff[n] = 0;            					//Clear the buffer used for the string
do
{while (!(Serial.available())); 											//Wait for first keypress
keypress = Serial.read();} 


while ((!(decimal_digit(keypress)))
&& (keypress != '-'));														//Ignore keypress if it is not OK
data_buff[0] = keypress;
int_string_to_display();                                           			//Update display with the first key press
while(1){
if ((keypress = wait_for_return_key())  =='\r')break;              		//Detect return key press (i.e \r or\r\n)
if ((decimal_digit(keypress)) || (keypress == '\b')\
 || (keypress == '-'))

{if (keypress == '\b'){													//Backspace key
for (int n = 0; n <= 7; n++)
data_buff[n] = data_buff[n + 1];}
else
{for(int n = 8; n>=1; n--)                                         		//Shift display for each new keypress
data_buff[n] = data_buff[n-1];
data_buff[0] = keypress;  }                                           		//Add new keypress           

int_string_to_display();
 }}                                                                			//Update display includes "cr_keypress"                                                 
cr_keypress = 1;                                                    		//End of string; return key press detected
int_string_to_display();
cr_keypress = 0;
SREG = SREG_BKP;

TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);                  		//Activate TWI and wait for contact from display pcb 
while (!(TWCR & (1 << TWINT)));
I_number =  byte(receive_byte_with_Ack());                          		//Build up the number as each byte is received
I_number = (I_number << 8) + byte(receive_byte_with_Ack());
I_number = (I_number << 8) + byte(receive_byte_with_Ack());
I_number = (I_number << 8) + byte(receive_byte_with_Nack());
TWCR = (1 << TWINT);
return I_number;}




/***************************************************************************************************************************************/
float Float_from_KBD(void){                    								//Acquires an integer string from the keyboard and returns the binary equivalent
char keypress;
float f_number;
float * Flt_ptr_local;
char * Char_ptr_local;
char SREG_BKP;

SREG_BKP = SREG;
sei();

Flt_ptr_local = &f_number;
Char_ptr_local = (char*)&f_number;

cr_keypress = 0;                                                    		//Set to one when carriage return keypress terminates the string
for(int n = 0; n<=7; n++) data_buff[n] = 0;                           		//Clear the buffer used to the string
do
{while (!(Serial.available())); 
keypress = Serial.read();} 
while ((!(decimal_digit(keypress)))
&& (keypress != '-')
&& (keypress != '.'));


data_buff[0] = keypress;
if (keypress == '.')data_buff[0] = '0' | 0x80;


float_string_to_display();                                           		//Update display with the first key press
while(1){
if ((keypress = wait_for_return_key())  =='\r')break;               		//Detect return key press (i.e \r or\r\n)

if ((decimal_digit(keypress)) || (keypress == '.')
|| (keypress == '\b')|| (keypress == '-')
|| (keypress == 'E') || (keypress == 'e'))

{if(keypress == '\b'){for (int n = 0; n <= 7; n++)
data_buff[n] = data_buff[n + 1];}

else

{if(keypress != '.')
{for(int n = 7; n>=1; n--)                                          		//Shift display for each new keypress except '.'
data_buff[n] = data_buff[n-1];
data_buff[0] = keypress;}                                              		//Add new keypress           
else data_buff[0] |= 0x80;}												//Intregrate decimal point and its parent digit



float_string_to_display();
}}                                                                  		//Update display includes "cr_keypress"                                                 
cr_keypress = 1;                                                     		//End of string; return key pressed
float_string_to_display();
cr_keypress = 0;
SREG = SREG_BKP;


TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);                    		//Activate TWI and wait for contact from display pcb 
while (!(TWCR & (1 << TWINT)));

*Char_ptr_local =  byte(receive_byte_with_Ack());  Char_ptr_local += 1;	//Build up the number as each byte is received
*Char_ptr_local =  byte(receive_byte_with_Ack());  Char_ptr_local += 1;  
*Char_ptr_local =  byte(receive_byte_with_Ack());  Char_ptr_local += 1;      
*Char_ptr_local =  byte(receive_byte_with_Nack());    
f_number = *Flt_ptr_local;

TWCR = (1 << TWINT);
return f_number;}




/**************************************************************************************************************************/
float float_num_from_eepom(char mode){
float f_number;
float * Flt_ptr_local;
char * Char_ptr_local;
int address = 0x3FF;

if((switch_3_down) && (mode))address = 0x3FA;
else address = 0x3FF;


Flt_ptr_local = &f_number;
Char_ptr_local = (char*)&f_number;

*Char_ptr_local =  byte(eeprom_read_byte((uint8_t*)(address--)));  Char_ptr_local += 1;
*Char_ptr_local =  byte(eeprom_read_byte((uint8_t*)(address--)));  Char_ptr_local += 1;
*Char_ptr_local =  byte(eeprom_read_byte((uint8_t*)(address--)));  Char_ptr_local += 1;
*Char_ptr_local =  byte(eeprom_read_byte((uint8_t*)(address))); 
f_number = *Flt_ptr_local;
return f_number;}



