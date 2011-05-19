#define Green_LED 	PB4
#define Red_LED 	PB5
#define MotorR_en   PB0
#define MotorL_en   PB3

#define battery_sample	7
#define half_wheelbase	3.25
//----UART defines
#define UART_BAUD  	4800
#define RX_BUFSIZE 	80

//----IR receiver ADC numbers
#define front_IR		1
#define back_IR			4
#define left_front_IR	2
#define right_front_IR	0
#define left_back_IR	3
#define right_back_IR	5

//----IR Emitter Port
#define IR_emitter		PORTD
#define front_em		7
#define back_em			4
#define left_front_em	2	
#define right_front_em	6
#define left_back_em	3
#define right_back_em	5

static uint8_t	pulse_width=10;   //ms

struct pos_type{
	int x,y;
} robot_position;



void io_init(){
	//   D7 - D2     D1   D0
	// IR-Emitters   TxD  RxD
	DDRD=0xFE; 
	//   C5 - C0 IR-Phototransistors
	DDRC= 0x00; // ADC input
	//   B7        B6       B5   B4      B3     B2    B1    B0
	// Cont_Rec  IR-Emit  Red-Grn LEDs  Mtr-L  OC1A  OC1B  Mtr-R
	DDRB=0x7F;
}
//----------------------------------- UART Init
void uart_init(void)
{
#if F_CPU < 2000000UL && defined(U2X)
  UCSR0A = _BV(U2X);             /* improve baud rate error by using 2x clk */
  UBRR0L = (F_CPU / (8UL * UART_BAUD)) - 1;
#else
  UBRR0L = (F_CPU / (16UL * UART_BAUD)) - 1;
#endif
  UCSR0B = _BV(TXEN0) | _BV(RXEN0); /* tx/rx enable */
}
//----------------------------------- PWM Init
void pwm_init(){
	TCCR1A=0xA3;
	TCCR1B |=0x09;
//	DDRB |=0x06;
}
//----------------------------------- ADC Init
void adc_init(void)
{
//select reference voltage
//AVCC with external capacitor at AREF pin
	ADMUX|=(0<<REFS1)|(1<<REFS0);
//set prescaller and enable ADC
	ADCSRA|=(1<<ADEN)|(1<<ADIE);//enable ADC with dummy conversion
//set sleep mode for ADC noise reduction conversion
	//set_sleep_mode(SLEEP_MODE_ADC);
}
//----------------------------------- Timer 0 init
void timer0_init(){
/*enable timer overflow interrupt*/
TIMSK0 |= (1<<TOV0);
/*start timer without presscaler*/
TCCR0A=0x00;
TCCR0B=0x03;
/* enable interrupts    each 10ms */   
TCNT0 =0x63;
sei(); 
}
