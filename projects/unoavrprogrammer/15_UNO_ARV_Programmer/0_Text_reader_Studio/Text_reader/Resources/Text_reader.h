
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>



/***************************************************************************/
char Char_from_flash(int);
void Determine_device_type(void);
void set_up_target_parameters(void);
unsigned char string_counter(int);
void print_string_num(int, int);


/***************************************************************************/
volatile char Prog_mem_address_H, Prog_mem_address_L;
volatile char Flash_readout;
int char_counter;
int FlashSZ;
int EE_size;
char User_response;
int EEP_MAX = 0x2000;
int sig_byte_2, sig_byte_3;



/**********************************************************************************/
#define setup_HW \
setup_watchdog;\
ADMUX |= (1 << REFS0);\
Initialise_IO;\
set_device_type_and_memory_size;\
cal_device;\
\
switch(sig_byte_2){\
	case 0x95: if(sig_byte_3 != 2)USART_init(0,25);\
	else USART_init_32(0,25); break;\
default: USART_init(0,25); break;}



/**********************************************************************************/
#define wdr()  __asm__ __volatile__("wdr")


#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;


/**********************************************************************************/
#define Initialise_IO \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;





/**********************************************************************************/
#define cal_device \
eeprom_write_byte((uint8_t*)(EE_size - 3), OSCCAL);\
if ((eeprom_read_byte((uint8_t*)(EE_size - 2)) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)(EE_size - 2)) < 0xF0) && (eeprom_read_byte((uint8_t*)(EE_size - 2))\
== eeprom_read_byte((uint8_t*)(EE_size - 1)))) OSCCAL = eeprom_read_byte((uint8_t*)(EE_size - 2));





/**********************************************************************************/
#define User_prompt \
while(1){\
	do{sendString("R?    ");}	 while((isCharavailable(250) == 0));\
	User_response = receiveChar();\
if((User_response == 'R') || (User_response == 'r'))break;} sendString("\r\n");






/*****************************************************************************/
const char *Device_type[8], *Device_family[2];
int device_ptr, family_ptr;

/*****************************************************************************/
#define Set_device_signatures \
Device_type[0] = "48/P";\
Device_type[1] = "88/P";\
Device_type[2] = "168/P";\
Device_type[3] = "328/P";\
Device_type[4] = "644";\
Device_type[5] = "32A";\
Device_type[6] = "44A";\
\
\
Device_family[0] = "Atmega ";\
Device_family[1] = "ATtiny ";

/*****************************************************************************/
#define set_device_type_and_memory_size \
Set_device_signatures;\
sig_byte_2 = eeprom_read_byte((uint8_t*)(EEP_MAX - 4));\
sig_byte_3 = eeprom_read_byte((uint8_t*)(EEP_MAX - 5));\
\
switch(sig_byte_2){\
	\
	case 0x92: FlashSZ = 0x800;  EE_size = 0x100;\
		switch (sig_byte_3)\
			{case 0x05: \
			case 0x0A: device_ptr = 0; family_ptr = 0; break;\
			case 0x07: device_ptr = 6; family_ptr = 1; break;}\
		break;\
		\
	case 0x93: FlashSZ = 0x1000; EE_size = 0x200;\
		switch (sig_byte_3)\
			{case 0x0A:\
			case 0x0F: device_ptr = 1; family_ptr = 0; break;}\
		break;\
		\
	case 0x94: FlashSZ = 0x2000; EE_size = 0x200;\
		switch (sig_byte_3)\
			{case 0x06: \
			case 0x0B: device_ptr = 2; family_ptr = 0; break;}\
		break;\
	\
	case 0x95: FlashSZ = 0x4000; EE_size = 0x400;\
		switch (sig_byte_3)\
			{case 0x14:\
			case 0x0F: device_ptr = 3; family_ptr = 0; break;\
			case 0x02: device_ptr = 5; family_ptr = 0; break;}\
		break;\
	\
	case 0x96: FlashSZ = 0x8000; EE_size = 0x800;\
		switch (sig_byte_3)\
			{case 0x09: device_ptr = 4; family_ptr = 0; break;}\
		break;}



