
/****************************************************************************
 Title  :   C  include file for the UART FUNCTIONS library (uart.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      30/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/


#ifndef UART_H
#define UART_H

/*##############################################################################################*/
/*                             START OF CONFIGURATION BLOCK                                     */
/*##############################################################################################*/

#ifndef F_CPU
#define F_CPU                         3686400   /* CPU clock frequency for STK500 */
#endif
#define UARTS_NEEDED                  2         /* how many uart you need */
#define UART_RX_BUFFER_SIZE           20        /* Receive buffer size  */
#define UART0_RX_TERMINATION_CHAR     '\r'
#define UART1_RX_TERMINATION_CHAR     '\r'
#define PUTI_DECIMAL_POINT            ','       /* The decimal point punctuation mark char */

/* Ways to reduce code size */
#define UART_DYNAMIC_BAUD_CHANGE     1          /* 1 if you will change baudrate during run time */
#define UART0_BAUDRATE               115200     /* Valid only if SUART_DYNAMIC_BAUD_CHANGE == 0 */
#define UART1_BAUDRATE               115200     /* Valid only if SUART_DYNAMIC_BAUD_CHANGE == 0 */

#define UART0_ERROR_CHECK_SUPPORT    1
#define UART0_AT_COMMAND_SUPPORT     1        
#define UART0_RAW_DATA_SUPPORT       1
#define UART1_ERROR_CHECK_SUPPORT    1
#define UART1_AT_COMMAND_SUPPORT     1        
#define UART1_RAW_DATA_SUPPORT       1

#define UART_MULTI_COMMAND_SUPPORT   1

/*##############################################################################################*/
/*                              END OF CONFIGURATION BLOCK                                      */
/*##############################################################################################*/
/*
   ***********************  UART COMMAND DEFINITIONS  ******************************
   When you issue a "uart_command(SELECT_UARTx)" (x=0 or 1) all the commands
   and functions that follow refer to that particular UART.
   To change UART issue another "uart_command(SELECT_UARTx)" command.

   The "uart_command(UART_GET_RX_COUNT)" returns the number of received chars.
   When in "UART_GET_RAW_DATA" mode it returns the number of received chars IN REAL TIME.
   When in "UART_GET_AT_CMD" mode it returns the number of received chars after and only after
   a valid "AT" command is received. Untill then it returns zero.
   When in "UART_GET_STRING" mode it returns the number of received chars after and only after
   a valid string is received. Untill then it returns zero.
   Use "uart_command(UART_GET_RX_COUNT)" to test if something is received.
   If something is received you can read it using the "uart_get_buffer()" macro
   or the "uart_get_buf()"command.

   When issuing UART COMMANDS Precedence is from LSB to MSB.
   That means that if you give multiple relevant commands the higher number command
   is executed last thus superseding the lower number commands.
   EXAMPLE: if you give the "suart_command(SELECT_UART0|UART_GET_AT_CMD | UART_RX_OFF)" by mistake
   the uart receiving will be disabled.
*/
#define SELECT_UART1              (1<<0)
#define SELECT_UART0              (1<<1)
#define UART_TX_ON                (1<<2)
#define UART_GET_STRING           (1<<3)        /* Get a string               */
#define UART_GET_AT_CMD           (1<<4)        /* waits for a new AT command */
#define UART_GET_RAW_DATA         (1<<5)        /* Enables RAW data reception */
#define UART_GET_ERROR            (1<<6)        /* Returns the error number if any */
#define UART_GET_RX_COUNT         (1<<7)        /* Returns the number of received chars */
#define UART_STRING_RECEIVED      (1<<8)        /* Returns the number of received chars in a string */
#define UART_AT_CMD_RECEIVED      (1<<9)        /* Returns the number of received chars in a string */
#define UART_ECHO_ON              (1<<10)
#define UART_ECHO_OFF             (1<<11)
#define UART_RX_OFF               (1<<12)
#define UART_TX_OFF               (1<<13)

#define OVERUN_ERROR               1
#define FRAMING_ERROR              2
#define BUFFER_OVERUN              3
#define UART_BAUDRATE_TOO_LOW      4
#define UART_BAUDRATE_TOO_HIGH     5
#define UART_HIGH_BAUDRATE_ERROR   6

#define UART0                      0
#define UART1                      1

#ifndef LOCATION_IS_RAM
#define LOCATION_IS_RAM            1
#endif
#ifndef LOCATION_IS_EEPROM
#define LOCATION_IS_EEPROM         2
#endif 
#ifndef LOCATION_IS_FLASH
#define LOCATION_IS_FLASH          3
#endif




/*##############################################################################################*/
/* UART CONTROL FUNCTIONS */
/*##############################################################################################*/
/* 
   Initialize UART. The function sets baud rate and gives varius uart commands.
   The uart commands can be ORed.
*/
#if    UART_DYNAMIC_BAUD_CHANGE == 1
extern unsigned char  uart_init(unsigned long int baud_rate, unsigned int command);
#elif  UART_DYNAMIC_BAUD_CHANGE == 0
extern void           uart_init(void);
#endif

/*
   Send commands to UART. See the #define section of this file for the commands.
   The uart commands can be ORed or single depending from the definition made earlier.
*/
extern unsigned char  uart_command(unsigned int command);

/*##############################################################################################*/
/* UART RECEIVED DATA FUNCTIONS */
/*##############################################################################################*/

/*
   Copies rx buffer to data and flushes the rx buffer if in UART_AT_RX mode.
   In UART_GET_RAW_DATA mode you get a precise copy of the specified size.
   Data in the rx buffer that weren't copied are realigned towards the start of the buffer
   and the new found space is reclaimed.
   The function performs destination array size check so it will return only as many chars
   as the destination array can hold. That way you can use the uart_get_buffer(*array) macro. 
   In UART_GET_AT_CMD mode you get the "AT" command without the "AT" prefix.
   After succesfull reception of an "AT" command receiving is disabled! so if you want to receive
   further "AT" commands reenable receiving issuing a "uart_command(UART_GET_AT_CMD)" command.
   In UART_GET_STRING mode you get the received string.
   After succesfull reception of a string receiving is disabled! so if you want to receive
   another string reenable receiving issuing a "uart_command(UART_GET_STRING)" command.
*/
extern void           uart_get_buf(unsigned char *data, unsigned char size);

/* get a char (interupt mode). Chars are written in the rx buffer also. */
extern unsigned char  uart_getc(void);

/*##############################################################################################*/
/* UART DATA TRANSMISSION  FUNCTIONS */
/*##############################################################################################*/

/* send a string (interrupt mode) and also checks for a NULL pointer or EMPTY string */
extern void           uart_puts(unsigned char *tx_data);

/* send a program memory string (interrupt mode). Use the MACRO uart_puts_P for convinience
   if a constant string is to be used  since it automatically creates and stores the string
   to program memory.                                                                    */
extern void           uart_puts_p(const char *progmem_tx_data);

/* send a string located at EEPROM memory (interrupt mode) */
extern void           uart_puts_e(unsigned char *eeprom_tx_data);

/* send a signed integer (interrupt mode) */
extern void           uart_puti(int value, unsigned char dot_position);

/* send a char (interrupt mode) */
extern  void          uart_putc(unsigned char tx_data);


/* MACRO's DEFINITIONS */
/* Macro to automatically store string to program memory */
#ifndef P
#define P(s) ({static const char c[] __attribute__ ((progmem)) = s;c;})
#endif
#define uart_puts_P(__s)         uart_puts_p(P(__s))
/* Macro to use uart_gets with one argument */
#define uart_getS(x)             uart_gets(x,sizeof(x))
#define uart_get_buffer(x)       uart_get_buf(x, sizeof(x)-1)


#endif  /* ifndef UART_H */
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/

