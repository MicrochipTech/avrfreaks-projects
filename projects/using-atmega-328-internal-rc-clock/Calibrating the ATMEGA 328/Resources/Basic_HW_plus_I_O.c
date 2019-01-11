void USART_init (unsigned char, unsigned char);
void nop_delay(int);
void Timer_T0_10mS_delay_x_m(int);
void Timer_T0_sub(char, unsigned char);
void Timer_T1_sub(char, unsigned int);
void sendChar(char);
void sendString(char*);


char waitforkeypress(void);
char receiveChar(void);
char isCharavailable(char);
void sendChar(char);
void sendString(char*);
void newline(void);
void sendLongNum(char, long);
void SBtoAL(char*, long, char);
void sendNumericString(char*);
void UBtoAL(char*, unsigned long, char);




/*********************************************************************/
void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
Timer_T1_sub(T1_delay_100ms);
UCSR0B = 0;
UBRR0H = UBRROH_N;  									
UBRR0L = UBRR0L_N;  								
UCSR0A = (1 << U2X0);
UCSR0B = (1 << RXEN0) | (1<< TXEN0);
UCSR0C =  (1 << UCSZ00)| (1 << UCSZ01);}

/*********************************************************************/
void nop_delay(int nop_counter){for(int q = 0; q<= nop_counter; q++) {asm("nop");}}

/*********************************************************************/
void Timer_T0_10mS_delay_x_m(int m)
{for (int n = 0; n < m; n++){Timer_T0_sub(T0_delay_10ms);}}

/*********************************************************************/
void Timer_T0_sub(char Counter_speed, unsigned char Start_point){ 
TCNT0 = Start_point;
TCCR0B = Counter_speed;
while(!(TIFR0 && (1<<TOV0)));
TIFR0 |= (1<<TOV0); TCCR0B = 0;}

/*********************************************************************/
void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TIFR1 = 0xFF;
TCCR1B = Counter_speed;
while(!(TIFR1 && (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}


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


/*********************************************************************/
void sendChar(char data){
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = data;}

/*********************************************************************/
void sendString(char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
sendChar(s[i++]);} }

/*********************************************************************/
void newline(void){sendString ("\r\n");}


/*********************************************************************/
void sendLongNum(char radix, long long_num){
char array[12];											//Long has 10 chars + sign + null terminator	
SBtoAL(array, long_num, radix);							//calls the Binary to askii subroutine
sendNumericString(array);}								//Prints characters in reverse order

/*********************************************************************/
void SBtoAL(char array[], long num, char radix){		//Signed Binary to Askii
int m=0;
long sign;

if (num == 0x80000000){
switch(radix){
case 10: array[0] = '8';array[1] = '4'; array[2] = '6';			
array[3] = '3';array[4] = '8';array[5] = '4'; array[6] = '7';
array[7] = '4';array[8] = '1';array[9] = '2';
array[10] = '-'; array[11] = '\0'; break;

case 16: array[0] = '0';array[1] = '0'; array[2] = '0';			
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
void sendNumericString(char* s){					
int n=0;
while (s[n] != '\0')n++;									//scroll to end of string counting the number of characters
for(int m = n; m; m--)sendChar(*(s + m-1));}				//print last character first


/*********************************************************************/




