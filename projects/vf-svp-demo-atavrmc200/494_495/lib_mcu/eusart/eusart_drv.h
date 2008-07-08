//***************************************************************************
//! @file eusart_drv.h,v
//!
//! Copyright (c) 2005 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief EUSART driver include file.
//!
//! @version 1.10 pwm3-ac-ctrl-motor-0_0_6
//!
//! @todo
//! @bug
//***************************************************************************


#ifndef _EUSART_DRV_H_
#define _EUSART_DRV_H_

//_____ I N C L U D E S ____________________________________________________

#include "config.h"

//_____ D E F I N I T I O N S ______________________________________________

/*
#ifndef FOSC
#  error  You must define FOSC in "config.h" file
#endif
*/

// ----------

#define MSK_UART_5BIT           0x00 /* UCSRC register */
#define MSK_UART_6BIT           0x02
#define MSK_UART_7BIT           0x04
#define MSK_UART_8BIT           0x06
#define MSK_UART_9BIT           0x06
#define MSK_UART_UPM            0x30
#define MSK_UART_USBS           0x08

#define MSK_UART_RX_DONE        0x80 /* UCSRA register */
#define MSK_UART_TX_COMPLETE    0x40
#define MSK_UART_DRE            0x20
#define MSK_UART_FE             0x10
#define MSK_UART_UPE            0x04
#define MSK_UART_DOR            0x08

#define MSK_UART_RXCIE          0x80 /* UCSRB register */
#define MSK_UART_TXCIE          0x40
#define MSK_UART_UDRIE          0x20
#define MSK_UART_ENABLE_RX      0x10
#define MSK_UART_ENABLE_TX      0x08
#define MSK_UART_TX_BIT9        0x01
#define MSK_UART_RX_BIT9        0x02

#define MSK_EUSART_TX_5BIT		0x00 /* EUCSRA register */
#define MSK_EUSART_TX_6BIT		0x10
#define MSK_EUSART_TX_7BIT		0x20
#define MSK_EUSART_TX_8BIT		0x30
#define MSK_EUSART_TX_9BIT		0x70
#define MSK_EUSART_TX_13BIT		0x80
#define MSK_EUSART_TX_14BIT		0x90
#define MSK_EUSART_TX_15BIT		0xA0
#define MSK_EUSART_TX_16BIT		0xB0
#define MSK_EUSART_TX_17BIT		0xF0

#define MSK_EUSART_RX_5BIT		0x00
#define MSK_EUSART_RX_6BIT		0x01
#define MSK_EUSART_RX_7BIT		0x02
#define MSK_EUSART_RX_8BIT		0x03
#define MSK_EUSART_RX_9BIT		0x07
#define MSK_EUSART_RX_13BIT		0x08
#define MSK_EUSART_RX_14BIT		0x09
#define MSK_EUSART_RX_15BIT		0x0A
#define MSK_EUSART_RX_16BIT		0x0B
#define MSK_EUSART_RX_1617BIT	0x0E
#define MSK_EUSART_RX_17BIT		0x0F

#define MSK_EUSART_BODR			0x01 /* EUCSRB register */
#define MSK_EUSART_EMCH			0x02
#define MSK_EUSART_EUSBS		0x08
#define MSK_EUSART_EUSART		0x10

#define MSK_EUSART_STP			0x03 /* EUCSRC register */
#define MSK_EUSART_F1617		0x04
#define MSK_EUSART_FEM			0x08

//_____ M A C R O S ________________________________________________________

#define Eusart_set_ubrr(fosc,bdr)  { UBRRH  = (U8)((((((((U32)fosc*1000L)<<1)/((U32)bdr*16))+1)>>1)-1)>>8); \
                                     UBRRL  = (U8)(( (((((U32)fosc*1000)<<1)/((U32)bdr*16))+1)>>1)-1)    ; }
#define Eusart_set_mubrr(fosc,bdr) { MUBRRH = (U8)(((((((U32)fosc*1000L)<<1)/((U32)(bdr)))+1)>>1)>>8); \
                                     MUBRRL = (U8)( (((((U32)fosc*1000)<<1)/((U32)(bdr)))+1)>>1)    ; }
    // ----------
#define Eusart_clear()           { UCSRB=0; EUCSRB=0; UCSRC=UDR; EUCSRC=EUDR; UCSRA=0x40; EUCSRA=0x33; \
                                   UCSRC=0x06; EUCSRC=0; UBRRH=0; UBRRL=0; MUBRRH=0; MUBRRL=0;         }
    // ----------
#define Eusart_enable()          ( EUCSRB |=  MSK_EUSART_EUSART )
#define Eusart_disable()         ( EUCSRB &= ~MSK_EUSART_EUSART )
#define Eusart_tx_complete()     ( UCSRA  &   MSK_UART_TX_COMPLETE )
#define Eusart_set_tx_busy()
#define Eusart_put_msb(c)        ( EUDR = c )
#define Eusart_put_lsb(c)        ( UDR  = c )
#define Eusart_rx_ready()        ( UCSRA & MSK_UART_RX_DONE )
#define Eusart_tx_buffer_ready() ( UCSRA & MSK_UART_DRE )
#define Eusart_get_msb()         ( EUDR )
#define Eusart_get_lsb()         ( UDR  )

    // ----------
#define Eusart_ack_rx()
#define Eusart_ack_tx()                 ( UCSRA &= ~MSK_UART_TX_COMPLETE )

    // ----------
#define Eusart_enable_rx()              ( UCSRB |= MSK_UART_ENABLE_RX )
#define Eusart_enable_tx()              ( UCSRB |= MSK_UART_ENABLE_TX )

    // ----------
#define Eusart_disable_rx()             ( UCSRB &= ~MSK_UART_ENABLE_RX )
#define Eusart_disable_tx()             ( UCSRB &= ~MSK_UART_ENABLE_TX )

    // ----------
#define Eusart_set_size(n_tx,n_rx)      { EUCSRA = 0; EUCSRA |= n_tx; EUCSRA |= n_rx; }

    // ----------
#define Eusart_enable_rx_interrupt()    ( UCSRB |=  MSK_UART_RXCIE )
#define Eusart_disable_rx_interrupt()   ( UCSRB &= ~MSK_UART_RXCIE )
#define Eusart_enable_udre_interrupt()  ( UCSRB |=  MSK_UART_UDRIE )
#define Eusart_disable_udre_interrupt() ( UCSRB &= ~MSK_UART_UDRIE )
#define Eusart_enable_tx_interrupt()    ( UCSRB |=  MSK_UART_TXCIE )
#define Eusart_disable_tx_interrupt()   ( UCSRB &= ~MSK_UART_TXCIE )

    // ----------
#define Eusart_enable_manchester()      ( EUCSRB |=  MSK_EUSART_EMCH )
#define Eusart_disable_manchester()     ( EUCSRB &= ~MSK_EUSART_EMCH )
#define Eusart_enable_msb_first()       ( EUCSRB |=  MSK_EUSART_BODR )
#define Eusart_enable_lsb_first()       ( EUCSRB &= ~MSK_EUSART_BODR )

    // ----------
#define Uart_set_parity(par)            { UCSRC  &= ~MSK_UART_UPM; UCSRC |= par; }
#define Uart_set_one_tx_stop()          ( UCSRC  &= ~MSK_UART_USBS )
#define Uart_set_two_tx_stop()          ( UCSRC  |=  MSK_UART_USBS )
#define Eusart_set_one_rx_stop()        ( EUCSRB &= ~MSK_EUSART_EUSBS )
#define Eusart_set_two_rx_stop()        ( EUCSRB |=  MSK_EUSART_EUSBS )

    // ----------
#define Eusart_get_parity_error()       ( UCSRA  & MSK_UART_UPE)
#define Eusart_get_ovr_error()          ( UCSRA  & MSK_UART_DOR)
#define Eusart_get_fe_error()           ( UCSRA  & MSK_UART_FE )
#define Eusart_get_fem_error()          ((EUCSRC & MSK_EUSART_FEM) << 2 )
#define Eusart_get_stp()                ( EUCSRC & MSK_EUSART_STP)

    // ----------
#define Eusart_get_f1617()              ((EUCSRC & MSK_EUSART_F1617) << 4 )
#define Eusart_get_bit17()              ((UCSRB  & MSK_UART_RX_BIT9) << 6 )


//__________________________________________________________________________

#endif  // _EUSART_DRV_H_

