

/************************************************************************************/
void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}



/************************************************************************************/
void I2C_Tx_2_integers(unsigned int s1, unsigned int s2){			
char num_bytes=4; char mode=1; char s[4];
for (int m = 0;  m < 4; m++){
switch (m){
case 0: s[m] = s1; break; 											//Send S1 lower byte
case 1: s[m] = s1 >> 8; break; 									//Send S1 higher byte									
case 2: s[m] = s2; 	break;											//Send S1 lower byte
case 3: s[m] = s2 >> 8; break;}}									//Send S1 higher byte
I2C_Tx(num_bytes,mode, s);}



/************************************************************************************/
void I2C_Tx(char num_bytes, char mode, char s[]){
waiting_for_I2C_master;
send_byte_with_Ack(num_bytes);
send_byte_with_Ack(mode);
for (int m = 0;  m < num_bytes; m++){
if (m==num_bytes-1){send_byte_with_Nack(s[m]);}
else {send_byte_with_Ack(s[m]);}}
TWCR = (1 << TWINT);}



/************************************************************************************/
void send_byte_with_Ack(char byte){
TWDR = byte;													//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);				//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}



/************************************************************************************/
void send_byte_with_Nack(char byte){
TWDR = byte;													//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEN);								//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}






/*********************************************************************/
void Char_to_PC(char data){
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = data;}

/*********************************************************************/
void String_to_PC(const char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
Char_to_PC(s[i++]);} }



/*********************************************************************/
char waitforkeypress(void){
while (!(UCSR0A & (1 << RXC0)));
return UDR0;}


/*********************************************************************/
char receiveChar(void)
{return UDR0;}


/*********************************************************************/
char isCharavailable (char m){int n = 0;
while (!(UCSR0A & (1 << RXC0))){n++;
if (n>8000) {m--;n = 0;}if (m == 0)return 0;}
return 1;}


/*******************************************************************************/
int Num_from_KBD(void){
char keypress;
int number;

do
{keypress =  waitforkeypress();} while(!(decimal_digit(keypress)));

keypress -= '0';
number =  keypress;

while(1){
if ((keypress = wait_for_return_key())  =='\r')break;
if (decimal_digit (keypress))		
{
keypress -= '0';
number = number * 10 + keypress;}}
return number;}




/*********************************************************************/
void Num_to_PC(char radix, long long_num){
char array[12];														//Long has 10 chars + sign + null terminator	
SBtoAL(array, long_num, radix);										//calls the Binary to askii subroutine
NumericString_to_PC(array);}										//Prints characters in reverse order

/*********************************************************************/
void SBtoAL(char array[], long num, char radix){					//Signed Binary to Askii
int m=0;
long sign;

if (num == 0x80000000){
switch(radix){
case 10: array[0] = '8';array[1] = '4'; array[2] = '6';			//0x80000000 * -1 = 0
array[3] = '3';array[4] = '8';array[5] = '4'; array[6] = '7';
array[7] = '4';array[8] = '1';array[9] = '2';
array[10] = '-'; array[11] = '\0'; break;

case 16: array[0] = '0';array[1] = '0'; array[2] = '0';			//0x80000000 * -1 = 0
array[3] = '0';array[4] = '0';array[5] = '0'; array[6] = '0';
array[7] = '8';array[8] = '-';array[9] = '\0';
array[10] = '\0'; array[11] = '\0'; break; } return;}

for (int n = 0; n <=11; n++)array[n] = 0;	
if ((sign = num) < 0) num = num * (-1);

do {array[m] = num % radix;
if(array[m] < 10)array[m] += '0'; else array[m] += '7';
m++;} while ((num = num/radix) > 0);
if (sign < 0) {array[m] = '-';m++;}}


/*********************************************************************/
void NumericString_to_PC(char* s){					
int n=0;
while (s[n] != '\0')n++;											//scroll to end of string counting the number of characters
for(int m = n; m; m--)Char_to_PC(*(s + m-1));}					//print last character first



char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}

char wait_for_return_key(void){	
char keypress,temp;
keypress = waitforkeypress();
if((keypress == '\r') || (keypress == '\n')){
if (isCharavailable(1)){temp = receiveChar();}keypress = '\r';}
return keypress;}

/************************************************************************/
void I2C_Tx_8_byte_array(char s[]){				
char num_bytes=8; char mode=1;
I2C_Tx(num_bytes,mode, s);}





long I2C_displayToNum(void){		
long L_number = 0;
char receive_byte;
char num_bytes=0;
char mode = 'I';

waiting_for_I2C_master;		
send_byte_with_Ack(num_bytes);
send_byte_with_Nack(mode);
TWCR = (1 << TWINT) | (1 << TWEN);						//clear interrupt and leave I2C active
waiting_for_I2C_master;

for (int m = 0; m<=3; m++){							//Receive 4 chars and assemble into unsigned long result
if (m ==3){receive_byte = receive_byte_with_Nack();}
else {receive_byte = receive_byte_with_Ack();}
switch(m){
case 0: case 1: case 2:L_number =  L_number + receive_byte; 
L_number = L_number << 8; break;
case 3: L_number =  L_number + receive_byte; break;}}
clear_I2C_interrupt;
return L_number;}


/***********************************************************/
char receive_byte_with_Ack(void){
char byte;
TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);		//Set Ack enable and clear interrupt
while (!(TWCR & (1 << TWINT)));						//Wait for interrupt
byte = TWDR;
return byte;}

/***********************************************************/
char receive_byte_with_Nack(void){
char byte;
TWCR = (1 << TWEN) | (1 << TWINT);						//Set Ack enable and clear interrupt
while (!(TWCR & (1 << TWINT)));						//Wait for interrupt
byte = TWDR;
return byte;}

/************************************************************************/
void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
UCSR0B = 0;
UBRR0H = UBRROH_N;  									
UBRR0L = UBRR0L_N;  								
UCSR0A = (1 << U2X0);
UCSR0B = (1 << RXEN0) | (1<< TXEN0);
UCSR0C =  (1 << UCSZ00)| (1 << UCSZ01);}
