#ifndef _AVR_UART_UTIL_H_
#define _AVR_UART_UTIL_H_

/************************************************************************
 *
 * File: uart_util.h
 * Author: Artur Lipowski <LAL@pro.onet.pl>
 * Version: 3.0, May 27 2006
 * Copyright 2003-2006 Artur Lipowski
 * 
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ************************************************************************/

/** \file uart_util.h
 *  \brief A header file for additional functionality of the U(S)ART library.
 *
 *  Requires the \c avr/pgmspace.h header (from the avr-libc).
 */

#include <avr/pgmspace.h>
#include <uart.h>


/** \fn void UART_gets(uint8_t device, uint8_t str[], uint8_t maxlen)
 *  \brief Receives maxlen characters from UART.
 *    
 *  This function places maxlen incoming bytes into str array.<br>
 *  It waits for data by active polling (no sleep - just simple infinite while loop).<br>
 *  Function does not perform any error checking.<br>
 *  
 *  \param device The number of the uart device (starts with 0) to read from.
 *  \param str Space where incoming datai will be placed.
 *  \param maxlen Exactly this number of bytes will be placed into str.
 */
extern void UART_gets(uint8_t device, uint8_t str[], uint8_t maxlen);


/** \fn void UART_gets_delim(uint8_t device, uint8_t str[], uint8_t delim, uint8_t maxlen)
 *  \brief Receives characters from the UART until specified delimiter is encounterd or
 *  maxlen characters is received.
 *    
 *  This function places up to maxlen received bytes into the str array. 
 *  If during data receiving delim charcter is encountered then function 
 *  stop acquiring data and returns.<br>
 *  It waits for data by active polling (no sleep - just simple infinite while loop).<br>
 *  Function does not perform any error checking.<br>
 *  
 *  \param device The number of the uart device (starts with 0) to read from.
 *  \param str Space where will be placed incoming data.
 *  \param delim Character with this value will stop data receiving.
 *  \param maxlen Up to this number of bytes will be placed into str.
 *  \return uint8_t Number of received characters (including delimiter).
 */
extern uint8_t UART_gets_delim(uint8_t device,
		uint8_t str[],
		uint8_t delim,
		uint8_t maxlen);


/** \fn void UART_puts(uint8_t device, const uint8_t str[])
 *  \brief Sends C type string to UART.
 *    
 *  This function sends characters from str throught U(S)ART by placing them into
 *  outgoing buffer until NULL byte is encountered.<br>
 *  If there is no space in the TX buffer then it waits (no sleep - just simple
 *  infinite while loop).<br>
 *  Function does not perform any error checking.<br>
 *  
 *  \param device The number of the uart device (starts with 0) to write to.
 *  \param str C string which will be transmitted (must be NULL terminated).
 */
extern void UART_puts(uint8_t device, const uint8_t str[]);


/** \fn void UART_puts_P(uint8_t device, PGM_P str)
 *  \brief Transmits C type string from FLASH to UART.
 *    
 *  This function sends characters from str through U(S)ART by placing them into 
 *  outgoing buffer until NULL byte is encoountered. A str is a pointer to data 
 *  into FLASH memory.<br>
 *  It waits for space in TX buffer (no sleep - just simple infinite while loop).<br>
 *  Function does not perform any error checking.<br>
 *  
 *  \param device The number of the uart device (starts with 0) to write to.
 *  \param str C string placed in FLASH memory which will be transmitted (must 
 *  be NULL terminated).
 */
extern void UART_puts_P(uint8_t device, PGM_P str);


/** \fn void UART_wait_RX(uint8_t device)
 *  \brief Functions blocks execution (waits) until the input buffer is filled
 *  with at least one byte.
 *  
 *  \param device The number of the uart device (starts with 0) to wait receiving.
 */
extern void UART_wait_RX(uint8_t device);


/** \fn void UART_sleep_RX(uint8_t device)
 *  \brief Functions blocks execution (CPU sleep) until there is received at 
 *  least one byte.
 *  
 * You have to enable sleep mode before e.g.:
 * \c set_sleep_mode(SLEEP_MODE_IDLE);
 *  
 *  \param device The number of the uart device (starts with 0) to wait receiving.
 */
extern void UART_sleep_RX(uint8_t device);


/** \fn void UART_wait_TX_empty(uint8_t device)
 *  \brief Functions blocks execution (waits) until the output buffer become empty.
 *  
 *  \param device The number of the uart device (starts with 0) to wait for empty TX queue.
 */
extern void UART_wait_TX_empty(uint8_t device);


/** \fn void UART_sleep_TX_empty(uint8_t device)
 *  \brief Functions blocks execution (CPU sleep) until the output buffer become empty.
 *  
 * You have to enable sleep mode before e.g.:
 * \c set_sleep_mode(SLEEP_MODE_IDLE);
 *  
 *  \param device The number of the uart device (starts with 0) to wait for empty TX queue.
 */
extern void UART_sleep_TX_empty(uint8_t device);


#endif
