

#define T0_delay_5ms 	5,178


void USART_init (unsigned char, unsigned char);
void Timer_T0_sub(char, unsigned char);
char isCharavailable (int);
char waitforkeypress(void);
void sendString(const char*);
void sendChar(char);
char receiveChar(void);



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
void Timer_T0_sub(char Counter_speed, unsigned char Start_point){ 

GTCCR | (1 << PSRSYNC);
TCNT0 = Start_point;
TCCR0B = Counter_speed;
while(!(TIFR0 & (1<<TOV0)));
TIFR0 |= (1<<TOV0); TCCR0B = 0;}



/**********************************************************************************************/
char isCharavailable (int m){int n = 0;		
while (!(UCSR0A & (1 << RXC0))){n++;
if (n>8000) {m--;n = 0;}if (m == 0)return 0;}
return 1;}



/**********************************************************************************************/
char waitforkeypress(void){
while (!(UCSR0A & (1 << RXC0)));
return UDR0;}



/**********************************************************************************************/
void sendString(const char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
sendChar(s[i++]);} }



/**********************************************************************************************/
void sendChar(char data){
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = data;}


/**********************************************************************************************/
char receiveChar(void)
{return UDR0;}

