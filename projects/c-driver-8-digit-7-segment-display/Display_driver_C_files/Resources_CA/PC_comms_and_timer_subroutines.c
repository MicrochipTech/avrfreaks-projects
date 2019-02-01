

void USART_init (unsigned char, unsigned char);
char waitforkeypress(void);
char receiveChar(void);
char isCharavailable(char);
void sendChar(char);
void sendString(char*);
void timer_T0_sub_with_interrupt(char, unsigned char);
void Timer_T1_sub(char, unsigned int);


/*********************************************************************/
void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
Timer_T1_sub(T1_delay_100ms);
UCSR0B = 0;
UBRR0H = UBRROH_N;  									
UBRR0L = UBRR0L_N;  								
UCSR0A = (1 << U2X0);
UCSR0B = (1 << RXEN0) | (1<< TXEN0);
UCSR0C =  (1 << UCSZ00)| (1 << UCSZ01);}



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

/*****************************************************************************************/
void timer_T0_sub_with_interrupt(char Counter_speed, unsigned char St_point){ 
TCNT0 = St_point;
TCCR0B = Counter_speed;}


/*****************************************************************************************/
void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TIFR1 = 0xFF;
TCCR1B = Counter_speed;
while(!(TIFR1 && (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}