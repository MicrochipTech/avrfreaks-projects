
void Timer_T0_sub(char, unsigned char);
char receiveChar(void);
void sendChar(char);

void Timer_T0_10mS_delay_x_m(int);
void Timer_T0_sub_with_interrupt(char, unsigned char);
void Timer_T1_sub(char, unsigned int);
void Timer_T1_sub_with_interrupt(char, unsigned int);
void Timer_T2_sub(char, unsigned char);
void Timer_T2_sub_with_interrupt(char, unsigned char); 

void newline(void);
void binUnwantedChars (void);
void sendString_with_pause(const char*);
void NumericString_to_PC_with_pause(char*);







/**********************************************************************************************/
void Timer_T0_10mS_delay_x_m(int m)
{for (int n = 0; n < m; n++){Timer_T0_sub(T0_delay_10ms);}}



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

GTCCR |= (1 << PSRASY);

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
void binUnwantedChars (void){char bin_char;
while(1){if (isCharavailable(5)==1)bin_char = receiveChar();else break;}}




/**********************************************************************************************/
void sendString_with_pause(const char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
sendChar(s[i++]);Timer_T0_sub(T0_delay_2ms);} }






/*********************************************************************/
void NumericString_to_PC_with_pause(char* s){					
int n=0;
while (s[n] != '\0')n++;								//scroll to end of string counting the number of characters
for(int m = n; m; m--)sendChar(*(s + m-1));}			//print last character first

