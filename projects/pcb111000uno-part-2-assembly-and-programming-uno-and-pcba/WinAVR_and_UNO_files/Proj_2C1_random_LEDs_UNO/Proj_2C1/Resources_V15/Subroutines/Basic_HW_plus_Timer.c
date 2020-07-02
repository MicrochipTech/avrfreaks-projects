void USART_init (unsigned char, unsigned char);
void nop_delay(int);
void Timer_T0_10mS_delay_x_m(int);
void Timer_T0_sub(char, unsigned char);
void Timer_T0_sub_with_interrupt(char, unsigned char);
void T1_65ms_clock_tick(void);
void Timer_T1_sub_with_interrupt(char, unsigned int);
void Timer_T1_sub(char, unsigned int);
void Timer_T2_sub(char, unsigned char);



/*********************************************************************/
void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
//Timer_T1_sub(T1_delay_10ms);
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
while(!(TIFR0 & (1<<TOV0)));
TIFR0 |= (1<<TOV0); TCCR0B = 0;}


/*********************************************************************/
void Timer_T0_sub_with_interrupt(char Counter_speed, unsigned char Start_point){ 
TIMSK0 |= (1 << TOIE0);
TCNT0 = Start_point;
TCCR0B = Counter_speed;}


/*********************************************************************/
void T1_65ms_clock_tick(void){ 
TIMSK1 |= (1 << TOIE1);TCCR1B = 0x02;}

/*********************************************************************/
void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TIFR1 = 0xFF;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}

/*********************************************************************/
void Timer_T1_sub_with_interrupt(char Counter_speed, unsigned int Start_point){ 
TIMSK1 |= (1 << TOIE1);
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TCCR1B = Counter_speed;}


/*********************************************************************/
void Timer_T2_sub(char Counter_speed, unsigned char Start_point){ 
TCNT2 = (Start_point );

TCCR2B = Counter_speed;
while(!(TIFR2 & (1<<TOV2)));
TIFR2 |= (1<<TOV2); 
TCCR2B = 0;}

