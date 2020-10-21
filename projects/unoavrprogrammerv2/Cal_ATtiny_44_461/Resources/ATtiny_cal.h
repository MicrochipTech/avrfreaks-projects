

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>


/***********************************************************************/
#define T0_delay_5ms 5,220
#define T1_delay_4sec 5,0x85F7
#define T1_delay_500ms 5,0xF0BE
#define T1_delay_250ms 5,0xF85F
#define T1_delay_50ms 3, 0xCF31
#define T1_delay_100ms 3, 0x9E62


/***********************************************************************/
unsigned char ReverseByte (unsigned char);
void Initialise_USI_Tx (void);
void Char_to_USI(unsigned char);
unsigned char Char_from_USI (char);
void String_to_USI(const char*);
void NumericString_to_PC(unsigned char*);
void SBtoAL(unsigned char*, long, char);
void Num_to_PC(char, long);

long compute_error(char, char);

void Timer_T0_sub(char, unsigned char);
void Initialise_T0(void);

void save_cal_values(unsigned char );
void printout_cal_values(void);


/***********************************************************************/
unsigned char Txdata;
volatile long error_sum;
volatile char char_transmitted;
volatile char char_received;
volatile long TCNT1_sum;
volatile char int_counter;

int sig_byte_2, sig_byte_3;
int EE_size;
unsigned int FlashSZ;

unsigned char OSCCAL_DV;

//unsigned char Rx_clock = 60;						//used to step through OCR0A values to determine optimum settings
//unsigned char Tx_clock = 90;


/***********************************************************************/

//Values for baud rates other than 38400 included for reference (For use by application only not the programmer)

///76800: Theoretical value is 105	
#define Tx_clock_76800						78
#define Rx_clock_76800						74
#define Start_clock_76800					TCCR0B = (1 << CS00);


///57600: Theoretical value is 139
#define Tx_clock_57600						112
#define Rx_clock_57600						110
#define Start_clock_57600					TCCR0B = (1 << CS00);


///38400: Theoretical value is 208	
#define Tx_clock_38400						185	
#define Rx_clock_38400						185	
#define Start_clock_38400					TCCR0B = (1 << CS00);


///19200: Theoretical value is 416 (52 x 8)	
#define Tx_clock_19200						48	
#define Rx_clock_19200						48	
#define Start_clock_19200					TCCR0B = (1 << CS01);


///9600: Theoretical value is 833 (104 X 8)		
#define Tx_clock_9600						100	
#define Rx_clock_9600						100
#define Start_clock_9600					TCCR0B = (1 << CS01);




/***********************************************************************/
#define setup_Attiny_HW \
setup_watchdog;\
setup_IO;\
set_device_type_and_memory_size;\
Initialise_USI_Tx ();\
OCR1A = 0xFF;



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
PORTB = 0xFF;


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




/***************************************************************************************************************/
#define WPU_on_DI_pin \
DDR_USI &= (!(1 << DI_pin));\
PORT_USI |= 1 << DI_pin;

#define Configure_DO_pin_as_Output \
DDR_USI |= (1 <<DO_pin);


#define set_USI_ports_to_WPU \
DDR_USI &= (~((1 <<USCK_pin) | (1 << DO_pin) | (1 << DI_pin)));\
PORT_USI |= (1 <<USCK_pin) | (1 << DO_pin) | (1 <<DI_pin);

#define Transmitter_active \
DDR_USI & (1 << DO_pin)

#define DI_pin_low \
!(PIN_USI & (1 << DI_pin))

#define Disable_PCI_on_DI_pin \
GIMSK &= (~(1 << PCIE0));



