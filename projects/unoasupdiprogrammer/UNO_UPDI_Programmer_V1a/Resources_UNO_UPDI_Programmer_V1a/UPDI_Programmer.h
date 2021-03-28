
#define Std_record_length			0x10
#define half_SRL					0x08


//All parameters that may change from device to device: Set for Attiny 1606

signed int PageSZ = 0x20;									//Size of a page of flash in 16 bit words
signed int PAmask = 0x1FE0;									//Used to obtain the flash page address from the hex address
volatile char record_type, record_type_old;
unsigned char block_SZ = 64;								//page size in bytes
unsigned int flash_start = 0x8000;							//bytes

/**************Values for ATtiny1606**************************************************/
unsigned int FlashSZ;										//Amount of flash memory in 16 bit words supplied on target device
unsigned int text_size;										//Size of data partition in words



#define set_flash_sz \
switch (device_type){\
case 806: FlashSZ = 0x1000; break;\
case 1606: FlashSZ = 0x2000; break;}

#define data_blocks \
((FlashSZ*2) - (text_size * 2))/256


/*
Flash can be partitioned into 256 (0x100) byte blocks
THe total number of blocks Num_blocks = (FlashSZ * 2)/0x100 
APPEND the end of the application section = (Num_blocks - text_size * 2)/0x100 

*/



//UPDI addresses and commands (see data sheet for more info)

#define NVMCTRL_CTRLA  				0x1000
#define Signature_base_address		0x1100
#define NVMCTRL_ADDR_reg 			0x1008
#define NVMCTRL_DATA_reg			0x1006
#define FUSE_base_address			0x1280

#define ASI_KEY_STATUS_reg			0x07
#define ASI_Reset_Request_reg		0x08
#define ASI_SYS_STATUS_reg			0x0B

#define UPDI_CTRLB					0x03

#define Reset_signature				0x59

#define WP_cmd 						0x01
#define send64bitKey_cmd 			0xE0
#define download_SIB				0xE5
#define cmd_update_fuse				0x07

#define LCDS						0x80
#define STCS						0xC0

#define LDS_from					0x00
#define word_address				0x04

#define STS							0x40
#define byte_data					0x04
#define int_data					0x05


#define ST							0x60
#define byte_pointer				0x08
#define word_pointer				0x09
#define inc_byte_ptr				0x04
#define inc_word_ptr				0x05
#define setup_repeat_op				0xA0


#define WDTCFG  	0x1280
#define BODCFG  	0x1281
#define	OSCCFG  	0x1282
#define TCD0CFG 	0x1284
#define SYSCFG0		0x1285
#define SYSCFG1  	0x1286
#define APPEND  	0x1287
#define BOOTEND  	0x1288

#define LOCKBIT_LOCKBIT  0x128A



/*************************************************************************************************************/

#define delay_of_0_25uS             asm("nop"); asm("nop"); asm("nop"); asm("nop");
#define delay_of_0_75uS             delay_of_0_25uS; delay_of_0_25uS; delay_of_0_25uS;
#define out_h                       PORTC |= (1 << PORTC0)
#define out_l                       PORTC &= (~(1 << PORTC0))


#define clock_delay_T               Timer_T0_sub(T0_delay_18us) 
#define clock_delay_R               Timer_T0_sub(T0_delay_18us) 


#define input_h                     (PINC & (1 << PINC0))
#define input_l                     (!(PINC & (1 << PINC0)))






/**********************************************************************************************************************/
void upload_hex(void);
void Program_Flash_Hex (void);
void Program_Flash_Text (void);
void Program_end_of_section(void);
void Verify_Flash_Hex (void);
unsigned int read_flash(int);


void Erase_code (void);
char set_up_NVM_prog(void);
void read_out_signature_bytes(void);
void write_fuse(int, unsigned char);
void read_out_fuses(void);
char txt2bin(char A, char B);
void inititalise_UPDI_cmd_write(int);
void UPDI_cmd_to_page_buffer(void);
void Write_page_to_NVM(int);


void UART_Tx(unsigned int);
unsigned char UART_Rx(void);



const char *Key_chip_erase = "0x4E564D4572617365";
const char *Key_NVM_prog = "0x4E564D50726F6720";

unsigned char SIB_byte[24], Rx_Byte;
int test = 0;
volatile char UPDI_timeout = 0;
volatile unsigned char SREG_BKP;
volatile int P_counter;

unsigned char data_byte_Tx, data_byte_Rx;
unsigned char Rx_bit, Tx_bit;
unsigned char parity_2;


/*************************************************************************/


#define T0_delay_18us	2, 220
#define T0_delay_200us 	3, 206
#define T0_delay_400us 	3, 156
#define T0_delay_2ms 	4,131
#define T0_delay_5ms 	5,178
#define T0_delay_10ms 	5,100


#define T1_delay_25ms 	2,15536
#define T1_delay_100ms 	4,58982
#define T1_delay_500ms 	4,32768
#define T1_delay_1Sec 	4,0


#define T2_delay_100us	3, 206
#define T2_delay_250us	4, 194
#define T2_delay_500us	4, 131
#define T2_delay_750us	4, 69

#define T2_delay_1ms 	4,6
#define T2_delay_3ms 	6,68
#define T2_delay_5ms 	7,178
#define T2_delay_10ms	7,100

	
unsigned int  cmd_counter;									//Counts commands as they are downloaded from the PC
volatile unsigned int prog_counter;							//Counts commands burned to flash
unsigned int  read_ops=0;									//Total number of commands read from flash
volatile int counter;										//Counts characters in a record as they are downloded from the PC
volatile int char_count;									//The number of askii character in a single record
volatile unsigned char Count_down;							//Counts commands as record is programmed
volatile int   tempInt1, tempInt2;							//Used to assemble commands and addresses as the are downloaded
int store[64];												//Used to store commands and address ready for the programmer
volatile unsigned char w_pointer,r_pointer;					//Read/write pointers to "store" to which hex file is saved
unsigned int Hex_cmd;										//Command read from flash during verification

unsigned int Hex_address;									//Address read from the hex file
unsigned int HW_address;									//Hard ware address (usually tracks Hex_address)
unsigned int page_address;									//Address of first location on a page of flash 
volatile unsigned int write_address;						//Address on page_buffer to which next command will be written

signed char short_record;									//Record  containing less that eight 16 bit commands
unsigned char page_offset;									//Address of first location on page buffer to be used
unsigned char space_on_page;									//Keeps a track of the space remaining on a page buffer
unsigned char Flash_flag;									//Indicates that the page buffer contains commands

signed char record_length;									//Num commands one one line of hex file (i.e. on one record)
volatile signed char record_length_old;								//If record length changes, length of the previous one is important
volatile signed char odd_line_length;
volatile unsigned int add_last_cmd;
signed char orphan;											//Indicates that the contents of a record span two flash pages
volatile signed char line_offset;							//LSB of address of first command in record (usually zero)

volatile char endoftext, text_started;
volatile char Rx_askii_char_old;                             //Required to check for a \r\n string
int address_in_flash;                                       //Address in flash at which text data is to be programmed
unsigned char  op_code;

char User_response;
volatile char T1OVFM = 0;


/************************************************************************************************************************************/
#define setup_328_HW \
\
setup_watchdog;\
ADMUX |= (1 << REFS0);\
initialise_IO;\
\
USART_init(0,68);\
set_flash_sz;\
text_size = (text_bytes/2);



/************************************************************************************************************************************/
#define wdr()  __asm__ __volatile__("wdr")


#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;


#define SW_reset {wdt_enable(WDTO_30MS);while(1);}



/************************************************************************************************************************************/
#define Initialise_variables_for_programming_flash \
prog_counter=0;   cmd_counter = 0; record_length_old = 0x10;\
Flash_flag = 0;  HW_address = 0;   orphan = 0;\
w_pointer = 0; r_pointer = 0; short_record=0;\
counter = 0;




/*************************Atmega programming commands (see data sheet)******************************/


#define inc_w_pointer \
w_pointer++;\
w_pointer = w_pointer & 0x3F;


#define inc_r_pointer \
r_pointer++;\
r_pointer = r_pointer & 0b00111111;





/************************************************************************************************************************************/
#define initialise_IO \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFE;\
PORTD = 0xFF;

//PORTC0 stays as Hi Z input




/************************************************************************************************************************************/
#define User_prompt \
while(1){\
do{sendString("R?    ");}	 while((isCharavailable(250) == 0));\
User_response = receiveChar();\
if((User_response == 'R') || (User_response == 'r'))break;} sendString("\r\n");




#define UPDI_reset \
Timer_T0_sub(T0_delay_200us);\
UART_Tx(0x55);\
UART_Tx(STCS | ASI_Reset_Request_reg);\
UART_Tx(Reset_signature);\
Timer_T0_sub(T0_delay_200us);\
UART_Tx(0x55);\
UART_Tx(STCS | ASI_Reset_Request_reg);\
UART_Tx(0x0);\
Timer_T0_sub(T0_delay_200us);




/************************************************************************************************************************************/
#define contact_target \
if(PINC & (1 << PINC0))\
DDRC |= (1 << DDC0);\
else {sendString ("Device not detected\r\n");\
while(1);}\
delay_of_0_25uS;\
DDRC &= (~(1 << DDC0));\
while(!(PINC & (1 << PINC0)));\
Timer_T0_sub(T0_delay_400us);


/************************************************************************************************************************************/
#define configure_updi \
UART_Tx(0x55);\
UART_Tx (STCS | 0x09);\
UART_Tx (0xFD);\
Timer_T0_sub(T0_delay_200us);\
UART_Tx(0x55);\
UART_Tx (STCS | 0x02);\
UART_Tx (0x46);\
Timer_T0_sub(T0_delay_200us);\


/************************************************************************************************************************************/
#define print_out_SIB \
\
UART_Tx(0x55);\
UART_Tx (download_SIB);\
while ((PINC & (1 << PINC0)) && (!(UPDI_timeout)));\
if(!(UPDI_timeout)){\
SIB_byte[0] = UART_Rx();\
for(int m = 1; m <= 15; m++){\
SIB_byte[m] = UART_Rx(); }\
\
for(int m = 0; m <= 15;m++){\
sendChar(SIB_byte[m]);}  }  




/************************************************************************************************************************************/

#define initialise_NVM_programming \
sendString("\r\nInitialising NVM programming");\
sendString("\r\nErase chip? -y- or AOK");\
if (waitforkeypress() == 'y'){sendString ("\tErasing chip\r\n");\
Erase_code();} else sendString ("\tChip not erased\r\n");\
newline();\
if (set_up_NVM_prog())\
sendString("\r\nSignature byte readout\t\t");\
else {sendString("\r\nPOR to proceed!"); while(1);}\
read_out_signature_bytes();\
newline();



/************************************************************************************************************************************/
#define Dissable_UPDI_sesion \
UART_Tx(0x55);\
UART_Tx(STCS | UPDI_CTRLB);\
UART_Tx(0xE7);



