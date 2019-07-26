

void Program_Flash (void);
void Verify_Flash (void);
void print_string_num(int, int);
char string_counter(int);


/************************Basic_IO_and_timer_subroutines*******************/
void USART_init (unsigned char, unsigned char);
void sendString(char*);
char isCharavailable (int);
char receiveChar(void);
void sendChar(char);
char waitforkeypress(void);
void newline(void);
void timer_T0_sub(char, unsigned char);
void Timer_T0_sub_with_interrupt(char, unsigned char);
void timer_T1_sub(char , unsigned int );

void Hex_to_PC(int);
void short_num_to_PC(char);



/************************Subroutines provided by .S file*******************/
extern void read_flash (void);
extern void clear_read_block(void);
extern void Page_erase (void);
extern void write_to_page_buffer(void);
extern void page_write(void);
extern void read_config_bytes(void);
extern void set_lock_byte (void);


int store[32];												//Used to store text received via UART for use by the programmer
volatile int w_pointer,r_pointer;							//Read/write pointers to locations in "store" to which characters are saved
volatile int counter;										//Counts characters as they are downloded from the PC


int address_in_sram;										//addresses in SRAM at which text data is stored
volatile char loc_in_mem_H, loc_in_mem_L;					//Used to pass addresses in SRAM to assembly routines

int address_in_flash;										//Address in flash at which text data is to be programmed
volatile char Prog_mem_address_H, Prog_mem_address_L;		//Used to pass addresses in flash to the assembly subroutines

volatile char Flash_readout;								//Used by assembly read routine to return data



/************************Subroutines provided by .S file*******************/
extern void read_flash (void);
extern void clear_read_block(void);							//Required afer programming before flash can be read
extern void Page_erase (void);
extern void write_to_page_buffer(void);
extern void page_write(void);
extern void read_config_bytes(void);
extern void set_lock_byte (void);





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
activity_leds;\
ADMUX |= (1 << REFS0);\
USART_init(0,16);

/**********************************************************************************/
#define config_WDT;\
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;


/**********************************************************************************/
#define cal_device;\
eeprom_write_byte((uint8_t*)0x3FD, OSCCAL);\
if ((eeprom_read_byte((uint8_t*)0x3FE) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x3FE) < 0xF0) && (eeprom_read_byte((uint8_t*)0x3FE)\
== eeprom_read_byte((uint8_t*)0x3FF))) OSCCAL = eeprom_read_byte((uint8_t*)0x3FE);


/**********************************************************************************/
#define  activity_leds;\
DDRB |= (1 << DDB0);\
LED_2_off;



/**********************************************************************************/
#define inc_r_pointer;\
r_pointer++;\
r_pointer = r_pointer & 0x1F;


#define inc_w_pointer;\
w_pointer++;\
w_pointer = w_pointer & 0x1F;


/**********************************************************************************/
#define LED_2_off; PORTB &= (~(1 << PB0));
#define LED_2_on; PORTB |= (1 << PB0);


#define Unused_I_O;\
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;

