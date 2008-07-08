//**************************************************************************
//! @file eusart_lib.c,v
//!
//! Copyright (c) 2005 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains EUSART lib routines for all ATMEL PWM devices.
//!
//! This template file can be parsed by Doxygen for automatic documentation
//! generation.
//! Put here the functional description of this file within the software
//! architecture of your program.
//!
//! @version 1.11 pwm3-ac-ctrl-motor-0_0_6
//!
//! @todo
//! @bug
//**************************************************************************

//_____ I N C L U D E S ____________________________________________________

#include "config.h"
#include "lib_mcu\eusart\eusart_lib.h"
#include "lib_mcu\eusart\eusart_drv.h"


//_____ G L O B A L    D E F I N I T I O N _________________________________

//                  / [bit-7) = bit17
//                  | [bit-6) = f1617
//                  | [bit-5) = fem error
// eusart_rx_error <  [bit-4) = fe error
//                  | [bit-3) = dor error
//                  | [bit-2) = parity error
//                  | [bit-1) = stp1
//                  \ [bit-0) = stp0
U8 eusart_rx_error;

//_____ D E F I N I T I O N ________________________________________________

//_____ M A C R O S ________________________________________________________

//_____ F U N C T I O N S __________________________________________________

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
//--------------------------------------------------------------------------
Bool eusart_init (U8 nbbit_tx, U8 nbbit_rx, t_encoding mode, \
                  t_bit_order order, U16 fosc, U16 bdr, t_parity_type parity, \
                  U8 stop_tx, U8 stop_rx)
{
   //- Clear EUSART
    Eusart_clear();

    //- Clear all global status flag
    eusart_rx_error = 0x00 ;

    //- Config EUSART
    Eusart_enable();

    //- Mode
    if (mode==MANCHESTER) { Eusart_enable_manchester();  }
    else                  { Eusart_disable_manchester(); }

    //- Data bits order
    if (order==MSB_FIRST) { Eusart_enable_msb_first(); }
    else                  { Eusart_enable_lsb_first(); }

    //- Config
    Eusart_set_mubrr(fosc, bdr);  // Manchester decoder counter
    Eusart_set_ubrr(fosc,  bdr);   // Baudrate generator

    //- Size
    Eusart_set_size(nbbit_tx, nbbit_rx);

    //- Parity
    if (mode!=MANCHESTER) { Uart_set_parity(parity); }

    //- Stop bit Tx
    if (stop_tx==1) { Uart_set_one_tx_stop(); }
    else            { Uart_set_two_tx_stop(); }

    //- Stop bit Rx
    if (stop_rx==1) { Eusart_set_one_rx_stop(); }
    else            { Eusart_set_two_rx_stop(); }

   //- Rx enable and Tx Enable
    Eusart_enable_rx();
    Eusart_enable_tx();

    return TRUE;
}

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
//--------------------------------------------------------------------------
void eusart_clear(void)
{
    Eusart_clear();
}

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
//--------------------------------------------------------------------------
void eusart_disable(void)
{
    U16 dummy;

    Eusart_disable_tx();
    while(Eusart_rx_ready())    // Empty receive buffer
    {
        dummy=eusart_get();
        dummy=~dummy;           // To be accepted by compiler
    }
    Eusart_ack_tx();

    //- Rx disable, Tx disable
    Eusart_disable_rx();
    Eusart_disable_tx();

    //- Config EUSART
    Eusart_disable();
}

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
void eusart_enable(void)
{
    eusart_rx_error = 0x00 ;
    Eusart_ack_tx();

    //- Config EUSART
    Eusart_enable();

    //- Rx enable and Tx Enable
    Eusart_enable_rx();
    Eusart_enable_tx();
}

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
//--------------------------------------------------------------------------
Bool eusart_tx_done(void)
{
    if (Eusart_tx_complete()!=0)
    {
        Eusart_ack_tx();
        return TRUE;
    }
    else return FALSE;
}


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
//--------------------------------------------------------------------------
U16 eusart_put(U16 ch)
{
    Union16 c;

    c.w = ch;
#ifndef EUSART_USED_FOR_DALI
    while(!Eusart_tx_buffer_ready());
#endif
    Eusart_put_msb(MSB(c));     // Even if <= 8-bit data
    Eusart_put_lsb(LSB(c));
    return ch;
}

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
//--------------------------------------------------------------------------
Bool eusart_rx_ready(void)
{
    if (Eusart_rx_ready()!=0) return TRUE;
    else return FALSE;
}


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
//--------------------------------------------------------------------------
U16 eusart_get(void)
{
    U16 eusart_data;
    //- Wait RxC
#ifndef EUSART_USED_FOR_DALI
    while(!Eusart_rx_ready());
#endif
    //- Read Stop bits
    eusart_rx_error = Eusart_get_stp();
    //- Capture all error flags
    eusart_rx_error |= Eusart_get_parity_error();   // Parity error
    eusart_rx_error |= Eusart_get_ovr_error();      // Data overrun error
    eusart_rx_error |= Eusart_get_fe_error();       // Level framing error
    eusart_rx_error |= Eusart_get_fem_error();      // Manchester framing error
    //- Read F16 interceptor ... then bit 17
    eusart_rx_error |= Eusart_get_f1617();
    eusart_rx_error |= Eusart_get_bit17();

    //- Read data buffer
    eusart_data  = (U16)(Eusart_get_msb() << 8);
    eusart_data |= (U16)(Eusart_get_lsb());

    return eusart_data;
}
