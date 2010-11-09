/**
 * @file pal_uart.h
 *
 * @brief PAL uart internal functions prototypes
 *
 * This header contains the function prototypes for transmit,
 * receive functions and macros used in UART module.
 *
 * $Id: pal_uart.h 18381 2009-10-07 09:56:16Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
 // Modified to support Teensy2++; declarations of SIO functions.  9/15/10  -jkl

/* Prevent double inclusion */
#ifndef PAL_UART_H
#define PAL_UART_H

/* === Includes ============================================================= */

#include "pal.h"
#include "pal_config.h"
#include "app_config.h"

/* === Types ================================================================ */

#if ((defined UART0) || (defined UART1))

/*
 * Structure containing the transmit and receive buffer
 * and also the buffer head, tail and count
 */
typedef struct uart_communication_buffer_tag
{
    /* Transmit buffer */
    uint8_t tx_buf[UART_MAX_TX_BUF_LENGTH];

    /* Receive buffer */
    uint8_t rx_buf[UART_MAX_RX_BUF_LENGTH];

    /* Transmit buffer head */
    uint8_t tx_buf_head;

    /* Transmit buffer tail */
    uint8_t tx_buf_tail;

    /* Receive buffer head */
    uint8_t rx_buf_head;

    /* Receive buffer tail */
    uint8_t rx_buf_tail;

    /* Number of bytes in transmit buffer */
    uint8_t tx_count;

    /* Number of bytes in receive buffer */
    uint8_t rx_count;

} uart_communication_buffer_t;

/* === Externals ============================================================ */


/* === Macros =============================================================== */

/*
 * Calculate the UART baud rate generator prescaler, based on the
 * global F_CPU setting, and the baud rate passed as parameter.  This
 * assumes the U2X bit will always be set.
 */
#define UART_BAUD(rate) \
    (((F_CPU) + 4UL * (rate)) / (8UL * (rate)) - 1UL)

/* Receive interrupt Mask */
#define RX_INT_MASK (0x80)

/* Transmit interrupt Mask */
#define TX_INT_MASK (0x40)

/* UART0 */
#ifdef UART0
/* Enables the RX interrupt of UART0 */
#define ENABLE_UART_0_RX_INT()   (UCSR0B |= RX_INT_MASK)

/* Enables the TX interrupt of UART0 */
#define ENABLE_UART_0_TX_INT()   (UCSR0B |= TX_INT_MASK)

/* Disables the RX interrupt of UART0 */
#define DISABLE_UART_0_RX_INT()  (UCSR0B &= ~RX_INT_MASK)

/* Disables the TX interrupt of UART0 */
#define DISABLE_UART_0_TX_INT()  (UCSR0B &= ~TX_INT_MASK)
#endif  /* UART0 */

/* UART1 */
#ifdef UART1
/* Enables the RX interrupt of UART1 */
#define ENABLE_UART_1_RX_INT()   (UCSR1B |= RX_INT_MASK)

/* Enables the TX interrupt of UART1 */
#define ENABLE_UART_1_TX_INT()   (UCSR1B |= TX_INT_MASK)

/* Disables the RX interrupt of UART1 */
#define DISABLE_UART_1_RX_INT()  (UCSR1B &= ~RX_INT_MASK)

/* Disables the TX interrupt of UART1 */
#define DISABLE_UART_1_TX_INT()  (UCSR1B &= ~TX_INT_MASK)
#endif  /* UART1 */
#endif /* #if ((defined UART0) || (defined UART1)) */
/* === Prototypes =========================================================== */
#ifdef __cplusplus
extern "C" {
#endif

void sio_uart_0_init(uint32_t baud_rate);
void sio_uart_1_init(uint32_t baud_rate);
uint8_t sio_uart_0_rx(uint8_t *data, uint8_t max_length);
uint8_t sio_uart_1_rx(uint8_t *data, uint8_t max_length);
uint8_t sio_uart_0_tx(uint8_t *data, uint8_t length);
uint8_t sio_uart_1_tx(uint8_t *data, uint8_t length);

#ifdef __cplusplus
} /* extern "C" */
#endif

//#endif /* #if ((defined UART0) || (defined UART1)) */

#endif /* PAL_UART_H */
/* EOF */
