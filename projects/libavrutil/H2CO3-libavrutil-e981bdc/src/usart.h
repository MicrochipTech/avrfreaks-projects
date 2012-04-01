/*
 * usart.h
 * libavrutil
 *
 * Created by Árpád Goretity on 18/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

/*
 * USART: Universal Synchronous Asynchronous Receiver Transmitter
 * Serial port communication implementation
 */

#ifndef __AVRUTIL_USART_H__
#define __AVRUTIL_USART_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include "base.h"


/*
 * Mode bitmask macros for the interrupt manager functions.
 */

#define AVR_USART_RX (1 << 0)
#define AVR_USART_TX (1 << 1)

/*
 * Channel macros
 */

#ifdef UCSRA

#define AVR_USART_CH0 0

#else /* UCSRA */

#ifdef UCSR0A
#define AVR_USART_CH0 0
#endif /* UCSR0A */

#ifdef UCSR1A
#define AVR_USART_CH1 1
#endif /* UCSR1A */

#ifdef UCSR2A
#define AVR_USART_CH2 2
#endif /* UCSR2A */

#ifdef UCSR3A
#define AVR_USART_CH3 3
#endif /* UCSR3A */

#endif /* UCSRA */

/*
 * Initialize the USART system for the requested baud rate.
 * This is the first function that must be called before any other
 * avr_usart_*() function call. Note that in some cases, the
 * actual baud rate may be different from the requested if F_CPU is not
 * divisible by (16 * baud). Example:
 * USART prescaler steps = F_CPU / (16 * baudrate), so:
 * USART prescaler register = USART prescaler steps - 1 = F_CPU / (16 * baudrate) - 1
 * So actual baud rate = F_CPU / ((prescaler register + 1) * 16)
 * For F_CPU = 16000000ULL (16 MHz) and 9600 baud, it becomes:
 * prescaler register = 16000000 / (16 * 9600) - 1 = 103 (INTEGER DIVISION! ROUNDED!)
 * baud rate = 16000000 / ((103 + 1) * 16) = 9615 baud!
 *
 * Channel is one of the AVR_USART_CH* macros. May be in the range 0...0
 * (on AVRs with only one channel) or 0...3 (on AVRs with 4 channels).
 */
void avr_usart_init(uint8_t channel, uint32_t baud);

/*
 * Disables the USART receiver and transmitter. If this is called, avr_usart_send()
 * and avr_usart_recv() should not be called on the specified channel
 * until another call to avr_usart_init() on the same channel.
 */
void avr_usart_close(uint8_t channel);

/*
 * Reads one byte from the USART receiver on the given channel.
 * Blocks until data is available. It likely won't block inside
 * a 'Data Received' interrupt handler.
 */
uint8_t avr_usart_recv(uint8_t channel);

/*
 * Writes one byte to the USART transmitter on the given channel.
 * Blocks if the transmitter is busy (i. e., its buffer is full).
 */
void avr_usart_send(uint8_t channel, uint8_t byte);

/*
 * Reads `length' bytes to the memory pointed to by `buf', from
 * the channel `channel'.
 * Uses avr_usart_recv(), so blocks until the data is fully received.
 */
void avr_usart_read(uint8_t channel, uint8_t *buf, size_t length);

/*
 * Writes `length` bytes from the memory pointed to by `buf', to
 * the channel `channel'.
 * Uses avr_usart_send(), so blocks until the data is fully sent.
 */
void avr_usart_write(uint8_t channel, const uint8_t *buf, size_t length);

/*
 * Performs a printf()-style string data transfer through the given channel.
 * For RAM considerations, only prints the first 255 character of the
 * final (already formatted) string.
 */
__attribute__((format(printf, 2, 3)))
size_t avr_usart_printf(uint8_t channel, const char *str, ...);

/*
 * Enables RX completed and/or TX completed interrupts on `channel'.
 * Interrupts will be generated whenever the AVR USART module encounters
 * either incoming data or completes sending data (depending on mode).
 * `mode' is composed of the above bitmask mode macros.
 * Global interrupts must be enabled for this to work.
 * You have to supply the appropriate Interrupt Service Routine (ISR)
 * to properly handle the interrupt. Refer to your AVR's datasheet for the
 * exact interrupt vector name. It may be something similar to USART_RX_vect.
 */
void avr_usart_intr_enable(uint8_t channel, uint8_t mode);

/*
 * Disables RX and TX completed interrupts, so no interrupt will be
 * generated on incoming or sent data on `channel'.
 */
void avr_usart_intr_disable(uint8_t channel);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__AVRUTIL_USART_H__ */

