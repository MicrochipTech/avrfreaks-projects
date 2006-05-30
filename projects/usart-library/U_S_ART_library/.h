#ifndef _AVR_UART_H_
#define _AVR_UART_H_

/************************************************************************
 *
 * Author: Artur Lipowski <LAL@pro.onet.pl>
 * Version: 3.0, May 27 2006
 * Copyright 2003-2006 Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ************************************************************************/

/** \file uart.h
 *  \brief A header file for core functionality of the U(S)ART library.
 *
 *  Requires \c inttypes.h and \c io.h headers (from avr-libc).
 */

#include <inttypes.h>
#include <avr/io.h>
#include <uart_guard.h>


/** \def TX_FULL_ERROR
 *  \brief A macro that defines a bitmask for the TX buffer full error.
 *
 *  If any bits in this masks are set in the value returned by the #UART_get_error 
 *  function then transmission buffer is full and no new bytes will be accepted
 *  until next byte is transmitted or buffer is pruned #UART_clear_TX_buf.
 */
#define	TX_FULL_ERROR	_BV(1)

/** \def RX_FULL_ERROR
 *  \brief A macro that defines a bitmask for RX buffer full error.
 *
 *  If any bits in this masks are set in the value returned by the #UART_get_error 
 *  function then transmission buffer is full and no new bytes will be received until 
 *  next byte is fetched or buffer is pruned (#UART_clear_RX_buf).
 */
#define	RX_FULL_ERROR	_BV(2)


/** \fn uint8_t UART_get_error(uint8_t device)
 *  \brief Return the last error code(s) for given device.
 *
 *  This function returns error code which consists of bits which 
 *  define UART errors.
 *  See: #TX_FULL_ERROR and #RX_FULL_ERROR
 *
 *  \param device The number of the "asked" uart device (starts with 0).
 *  \return The last error code.
 */
extern uint8_t UART_get_error(uint8_t device);


/** \fn void UART_set_speed(uint8_t device, uint16_t UBRR_val)
 *  \brief Initializes UART library internals and sets UART speed.
 *  
 *  For non USART devices (eg. AT91S8515) only 8 LSB bits will be used from 
 *  the UBRR_val parameter.
 *  This function should be called before UART_RX_init() and/or UART_TX_init().
 *  Interrupts are not enabled (no sei() call) so remeber to do it.
 *  
 *  \param device The number of the initialized uart device (starts with 0).
 *  \param UBRR_val This value will be used to set UBRR register.
 */
extern void UART_set_speed(uint8_t device, uint16_t UBRR_val);

	
/** \fn void UART_init_RX(uint8_t device)
 *  \brief Initializes receiver part of the UART library.
 *
 *  This function should be called after UART_set_speed().
 *  It resets (existing data is removed) input buffer and clears input errors.
 *
 *  \param device The number of the initialized uart device (starts with 0).
 */
extern void UART_init_RX(uint8_t device);

	
/** \fn void UART_init_TX(uint8_t device)
 *  \brief Initializes transmitter part of the UART library.
 *
 *  This function should be called after the #UART_set_speed.
 *  As a side effect this function clears output errors.
 *  Existing bytes in TX buffer are discarded (no flushing is performed).
 *
 *  \param device The number of the initialized uart device (starts with 0).
 */
extern void UART_init_TX(uint8_t device);

	
/** \fn uint8_t UART_getchar(uint8_t device, uint8_t *a_byte)
 *  \brief Receives one byte from UART.
 *  
 *  The data receiving is driven by UART_RECV interrupt.
 *  Incoming data is placed into buffer.<br>
 *  If the input buffer is full then #RX_FULL_ERROR bit is set in value returned by
 *  the #UART_get_error and next incoming bytes are discarded (lost).
 *  If there was #RX_FULL_ERROR error then next (in this case succesfull) 
 *  function call will reset it.<br>
 *  This function will not wait for incoming data.<br>
 *  Retrieved data is removed from input buffer so there is no way to "unget".
 *  
 *  \param device The number of the uart device (starts with 0) to read from.
 *  \param a_byte If there is any received byte then it will be placed into 
 *  this address.
 *  \return 1 if success or 0 if there is no waiting data.
 */
extern uint8_t UART_getchar(uint8_t device, uint8_t *a_byte);


/** \fn int UART_get(uint8_t device)
 *  \brief Receives one byte from UART.
 *  
 *  This function is provided to make easier usage of the standard IO module.<br>
 *  Current implementation uses UART_getchar and 1 byte buffer in the 
 *  .noinit memory section<br>
 *  UART_get does not wait for data - if input buffer is emtpy it returns -1
 *  and this means that standard IO functions will return immediately with
 *  error (eg. EOF).
 *  
 *  \param device The number of the uart device (starts with 0) to read from.
 *  \return received byte if success or -1 if there is no waiting data.
 */
int UART_get(uint8_t device);


/** \fn void UART_clear_RX_buf(uint8_t device)
 *  \brief Clears input buffer.
 *
 *  All data in input buffer is lost.<br>
 *  Input errors are also cleared.
 *  
 *  \param device The number of the uart device (starts with 0) to clear buffer.
 */
extern void UART_clear_RX_buf(uint8_t device);


/** \fn uint8_t UART_get_RX_len(uint8_t device)
 *  \brief Returns numer of received bytes in the input buffer.
 *  
 *  \param device The number of the uart device (starts with 0) to get the queue length.
 *  \return Number of bytes in the input buffer.
 */
extern uint8_t UART_get_RX_len(uint8_t device);


/** \fn uint8_t UART_putchar(uint8_t device, uint8_t a_byte)
 *  \brief Sends one byte to UART.
 *  
 *  The byte sending is acknowledged by UART_DATA interrupt.<br>
 *  If there is no pending data to send then outgoing byte is placed directly into UDR
 *  ootherwise into the TX buffer.<br>
 *  If the buffer is full then #TX_FULL_ERROR bit is set in value returned by
 *  the #UART_get_error function and no action is performed. If there was
 *  #TX_FULL_ERROR error then next succesfull function call will reset it.<br>
 *  This function will not wait for free space in the output buffer.<br>
 *  
 *  \param device The number of the uart device (starts with 0) to write to.
 *  \param a_byte This value will be placed into output buffer.
 *  \return 1 if success or 0 if there is no space in output buffer.
 */
extern uint8_t UART_putchar(uint8_t device, uint8_t a_byte);

	
/** \fn int UART_put(uint8_t device, uint8_t a_byte)
 *  \brief Sends one byte to UART.
 *  
 *  This function is provided for make easier usage of the standard IO module.
 *  See \ref example_io. <br>
 *  Current implementation uses UART_putchar.<br>
 *  UART_put does not wait for free space when the output buffer is full - it returns -1
 *  and this means that standard IO functions will return immediately with
 *  error (eg. EOF).
 *  
 *  \param device The number of the uart device (starts with 0) to write to.
 *  \param a_byte This value will be placed into output buffer.
 *  \return 0 if success or -1 if there is no space in the output buffer.
 */
int UART_put(uint8_t device, uint8_t a_byte);

/** \fn void UART_clear_TX_buf(uint8_t device)
 *  \brief Clears output buffer.
 *
 *  After call all data in the output buffer is lost.<br>
 *  Output errors are also cleared.
 *  
 *  \param device The number of the uart device (starts with 0) to clear buffer.
 */
extern void UART_clear_TX_buf(uint8_t device);


/** \fn uint8_t UART_get_TX_len(uint8_t device)
 *  \brief Returns the number of bytes waiting for transmission in the output buffer.
 *  
 *  \param device The number of the uart device (starts with 0) to get the queue length.
 *  \return Number of bytes in the output buffer.
 */
extern uint8_t UART_get_TX_len(uint8_t device);


#endif
