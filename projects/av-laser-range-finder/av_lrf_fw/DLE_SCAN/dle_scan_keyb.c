/***C*********************************************************************************************
**
** SRC-FILE     :   dle_scan_keyb.c
**                                        
** PROJECT      :   DLE SCAN
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   2011-05-09
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   Keyboard processing routines
**
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2011
**
****C*E******************************************************************************************/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "dle_typedef.h"
#include "dle_scan.h"
#include "dle_scan_lcd.h"
#include "dle_scan_meas.h"
#include "dle_scan_keyb.h"

#define KEYB_SCAN_MATRIX_ROWS  5
uint8 guca_keyb_scan_res[KEYB_SCAN_MATRIX_ROWS];
uint8 guc_keyb_blocked;

#define KEYB_SCAN0_PIN  PB0
#define KEYB_SCAN1_PIN  PB1
#define KEYB_SCAN2_PIN  PB2
#define KEYB_SCAN3_PIN  PB3
#define KEYB_SCAN4_PIN  PB4

#define KEYB_SCAN_ALL_MASK  \
    (_BV(KEYB_SCAN0_PIN) | \
     _BV(KEYB_SCAN1_PIN) | \
     _BV(KEYB_SCAN2_PIN) | \
     _BV(KEYB_SCAN3_PIN) | \
     _BV(KEYB_SCAN4_PIN))

#define KEYB_SCAN_PORT  PORTB
#define KEYB_SCAN_PIN   PINB
#define KEYB_SCAN_DIR   DDRB

typedef struct keyb_butt_info_tag {
    uint8 uc_row;
    uint8 uc_mask;
    void  (*pf_func)(void);
} T_KEYB_BUTT_INFO;

#define KEYB_BUTTON_NUM 10
extern T_KEYB_BUTT_INFO gta_keyb_info[KEYB_BUTTON_NUM]; // forward declaration

// =======================================
//          Keyboard scan matrix          
// =======================================
//       |  PB4     PB3   PB2   PB1   PB0
// ---------------------------------------
// PB0   |   x     5,10   1,8   4,9.   x 
// PB1   |   x     5,6    1,2    x    7,9.
// PB2   | 1,2,8    3.     x     2.    8.
// PB3   | 5,6,10   x      3     6.   10.
// PB4   |   x      5.     1.    4.    7.
//
// Note: Connecting GND to a row pin and pressing a button from that row 
//       commutes GND to a column pin(s).  
//       GND on PB4 + Button 5 pressed = GND on PB3

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Set scaning lines to Hi-Z state
**                  
***C*F*E**********************************************************************/
void init_keyb(){

    // Set all scan line to Hi-Z
    KEYB_SCAN_DIR &= ~KEYB_SCAN_ALL_MASK;
    KEYB_SCAN_PORT &= ~KEYB_SCAN_ALL_MASK;

    // Just in case
    // Initialy keyboard is blocked until all key released
    guc_keyb_blocked = 1;
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Scan for a pressed button according to the keyboard matrix
**                  and if pressed executes callback function assigned for that 
**                  button (see gta_keyb_info[]). 
**                  After button press detected, the keybord is blocked until 
**                  a pressed button is released.
**                  
***C*F*E**********************************************************************/
void keyb_scan()
{
    uint8 uc_i;
    uint8 uc_pin_mask_set;

    // Scan keyboard according with scanning matrix
    uc_pin_mask_set =  1;
    for( uc_i = 0; uc_i < KEYB_SCAN_MATRIX_ROWS; uc_i++)
    {
        KEYB_SCAN_PORT |= KEYB_SCAN_ALL_MASK;

        KEYB_SCAN_PORT &= ~uc_pin_mask_set;
        KEYB_SCAN_DIR  |= uc_pin_mask_set;

        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);

        // Get result
        guca_keyb_scan_res[uc_i] = KEYB_SCAN_PIN;
        KEYB_SCAN_DIR  &= ~KEYB_SCAN_ALL_MASK;

        uc_pin_mask_set <<= 1;
    }

    // All lines to HI-Z
    KEYB_SCAN_DIR &= ~KEYB_SCAN_ALL_MASK;
    KEYB_SCAN_PORT &= ~KEYB_SCAN_ALL_MASK;

    // Transformation
    guca_keyb_scan_res[1] = guca_keyb_scan_res[0] >> 1; 

    T_KEYB_BUTT_INFO    *pt_butt_info;
    pt_butt_info = &gta_keyb_info[0];

    // ~60 cycles
    for(uc_i = 0; uc_i < KEYB_BUTTON_NUM; uc_i++)
    {
        uint8 uc_row, uc_mask;

        uc_row = pgm_read_byte_near(&pt_butt_info->uc_row);
        uc_mask = pgm_read_byte_near(&pt_butt_info->uc_mask);

        if ( (guca_keyb_scan_res[uc_row] & uc_mask) == 0)
        {
            if (guc_keyb_blocked == 0)
            {
                void  (*pf_func)(void);
                pf_func = (void *)pgm_read_word_near(&pt_butt_info->pf_func);
                pf_func();
                guc_keyb_blocked = 1;
            }
            break;
        }
        pt_butt_info ++;
    }

    if (uc_i == KEYB_BUTTON_NUM)
    { // No buttons pressed
        guc_keyb_blocked = 0; 
    }

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Buttons call back functions
**                  
***C*F*E**********************************************************************/
void butt_act_geom     () { guca_lcd_digits_set1[2] = 0x00;  guc_lcd_redraw = 1; }
void butt_act_mplus    () { guca_lcd_digits_set1[2] = 0x01;  guc_lcd_redraw = 1; }

void butt_act_clear    () 
{ 

    if (gt_meas_info.e_state == MEAS_STATE_DISABLED)
    { // Laser AIM on/off
        guca_in_msg_buff[0] = 0xD5;
        guca_in_msg_buff[1] = 0x32;
        guca_in_msg_buff[2] = 0x01;
        guca_in_msg_buff[3] = 0x02;
        guc_in_msg_idx = 4;
    }

}
void butt_act_bigred   () 
{
    if (gt_meas_info.e_state == MEAS_STATE_DISABLED)
    { // Scan start
        guca_in_msg_buff[0] = 0xD5;
        guca_in_msg_buff[1] = 0x52;
        guca_in_msg_buff[2] = 0x04;

        // Calibration APD 0x9010
        guca_in_msg_buff[3] = 0x10;
        guca_in_msg_buff[4] = 0x90;

        // Measure APD 0x9030
        guca_in_msg_buff[5] = 0x30;
        guca_in_msg_buff[6] = 0x90;
        
        guc_in_msg_idx = 7;
    }
    else
    { // Scan stop (short command)
        guca_in_msg_buff[0] = 0x04;
        guc_in_msg_idx = 2;
    }

}
void butt_act_mode     () 
{ 
    if (gt_meas_info.e_state != MEAS_STATE_DISABLED)
    { // calibrate (short command)
        guca_in_msg_buff[0] = 0x93;
        guca_in_msg_buff[1] = 0x10;
        guc_in_msg_idx = 2;
    }
}


void butt_act_mminus   () { guca_lcd_digits_set1[2] = 0x05;  guc_lcd_redraw = 1; }
void butt_act_power    () { guca_lcd_digits_set1[2] = 0x06;  guc_lcd_redraw = 1; }
void butt_act_reference() { guca_lcd_digits_set1[2] = 0x07;  guc_lcd_redraw = 1; }
void butt_act_mequal   () { guca_lcd_digits_set1[2] = 0x08;  guc_lcd_redraw = 1; }
void butt_act_laser    () { guca_lcd_digits_set1[2] = 0x09;  guc_lcd_redraw = 1; }

/***C*F************************************************************************
**
** Button call back assignment table
**                  
***C*F*E**********************************************************************/

T_KEYB_BUTT_INFO gta_keyb_info[KEYB_BUTTON_NUM] __attribute__ ((section (".text"))) = {
    { 4, _BV(2), butt_act_geom      }, // 1  
    { 2, _BV(1), butt_act_mplus     }, // 2  
    { 2, _BV(3), butt_act_clear     }, // 3  
    { 4, _BV(1), butt_act_bigred    }, // 4  
    { 4, _BV(3), butt_act_mode      }, // 5  
    { 3, _BV(1), butt_act_mminus    }, // 6  
    { 4, _BV(0), butt_act_power     }, // 7  
    { 2, _BV(0), butt_act_reference }, // 8  
    { 1, _BV(0), butt_act_mequal    }, // 9  modified!
    { 3, _BV(0), butt_act_laser     }, // 10  
};

