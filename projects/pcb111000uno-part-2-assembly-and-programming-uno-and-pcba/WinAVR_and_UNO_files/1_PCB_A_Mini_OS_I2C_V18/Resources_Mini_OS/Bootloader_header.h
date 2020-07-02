
void Program_Flash (void);
void Clock_period(void);
void Clock_period(void){for(int p = 0; p<= 3; p++){asm("nop");}}

#define PGClock_L  
#define PGClock_H  Clock_period();


#define delay_30ms 5,20
#define delay_20ms 5,100
#define delay_10ms 5,183
#define delay_5ms 5,220
#define delay_2ms 4,195

#define two_msec_delay;				timer_T0_sub_x_M(delay_2ms,1);
#define five_msec_delay;			timer_T0_sub_x_M(delay_5ms,1);
#define ten_msec_delay;				timer_T0_sub_x_M(delay_10ms,1);
#define twenty_msec_delay;			timer_T0_sub_x_M(delay_20ms,1);
#define thirty_msec_delay;			timer_T0_sub_x_M(delay_30ms,1);
#define three_hundred_msec_delay;	timer_T0_sub_x_M(delay_30ms,10);
#define one_second_delay;			timer_T0_sub_x_M(delay_10ms,100);


void USART_Rx_init (unsigned char, unsigned char);
void sendString(char*);
char isCharavailable (void);
char receiveChar(void);
void sendChar(char);
void newline(void);
void Read_on_chip_EEPROM(int);
void new_record(void);
void start_new_code_block(void);
void Program_record(void);
void copy_cmd_to_page_buffer(void);
void get_next_hex_cmd(void);
void write_page_SUB(signed int);

char waitforkeypress(void);


char Atmel_config ( unsigned int, unsigned int );
char Read_write_mem(char, int, char);
void Load_page(char, int, unsigned char);


void timer_T0_sub_x_M(char, char, char);


#define inc_r_pointer;\
r_pointer++;\
r_pointer = r_pointer & 0b00011111;

int  cmd_counter;											//Counts commands as they are downloaded from the PC
int prog_counter;											//Counts commands burned to flash
volatile int counter;										//Counts characters in a record as they are downloded from the PC
volatile int char_count;									//The number of askii character in a single record
volatile char Count_down;									//Counts commands as record is programmed
volatile int   tempInt1, tempInt2;							//Used to assemble commands and addresses as the are downloaded
int store[36];												//Used to store commands and address ready for the programmer
volatile char w_pointer,r_pointer;							//Read/write pointers to "store" to which hex file is saved
unsigned int Hex_cmd;										//Command read from flash during verification


unsigned char cmd_pin, resp_pin, clock_pin, reset_pin;		//Used to define the programming pins


int Hex_address;											//Address read from the hex file
int HW_address;												//Hard ware address (usually tracks Hex_address)
signed int page_address;									//Address of first location on a page of flash 
volatile int write_address;									//Address on page_buffer to which next command will be written
//signed int FlashSZ;											//Amount of flash memory supplied on target device
signed int PAmask;											//Used to obtain the flash page address from the hex address
signed int PageSZ;											//Size of a page of flash

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


volatile char Prog_mem_address_H, Prog_mem_address_L;		//Used to pass addresses in flash to the assembly subroutines
volatile char Flash_readout;




#define Prog_enable 0xAC530000
#define Prog_enable_h 0xAC53

#define signature_bit_1 0x30000000
#define signature_bit_1_h 0x3000


#define write_lock_bits 0xACE00000
#define write_lock_bits_h 0xACE0

#define write_fuse_bits_H 0xACA80000
#define write_fuse_bits_H_h 0xACA8

#define write_fuse_bits 0xACA00000
#define write_fuse_bits_h 0xACA0

#define write_extended_fuse_bits 0xACA40000
#define write_extended_fuse_bits_h 0xACA4

#define Chip_erase 0xAC800000
#define Chip_erase_h 0xAC80


/********HW V 1.3 Define target Pin & CC LED definitions START for boot loader**********/
#define PGD_cmd_H PORTB |= cmd_pin
#define PGD_cmd_L PORTB &= ~(cmd_pin)
#define PGD_resp_H PINB & resp_pin
#define PGC_L PORTB &= ~(clock_pin)
#define PGC_H PORTB |= clock_pin
#define Reset_L PORTC &= ~(reset_pin)
#define Reset_H PORTC |= reset_pin

#define boot_target; cmd_pin =  0x08; resp_pin = 0x10; clock_pin =  0x20; reset_pin = 0x08; DDRB |= 0x28; DDRC |= 0x08;


#define Atmel_powerup;\
{two_msec_delay;}\
Reset_L;\
PGC_L;\
{two_msec_delay;}\
Reset_H;\
{two_msec_delay;}\
Reset_L;\
{twenty_msec_delay;}


#define Atmel_powerup_and_target_detect;\
Atmel_powerup;\
if((Atmel_config(Prog_enable_h, 0)==0x53) && (Atmel_config(signature_bit_1_h, 0) == 0x1E))target_detected=1;\
else target_detected=0;



#define Initialise_variables_for_programming_flash;\
prog_counter=0; prog_led_control = 0; cmd_counter = 0; record_length_old=0;\
Flash_flag = 0;  HW_address = 0;  section_break = 0; orphan = 0;\
w_pointer = 0; r_pointer = 0;short_record=0;\
counter = 1;

#define set_up_WDT;\
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;


#define initialise_IO;\
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;\

#define UNO_reset_cntrl_low_CC \
PORTB &= (~(1 << PB2));\
DDRB |= (1 << DDB2);

#define UNO_reset_cntrl_low_CA \
PORTB &= (~(1 << PB1));\
DDRB |= (1 << DDB1);

#define Reset_UNO_low \
PORTC &= (~(1 << PC3));\
DDRC |= (1 << DDC3);

#define Reset_UNO_high \
DDRC &= (~(1 << DDC3));\
PORTC |= (1 << PC3);





#define initialise_leds;\
DDRD |= (1 << PD7);


#define led_off;\
PORTD &= (~(1 << PD7));


#define led_on;\
PORTD |= (1 << PD7);

#define cal_device;\
eeprom_write_byte((uint8_t*)0x3FD,OSCCAL);\
if ((eeprom_read_byte((uint8_t*)0x3FE) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x3FE) < 0xF0)\
 && (eeprom_read_byte((uint8_t*)0x3FE)\
== eeprom_read_byte((uint8_t*)0x3FF)))\
{OSCCAL = eeprom_read_byte((uint8_t*)0x3FE);}
