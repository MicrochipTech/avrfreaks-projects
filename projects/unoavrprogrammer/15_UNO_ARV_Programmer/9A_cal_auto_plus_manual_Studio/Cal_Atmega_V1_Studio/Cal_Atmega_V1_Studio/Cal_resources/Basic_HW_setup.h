
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>



/***************************************************************************/
void save_cal_values(unsigned char);
void printout_cal_values(void);
void initialise_timers_for_cal_error(void);
void start_timers_for_cal_error(void);
void Minimise_error_down(int, unsigned char*, unsigned char*, long*, unsigned char*, char);
void Minimise_error_up(int, unsigned char*, unsigned char*, long*, unsigned char*, char);
long compute_error(char, char, char);

void Auto_cal (char);
void Manual_cal(void);



/***************************************************************************/
unsigned char OSCCAL_WV;
unsigned char OSCCAL_UV;
unsigned char OSCCAL_DV;
int EE_size;
unsigned int FlashSZ;

long buffer[45];
volatile int EA_counter, EA_buff_ptr;
volatile long error_SUM;

volatile char cal_mode; 							//Defines number of averages used when measuring osccal_error	
volatile char T1_OVF;

int sig_byte_2, sig_byte_3;


/***************************************************************************/
#define delay_2ms 4,195
#define delay_20ms 5,100

#define T0_delay_5ms 5,220
#define T0_delay_10ms 5,178
#define T0_delay_20ms 5,100

#define T1_delay_100ms 5,0xFCF2
#define T1_delay_1sec 5,0xE17D
#define delay_2us 1,254
#define delay_10ms 5,183




/*****************************************************************************/
#define setup_HW \
setup_watchdog;\
ADMUX |= (1 << REFS0);\
Initialise_IO;\
set_device_type_and_memory_size;\
\
switch(sig_byte_2){\
	case 0x95: if(sig_byte_3 != 2)USART_init(0,25);\
	else USART_init_32(0,25); break;\
default: USART_init(0,25); break;}




/*****************************************************************************/
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}

#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")



/*****************************************************************************/
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





/*****************************************************************************/
#define Get_ready_to_calibrate \
TIMSK2 |= (1 << TOIE2);\
TIMSK1 |= (1 << TOIE1);\
initialise_timers_for_cal_error();\
start_timers_for_cal_error();

/*****************************************************************************/
#define close_calibration \
initialise_timers_for_cal_error();\
TIMSK2 &= (~(1 << TOIE2));\
TIMSK1 &= (~(1 << TOIE1));


/*****************************************************************************/
#define calibrate_without_sign_plus_warm_up_time \
cal_mode = 5;\
cal_error = compute_error(0,cal_mode,0);\
cal_error = compute_error(0,cal_mode,0);\
cal_error = compute_error(0,cal_mode,0);





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

