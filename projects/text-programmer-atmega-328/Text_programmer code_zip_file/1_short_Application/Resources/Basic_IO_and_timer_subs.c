

void USART_init (unsigned char, unsigned char);
void sendString(char*);
char isCharavailable (int);
char receiveChar(void);
void sendChar(char);
char waitforkeypress(void);
void newline(void);

void timer_T0_sub(char, unsigned char);
void Timer_T0_sub_with_interrupt(char, unsigned char);
void timer_T1_sub(char , unsigned int );
void Hex_to_PC(int);
void short_num_to_PC(char);

int Num_from_KBD(void);
char wait_for_return_key(void);
char decimal_digit (char);

void Num_to_PC(char, int);
void SBtoAL(char*, int, char);
void NumericString_to_PC(char* );



/*********************************************************************************************/
void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
{twenty_msec_delay;}	
UCSR0B = 0;
UBRR0H = UBRROH_N;  
UBRR0L = UBRR0L_N;  
UCSR0A = (1 << U2X0);
UCSR0B = (1 << RXEN0) | (1<< TXEN0);
UCSR0C =  (1 << UCSZ00)| (1 << UCSZ01);} 



/*********************************************************************************************/
void sendString(char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
sendChar(s[i++]);} }


/*********************************************************************************************/
char isCharavailable (int m){int n = 0;		
while (!(UCSR0A & (1 << RXC0))){n++;
if (n>4000) {m--;n = 0;}if (m == 0)return 0;}
return 1;}


/*********************************************************************************************/
char receiveChar(void)
{return UDR0;}



/*********************************************************************************************/
void sendChar(char data){
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = data;}



/*********************************************************************************************/
char waitforkeypress(void){
while (!(UCSR0A & (1 << RXC0)));
return UDR0;}


/*********************************************************************************************/
void newline(void){sendString ("\r\n");}



/***********************************************************************************************/
void timer_T0_sub(char Counter_speed, unsigned char Start_point){ 
TCNT0 = Start_point;
TCCR0B = Counter_speed;
while(!(TIFR0 & (1<<TOV0)));
TIFR0 |= (1<<TOV0); TCCR0B = 0;}



/***************************************************************************************************************************************************/
void Timer_T0_sub_with_interrupt(char Counter_speed, unsigned char Start_point){ 
TIMSK0 |= (1 << TOIE0);
TCNT0 = Start_point;
TCCR0B = Counter_speed;}



/***************************************************************************************************************************************************/
void timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TIFR1 = 0xFF;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}


/***********************************************************************************************/
void Hex_to_PC(int hex){
short_num_to_PC(hex>>8);											//Send hex integer to PC
short_num_to_PC(hex);}



/***********************************************************************************************/ 
void short_num_to_PC(char x)										//convert a number to three chars
{if((x>>4)>0x9)
sendChar((x>>4) + '0' + 7);
else sendChar((x>>4) + '0');

if((x&0xF)>0x9)
sendChar((x & 0xF) + '0' + 7);		
else sendChar((x & 0xF) + '0');}



/***************************************************************************************************************************************************/
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



/***************************************************************************************************************************************************/
char wait_for_return_key(void){	
char keypress,temp;
keypress = waitforkeypress();
if((keypress == '\r') || (keypress == '\n')){
if (isCharavailable(1)){temp = receiveChar();}keypress = '\r';}
return keypress;}



/***************************************************************************************************************************************************/
char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}



/*********************************************************************/
void Num_to_PC(char radix, int int_num){
char array[7];												//Int has 5 chars + sign + null terminator	
SBtoAL(array, int_num, radix);								//calls the Binary to askii subroutine
NumericString_to_PC(array);}								//Prints characters in reverse order



/*********************************************************************/
void SBtoAL(char array[], int num, char radix){			//Signed Binary to Askii
int m=0;
long sign;

if (num == 0x8000){
switch(radix){
case 10: array[0] = '8'; array[1] = '6';
array[2] = '7';array[3] = '2';array[4] = '3';
array[5] = '-'; array[6] = '\0'; break;

case 16: array[0] = '0';array[1] = '0'; array[2] = '0';	
array[3] = '8';array[4] = '-';
array[5] = '\0'; array[6] = '\0'; break; } return;}

for (int n = 0; n <=6; n++)array[n] = 0;	
if ((sign = num) < 0) num = num * (-1);

do {array[m] = num % radix;
if(array[m] < 10)array[m] += '0'; else array[m] += '7';
m++;} while ((num = num/radix) > 0);
if (sign < 0) {array[m] = '-';m++;}}



/*********************************************************************/
void NumericString_to_PC(char* s){					
int n=0;
while (s[n] != '\0')n++;									//scroll to end of string counting the number of characters
for(int m = n; m; m--)sendChar(*(s + m-1));}	

