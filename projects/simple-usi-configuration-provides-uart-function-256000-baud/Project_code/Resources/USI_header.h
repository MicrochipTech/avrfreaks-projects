



/********************************************************************************************************************/
char Char_from_flash(int);
void Determine_device_type(void);
void set_up_target_parameters(void);
unsigned char string_counter(int);
void print_string_num(int, int);

unsigned char ReverseByte (unsigned char);
void Initialise_USI_Tx (void);
void Char_to_USI(unsigned char);							//Transmit single characters
unsigned char Char_from_USI (char);							//Receive single characters as typed in at keyboard						
void String_from_USI (unsigned char*);						//Receive strings (were there is no pause between characters)
void newline(void);
void String_to_USI(const char*);							//The equivalent of sendString
void Echo_string(unsigned char*);							//Echo string received from keyboard
char wait_for_return_key(void);
char isCharavailable (int);

int Num_from_KBD(void);
char decimal_digit (char);
void Flash_String_to_USI(const char*);



/**********************************************************************************************************************/
volatile char Prog_mem_address_H, Prog_mem_address_L;
volatile char Flash_readout;
int char_counter;

int EEP_MAX = 0x2000;
int sig_byte_2, sig_byte_3;
int EE_size;
unsigned int FlashSZ;
char User_response;
unsigned char OSCCAL_DV;

unsigned char line_length = 0;
char next_char;


/**********************************************************************************************************************/
//Theoretical clock value = 8,000,000/Baud rate

//256000: Theoretical value is 31			(Always use empirical values)
#define Tx_clock_256000						33
#define Rx_clock_256000						29
#define Start_clock_256000					TCCR0B = (1 << CS00);
#define Half_Rx_clock_256000				15


//144000: Theoretical value is 55			(Theoretical values always OK)
#define Tx_clock_144000						55			//58 (empirical value)
#define Rx_clock_144000						55			//53
#define Start_clock_144000					TCCR0B = (1 << CS00);
#define Half_Rx_clock_144000				27			//26


//128000: Theoretical value is 62			(Theoretical values always OK)
#define Tx_clock_128000						62		//64
#define Rx_clock_128000						62		//62
#define Start_clock_128000					TCCR0B = (1 << CS00);
#define Half_Rx_clock_128000				31		//31


//115200: Theoretical value is 69			(Empirical Rx values needed for strings from keyboard)
#define Tx_clock_115200						69		//68
#define Rx_clock_115200						66
#define Start_clock_115200					TCCR0B = (1 << CS00);
#define Half_Rx_clock_115200				33


//76800: Theoretical value is 105			(Theoretical values always OK)
#define Tx_clock_76800						105			//106
#define Rx_clock_76800						105			//102
#define Start_clock_76800					TCCR0B = (1 << CS00);
#define Half_Rx_clock_76800					52			//51


//57600: Theoretical value is 139			(Empirical RX values needed for strings from keyboard)
#define Tx_clock_57600						139			//138
#define Rx_clock_57600						134			//134
#define Start_clock_57600					TCCR0B = (1 << CS00);
#define Half_Rx_clock_57600					67			//67


//38400: Theoretical value is 208			(Theoretical values always OK)
#define Tx_clock_38400						208		//210
#define Rx_clock_38400						208		//206
#define Start_clock_38400					TCCR0B = (1 << CS00);
#define Half_Rx_clock_38400					104		//103

//19200: Theoretical value is  416 (52 x 8)	(Theoretical values always OK)
#define Tx_clock_19200						52			//52
#define Rx_clock_19200						52			//50
#define Start_clock_19200					TCCR0B = (1 << CS01);
#define Half_Rx_clock_19200					26			//25

//9600: Theoretical value is 833 (104 X 8)	(Theoretical values always OK)
#define Tx_clock_9600						104		//106
#define Rx_clock_9600						104		//102
#define Start_clock_9600					TCCR0B = (1 << CS01);
#define Half_Rx_clock_9600					52		//51





/***********************************************************************/

#define setup_ATtiny_HW \
setup_watchdog;\
setup_IO;\
set_device_type_and_memory_size;\
Initialise_USI_Tx ();\
cal_device;



/***********************************************************************/
#define wdr()  __asm__ __volatile__("wdr")

#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define setup_IO_44 \
MCUCR &= (~(1 << PUD));\
DDRA = 0;\
PORTA = 0xFF;\
DDRB = 0;\
PORTB = 0x0F;

#define setup_IO_461 \
MCUCR &= (~(1 << PUD));\
DDRA = 0;\
PORTA = 0xFF;\
DDRB = 0;\
PORTB = 0xFF;\
USIPP |= 1 << USIPOS;


#define cal_device \
eeprom_write_byte((uint8_t*)(EE_size - 3), OSCCAL);\
if ((eeprom_read_byte((uint8_t*)(EE_size - 2)) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)(EE_size - 2)) < 0xF0) && (eeprom_read_byte((uint8_t*)(EE_size - 2))\
== eeprom_read_byte((uint8_t*)(EE_size - 1)))) OSCCAL = eeprom_read_byte((uint8_t*)(EE_size - 2));


/**********************************************************************************/
#define User_prompt \
while(1){\
	do{String_to_USI("R?    ");}	 while(!(User_response = isCharavailable(125)));\
	\
if((User_response == 'R') || (User_response == 'r'))break;} String_to_USI("\r\n");




/*****************************************************************************/
const char *Device_type[8];
int device_ptr;



/*****************************************************************************/
#define Set_device_signatures \
Device_type[0] = "24A";\
Device_type[1] = "44A";\
Device_type[2] = "84A";\
Device_type[3] = "261A";\
Device_type[4] = "461A";\
Device_type[5] = "861A";\
Device_type[6] = "26/L";\
Device_type[7] = "? Unknown device! Op halted.";


/***************************************************************************************************************/
#define set_device_type_and_memory_size \
Set_device_signatures;\
sig_byte_2 = eeprom_read_byte((uint8_t*)(EEP_MAX - 4));\
sig_byte_3 = eeprom_read_byte((uint8_t*)(EEP_MAX - 5));\
\
switch(sig_byte_2){\
	\
	case 0x91: FlashSZ = 0x400;  EE_size = 0x80;\
	switch (sig_byte_3)\
	{case 0x09: device_ptr = 6; break;\
		case 0x0B: device_ptr = 0; break;\
		case 0x0C: device_ptr = 3; break;\
	default: device_ptr = 7; break;}\
	break;\
	\
	case 0x92: FlashSZ = 0x800;  EE_size = 0x100;\
	switch (sig_byte_3)\
	{case 0x07: device_ptr = 1; break;\
		case 0x08: device_ptr = 4; break;\
	default: device_ptr = 7; break;	}\
	break;\
	\
	case 0x93: FlashSZ = 0x1000;  EE_size = 0x200;\
	switch (sig_byte_3)\
	{case 0x0C: device_ptr = 2; break;\
		case 0x0D: device_ptr = 5; break;\
	default: device_ptr = 7; break;	}\
	break;\
	\
default:  device_ptr = 7;	break;}

