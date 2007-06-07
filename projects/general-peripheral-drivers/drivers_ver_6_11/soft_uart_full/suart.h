/****************************************************************************
 Title  :   C  include for the SOFTWARE UART FUNCTIONS library (suart.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      10/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  Based on application note AVR 305
            This software is FREE.
*****************************************************************************/

/*################### DO NOT CHANGE ANYTHING BELOW THIS LINE !!! #####################################*/
/*############## ALL CONFIGURATION IS DONE IN "softuart.cfg" file !!! ################################*/

#ifndef SUART_0_H
#define SUART_0_H

#define SUART_0_NO_RX_LIMIT         0xFF

/* ERROR CODES */
#define SUART_0_BAUDRATE_TOO_LOW    1
#define SUART_0_BAUDRATE_TOO_HIGH   2 

#include "softuart_0.cfg"

/* 
   GLOBAL CONTROL VARIABLES VISIBLE TO ALL. USE THEM TO CONTROL THE SUART FUNCTIONS. 
   I have selected variables instead of function calls due to lower overhead and time is not enough.
   For usage of the below variables see the "test_suart.c" file. 
*/
extern volatile unsigned char  suart_0_rx_buffer[SUART_0_BUF_SIZE+1];
extern volatile unsigned char  suart_0_rx_count;
extern volatile unsigned char  suart_0_string_received;
#if  SUART_0_TIMEOUT_ENABLED == 1    
extern volatile unsigned char  suart_0_timeout_flag;
#endif



/* FUNCTION PROTOTYPES  */
#if SUART_0_DYNAMIC_BAUD_CHANGE == 1 && SUART_0_WILL_USE_A_TIMER == 0
extern unsigned char    suart_0_initialize(unsigned int baudrate);
#elif SUART_0_DYNAMIC_BAUD_CHANGE == 1 && SUART_0_WILL_USE_A_TIMER == 1
extern unsigned char    suart_0_initialize(unsigned long baudrate);
#elif SUART_0_DYNAMIC_BAUD_CHANGE == 0
extern unsigned char    suart_0_init(void);
#endif
#if SUART_0_DYNAMIC_IO_SETTINGS == 1
extern void             suart_0_configure_io(volatile unsigned char* port, unsigned char rx_pin, unsigned char tx_pin);
#endif

extern void             suart_0_raw_rx(unsigned char number_of_chars);
extern void             suart_0_rx_off(void);
extern void             suart_0_putc(unsigned char data);
extern void             suart_0_puts(unsigned char *tx_data);
extern void             suart_0_puts_p(const char *progmem_tx_data);
extern void             suart_0_puts_e(unsigned char *eeprom_tx_data);
extern void             suart_0_puti(int value, unsigned char dot_position);
extern unsigned char    suart_0_getc(void);
extern void             suart_0_gets(unsigned char number_of_chars);
extern void             suart_0_get_buf(unsigned char *data, unsigned char size);

/* MACRO DEFINITIONS */



#ifndef P
#define P(s) ({static const char c[] __attribute__ ((progmem)) = s;c;})
#endif

#define suart_0_puts_P(__s)         suart_0_puts_p(P(__s))

#define suart_0_get_buffer(x)       suart_0_get_buf(x, sizeof(x)-1)


#if SUART_0_DYNAMIC_BAUD_CHANGE == 1 && SUART_0_WILL_USE_A_TIMER == 0
#define suart_0_init(baud)          suart_0_initialize(baud/10)
#elif SUART_0_DYNAMIC_BAUD_CHANGE == 1 && SUART_0_WILL_USE_A_TIMER == 1
#define suart_0_init(baud)          suart_0_initialize(baud)
#endif

#define suart_0_config_io(port,rx_pin,tx_pin) suart_0_configure_io( (&(port)),rx_pin,tx_pin )

#endif
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/

