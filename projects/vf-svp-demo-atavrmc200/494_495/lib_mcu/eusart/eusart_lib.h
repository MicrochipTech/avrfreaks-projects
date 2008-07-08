//**************************************************************************
//! @file eusart_lib.h,v
//!
//! Copyright (c) 2005 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains all prototypes and macros of EUSART lib
//!        routines for all ATMEL PWM devices.
//!
//! @version 1.8 pwm3-ac-ctrl-motor-0_0_6
//!
//! @todo
//! @bug
//**************************************************************************


#ifndef _EUSART_LIB_H_
#define _EUSART_LIB_H_

#include "lib_mcu/eusart/eusart_drv.h"

//_____ C O N F I G U R A T I O N _________________________________________

//                  / [bit-7) = bit17
//                  | [bit-6) = f1617
//                  | [bit-5) = fem error
// eusart_rx_error <  [bit-4) = fe error
//                  | [bit-3) = ovr error
//                  | [bit-2) = parity error
//                  | [bit-1) = stp1
//                  \ [bit-0) = stp0
extern U8 eusart_rx_error;

//_____ D E F I N I T I O N S ______________________________________________

typedef enum encoding_mode {LEVEL, MANCHESTER} t_encoding;
typedef enum bit_order {MSB_FIRST, LSB_FIRST} t_bit_order;
typedef enum parity_type {PARITY_NONE=0x00,PARITY_EVEN=0x20, PARITY_ODD=0x30} t_parity_type;


//_____ D E C L A R A T I O N ______________________________________________

//_____ M A C R O S ________________________________________________________

//_____ P R O T O T Y P E S ________________________________________________

//--------------------------------------------------------------------------
//  @fn eusart_init
//!
//! EUSART (& USART) peripheral initialization. Reset both EUSART & UART,
//! initialize the peripheral and enable it.
//!
//! @warning none
//!
//! @param  none
//!
//! @return Status
//!         ==TRUE:  function failed
//!         ==FALSE: function performed
//!
extern Bool eusart_init (U8 nbbit_tx, U8 nbbit_rx, t_encoding mode, \
                         t_bit_order order, U16 fosc, U16 bdr, t_parity_type parity, \
                         U8 stop_tx, U8 stop_rx);

//--------------------------------------------------------------------------
//  @fn eusart_clear
//!
//! EUSART (& USART) peripheral disable. Disable both EUSART & UART writting
//! all registers with their default (reset) value.
//!
//! @warning none
//!
//! @param  none
//!
//! @return none
//!
extern void eusart_clear(void);

//--------------------------------------------------------------------------
//  @fn eusart_disable
//!
//! EUSART (& USART) peripheral disable. Disable both EUSART & UART and
//! clean out the receive buffer.
//!
//! @warning none
//!
//! @param  none
//!
//! @return none
//!
extern void eusart_disable(void) ;

//--------------------------------------------------------------------------
//  @fn eusart_enable
//!
//! EUSART (& USART) peripheral enable. Enable both EUSART & UART.
//!
//! @warning Previous 'eusart_init()' must be done before
//!
//! @param  none
//!
//! @return none
//!
//--------------------------------------------------------------------------
extern void eusart_enable(void) ;

//--------------------------------------------------------------------------
//  @fn eusart_tx_done
//!
//! Control if data has been send by EUSART peripheral.
//!
//! @warning none
//!
//! @param  none
//!
//! @return Status
//!         ==TRUE:  sending performed
//!         ==FALSE: sending on-going
//!
extern Bool eusart_tx_done(void) ;

//--------------------------------------------------------------------------
//  @fn eusart_put
//!
//! Send data on EUSART peripheral.
//!
//! @warning none
//!
//! @param  data to send via EUSART
//!
//! @return data sent
//!
extern U16 eusart_put(U16 uc_wr_byte) ;

//--------------------------------------------------------------------------
//  @fn eusart_rx_ready
//!
//! Control if data is ready EUSART peripheral.
//!
//! @warning none
//!
//! @param  none
//!
//! @return Status
//!         ==TRUE:  data ready
//!         ==FALSE: data NOT ready
//!
extern Bool eusart_rx_ready(void) ;

//--------------------------------------------------------------------------
//  @fn eusart_get
//!
//! Get data from EUSART peripheral and update "eusart_rx_error" global
//! variable.
//!
//! @warning "eusart_rx_error" bit organization (c.f. header)
//!          "return" byte organization
//!
//! @param  none
//!
//! @return read (received) data on EUSART
//!
extern U16 eusart_get(void) ;


//__________________________________________________________________________

#endif // _EUSART_LIB_H_
