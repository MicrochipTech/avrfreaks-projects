// -----------------------------------------
// cpu:     ATmega8L
// speed:   8 mhz (max: 8 mhz)
// voltage: 3.3 V
// ram:     1024 bytes (0x0060-0x045f)
// rom:     4096 bytes (0x0000-0x0fff)
// eeprom:  512 bytes (0x0000-0x01ff)
// author : mjmg
// Date		Nov. 16, '09
// Process  up down counter
// -----------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#if !defined (__AVR_ATmega8__)
#error __AVR_ATmega8__ not defined !
#endif

#if !defined (F_CPU)
#define F_CPU 8000000
#endif

#define RAMSTART 0x0060
#define RAMSIZE (RAMEND-RAMSTARTPIND 3 16)

#define sleep()


#define CHECKBIT(x,y) (x & (y)) /* Check bit y in byte x*/
#define SETBIT(x,y) (x |= (y)) /* Set bit y in byte x*/
#define CLEARBIT(x,y) (x &= (~y)) /* Clear bit y in byte x*/

//INPUT PIND DEFINITION
#define RESET0 0x01
#define ch_N 0x08
#define ch_A 0x04
#define ch_B 0x02
#define in_recall 0x10
#define res_Total 0x20
/*----------------------------------------------------------------
-------------CONNECTION BETWEEN 7 segment AND ATTINY2313-----------
-----------------------------------------------------------------*/
// PORTB 7 segment LED
#define DATA_DDR	DDRB
#define DATA_PORT	PORTB
#define Pin_a     	3
#define Pin_b     	2
#define Pin_c     	1
#define Pin_d     	7
#define Pin_e     	6
#define Pin_f     	4
#define Pin_g     	5
#define Pin_neg     0

// PORTC digit
#define MUX_PORT	PORTC
#define digit_1    	0x20 
#define digit_10   	0x10 
#define digit_100  	0x08 
#define digit_1k  	0x04
#define digit_10k  	0x02
#define digit_100k 	0x01

// input encoder PORT
#define	IN_PORT		PIND

//output LED NULL 
#define LED_PORT	PORTD
#define LED_NULL	7 //PD7

int32_t number = 0;
uint16_t temp ;
//int32_t num_scale = 0;
//int32_t num_recall = 0;

ISR(INT0_vect) // Interrupt 0 
//bool z
{
	if ((CHECKBIT(IN_PORT,ch_A)) && (!CHECKBIT(IN_PORT,ch_B)))
	{
		number++;
	}

	else if ((CHECKBIT(IN_PORT,ch_A)) && (CHECKBIT(IN_PORT,ch_B)))
	{
		number--;
	}

}

ISR(INT1_vect) // Interrupt 1 
{

	if (CHECKBIT(IN_PORT,ch_N)) 
		{
			LED_PORT|=_BV(LED_NULL); //SETBIT(LED_PORT,LED_NULL);
		}
	else 
		{
			LED_PORT&=~_BV(LED_NULL); //CLEARBIT(LED_PORT,LED_NULL);
	 	}
}

ISR(TIMER0_OVF_vect) // Timer/Counter0 Overflow
{
	TCNT0 = 178;
	// ... ;
}

// ************************************************************************* */
// ;I/O PORT Initialisation												   */
// ************************************************************************* */
/*	Note: pull-up function requires PUD in MCUCR to be zero (default value).
	PORTB(input):  1=pull-up on, 0=pull-up off
	PORTB(output): 1=output high (source), 0=output low (sink)
	DDRB: 1=output, 0=input	*/					

void Ports_Init(void)
{
	DDRB  = (1<<DDB7)|(1<<DDB6)|(1<<DDB5)|(1<<DDB4)|(1<<DDB3)|(1<<DDB2)|(1<<DDB1)|(1<<DDB0);
	PORTB = (1<<PB7)|(1<<PB6)|(1<<PB5)|(1<<PB4)|(1<<PB3)|(1<<PB2)|(1<<PB1)|(1<<PB0);

	DDRC  = (0<<DDC6)|(1<<DDC5)|(1<<DDC4)|(1<<DDC3)|(1<<DDC2)|(1<<DDC1)|(1<<DDC0);
	PORTC = (1<<PC6)|(1<<PC5)|(1<<PC4)|(1<<PC3)|(1<<PC2)|(1<<PC1)|(1<<PC0);

	DDRD  = (1<<DDD7)|(1<<DDD6)|(0<<DDD5)|(0<<DDD4)|(0<<DDD3)|(0<<DDD2)|(0<<DDD1)|(0<<DDD0);
	PORTD = (1<<PD7)|(1<<PD6)|(1<<PD5)|(1<<PD4)|(1<<PD3)|(1<<PD2)|(1<<PD1)|(1<<PD0);

}

// ---------------
// --- ext irq ---
// ---------------

void ExtIrq_Init(void)
{
	// enable INT0 and disable INT1 interrupt on change level.
	MCUCR = (0<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00);
	GICR = (0<<INT1)|(1<<INT0);

}

// --------------
// --- Timer0 ---
// --------------

void Timer0_Init(void)
{
	// Timer0 settings: ~ 79872 ticks (9984 us / 9.984 ms / 0.009984 sec)
	TCCR0 = (1<<CS02) | (1<<CS00); // CLK/1024
	TIMSK = (0 << TOIE0); // Timer/Counter0 Overflow Interrupt Enable
	// disable timer
	TCNT0 = 178;
}

void ANA_COMP_Init(void)
// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
{

	ACSR	= (1<<ACD);
}

// ---------------------------
// --- check type of reset ---
// ---------------------------

void onPowerOnReset()
{
	SP	= RAMEND	;
}
void onExternalReset()
{
	// ... ;
}
void onBrowOutReset()
{
	// ... ;
}
void onWatchdogReset()
{
	// ... ;
}

// --------------
// --- main() ---
// --------------





int main()
{
uint8_t  first_digit_tmp ;
uint8_t  second_digit_tmp ;
uint8_t  third_digit_tmp ;
uint8_t  fourth_digit_tmp ;
uint8_t	 fifth_digit_tmp;
uint8_t	 sixth_digit_tmp;
static uint8_t rcall;

//unsigned char r_low;

//	in_recall;



	if (MCUCSR & (1<<PORF)) onPowerOnReset(); // Power-on Reset Flag
	if (MCUCSR & (1<<EXTRF)) onExternalReset(); // External Reset Flag
	if (MCUCSR & (1<<BORF)) onBrowOutReset(); // Brown-out Reset Flag
	if (MCUCSR & (1<<WDRF)) onWatchdogReset(); // Watchdog Reset Flag
	Ports_Init();
	ExtIrq_Init();
//	USART0_Init();
	Timer0_Init();
	ANA_COMP_Init;

	sei(); // enable interrupts

for (;;)  
{
//  Init_Ports();
  do
  {
	
	if (!CHECKBIT(IN_PORT,RESET0)) //check reset button
	{
		_delay_ms(10);	//reset button debounce for 10mS
		if (!CHECKBIT(IN_PORT,RESET0)) number=0; //check after debounce and reset number if true
	}

	if (number>=0)
		{
			DATA_PORT&=~_BV(Pin_neg);
		}
	else
		{ 
			DATA_PORT|=_BV(Pin_neg);
		}

// DISPLAY PROCESSING OF 6 DIGIT NUMBER VALUE
		// One's Digit Display

		first_digit_tmp = abs(number % 10); // take the absolute value
		temp = abs(number / 10);	


	Display(first_digit_tmp);	// LED data code
	MUX_PORT = ~digit_1;				// display multiplexing
	_delay_us(500);				// delay routine

//	temp = abs(number / 10);	

	// Ten's Digit Display	
	if (temp>=1)
		{
		second_digit_tmp= temp % 10;
		Display(second_digit_tmp);
		}
	else Display(0);

	MUX_PORT = ~digit_10;				// display multiplexing
	_delay_us(500);				// delay routine

	// Hundred's Digit Display	
	temp = temp / 10;

	if (temp>=1)
		{
		third_digit_tmp= temp % 10;
		Display(third_digit_tmp);
		}
	else	 Display(0);
	
	MUX_PORT = ~digit_100;				// display multiplexing
	_delay_us(500);				// delay routine

	// Thousand's Digit Display	
	temp = temp / 10;
	if (temp>=1)
		{
		fourth_digit_tmp= temp % 10;
		Display(fourth_digit_tmp);
		}
	else	 Display(0);

	MUX_PORT = ~digit_1k;				// display multiplexing
	_delay_us(500);				// delay routine

	// Ten Thousand's Digit Display	
		temp = temp / 10;
	if (temp>=1)
		{
		fifth_digit_tmp= temp % 10;
		Display(fifth_digit_tmp);
		}
	else	 Nodisplay();

	MUX_PORT = ~digit_10k;				// display multiplexing
	_delay_us(500);				// delay routine

	// Hundred Thousand Digit Display	
		temp = temp / 10;
	if (temp>=1)
		{
		sixth_digit_tmp= temp % 10;
		Display(sixth_digit_tmp);
		}
	else	 Nodisplay();

	MUX_PORT = ~digit_100k;				// display multiplexing
	_delay_us(500);				// delay routine


}
  while(1);

} 


}


/*----------------------------------------------------------------
-----------------FUNCTIONS TO DISPLAY VALUES----------------------
-----------------------------------------------------------------*/  
  int Display(int a)
  {
  switch(a)
  {
  case 0:  
			DATA_PORT|=_BV(Pin_g);
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_f));
			break;
			
  case 1:
			
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_f)|_BV(Pin_g));
			DATA_PORT&=~(_BV(Pin_b)|_BV(Pin_c));
			break;
			
  case 2:  
			
  
			DATA_PORT|=(_BV(Pin_c)|_BV(Pin_f));
			DATA_PORT&=~(_BV(Pin_b)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_a)|_BV(Pin_g));
			break;
			
	
  case 3:  
			DATA_PORT|=(_BV(Pin_e)|_BV(Pin_f));
			DATA_PORT&=~(_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_g));
			break;
			

  case 4:  
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_d)|_BV(Pin_e));
			DATA_PORT&=~(_BV(Pin_c)|_BV(Pin_b)|_BV(Pin_f)|_BV(Pin_g));
			break;
			
  case 5:  
			DATA_PORT|=(_BV(Pin_b)|_BV(Pin_e));
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_f)|_BV(Pin_g));
			break;
			
  case 6:  
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_b));
			DATA_PORT&=~(_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_f)|_BV(Pin_g)|_BV(Pin_e));
			break;
			
  case 7:  
			DATA_PORT|=(_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_f)|_BV(Pin_g));
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_c)|_BV(Pin_b));
			break;
			
  case 8:  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_c)|_BV(Pin_b)|_BV(Pin_f)|_BV(Pin_g));
			break;
		
  case 9:  
			DATA_PORT|=(_BV(Pin_d)|_BV(Pin_e));
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_c)|_BV(Pin_b)|_BV(Pin_f)|_BV(Pin_g));
			break;
					
	return 0;
	}
	}


int Nodisplay(int b)
{
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_c)|_BV(Pin_b)|_BV(Pin_f)|_BV(Pin_g));

}
