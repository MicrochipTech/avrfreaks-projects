
int uart_putchar(char c, FILE *mystr_output);
int uart_getchar(FILE *mystr_input);				//accepts any char
int uart_getHex(FILE *mystr_input);					//Ignores non-Hex chars
int uart_getDecimal(FILE *mystr_input);				//Ignores non-Decimal chars
int uart_getDouble(FILE *mystr_input);				//Ignores non-double chars
int uart_getDouble_from_csv(FILE *mystr_input);	
	
char isCharavailable(char);
char decimal_digit (char);
char hex_digit (char);	

void USART_init (unsigned char, unsigned char);
void nop_delay(int);
void Timer_T0_10mS_delay_x_m(int);
void Timer_T0_sub(char, unsigned char);
void Timer_T1_sub(char, unsigned int);
void binUnwantedChars (void);


/*************************************************************************************/
int uart_putchar(char c, FILE *mystr_output)
{if (c == '\n')return 0;
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = c;
return 0;}



/*************************************************************************************/
int uart_getchar(FILE *mystr_input)
{while (!(UCSR0A & (1 << RXC0)));
return UDR0;}



/*************************************************************************************/
int uart_getHex(FILE *mystr_input)
{char keypress; 

do
{while (!(UCSR0A & (1 << RXC0)));		
keypress = UDR0;}
while(!(hex_digit (keypress)) &&\
(keypress != '\r') && (keypress != '\n'));
return keypress;}


/*************************************************************************************/
int uart_getDecimal(FILE *mystr_input)
{char keypress;

do
{while (!(UCSR0A & (1 << RXC0)));	
keypress = UDR0;}
while(!(decimal_digit (keypress)) &&\
(keypress != '\r') && (keypress != '\n')\
&& (keypress != '-'));
return keypress;}


/*************************************************************************************/
int uart_getDouble(FILE *mystr_input)	
{char keypress;

do
{while (!(UCSR0A & (1 << RXC0)));		
keypress = UDR0;}
while(!(decimal_digit (keypress)) && (keypress != '\r')
&& (keypress != '\n')&& (keypress != '-')&& (keypress != '.')
&& (keypress != 'e') && (keypress != 'E'));
return keypress;}



/*************************************************************************************/
int uart_getDouble_from_csv(FILE *mystr_input)
{char keypress;
do
{while (!(UCSR0A & (1 << RXC0)));	
keypress = UDR0;}
while(!(decimal_digit (keypress)) && (keypress != '\r')
&& (keypress != '\n')&& (keypress != '-')&& (keypress != '.')&& (keypress != '\t')
&& (keypress != 'e') && (keypress != 'E')&& (keypress != ','));
if ((keypress == ',') || (keypress == '\t')  || (keypress == '\r')){keypress = '\n';num_counter += 1;}
return keypress;}



/*************************************************************************************/
char isCharavailable (char m){int n = 0;
while (!(UCSR0A & (1 << RXC0))){n++;
if (n>8000) {m--;n = 0;}if (m == 0)return 0;}
return 1;}


/*********************************************************************/
char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}


/*********************************************************************/
char hex_digit (char data){
switch(data){
case '0': case '1': case '2': case '3': case '4': case '5': case '6':
case '7': case '8': case '9': case 'A': case 'B': case 'C': case 'D':
case 'E': case 'F': return 1; break;
default: return 0; break;}}



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
void binUnwantedChars (void){char bin_char;
while(1){if (isCharavailable(5)==1)bin_char = UDR0;else break;}}
