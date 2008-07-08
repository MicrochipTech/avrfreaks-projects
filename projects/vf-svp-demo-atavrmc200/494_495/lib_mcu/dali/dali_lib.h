//**************************************************************************
//! @file dali_lib.h,v
//!
//! Copyright (c) 2005 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains all prototypes and macros of DALI lib routines
//!        for DEVICE TYPE 0.
//!
//! @version 1.1 pwm3-ac-ctrl-motor-0_0_6
//!
//! @todo
//! @bug
//**************************************************************************


#ifndef _DALI_LIB_H_
#define _DALI_LIB_H_

// _____(mapped on EUSART)__________________________________________________
#include "lib_mcu/eusart/eusart_drv.h"
#include "lib_mcu/eusart/eusart_lib.h"

//_____ C O N F I G U R A T I O N __________________________________________

//_____ D E F I N I T I O N S ______________________________________________

// @brief This constant is the communication error control byte without error.
#define DALI_RX_NO_COM_ERROR      0x03
#define DALI_RX_COM_ERROR_MASK    0XEF    // FE Error may appears

// @brief This constant is used as return value for "can_cmd" function.
#define DALI_CMD_REFUSED          0xFF
// ----------
// @brief This constant is used as return value for "can_cmd" function.
#define DALI_CMD_ACCEPTED         0x00

// ----------
// @brief This constant is used as return value for "can_get_status" function.
#define DALI_STATUS_COMPLETED     0x00
// ----------
// @brief This constant is used as return value for "can_get_status" function.
#define DALI_STATUS_NOT_COMPLETED 0x01
// ----------
// @brief This constant is used as return value for "can_get_status" function.
#define DALI_STATUS_ERROR         0x02
// ----------
// @brief These constants are used to define the type of DALI address.
#define SHORT_ADD_TYPE            0x00
#define GROUP_ADD_TYPE            0x40
#define BROAD_ADD_TYPE            0x7F
#define NO_ADD_TYPE               0xFF
// ----------
// @brief These constants are used to define the type of DALI command code.
#define CT00    0x00    // Reserved Command Code or No Command Code, st_mess->dtr=0
#define CT01    0x01    // st_mess->code=( rec_data|0x0100), st_mess->dtr=0
#define CT02    0x02    // st_mess->code=((rec_data&0xF0)|0x0100), st_mess->dtr=(rec_data&0x0F)
#define CT11    0x11    // st_mess->code=( rec_data|0x0100), st_mess->dtr=0, must be received twice
#define CT12    0x12    // st_mess->code=((rec_data&0xF0)|0x0100), st_mess->dtr=(rec_data&0x0F), must be received twice
#define CT21    0x21    // st_mess->code=( rec_data|0x0100), st_mess->dtr=0, query command
#define CT22    0x22    // st_mess->code=((rec_data&0xF0)|0x0100), st_mess->dtr=(rec_data&0x0F), query command
#define CT23    0x23    // st_mess->code=((rec_data&0xC0)|0x0100), st_mess->dtr=(rec_data&0x3F), query command
#define CT09    0x09    // st_mess->code=( rec_dali), st_mess->dtr=0
#define CT0C    0x0C    // st_mess->code=( rec_dali&0xFF00), st_mess->dtr=(rec_data>>1)
#define CT0D    0x0D    // st_mess->code=( rec_dali&0xFF00), st_mess->dtr=rec_data
#define CT29    0x29    // st_mess->code=( rec_dali), st_mess->dtr=0, query command
    // -----
#define CTX1    0x01    // Only for the update of the message structure
#define CTX2    0x02    // Only for the update of the message structure
#define CTX3    0x03    // Only for the update of the message structure
#define CTX9    0x09    // Only for the update of the message structure
#define CTXC    0x0C    // Only for the update of the message structure
#define CTXD    0x0D    // Only for the update of the message structure
    // -----
#define CT0X    0x00    // Only for RX_CODE return value
#define CT1X    0x10    // Only for 2ND_CODE_NEEDED return value
#define CT2X    0x20    // Only for RX_QUERYreturn value

// ----------
// @brief This enumeration is used to select an action for a specific message
// declared in st_dali_slave_mess_t structure.
typedef enum {
    CMD_NONE,
    CMD_RX_CMD,
    CMD_TX_ANSWER,
    CMD_ABORT
} en_com_cmd_t;

// ----------
// @brief This enumeration is used to flag "dali_slave_get_status()" operation
// in st_dali_slave_mess_t structure.
typedef enum {
    NO_STATUS,
    DALI_TX_COMPLETED,
    DALI_RX_CODE_SHORT,
    DALI_RX_CODE_GROUP,
    DALI_RX_CODE_BROAD,
    DALI_RX_2ND_CODE_NEEDED_SHORT,
    DALI_RX_2ND_CODE_NEEDED_GROUP,
    DALI_RX_2ND_CODE_NEEDED_BROAD,
    DALI_RX_QUERY_SHORT,
    DALI_RX_QUERY_GROUP,
    DALI_RX_QUERY_BROAD,
    DALI_RX_COM_ERROR,
    DALI_RX_NO_ADD_TYPE,
    DALI_RX_ADD_OUT_RANGE,
    DALI_RX_CODE_ERROR
} en_status_t;

// ----------
// @brief This message descriptor structure allows to define a specific
//        action on DALI network for a SLAVE.
//
//  1) handle:    Time_count
//  2) cmd:       initialize by the application to select the operation.
//  3) short_add: initialize by the application to filter the short_address,
//                it is the "short_address" variable defined in DALI Standard.
//  4) group:     initialize by the application as address mask, it is the
//                "group_8_15,group_0_7" variable(s) defined in DALI Standard.
//                (In this 16-bit data, each bit = '1' enabled a group number,
//                bit-0 for group_0,  bit-1 for group_1, etc...)
//  5) code:      c.f. DALI command code definitions in "dali_code.h".
//  6) p_cc:      "Previous Command Code" used to verify the 2ND_CODE_NEEDED.
//  7) dtr:       DALI "Data Transfert Register" for data received or data to
//                send.
//  8) status:    Dali communication status
typedef  struct{
    U8           handle;
    en_com_cmd_t cmd;
    U8           short_add;
    U16          group;
    U16          g_code;
    U16          p_cc;
    U8           dtr;
    en_status_t  status;
} st_dali_slave_mess_t;

//_____ D E C L A R A T I O N ___(mapped on EUSART)_________________________
#define DALI_SLAVE_RX_ERROR   eusart_rx_error

//_____ M A C R O S _____(mapped on EUSART)_________________________________
#define Dali_slave_init(fosc) ( eusart_init(MSK_EUSART_TX_8BIT,MSK_EUSART_RX_16BIT, MANCHESTER, \
                                            MSB_FIRST, fosc, 1200, PARITY_NONE, 2, 2) )
//#define Dali_slave_disable()  ( eusart_disable() )
#define Dali_slave_disable()    ( eusart_clear() )
//#define Dali_slave_enable()   ( eusart_enable()  )
#define Dali_slave_enable(fosc) ( eusart_init(MSK_EUSART_TX_8BIT,MSK_EUSART_RX_16BIT, MANCHESTER, \
                                              MSB_FIRST, fosc, 1200, PARITY_NONE, 2, 2) )
#define Dali_slave_tx_done()    ( eusart_tx_done() )
#define Dali_slave_rx_ready()   ( eusart_rx_ready())
#define Dali_slave_get()        ( eusart_get()     )
#define Dali_slave_put(u8)      ( eusart_put((U16)(u8)))

//_____ P R O T O T Y P E S ________________________________________________

//--------------------------------------------------------------------------
//  @fn dali_slave_init
//!
//! DALI SLAVE peripheral initialization. Reset and initialize the peripheral
//! and enable it.
//!
//! @warning none
//!
//! @param  none
//!
//! @return Status
//!         ==TRUE:  function failed
//!         ==FALSE: function performed
//!
extern Bool dali_slave_init(void);

//--------------------------------------------------------------------------
//  @fn dali_slave_disable
//!
//! DALI SLAVE peripheral disable. Disable and clean out the receive buffer.
//!
//! @warning none
//!
//! @param  none
//!
//! @return none
//!
extern void dali_slave_disable(void);

//--------------------------------------------------------------------------
//  @fn dali_slave_enable
//!
//! DALI SLAVE peripheral enable. Enable without initializatio.
//!
//! @warning Previous 'dali_slave_init()' must be done before
//!
//! @param  none
//!
//! @return none
//!
extern void dali_slave_enable(void);

//--------------------------------------------------------------------------
//  @fn dali_slave_cmd
//!
//! This function takes a DALI SLAVE descriptor, analyses the action to do:
//! transmit, receive or abort.
//! This function returns a status (CAN_CMD_ACCEPTED or CAN_CMD_REFUSED) if
//! a MOb for Rx or Tx has been found. If no MOB has been found, the
//! application must be retry at a later date.
//! This function also updates the CAN descriptor status (MOB_PENDING or
//! MOB_NOT_REACHED) if a MOb for Rx or Tx has been found. If aborting
//! is performed, the CAN descriptor status will be set to STATUS_CLEARED.
//!
//! @param  st_dali_slave_mess_t* - Can_descriptor pointer on DALI SLAVE
//!         descriptor structure to select the action to do.
//!
//! @return CAN_CMD_ACCEPTED - command is accepted
//!         CAN_CMD_REFUSED  - command is refused
//!
extern U8 dali_slave_cmd (st_dali_slave_mess_t *);

//--------------------------------------------------------------------------
//  @fn dali_slave_get_status
//!
//! This function allows to return if the command has been performed or not.
//! In an reception case, all the CAN message is stored in the structure.
//! This function also updates the CAN descriptor status (MOB_TX_COMPLETED,
//!  MOB_RX_COMPLETED, MOB_RX_COMPLETED_DLCW or MOB_???_ERROR).
//!
//! @param  st_dali_slave_mess_t* pointer on DALI SLAVE descriptor structure.
//!
//! @return CAN_STATUS_COMPLETED     - Rx or Tx is completed
//!         CAN_STATUS_NOT_COMPLETED - Rx or Tx is not completed
//!         CAN_STATUS_ERROR         - Error in configuration or in the
//!                  CAN communication
//!
extern U8 dali_slave_get_status (st_dali_slave_mess_t *);


//__________________________________________________________________________

#endif // _DALI_LIB_H_
