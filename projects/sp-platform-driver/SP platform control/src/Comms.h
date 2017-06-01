/*
 * Comms.h
 *
 * Created: 5/21/2017 12:10:14 AM
 *  Author: Big Daddy
 */ 


#ifndef COMMS_H_
#define COMMS_H_

#define USART                 (&AVR32_USART0)
#define USART_RX_PIN          AVR32_USART0_RXD_0_1_PIN
#define USART_RX_FUNCTION     AVR32_USART0_RXD_0_1_FUNCTION
#define USART_TX_PIN          AVR32_USART0_TXD_0_1_PIN
#define USART_TX_FUNCTION     AVR32_USART0_TXD_0_1_FUNCTION
#define USART_IRQ             AVR32_USART0_IRQ
#define USART_BAUDRATE        57600
#define TARGET_PBACLK_FREQ_HZ BOARD_OSC0_HZ // PBA clock target frequency, in Hz

// local or extern?
#ifdef _COMMS_
#define GLOBAL
#define _I(x) x
#else // end #ifdef _COMMS_
#define GLOBAL extern
#define _I(x)
#endif // end #ifndef _COMMS_

GLOBAL void Comms_Init(void);

// cleanup
#undef _I
#undef GLOBAL

#endif /* COMMS_H_ */