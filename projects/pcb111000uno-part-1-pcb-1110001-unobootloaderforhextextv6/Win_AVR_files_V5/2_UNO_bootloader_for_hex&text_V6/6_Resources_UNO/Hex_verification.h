

void Verify_Flash (void);


/*********Subroutines provided by Hex_verification_SW_subs.c file**************/
void newline(void);
void Timer_T0_sub(char, unsigned char);
void Timer_T1_sub(char, unsigned int);
void Hex_to_PC(int);
void short_num_to_PC(char);
void binUnwantedChars (void);
int readCMD(int);
int askiX2_to_hex (char*);
char non_numeric_char (char);
void sendCharasASKI(char, unsigned char);
void my_chartoa(char, unsigned char, char*);


/*********Subroutines provided by Basic_IO_and_timer_subs.c file**************/
void USART_init (unsigned char, unsigned char);
void sendString(char*);
char isCharavailable (int);
char receiveChar(void);
void sendChar(char);
char waitforkeypress(void);




/************************Subroutines provided by .S file*******************/
extern void read_flash (void);
extern void clear_read_block(void);
extern void read_config_bytes(void);



/***************************************************************************/
int  cmd_counter;											//Counts commands as they are downloaded from the PC
int prog_counter;											//Counts commands burned to flash
signed int  read_ops=0;										//Total number of commands read from flash
unsigned int Hex_cmd;										//Command read from flash during verification
signed int FlashSZ;											//Amount of flash memory supplied on target device


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
#define T0_delay_5ms 5,220
#define T0_delay_20ms 5,100
#define T1_delay_500mS 5,0xF0BD
#define twenty_msec_delay;	Timer_T0_sub(T0_delay_20ms);


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
#define  activity_leds;\
DDRB |= (1 << DDB0) | (1 << DDB1);\
LED_2_off;\
LED_2_off;



/**********************************************************************************/
#define LED_2_off; PORTB &= (~(1 << PB0));
#define LED_2_on; PORTB |= (1 << PB0);

#define LED_1_off; PORTB &= (~(1 << PB1));
#define LED_1_on; PORTB |= (1 << PB1);


#define Unused_I_O;\
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;



/************************************************************************************************************************************/
#define send_as_askii;\
if((((Hex_cmd>>8)<0x20)||((Hex_cmd>>8)>0x7E))&&(((Hex_cmd & 0x00FF)<0x20)||((Hex_cmd & 0x00FF)>0x7E)))Hex_to_PC(Hex_cmd);\
else{if(((Hex_cmd>>8)>=0x20) && ((Hex_cmd>>8)<=0x7E) && ((Hex_cmd & 0x00FF)>=0x20) && ((Hex_cmd & 0x00FF)<=0x7E))\
{sendChar(Hex_cmd>>8); sendChar(Hex_cmd & 0x00FF);}\
else { if(((Hex_cmd>>8)>=0x20) && ((Hex_cmd>>8)<=0x7E)){sendChar(Hex_cmd>>8); sendCharasASKI(16, (Hex_cmd & 0x00FF));}\
if(((Hex_cmd & 0x00FF)>=0x20) && ((Hex_cmd & 0x00FF)<=0x7E)){sendCharasASKI(16, (Hex_cmd>>8)); sendChar(Hex_cmd & 0x00FF);}}}

