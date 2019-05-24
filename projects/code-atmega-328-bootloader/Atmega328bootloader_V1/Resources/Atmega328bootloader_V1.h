

void Program_Flash (void);
void Verify_Flash (void);

void new_record(void); 
void start_new_code_block(void);
void Program_record(void);
void copy_cmd_to_page_buffer(void);
void get_next_hex_cmd(void);
void write_page_SUB(int);
char Lock_byte(void);
int readCMD(int);
void Clear_flash (void);

void USART_init (unsigned char, unsigned char);
void sendString(char*);
char isCharavailable (int);
char receiveChar(void);
void sendChar(char);
char waitforkeypress(void);
void newline(void);
void timer_T0_sub(char, unsigned char);
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



/***************************************************************************/
int  cmd_counter;											//Counts commands as they are downloaded from the PC
int prog_counter;											//Counts commands burned to flash
signed int  read_ops=0;										//Total number of commands read from flash
volatile int counter;										//Counts characters in a record as they are downloded from the PC
volatile int char_count;									//The number of askii character in a single record
volatile char Count_down;									//Counts commands as record is programmed
volatile int   tempInt1, tempInt2;							//Used to assemble commands and addresses as the are downloaded
int store[18];												//Used to store commands and address ready for the programmer
volatile char w_pointer,r_pointer;							//Read/write pointers to "store" to which hex file is saved
unsigned int Hex_cmd;										//Command read from flash during verification

int Hex_address;											//Address read from the hex file
int HW_address;												//Hard ware address (usually tracks Hex_address)
signed int page_address;									//Address of first location on a page of flash 
volatile int write_address;									//Address on page_buffer to which next command will be written
signed int FlashSZ;											//Amount of flash memory supplied on target device
signed int PAmask;											//Used to obtain the flash page address from the hex address
signed int PageSZ;											//Size of a page of flash
int AppSZ;													//Space available for the application (ox7000 bytes)

signed char short_record;									//Record  containing less that eight 16 bit commands
signed char page_offset;									//Address of first location on page buffer to be used
signed char space_on_page;									//Keeps a track of the space remaining on a page buffer
char Flash_flag;											//Indicates that the page buffer contains commands

signed char record_length;									//Num commands one one line of hex file (i.e. on one record)
signed char record_length_old;								//If record length changes, length of the previous one is important
signed char orphan;											//Indicates that the contents of a record span two flash pages
signed char section_break;									//Set to 1 if at least one page of flash memory will be unused.
signed char page_break;										//Page only partialy filled before programming next one starts
volatile signed char line_offset;							//LSB of address of first command in record (usually zero)
unsigned int prog_led_control;								//Used to control Leds as hex file is downloaded

char print_mode;											//Print out hex file if required


volatile char loc_in_mem_H, loc_in_mem_L;					//Used to pass addresses in SRAM to assembly routines (i.e. location of programming data)
int address_in_flash;										//Address in flash at which programming data is to be stored (for example)
volatile char Prog_mem_address_H, Prog_mem_address_L;		//Used to pass addresses in flash to the assembly subroutines
volatile char Flash_readout;								//Used by assembly routines to return data



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
r_pointer = r_pointer & 0b00001111;



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

