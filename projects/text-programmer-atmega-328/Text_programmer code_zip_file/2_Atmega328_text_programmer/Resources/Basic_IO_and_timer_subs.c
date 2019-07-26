



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



/**********************************************************************************************/
void Timer_T0_sub_with_interrupt(char Counter_speed, unsigned char Start_point){ 
TIMSK0 |= (1 << TOIE0);
TCNT0 = Start_point;
TCCR0B = Counter_speed;}



/**********************************************************************************************/
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



