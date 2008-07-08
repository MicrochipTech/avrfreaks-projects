//**************************************************************************
//! @file dali_lib.c,v
//!
//! Copyright (c) 2005 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains DALI lib routines available for DEVICE TYPE 0.
//!
//! This template file can be parsed by Doxygen for automatic documentation
//! generation.
//! Put here the functional description of this file within the software
//! architecture of your program.
//!
//! @version 1.1 pwm3-ac-ctrl-motor-0_0_6
//!
//! @todo
//! @bug
//**************************************************************************

//_____ I N C L U D E S ____________________________________________________

#include "config.h"
#include "lib_mcu\dali\dali_code.h"
#include "lib_mcu\dali\dali_lib.h"

extern volatile U16 fosc;

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
//--------------------------------------------------------------------------
Bool dali_slave_init(void)
{
  return Dali_slave_init(fosc);
}

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
//--------------------------------------------------------------------------
void dali_slave_disable(void)
{
    Dali_slave_disable();
}

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
//--------------------------------------------------------------------------
void dali_slave_enable(void)
{
    Dali_slave_enable(fosc);
}

//--------------------------------------------------------------------------
//  @fn dali_slave_cmd
//!
//! This function takes a DALI SLAVE descriptor, analyses the action to do:
//! transmit, receive or abort (or nothing).
//! This function returns a status DALI_CMD_ACCEPTED or DALI_CMD_REFUSED)
//! if some problem occurs, the application must be retry later.
//!
//! @param  st_dali_slave_mess_t* - pointer on DALI SLAVE message descriptor
//!                                 structure to select the action to do.
//!
//! @return DALI_CMD_ACCEPTED - command is accepted
//!         DALI_CMD_REFUSED  - command is refused
//!
//--------------------------------------------------------------------------
U8 dali_slave_cmd (st_dali_slave_mess_t*  st_mess)
{
    switch (st_mess->cmd)
    {
        case CMD_NONE:
            return DALI_CMD_ACCEPTED;
        break;

        case CMD_RX_CMD:
            st_mess->handle = 0;
            //- 'st_mess->cmd' must be initialized by the main application
            //         program with one of the commands proposed in "dali_lib.h".
            //- 'st_mess->short_add' must be initialized by the main application
            //         program with the a 6-bit number 'SHORT_ADDRESS' as
            //         described in the table of variables of DALI standard.
            //- 'st_mess->group' must be initialized by the main application
            //         program with the concatenation of 'GROUP 8-15' and
            //         'GROUP 0-7' as described in the table of variables of
            //         DALI standard.
            st_mess->g_code = NO_DALI_COMMAND_CODE;
            st_mess->p_cc = 0;      // Clear "Previous Command Code"
            dali_slave_enable();
            return DALI_CMD_ACCEPTED;
        break;

        case CMD_ABORT:
            dali_slave_disable();
            st_mess->handle = 0;
            st_mess->g_code = NO_DALI_COMMAND_CODE;
            st_mess->p_cc = 0;
            return DALI_CMD_ACCEPTED;
        break;

        case CMD_TX_ANSWER:
            st_mess->handle = 0;
            st_mess->g_code = NO_DALI_COMMAND_CODE;
            st_mess->p_cc = 0;
            //- 'st_mess->dtr' must be initialized by the main application
            dali_slave_enable();
            Dali_slave_put(st_mess->dtr);
            return DALI_CMD_ACCEPTED;
        break;

        default:
            return DALI_CMD_REFUSED;
        break;
    }

}

//--------------------------------------------------------------------------
//  @fn dali_slave_get_status
//!
//! This function allows to return if the command has been performed or not.
//! In an reception case, all the DALI message is stored in the structure.
//! This function also updates the DALI descriptor status (DALI_TX_COMPLETED,
//! DALI_RX_CODE_xyz, DALI_RX_2ND_CODE_NEEDED_xyz, DALI_RX_QUERY_xyz,
//! DALI_RX_COM_ERROR, DALI_RX_NO_ADD_TYPE, DALI_RX_ADD_OUT_RANGE or
//! DALI_RX_CODE_ERROR).
//!
//! @param  st_dali_slave_mess_t* pointer on DALI SLAVE descriptor structure.
//!
//! @return DALI_STATUS_COMPLETED     - Rx or Tx is completed
//!         DALI_STATUS_NOT_COMPLETED - Rx or Tx is not completed
//!         DALI_STATUS_ERROR         - Error in configuration or in the
//!                                     DALI communication
//!
//--------------------------------------------------------------------------
U8 dali_slave_get_status (st_dali_slave_mess_t* st_mess)
{
    Bool direct_cmd;
    Bool special_cmd;
    U8   code_type;
    U8   add_type;
    U8   rec_add;
    U8   rec_data;
    U16  rec_dali;

//----- 'Handle' incrementation - Used to date DALI communication
    st_mess->handle += 1;   // Time_count increment

//----- CMD_TX_ANSWER ?
    if (st_mess->cmd == CMD_TX_ANSWER)
    {
        st_mess->p_cc = 0;
        //- Get status after CMD_TX_ANSWER
        if (Dali_slave_tx_done()==TRUE)
        {
            st_mess->status = DALI_TX_COMPLETED;
            dali_slave_disable();
            return DALI_STATUS_COMPLETED;
        }
        else return DALI_STATUS_NOT_COMPLETED;
    }

//----- Get DALI communication
    if (Dali_slave_rx_ready())
    {
        rec_dali = Dali_slave_get();  // MSB='address byte', LSB='data byte'
        //- Communication error control
        if ((DALI_SLAVE_RX_ERROR & DALI_RX_COM_ERROR_MASK) != DALI_RX_NO_COM_ERROR)
        {
            st_mess->p_cc = 0;
            st_mess->dtr = DALI_SLAVE_RX_ERROR; // For debugging if needed
            st_mess->status = DALI_RX_COM_ERROR;
            Dali_slave_disable();
            return DALI_STATUS_ERROR;
        }
//        rec_dali = Dali_slave_get();  // MSB='address byte', LSB='data byte'
    }
    else return DALI_STATUS_NOT_COMPLETED;

//----- Direct address_test
    direct_cmd = FALSE;  // Default
    if ((rec_dali & 0x0100) == 0) direct_cmd = TRUE;

//----- Short_Group_Broadcast_Address_Test
    add_type = NO_ADD_TYPE;  // Default
    special_cmd = FALSE;     // Default
    rec_add = (rec_dali >> 9);

    // Care with 'rec_add': 7-bit variable (rec_add = (rec_dali >> 9))
    if ((rec_add & 0x40) == 0) add_type = SHORT_ADD_TYPE;
    else
    {
        // Care with 'rec_add': 7-bit variable
        if ((rec_add & 0x70) == 0x40) add_type = GROUP_ADD_TYPE;
        else
        {
            // Care with 'rec_add': 7-bit variable
            if ((rec_add & 0x7F) == 0x7F) add_type = BROAD_ADD_TYPE;
            else
            {
                // Special commands are broadcast commands
                // Care with 'rec_add': 7-bit variable
                if ( (((rec_add & 0x70) == 0x50) || ((rec_add & 0x70) == 0x60)) && (direct_cmd == FALSE) )
                {
                    add_type = BROAD_ADD_TYPE;
                    special_cmd = TRUE;
                }
                else
                {
                    st_mess->p_cc = 0;
                    st_mess->dtr = rec_add;     // For debugging if needed, care: 7-bit variable
                    st_mess->status =  DALI_RX_NO_ADD_TYPE;
                    Dali_slave_disable();
                    return DALI_STATUS_ERROR;
                }
            }
        }
    }

//----- Address_Range_Test
    if (add_type != BROAD_ADD_TYPE)
    {
        if (add_type == SHORT_ADD_TYPE)
        {
            // Care with 'rec_add': 7-bit variable
            if ((st_mess->short_add) != rec_add)
            {
                st_mess->p_cc = 0;
                st_mess->dtr = rec_add;     // For debugging if needed
                st_mess->status =  DALI_RX_ADD_OUT_RANGE;
                Dali_slave_disable();
                return DALI_STATUS_ERROR;
            }
        }
        else
        {
            // Care with 'rec_add': 7-bit variable
            if (((st_mess->group) & (1 << (rec_add & 0x0F))) == 0)
            {
                st_mess->p_cc = 0;
                st_mess->dtr = rec_add & 0x0F;     // For debugging if needed
                st_mess->status = DALI_RX_ADD_OUT_RANGE;
                Dali_slave_disable();
                return DALI_STATUS_ERROR;
            }
        }
    }

//----- Get-Command_Code (c.f. dali_lib.h file)
    rec_data = (U8)(rec_dali & 0x00FF);

    // Output direct arc power control command
    if (direct_cmd == TRUE)
    {
        st_mess->p_cc = 0;
        st_mess->dtr = rec_data;
        st_mess->g_code = DIRECT_ARC_POWER_CONTROL;
        if      (add_type == SHORT_ADD_TYPE) st_mess->status = DALI_RX_CODE_SHORT ;
        else if (add_type == GROUP_ADD_TYPE) st_mess->status = DALI_RX_CODE_GROUP ;
        else                                 st_mess->status = DALI_RX_CODE_BROAD ;
        Dali_slave_disable();
        return DALI_STATUS_COMPLETED;
    }

    if (special_cmd == FALSE)
    {
        // Test: Indirect arc power control commands
        if      (  (rec_data >= ((U8)(IMMEDIATE_OFF & 0xFF))) && (rec_data <= ((U8)(ON_AND_STEP_UP & 0xFF))) )
                {   code_type = CT01; }
        else if ( ((rec_data & 0xF0) == ((U8)(GO_TO_SCENE & 0xFF))) )
                {   code_type = CT02; }
        // Test: Configuration commands (must be received twice, c.f. "CMD_RX_CMD_2ND")
        else if (  (rec_data == ((U8)(RESET & 0xFF))) || (rec_data == ((U8)(STORE_ACTUAL_DIM_LEVEL_IN_DTR & 0xFF))) )
                {   if (rec_dali == st_mess->p_cc) code_type = CT01; else code_type = CT11;}
        else if (  (rec_data >= ((U8)(STORE_THE_DTR_AS_MAX_LEVEL & 0xFF))) && (rec_data <= ((U8)(STORE_THE_DTR_AS_FADE_RATE & 0xFF))) )
                {   if (rec_dali == st_mess->p_cc) code_type = CT01; else code_type = CT11;}
        else if ( ((rec_data & 0xF0) >= ((U8)(STORE_THE_DTR_AS_SCENE & 0xFF))) && ((rec_data & 0xF0) <= ((U8)(REMOVE_FROM_GROUP & 0xFF))) )
                {   if (rec_dali == st_mess->p_cc) code_type = CT02; else code_type = CT12;}
        else if (  (rec_data == ((U8)(STORE_DTR_AS_SHORT_ADDRESS & 0xFF))) )
                {   if (rec_dali == st_mess->p_cc) code_type = CT01; else code_type = CT11;}
        // Test: Query commands (answer needed, c.f. "CMD_TX_ANSWER")
        else if ( (rec_data >= ((U8)(QUERY_STATUS & 0xFF))) && (rec_data <= ((U8)(QUERY_POWER_FAILURE & 0xFF))) )
                {   code_type = CT21; }
        else if (  (rec_data >= ((U8)(QUERY_ACTUAL_LEVEL & 0xFF))) && (rec_data <= ((U8)(QUERY_FADE & 0xFF))) )
                {   code_type = CT21; }
        else if (  ((rec_data & 0xF0) == ((U8)(QUERY_SCENE_LEVEL & 0xFF))) )
                {   code_type = CT22; }
        else if (  (rec_data >= ((U8)(QUERY_GROUPS_0_7 & 0xFF))) && (rec_data <= ((U8)(QUERY_RANDOM_ADDRESS_L & 0xFF))) )
                {   code_type = CT21; }
        else if ( ((rec_data & 0xE0) == ((U8)(QUERY_APPLICATION_EXTENTED_COMMAND & 0xFF))) )
                {   code_type = CT23; }     // For '0xE0', verify Standard Command code for QUERY_APPLICATION_EXTENTED_COMMAND
        else    {   code_type = CT00; }
    }
    else    // special_cmd == TRUE
    {
        // Test: Special commands
        if      (   rec_dali == TERMINATE)
                {   code_type = CT09; }
        else if ( ((rec_dali & 0xFF00) == DATA_TRANSFER_REGISTER) || ((rec_dali & 0xFF00) == INITIALISE) )
                {   code_type = CT0D; }
        else if ( ( rec_dali == RANDOMISE) || ( rec_dali == COMPARE)  || ( rec_dali == WITHDRAW) )
                {   code_type = CT09; }
        else if ( ((rec_dali & 0xFF00) == SEARCHADDRH) || ((rec_dali & 0xFF00) == SEARCHADDRM) || ((rec_dali & 0xFF00) == SEARCHADDRL) )
                {   code_type = CT0D; }
        else if ( ((rec_dali & 0xFF81) == (PROGRAM_SHORT_ADDRESS+1)) || ((rec_dali & 0xFF81) == (VERIFY_SHORT_ADDRESS+1)) )
                {   code_type = CT0C; }
        else if (   rec_dali == QUERY_SHORT_ADDRESS )
                {   code_type = CT29; }
        else if (   rec_dali == PHYSICAL_SELECTION )
                {   code_type = CT09; }
        else if ( ((rec_dali & 0xFF00) == ENABLE_DEVICE_TYPE_X) )
                {   code_type = CT0D; }
        else    {   code_type = CT00; }
    }

//----- Update message structure and return value
    // Update message structure and return value for CT00
    if (code_type == CT00)
    {
        st_mess->g_code = NO_DALI_COMMAND_CODE;
        st_mess->dtr = rec_add;      // For debugging if needed
        st_mess->p_cc = 0;
        st_mess->status = DALI_RX_CODE_ERROR;
        Dali_slave_disable();
        return DALI_STATUS_ERROR;
    }

    // Only update message structure
    switch (code_type & 0x0F)
    {
        case CTX1:
            st_mess->g_code = ((U16)(rec_data)) | 0x0100;
            st_mess->dtr = 0;
        break;

        case CTX2:
            st_mess->g_code = ((U16)(rec_data & 0xF0)) | 0x0100;
            st_mess->dtr = rec_data & 0x0F;
        break;

        case CTX3:
            st_mess->g_code = ((U16)(rec_data & 0xE0)) | 0x0100;     // For '0xE0', verify Standard Command code List
            st_mess->dtr = rec_data & 0x1F;     // For '0x1F', verify Standard Command code List
        break;

        case CTX9:
            st_mess->g_code = rec_dali;
            st_mess->dtr = 0;
        break;

        case CTXC:
            st_mess->g_code = (rec_dali & 0xFF00);
            st_mess->dtr = rec_data >> 1;
        break;

        case CTXD:
            st_mess->g_code = (rec_dali & 0xFF00);
            st_mess->dtr = rec_data;
        break;

        default:    // Ooops! Here some code is necessary
            st_mess->g_code = NO_DALI_COMMAND_CODE;
            st_mess->p_cc = st_mess->dtr = 0;
            st_mess->status = DALI_RX_CODE_ERROR;
            Dali_slave_disable();
            return DALI_STATUS_ERROR;
        break;
    }

    // Only update message structure
    switch (code_type & 0xF0)
    {
        case CT0X:      // Commands
            st_mess->p_cc = 0;   // Kind of reset of previous DALI Command Code
            if      (add_type == SHORT_ADD_TYPE) st_mess->status = DALI_RX_CODE_SHORT;
            else if (add_type == GROUP_ADD_TYPE) st_mess->status = DALI_RX_CODE_GROUP;
            else                                 st_mess->status = DALI_RX_CODE_BROAD;
            Dali_slave_disable();
            return  DALI_STATUS_COMPLETED;
        break;

        case CT1X:      // Configuration
            st_mess->p_cc = rec_dali;   // Recording previous DALI Command Code
            st_mess->handle = 0;        // Reset Time-count
            if      (add_type == SHORT_ADD_TYPE) st_mess->status = DALI_RX_2ND_CODE_NEEDED_SHORT;
            else if (add_type == GROUP_ADD_TYPE) st_mess->status = DALI_RX_2ND_CODE_NEEDED_GROUP;
            else                                 st_mess->status = DALI_RX_2ND_CODE_NEEDED_BROAD;
            return  DALI_STATUS_NOT_COMPLETED;
        break;

        case CT2X:      // Queries
            st_mess->p_cc = 0;   // Kind of reset of previous DALI Command Code
            st_mess->handle = 0;        // Reset Time-count
            if      (add_type == SHORT_ADD_TYPE) st_mess->status = DALI_RX_QUERY_SHORT;
            else if (add_type == GROUP_ADD_TYPE) st_mess->status = DALI_RX_QUERY_GROUP;
            else                                 st_mess->status = DALI_RX_QUERY_BROAD;
            Dali_slave_disable();
            return  DALI_STATUS_COMPLETED;
        break;

        default:    // Ooops! Here some code is necessary
            st_mess->g_code = NO_DALI_COMMAND_CODE;
            st_mess->p_cc = st_mess->dtr = 0;
            st_mess->status = DALI_RX_CODE_ERROR;
            Dali_slave_disable();
            return DALI_STATUS_ERROR;
        break;
    }
    // Usually, unreachable code !
    Dali_slave_disable();
    return DALI_STATUS_ERROR;
}
