


/************************Subroutine provided by .S file*******************/
extern void read_flash (void);



/***************************************************************************/
volatile char loc_in_mem_H, loc_in_mem_L;					//Used to pass addresses in SRAM to assembly
volatile char Prog_mem_address_H, Prog_mem_address_L;		//Used to pass addresses in flash to the assembly subroutines
volatile char Flash_readout;





void I2C_Tx_2_integers(unsigned int, unsigned int);
void I2C_Tx(char, char, char*);
void send_byte_with_Ack(char);
void send_byte_with_Nack(char);
void Timer_T1_sub(char, unsigned int);

void Char_to_PC(char);
void String_to_PC(const char*);
char waitforkeypress(void);
char receiveChar(void);
char isCharavailable(char);

int Num_from_KBD(void);
void Num_to_PC(char, long);
void SBtoAL(char*, long, char);
void NumericString_to_PC(char*);

char decimal_digit (char);
char wait_for_return_key(void);
void I2C_Tx_8_byte_array(char*);
long I2C_displayToNum(void);

char receive_byte_with_Ack(void);
char receive_byte_with_Nack(void);

void USART_init (unsigned char, unsigned char);

#define T1_delay_50ms 5,0xFE78
#define T1_delay_100ms 5,0xFCF2
#define T1_delay_250ms 5,0xF85F
#define T1_delay_500ms 5,0xF0BE


/*****************************************************************************/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>


/*****************************************************************************/
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}

/*****************************************************************************/
#define setup_HW \
setup_watchdog;\
Unused_I_O;\
set_up_I2C;\
ADMUX |= (1 << REFS0);\
USART_init(0,16);


/*****************************************************************************/
#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")


/*****************************************************************************/
#define set_up_I2C \
TWAR = 0x02;


/*****************************************************************************/
#define Unused_I_O \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC  = 0xFF;\
PORTD  = 0xFF;


/*****************************************************************************/
#define waiting_for_I2C_master \
TWCR = (1 << TWEA) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));\
TWDR;

#define clear_I2C_interrupt \
TWCR = (1 << TWINT);

