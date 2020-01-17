

/*********Subroutines provided by Basic_IO_and_timer_subs.c file**************/
void USART_init (unsigned char, unsigned char);
void sendString(char*);
char isCharavailable (int);
char receiveChar(void);
void sendChar(char);
char waitforkeypress(void);





/************************Subroutine provided by .S file*******************/
extern void read_flash (void);
extern void clear_read_block(void);							//Required afer programming before flash can be read


/***************************************************************************/
volatile char loc_in_mem_H, loc_in_mem_L;					//Used to pass addresses in SRAM to assembly
volatile char Prog_mem_address_H, Prog_mem_address_L;		//Used to pass addresses in flash to the assembly subroutines
volatile char Flash_readout;


/**********************************************************************************/
#define delay_20ms 5,100
#define twenty_msec_delay;	timer_T0_sub(delay_20ms);


#define wdr()  __asm__ __volatile__("wdr")



/**********************************************************************************/
#define setup_HW;\
config_WDT;\
Unused_I_O;\
ADMUX |= (1 << REFS0);\
USART_init(0,16);



/**********************************************************************************/
#define config_WDT;\
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;




/**********************************************************************************/
#define Unused_I_O;\
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;

