/** @file usart.h Usart driver header file. */

#ifndef __USART_HEADER__
#define __USART_HEADER__

#include <inttypes.h>
#include "c_ext.h"

/*******************************************************************************
* Baud rate definitions
*******************************************************************************/

/** @def USART_BAUD_RATE_9600 Defines the value of the UBRR register that
	produces a 9600 baud rate. This value depends on 1MHz crystal and the
	U2X bit set to 1 (x2 speed enabled). 9600 baud is the only baud rate
	that can be produced with an acceptable error. */
#define USART_BAUD_RATE_9600	12

#define USART_BAUD_RATE_19200	6	// NOTE: Error -7.0%
#define USART_BAUD_RATE_38400	2	// NOTE: Error 8.5%
#define USART_BAUD_RATE_57600	1	// NOTE: Error 8.5%
#define USART_BAUD_RATE_115200	0	// NOTE: Error 8.5%

/*******************************************************************************
* Exported functions
*******************************************************************************/

extern void UsartInit(uint16_t baudrate);
extern void UsartDisable(void);
extern BOOL IsUsartEnabled(void);
extern void UsartTxChar(uint8_t byte);
extern uint8_t UsartNewDataIn(void);
extern uint8_t UsartRxChar(void);
extern void UsartTxString(char *str);
extern void UsartTxString_F(const char *str);

#endif //__USART_HEADER__
