

void sendString(char*);
char isCharavailable (int);
char receiveChar(void);
void sendChar(char);
void Hex_to_PC(int);
void short_num_to_PC(char);

void initialise_timers_for_cal_error(void);
void start_timers_for_cal_error(void);
int cal_error(void);
int get_error (char);



/**********************************************************************************/
#define delay_5ms 5,220
#define delay_20ms 5,100

#define five_msec_delay;	timer_T0_sub(delay_5ms);
#define twenty_msec_delay;	timer_T0_sub(delay_20ms);


#define wdr()  __asm__ __volatile__("wdr")

/**********************************************************************************/
#define setup_HW;\
config_WDT;\
Unused_I_O;\
ADMUX |= (1 << REFS0);


/**********************************************************************************/
#define config_WDT;\
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;




#define Unused_I_O;\
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;

