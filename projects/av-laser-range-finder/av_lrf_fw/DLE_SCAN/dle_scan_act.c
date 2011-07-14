/***C*********************************************************************************************
**
** SRC-FILE     :   dle_scan_act.c
**                                        
** PROJECT      :   DLE SCAN
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   2011-05-09
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   Set of primitive actions such as laser on/off, SPI raw access, LCD control, etc 
**                  to control DLE behavior and measuremnts
**                  
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2011
**
****C*E******************************************************************************************/
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "dle_typedef.h"
#include "dle_scan.h"
#include "dle_scan_meas.h"
#include "dle_scan_lcd.h"
#include "dle_scan_act.h"

//------ Global variables ---------------------

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Sub function. 
**                  Turn off APD and laser, then sets modulator to the initial state
**
***C*F*E**********************************************************************/
void disable_modulator()
{
    // Read SPI current state
    spi_read_curr();

    spi_set_byte(0x16, 0x76);

    // Init APD bias if not zero 0x12 != 0x10 0x00
    if ( (guca_spi_curr_state[9]  != 0x10) ||
         (guca_spi_curr_state[10] != 0x00) )
    {  
        spi_set_2bytes(0x12, 0x10, 0);
    }

    // Disable X3 if set
    if ((guca_spi_curr_state[3] & 0xC0) == 0xC0)
    {   // 0x06 &= ~0xC0;
        guca_spi_curr_state[3] &= ~0xC0;
        spi_set_byte(0x06, guca_spi_curr_state[3]);          // F4->34
    }

    // Disable modulator & laser if enabled             
    if (guca_spi_curr_state[8] != 0x00)
    {   // 0x10 = 0x00;
        spi_set_byte(0x10, 0x00);                            // 66->00
        guca_spi_curr_state[3] &= ~0x10;
        spi_set_byte(0x06, guca_spi_curr_state[3]);          // 34->24
    }

    // Disable APD bias if not disabled (0x06 & 0x20) != 0
    if ((guca_spi_curr_state[3] & 0x20) != 0)
    {   // 0x06 &= ~0x20;       
        spi_set_byte(0x06, (guca_spi_curr_state[3] & ~0x20));  // 24 -> 04
    }

                                                             // 76->F6
    spi_set_byte(0x16, 0xF6);

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Writes initial data to the modulator's SPI registers
**
***C*F*E**********************************************************************/
void init_modulator()
{
    uint8 uc_i, uc_feedback;
        
    guca_spi_curr_state[0x00] = 0x02;
    guca_spi_curr_state[0x01] = 0xC4;
    guca_spi_curr_state[0x02] = 0x93;
    guca_spi_curr_state[0x03] = 0x04;
    guca_spi_curr_state[0x04] = 0xF0;
    guca_spi_curr_state[0x05] = 0x78;
    guca_spi_curr_state[0x06] = 0x0F;
    guca_spi_curr_state[0x07] = 0x0F;
    guca_spi_curr_state[0x08] = 0x00;
    guca_spi_curr_state[0x09] = 0x10;
    guca_spi_curr_state[0x0A] = 0x00;
    guca_spi_curr_state[0x0B] = 0xF6;
    guca_spi_curr_state[0x0C] = 0x80;
    guca_spi_curr_state[0x0D] = 0x3C;

    SPI_PORT &= ~_BV(SPI_CS_PIN);

    // Send raw data, get feedback and write to message
    for (uc_i = 0; uc_i < SPI_MAX_LEN; uc_i++){
        SEND_SPI_BYTE(uc_feedback, guca_spi_curr_state[uc_i]);
    }

    SPI_PORT |= _BV(SPI_CS_PIN);

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Command function
**                  RAW SPI read access. 
**
***C*F*E**********************************************************************/
void action_spi_rd(){

    uint8 uc_spi_len, uc_i;
    uint8 uc_feedback;

    // Command
    // [1]     [2]      [3]
    // 0x15    0x01     LL
    // CMD     Length   Read request len 0..14

    // response 
    // [0]  [1]     [2]    ... [LL+2]
    // 0x16 LL+1    DD
    // CMD  Length  Data

   
    // wait until full message received
    while(guc_in_msg_idx < 4);

    // --- complete message received here ---
    uc_spi_len = guca_in_msg_buff[3];

    // Write message header
    guca_out_msg_buff[0] = 0x16;
    guca_out_msg_buff[1] = uc_spi_len+1;

    SPI_PORT &= ~_BV(SPI_CS_PIN);

    SEND_SPI_BYTE(uc_feedback, 0x80);
    guca_out_msg_buff[2] = uc_feedback;

    // Send raw data, get feedback and write to message
    for (uc_i = 0; uc_i < uc_spi_len; uc_i++){
        SEND_SPI_BYTE(uc_feedback, 0x00);
        guca_out_msg_buff[uc_i+3] = uc_feedback;
    }

    SPI_PORT |= _BV(SPI_CS_PIN);

    // Check no more input data received
    if (guc_in_msg_idx != 4)
        FATAL_TRAP(__LINE__);

    // --- SPI transaction finished. Send feedback upward ---
    guc_out_msg_wr_idx = uc_i + 3;

    // End of action_spi_rd
    return;
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Command function
**                  RAW SPI write access. 
**
***C*F*E**********************************************************************/
void action_spi_wr(){

    uint8 uc_spi_len, uc_i;
    uint8 uc_feedback;

    // Command
    // [1]    [2]                   [3] ... [LL+2]
    // 0x15   LL                    DD
    // CMD    Data Length  0..14    Data
    
    // response 
    // [0]    [1]                   [2]    ... [LL+1]
    // 0x15   LL                    DD
    // CMD    Response Length       SPI feedback

    // Check is message length available, wait if not yet
    while(guc_in_msg_idx < 3);

    uc_spi_len = guca_in_msg_buff[2];

    // Check is complete message received
    while(guc_in_msg_idx < (uint8)(uc_spi_len+3));

    // --- complete message received here ---

    // Write message header
    guca_out_msg_buff[0] = 0x15;
    guca_out_msg_buff[1] = uc_spi_len;

    SPI_PORT &= ~_BV(SPI_CS_PIN);                \

    // Send raw data, get feedback and write to message
    for (uc_i = 0; uc_i < uc_spi_len; uc_i++){
        SEND_SPI_BYTE(uc_feedback, guca_in_msg_buff[uc_i+3]);
        guca_out_msg_buff[uc_i+2] = uc_feedback;
    }

    SPI_PORT |= _BV(SPI_CS_PIN);                 \

    // Check no more input data received
    if (guc_in_msg_idx != (uint8)(uc_spi_len+3))
        FATAL_TRAP(__LINE__);

    // --- SPI transaction finished. Send feedback upward ---
    guc_out_msg_wr_idx = uc_i + 2;

    // End of action_spi_wr
    return;
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Command function
**                  Responds with firmware version and name
**
***C*F*E**********************************************************************/
void action_signature(){

    uint8 uc_i;

    // Command
    // [1]    
    // 0x11   
    // CMD    
    
    // response 
    // [0]    [1]                   [2]     [3]     [4]   ... [LL+2]
    // 0x11   LL                    0xMM    0xMM    0xCC
    // CMD    Response Length       Major   Minor   String name

    // Write header & length
    guca_out_msg_buff[0] = 0x11;
    guca_out_msg_buff[1] = SIGN_LEN+2;

    // Write version number
    guca_out_msg_buff[2] = gt_hw_info.uc_ver_maj;
    guca_out_msg_buff[3] = gt_hw_info.uc_ver_min;

    // Write signature
    for(uc_i = 0; uc_i < SIGN_LEN; uc_i++){
        guca_out_msg_buff[uc_i+4] = gt_hw_info.ca_signature[uc_i];
    }

    // Check no more input data received
    if (guc_in_msg_idx != 2)
        FATAL_TRAP(__LINE__);

    guc_out_msg_wr_idx = SIGN_LEN + 4;

    // End of action_signature
    return;
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Command function
**                  Open/Close the curtain
**
***C*F*E**********************************************************************/
void action_curtain(){

    uint8 uc_act;
    uint8 uc_tmp;


    // Command
    // [1]  [2]    [3]
    // 0x41 0x01   0xDD 
    // CMD  Length ACT
    
    // response 
    // [0]    [1]     [2]
    // 0x41   0x01    0xDD
    // CMD    Length  ACT

    // ACT=0 -> laser curtain open (up)
    // ACT=1 -> laser curtain close (down)
    // ACT=2 -> laser curtain disabled (IO pin disconnected)

    // wait until full message received
    while(guc_in_msg_idx < 4);
    
    uc_act = guca_in_msg_buff[3];

    if (uc_act != 2)
    { // either close or open curtain

        // Prepare IO port and dir registers
        CURTAIN_PORT |= 
             ((1 << CURTAIN_UP_PIN  )|
              (1 << CURTAIN_DOWN_PIN));

        CURTAIN_DIR |= 
             ((1 << CURTAIN_UP_PIN  )|
              (1 << CURTAIN_DOWN_PIN));

        // Disable JTAG (enabled at reset), in order to override IO pins
        uc_tmp = MCUCR | (1 << JTD);
        MCUCR = uc_tmp;
        MCUCR = uc_tmp;

        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
    
        if (uc_act)
        {
            CURTAIN_PORT &= ~(1 << CURTAIN_DOWN_PIN);
        }
        else
        {
            CURTAIN_PORT &= ~(1 << CURTAIN_UP_PIN);
        }

        // wait 50ms
        guc_240usec_timer = (50000/240);
        while(guc_240usec_timer);

        // remove driver signal
        CURTAIN_PORT |= 
            (1 << CURTAIN_UP_PIN  )|
            (1 << CURTAIN_DOWN_PIN);

        // Note: Curtain IO pins remain active, normally should override signal from mkII
        //       In order to return JTAG control IO pins must be disabled
    }
    else
    { // ACT=2. Disable curtain IO pins by enabling JTAG
        uc_tmp = MCUCR & ~(1 << JTD);
        MCUCR = uc_tmp;
        MCUCR = uc_tmp;
    }

    // Send command response to host
    guca_out_msg_buff[0] = 0x41;
    guca_out_msg_buff[1] = 1;
    guca_out_msg_buff[2] = uc_act;
    guc_out_msg_wr_idx = 3;

    // Check no more input data received
    if (guc_in_msg_idx != 4)
        FATAL_TRAP(__LINE__);

    // End of action_curtain
    return;

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Command function
**                  Enable / Disable laser. Just for aiming purposes
**
***C*F*E**********************************************************************/
void action_laser_aim(){

    uint8 uc_action;

    // Command
    // [1]    [2]       [3]
    // 0x32   0x01      0xDD 
    // CMD    Length    ACT
    
    // response 
    // [0]    [1]       [2]
    // 0x32   0x01      0xDD 
    // CMD    Length    Action
    
    // ACT=0 -> Laser OFF
    // ACT=1 -> Laser ON
    // ACT=2 -> Laser Toggle

    // Wait until complete message received
    while(guc_in_msg_idx < 4);

    uc_action = guca_in_msg_buff[3];

    // --- complete message received here ---

    // Write message header
    guca_out_msg_buff[0] = 0x32;
    guca_out_msg_buff[1] = 0x01;


    // Read SPI current state
    spi_read_curr();

    // Get current laser state. Consider 0x10, 0xAB as Enabled, disabled otherwise
    if (uc_action == 2)     // AIM toggle
    {
        if (guca_spi_curr_state[8] == 0xAB)      // Is AIM enabled
        { // Toggle to disable
            uc_action = 0;
        }
        else
        { // Toggle to enable
            uc_action = 1;
        }
    }

    guca_out_msg_buff[2] = uc_action;

    if (uc_action == 1)
    { // AIM enable
        // Enable laser if disabled (0x06 & 0x10) == 0
        if ((guca_spi_curr_state[3] & 0x10) == 0)
        {   // 0x06 |= 0x10;
            spi_set_byte(0x06, guca_spi_curr_state[3] | 0x10);
        }

        // Enable AIM mode if not set
        if (guca_spi_curr_state[8] != 0xAB)
        {   // 0x10 = 0xAB;
            spi_set_byte(0x16, 0x76);
            spi_set_byte(0x10, 0xAB);
            spi_set_byte(0x16, 0xF6);
        }

        LCD_SET_SIGN(14);
        guc_lcd_redraw = 1;

    }
    else
    { // AIM disable
        // Clear AIM mode if not zero
        if (guca_spi_curr_state[8] != 0x00)
        {   // 0x10 = 0x00;
            spi_set_byte(0x16, 0x76);
            spi_set_byte(0x10, 0x00);
            spi_set_byte(0x16, 0xF6);
        }

        // Disable laser if enable (0x06 & 0x10) != 0
        if ((guca_spi_curr_state[3] & 0x10) != 0)
        {   // 0x06 &= ~0x10;
            spi_set_byte(0x06, (guca_spi_curr_state[3] & ~0x10));
        }

        LCD_CLR_SIGN(14);
        guc_lcd_redraw = 1;
    }

    // Check no more input data received
    if (guc_in_msg_idx != 4)
        FATAL_TRAP(__LINE__);

    // --- SPI transaction finished. Send feedback upward ---
    guc_out_msg_wr_idx = 3;

    // End of action_laser_aim
    return;

}
/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Initiates distance continuos measuring by initializing 
**                  the measurement finite state machine which is called from 
**                  the main loop.
**                  Can be stopped by short command only
**                  
***C*F*E**********************************************************************/
void action_scan_start(){

    // Command
    // [1]  [2]     [3]          [5]
    // 0x52 0x04    0xDDDD       0xDDDD       
    // CMD  Length  APD calib    APD measure

    // response 
    // [0]  [1]       
    // 0x52 0x00      
    // CMD  H.Length

    // Wait until complete message received
    while(guc_in_msg_idx < 7);

    // --- complete message received here ---

    guca_out_msg_buff[0] = 0x52;
    guca_out_msg_buff[1] = 0;

    // -----------------------------------------------------------------
    // --- Initialize modulator 
    // -----------------------------------------------------------------
    init_modulator();

    // Wait a little
    guc_240usec_timer = (50000/240);
    while(guc_240usec_timer);

    // -----------------------------------------------------------------
    // --- Prepare for measurement
    // -----------------------------------------------------------------

    // Enable APD bias if not enabled (0x06 & 0x20) == 0
    if ((guca_spi_curr_state[3] & 0x20) == 0)
    {   // 0x06 |= 0x20;    
        guca_spi_curr_state[3] |= 0x20;
        spi_set_byte(0x06, guca_spi_curr_state[3]);         // 04 -> 24

        // Wait a little after APD power ON (50ms)
        guc_240usec_timer = (50000/240);
        while(guc_240usec_timer);

    } 
    
    // Enable laser if disabled (0x06 & 0x10) == 0
    if ((guca_spi_curr_state[3] & 0x10) == 0)
    {   // 0x06 |= 0x10;
        guca_spi_curr_state[3] |= 0x10;                     // 24->34
        spi_set_byte(0x06, guca_spi_curr_state[3]);   
    }

    // wait a little
    guc_240usec_timer = (10000/240);
    while(guc_240usec_timer);

    // Enable modulator if disabled                
    if (guca_spi_curr_state[8] != 0x66)
    {   // 0x10 = 0x66;
        spi_set_byte(0x16, 0x76);                           // 0x00->0xAB->0x66
        spi_set_byte(0x10, 0xAB);
        spi_set_byte(0x16, 0xF6);

        // wait a little
        guc_240usec_timer = (1000/240);
        while(guc_240usec_timer);

        spi_set_byte(0x16, 0x76);
        spi_set_byte(0x10, 0x66);
        spi_set_byte(0x16, 0xF6);
    }

    // wait a little
    guc_240usec_timer = (10000/240);
    while(guc_240usec_timer);

    // Enable X3 if not set
    if ((guca_spi_curr_state[3] & 0xC0) != 0xC0)
    {   // 0x06 |= 0xC0;
        guca_spi_curr_state[3] |= 0xC0;
        spi_set_byte(0x06, guca_spi_curr_state[3]);          // 34->F4
    }

    // wait a little
    guc_240usec_timer = (10000/240);
    while(guc_240usec_timer);

    // Write APD values for calibration and for measurements to meas_info structure
    gt_meas_info.us_calib_bias_ctrl = guca_in_msg_buff[3] | (guca_in_msg_buff[4] << 8);
    gt_meas_info.us_meas_bias_ctrl  = guca_in_msg_buff[5] | (guca_in_msg_buff[6] << 8);

    gt_meas_info.uc_calibration = 1;                         // Start measurememnt from calibration
    gt_meas_info.uc_delay = APD_DELAY;

    gt_meas_info.e_state = MEAS_STATE_IDLE;

    // Check no more input data received
    if (guc_in_msg_idx != 7)
        FATAL_TRAP(__LINE__);

    // --- Action finished. Send feedback upward ---
    guc_out_msg_wr_idx = 2;

#if CURTAIN_ENABLED

    uint8 uc_tmp;

    // Prepare IO port and dir registers
    CURTAIN_PORT |= 
         ((1 << CURTAIN_UP_PIN  )|
          (1 << CURTAIN_DOWN_PIN));

    CURTAIN_DIR |= 
         ((1 << CURTAIN_UP_PIN  )|
          (1 << CURTAIN_DOWN_PIN));


    // Disable JTAG (enabled at reset), in order to override IO pins
    uc_tmp = MCUCR | (1 << JTD);
    MCUCR = uc_tmp;
    MCUCR = uc_tmp;

    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
#endif

    // Init other measure statics
    gt_meas_info.e_apd_mode = APD_MODE_AUTO;

    // End of action_scan_start
    return;

}
/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   LCD control command function
**                  See documentation or "dle_scan_lcd.c" for segement mapping
**                  
***C*F*E**********************************************************************/
void action_lcd(){

    uint8 uc_i;

    // Command                                 
    // [1]  [2]     [3]           [8]           [12]
    // 0x61 0x0E    0xDDDDDDDDDD  0xDDDDDDDDDD  0xDDDDDDDD
    // CMD  Length  Digits set 0  Digits set 1  Signs bitmask

    // response 
    // [0]  [1]       
    // 0x61 0x00      
    // CMD  Length  

    // Wait until complete message received
    while(guc_in_msg_idx < 17);

    // --- complete message received here ---

    // Write response message header
    guca_out_msg_buff[0] = 0x61;
    guca_out_msg_buff[1] = 0x00;

    // write LCD data from input command to LCD internal structures
    for (uc_i = 0; uc_i < LCD_SET_DIGITS_NUM; uc_i++)
    {
        guca_lcd_digits_set0[uc_i] = guca_in_msg_buff[LCD_DIGITS0_INP_IDX + uc_i];    
        guca_lcd_digits_set1[uc_i] = guca_in_msg_buff[LCD_DIGITS1_INP_IDX + uc_i];    
    }

    for (uc_i = 0; uc_i < sizeof(guca_lcd_signs); uc_i ++)
    {
        guca_lcd_signs[uc_i] = guca_in_msg_buff[LCD_SIGNS_INP_IDX+uc_i];
    }

    // refresh LCD
    lcd_render();

    // Check no more input data received
    if (guc_in_msg_idx != 17)
        FATAL_TRAP(__LINE__);

    // --- Action finished. Send feedback upward ---
    guc_out_msg_wr_idx = 2;

}

//
T_ACTION gta_action_table[64] __attribute__ ((section (".text"))) = {
    {"mark", 0x00, (void*)0xFEED        },    // Table signature
    {"sign", 0x11, action_signature     },    // Device signature
    {"spiw", 0x15, action_spi_wr        },    // SPI raw write
    {"spir", 0x16, action_spi_rd        },    // SPI raw read
    {"curt", 0x41, action_curtain       },    // Laser curatain UP/DOWN
    {"laim", 0x32, action_laser_aim     },    // Laser AIM
    {"scan", 0x52, action_scan_start    },    // Start scanning
    {"lcd ", 0x61, action_lcd           },    // LCD control
    {""    , 0xFF, NULL                 }     // End of table
};

