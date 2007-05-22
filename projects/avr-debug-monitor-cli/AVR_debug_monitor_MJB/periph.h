/*
*   periph.h
*/
#ifndef  _PERIPH_H_
#define  _PERIPH_H_

#include "system.h"


#define  MSEC_PER_TICK              5     // Timer #2 tick interval, msec
#define  TICKS_PER_200MSEC         40     // Timer #2 ticks in 200ms

#define  HALT(n)   { DISABLE_GLOBAL_IRQ; PORTA = n; while (1); }  // Debug aid

#define  SET_DEBUG_FLAG(bm)   (gwDebugFlags |= bm)        // Debug aid

#define  PEEK_CODE_WORD(addr)  (*(uint16  __hugeflash *)(addr))
#define  PEEK_CODE_BYTE(addr)  (*(uint8  __hugeflash *)(addr))

#define  ENABLE_TICK_TIMER   (TIMSK |= (1<<OCIE1A))
#define  DISABLE_TICK_TIMER  (TIMSK &= ~(1<<OCIE1A))
#define  HEARTBEAT_LED_TOGL  (PORTD ^= BIT_5)
#define  CLEAR_RESET_FLAGS   (MCUCSR &= ~0x1F)        // Clear the MCU hardware reset flags


// Globals...
extern  bool    b5msecTaskReq;
extern  bool    b50mSecTaskReq;
extern  bool    b500msecTaskReq;
extern  uint16  gwDebugFlags;


/* General system functions */

#pragma  vector = TIMER1_COMPA_vect
_INTERRUPT_ void  isrMCU_Timer1_compA( void );

void    initMCUports( void );
void    initMCUtimers( void );
uint32  getTickCount( void );
void    buttonInputScan( void );
bool    button_pressed( uint8 bitmask );
bool    button_hit( uint8 bitmask );

// UART0 (debug port) driver functions

#pragma  vector = USART0_RXC_vect
__interrupt void  uart0_isr( void );

void    init_UART0( void );
void    UART0_RX_IRQctrl( bool yIRQmask );
bool    UART0_TX_Ready( void );
void    UART0_TX_byteOut( uchar bTXdata );


#endif  /* _PERIPH_H_ */
