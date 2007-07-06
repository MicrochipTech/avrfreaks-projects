
//GLOBALS

#define MIN_DELTA 16
//the minimal step between steps
#define SEQ_LENGTH 64
//the max length of the key
#define SETUP_DONE 42
#define FIRTS_BOOT 0xFF
//just the character for the definition of the setup being done and not being done 

#define END_CHARACTER 'e'		//with this character ends the stream of chars

#define reset_timer() {TCNT1 = 0;}	//reseting of the timer
#define start_adc() sbi(ADCSRA,6);//a macro for starting the ADC
#define wait_for_adc() loop_until_bit_is_clear(ADCSRA,6);//rather useles, as the ADC is linked to the interrupt

unsigned char seq[SEQ_LENGTH];	//the constant sequence 
unsigned char seq_comp[SEQ_LENGTH];	//the buffer for the stored data
volatile unsigned char enabled = 0;	//is the lock locked or not?

unsigned char adc_prev=0;	//the last recorded value gotten from the ADC
unsigned char prev_val = 'G';	//the previos value of the input (ABCD)
volatile unsigned int incursions = 0;	//the number of times someone tried to communicate with the lock and be evil 



//FUNCTION PROTOTYPES
static int usart_putchar(char c, FILE *stream);	//the FILE part is there just for printf compatibility
unsigned char usart_getchar(void);	//wait for the char to be received and then return it
unsigned char compare_seq();	//a comparator, if the sequence provided matches the constant one
void wait(unsigned int a);	//the wait function not only provides a few seconds of nothing, it also blinks the leds on the port
static inline void unlock();	//this will do all that is necesary to unlock the lock 
static inline void lock();	//lock the lock after unlocking 
static inline void clear_sequence();	//this function will reset the combination lock 
static inline void show_sequence();	//does nothing, used for debugging
static inline void load_settings();//this function reads the code from the eeprom or asks the user for a new one
static inline void io_init();	//initializations of the HW


static FILE usart = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);//make a file out of the usart


//////////////////////////The actual file/////////////////////////////////////////////////////////////////////////////

static int usart_putchar(char c, FILE *stream)	//the FILE part is there just for printf compatibility
{
	if(c==0)return 0;
	while (!(UCSRA&(1<<UDRE)));	
	UDR=c;
	return 0;
} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char usart_getchar(void) 	//wait for the char to be received and then return it
{
	unsigned char temp=PORTD;
	PORTD = 0xFF;	//light the leds so they will indicate to the user that the device is requesting something
	loop_until_bit_is_set(UCSRA,RXC);
	PORTD = temp;
	return UDR;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char compare_seq()	//a comparator, if the sequence provided matches the constant one
{
	unsigned char a; 
	for(a=0;a!=SEQ_LENGTH;a++)
	{
		if(seq[a] == END_CHARACTER) return 1;
		if(seq[a] != seq_comp[a]) return 0;
	}
	return 1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void wait(unsigned int a)	//the wait function not only provides a few seconds of nothing, it also blinks the leds on the port
{							
	volatile unsigned int b,c;
	unsigned char pp=PORTD;
	PORTD = 0x30 | _BV(0);
#ifndef DEBUG
	for(b=0;b!= a; b++)
	{
		if(b%256==0)
		{
			PORTD = ~PORTD | _BV(0);
		}
		for(c=0;c!= 200;c++)
		{
			wdt_reset();
		}
	}
	PORTD = pp;
#endif
	return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void unlock()	//this will do all that is necesary to unlock the lock 
{
	printf("Lock opened!\n");
	sbi(PORTB,0);
	if(incursions != 0) printf("There have been %i unauthorized attempts to open communicate with this lock! Beware!\n",incursions);
	incursions =0;
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void lock()	//lock the lock after unlocking 
{
	printf("Lock closing...\n");
	cbi(PORTB,0);
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void clear_sequence()	//this function will reset the combination lock 
{	
	unsigned char a;
	for(a=0;a!=SEQ_LENGTH;a++)seq_comp[a] = 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void show_sequence()
{		//this has been commented out due to the lag it produced on the ADC interrupt. if we think of the use of the device
		//this function was rather useless anyway... I mean, the serial port is supposed to be connected only for a few 
		//moments during the configuration. It was more of a debugging thingie
/*	unsigned char c; 
	for(c=0;c!=SEQ_LENGTH;c++)
	{
		usart_putchar(seq_comp[c],NULL);
	}
	usart_putchar('\n',NULL);*/
	return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void load_settings()//this function reads the code from the eeprom or asks the user for a new one
{
	unsigned char a,b;
	printf("Welcome to daqqs codelock. For the manual and other info check out www.daqq.eu  \n");
	a=eeprom_read_byte((uint8_t*)SEQ_LENGTH+1);	//check if we're past the initial setup

	if(a!=SETUP_DONE)	//if we haven't done the initial setup
	{
		printf("\nThe lock does not have a code set! Provide one:\n");
		for(a=0;a!=SEQ_LENGTH-1;a++)
		{
		  setup_start:
			b = usart_getchar();
			if((b > 'D' || b < 'A') && b != END_CHARACTER)//check the character we got if it is valid
			{
				printf("\nIncorrect input!\n");
				goto setup_start;
			}

			if(b== END_CHARACTER)break;//exits the cycle if the received character is the end
			
			if(seq[a-1]-b!=1 && seq[a-1]-b!=-1 && a!=0) //if the step is too high
			{
				printf("\nThe maximal step is 1 or -1. Example:Instead of providing AD you must provide ABCD.\n");
				goto setup_start;
			}
			seq[a]=b;

			printf("\nCurrent combination: ");
			for(b=0;b!=a+1;b++)
			{
				usart_putchar(seq[b],NULL);
			}
			usart_putchar('\n',NULL);
		}

		if(a == SEQ_LENGTH-1)
		{
			printf("Maximum length achieved!\n");
		}

		seq[a] = END_CHARACTER;//the termination character
		printf("\nFinal sequence: ");
		for(b=0;b!=SEQ_LENGTH;b++)
		{
			eeprom_write_byte(b,seq[b]);
			usart_putchar(seq[b],NULL);
		}
		eeprom_write_byte((uint8_t*)SEQ_LENGTH+1,SETUP_DONE);
		usart_putchar('\n',NULL);
	}

	else//if we already have been configured
	{
		for(a=0;a!=SEQ_LENGTH;a++)
		{
			seq[a] = eeprom_read_byte(a);//retrieve the previos setting from the memory
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void io_init()	//initializations of the HW
{
	wdt_disable();	//disable the watchdog timer

	TIMSK = _BV(TOIE1);	//enable the interrupt for the TIMER 1 overflow
	TCCR1B = _BV(CS12) | _BV(CS10);	//sets the timer1 prescaler to 1024
	
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	ADMUX = _BV(ADLAR) | _BV(REFS0) | _BV(MUX2);//setting up the ADC

	UBRRL = 51;	//9600
	UCSRB = _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);
	UCSRC = _BV(UCSZ0) | _BV(UCSZ1) | _BV(URSEL);//the usart settings...the simplest 9600 baud

	DDRD = _BV(PB7) | _BV(PB6) | _BV(PB5) | _BV(PB4);//enavle the outputs on the LEDs
	DDRB = _BV(0);//the pin for the relay
	PORTD = _BV(0);	//enable the pullup on the serial line. it IS necesary
	PORTB = 0;
}


