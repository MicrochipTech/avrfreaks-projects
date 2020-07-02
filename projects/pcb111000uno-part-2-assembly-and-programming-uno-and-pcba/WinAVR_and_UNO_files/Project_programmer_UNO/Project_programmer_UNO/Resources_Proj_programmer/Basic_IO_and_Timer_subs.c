
void USART_init (unsigned char, unsigned char);
void timer_T0_sub(char, unsigned char);
void timer_T1_sub(char, unsigned int);
void newline(void);
char waitforkeypress(void);
char receiveChar(void);
char isCharavailable (int);

void binUnwantedChars (void);
void sendChar(char);
void sendString_with_pause(const char*);
void sendString(const char*);







void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
timer_T0_sub(T0_delay_5ms);
UCSR0B = 0;
UBRR0H = UBRROH_N; 
UBRR0L = UBRR0L_N;  
UCSR0A = (1 << U2X0);
UCSR0B = (1 << RXEN0) | (1<< TXEN0);
UCSR0C =  (1 << UCSZ00)| (1 << UCSZ01);} 




void timer_T0_sub(char Counter_speed, unsigned char Start_point){ 
TCNT0 = Start_point;
TCCR0B = Counter_speed;
while(!(TIFR0 & (1<<TOV0)));
TIFR0 |= (1<<TOV0); TCCR0B = 0;}




void timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TIFR1 = 0xFF;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}



void newline(void){sendString ("\r\n");}




char waitforkeypress(void){
while (!(UCSR0A & (1 << RXC0)));
return UDR0;}




char receiveChar(void)
{return UDR0;}


char isCharavailable (int m){int n = 0;		
while (!(UCSR0A & (1 << RXC0))){n++;
if (n>4000) {m--;n = 0;}if (m == 0)return 0;}
return 1;}





void binUnwantedChars (void){char bin_char;
while(1){if (isCharavailable(5)==1)bin_char = receiveChar();else break;}}



void sendChar(char data){
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = data;}



void sendString_with_pause(const char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
sendChar(s[i++]);timer_T0_sub(T0_delay_2ms);} }


void sendString(const char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
sendChar(s[i++]);} }
