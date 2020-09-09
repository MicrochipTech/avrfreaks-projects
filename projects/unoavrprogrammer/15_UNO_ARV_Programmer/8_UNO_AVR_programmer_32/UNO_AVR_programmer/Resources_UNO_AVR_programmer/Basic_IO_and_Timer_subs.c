
void USART_init (unsigned char, unsigned char);

void Timer_T0_10mS_delay_x_m(int);
void Timer_T0_sub(char, unsigned char);
void Timer_T0_sub_with_interrupt(char, unsigned char);

void Timer_T1_sub(char, unsigned int);
void Timer_T1_sub_with_interrupt(char, unsigned int);

void Timer_T2_sub(char, unsigned char);
void Timer_T2_sub_with_interrupt(char, unsigned char); 

void newline(void);
char waitforkeypress(void);
char receiveChar(void);
char isCharavailable (int);

void binUnwantedChars (void);
void sendChar(char);
void sendString_with_pause(const char*);
void sendString(const char*);
void NumericString_to_PC_with_pause(char*);





/**********************************************************************************************/
void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
Timer_T0_sub(T0_delay_5ms);
UCSR0B = 0;
UBRR0H = UBRROH_N; 
UBRR0L = UBRR0L_N;  
UCSR0A = (1 << U2X0);
UCSR0B = (1 << RXEN0) | (1<< TXEN0);
UCSR0C =  (1 << UCSZ00)| (1 << UCSZ01);} 



/**********************************************************************************************/
void Timer_T0_10mS_delay_x_m(int m)
{for (int n = 0; n < m; n++){Timer_T0_sub(T0_delay_10ms);}}



/**********************************************************************************************/
void Timer_T0_sub(char Counter_speed, unsigned char Start_point){ 
TCNT0 = Start_point;
TCCR0B = Counter_speed;
while(!(TIFR0 & (1<<TOV0)));
TIFR0 |= (1<<TOV0); TCCR0B = 0;}



/**********************************************************************************************/
void Timer_T0_sub_with_interrupt(char Counter_speed, unsigned char Start_point){ 
TIMSK0 |= (1 << TOIE0);
TCNT0 = Start_point;
TCCR0B = Counter_speed;}




/**********************************************************************************************/
void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TIFR1 = 0xFF;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}




/**********************************************************************************************/
void Timer_T1_sub_with_interrupt(char Counter_speed, unsigned int Start_point){ 
TIMSK1 |= (1 << TOIE1);
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TIFR1 = 0xFF;
TCCR1B = Counter_speed;
}



/**********************************************************************************************/
void Timer_T2_sub(char Counter_speed, unsigned char Start_point){ 
TCNT2 = (Start_point );
TCCR2B = Counter_speed;
while(!(TIFR2 & (1<<TOV2)));
TIFR2 |= (1<<TOV2); 
TCCR2B = 0;}




/**********************************************************************************************/
void Timer_T2_sub_with_interrupt(char Counter_speed, unsigned char Start_point){ 
TIMSK2 |= (1 << TOIE2);
TCNT2 = (Start_point );
TCCR2B = Counter_speed;}


/**********************************************************************************************/
void newline(void){sendString ("\r\n");}



/**********************************************************************************************/
char waitforkeypress(void){
while (!(UCSR0A & (1 << RXC0)));
return UDR0;}



/**********************************************************************************************/
char receiveChar(void)
{return UDR0;}



/**********************************************************************************************/
char isCharavailable (int m){int n = 0;		
while (!(UCSR0A & (1 << RXC0))){n++;
if (n>4000) {m--;n = 0;}if (m == 0)return 0;}
return 1;}




/**********************************************************************************************/
void binUnwantedChars (void){char bin_char;
while(1){if (isCharavailable(5)==1)bin_char = receiveChar();else break;}}


/**********************************************************************************************/
void sendChar(char data){
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = data;}




/**********************************************************************************************/
void sendString_with_pause(const char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
sendChar(s[i++]);Timer_T0_sub(T0_delay_2ms);} }




/**********************************************************************************************/
void sendString(const char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
sendChar(s[i++]);} }


/*********************************************************************/
void NumericString_to_PC_with_pause(char* s){					
int n=0;
while (s[n] != '\0')n++;								//scroll to end of string counting the number of characters
for(int m = n; m; m--)sendChar(*(s + m-1));}			//print last character first

