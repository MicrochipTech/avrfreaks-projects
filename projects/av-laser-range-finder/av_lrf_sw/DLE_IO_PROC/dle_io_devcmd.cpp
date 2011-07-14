/***C*********************************************************************************************
**
** SRC-FILE     :   dle_io_devcmd.cpp
**                                        
** PROJECT      :   DLE
**                                                                
** SRC-VERSION  :   
**              
** DATE         :   22/01/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   The functions set to convert user commands from DLE_CMD_LIB to device commands
**                  Typicall function extract data from incoming user's command, repack them into
**                  binary packet and sends to DLE device
**                  
** COPYRIGHT    :   (c) 2011 Andrejs Vasiljevs. All rights reserved.
**
****C*E******************************************************************************************/
#include <windows.h>
#include <wchar.h>
#include "FTD2XX.H"
#include "dle_io_measure.h"
#include "dle_io_proc.h"
#include "dle_io_devcmd.h"
#include "dle_cmd_lib.h"

typedef enum e_dle_scan_state {
    E_SCAN_STATE_DISABLED = 0,
    E_SCAN_STATE_ENABLED,
} E_DLE_SCAN;

typedef struct t_dle_scan_tag {

    E_DLE_SCAN e_state;
    int n_apd_auto;

} T_DLE_SCAN;

T_DLE_SCAN gt_dle_scan = {E_SCAN_STATE_DISABLED, 0};

OVERLAPPED gt_tx_io_overlapped = { 0 };

void send_devcmd(DWORD dw_byte_to_write, unsigned char *pc_cmd, const WCHAR *pc_cmd_name){

    int n_rc, n_gle;
    DWORD dw_bytes_written;

    if (gt_flags.dev_conn != FL_SET) return;

    n_rc = FT_W32_WriteFile(
        gh_dle_dev,
        pc_cmd, 
        dw_byte_to_write,
        &dw_bytes_written, 
        &gt_tx_io_overlapped);

    if (!n_rc)
    {
        n_gle = FT_W32_GetLastError(gh_dle_dev);
        if (n_gle != ERROR_IO_PENDING)
        {
            n_rc = FALSE;
            goto cleanup_send_devcmd;
        }
    }

    n_rc = FT_W32_GetOverlappedResult(gh_dle_dev, &gt_tx_io_overlapped, &dw_bytes_written, TRUE);
    if (!n_rc || (dw_byte_to_write != dw_bytes_written))
    {
        n_rc = FALSE;
        goto cleanup_send_devcmd;
    }

    n_rc = TRUE;

    // lock further command processing until response received
    // ...

cleanup_send_devcmd:

    if (n_rc)
    {
        wprintf(L"io --> mcu : %s \n", pc_cmd_name);
    }
    else
    {
        wprintf(L"Error writing command %s\n", pc_cmd_name);
        gt_flags.dev_conn = FL_FALL;
        SetEvent(gha_events[HANDLE_NOT_IDLE]);
    }

    return;

}

void dle_cmd_io_dev_sign(){

    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_sign{
        BYTE    uc_mark;
        BYTE    uc_cmd;
    } t_cmd = { 0xD5, 0x11 };

    dw_byte_to_write = sizeof(t_cmd);
    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"DEV_SIGN");
    
    return;
}

void dle_cmd_io_spi_wr(){

    int n_data, n_len;
    WCHAR *pc_tok;

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_spi_wr{
        BYTE    uc_mark;
        BYTE    uc_cmd;
        BYTE    uc_len;
        BYTE    uca_raw_data[14];
    } t_cmd = { 0xD5, 0x15 };

    
    // field addr contains initial spi addres to write
    // field data contains string with data - one byte per token

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    pc_tok = wcstok(gt_cmd_spi_wr.data.pc_str, L" ");
    n_rc = swscanf(pc_tok, L"%i", &n_data);
    if (n_rc != 1){
        wprintf(L"Unexpected argument %s of %s\n", pc_tok, gt_cmd_spi_wr.data.pc_name);
        n_rc = FALSE;
    }

    // Check address (first txt token)
    if ( (n_data > 0x1A || n_data < 0x02) && n_data != 0x80 ){
        wprintf(L"Unexpected argument %s of %s\n", pc_tok, gt_cmd_spi_wr.data.pc_name);
        n_rc = FALSE;
    }
    
    // write addres to raw buffer
    n_len = 0;
    t_cmd.uca_raw_data[n_len++] = (BYTE)n_data;

    // fill spi data buff with data from command 
    pc_tok = wcstok(NULL, L" ");
    while(pc_tok && n_len < sizeof(t_cmd.uca_raw_data) ){
        int n_rc, n_data;

        n_rc = swscanf(pc_tok, L"%i", &n_data);
        if (n_rc != 1){
            wprintf(L"Unexpected argument %s of %s\n", pc_tok, gt_cmd_spi_wr.data.pc_name);
            n_rc = FALSE;
        }else{
            t_cmd.uca_raw_data[n_len++] = (BYTE)n_data;
        }

        pc_tok = wcstok(NULL, L" ");
    }

    // check raw data length
    if (n_len < 2)
        n_rc = FALSE;

    // write cmd len
    t_cmd.uc_len = n_len;

    if (!n_rc)
        goto cmd_io_spi_wr_raw_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 3+t_cmd.uc_len;

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"SPI_WR");

    return;

cmd_io_spi_wr_raw_error:

    wprintf(L"command error : SPI_WR\n");
    return;
}


void dle_cmd_io_spi_rd(){

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_spi_rd{
        BYTE    uc_mark;
        BYTE    uc_cmd;
        BYTE    uc_len;
        BYTE    uc_rd_len;
    } t_cmd = { 0xD5, 0x16, 0x01 };

   
    // field addr contains initial spi addres to write
    // field data contains string with data - one byte per token

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;
    //if (gt_cmd_spi_rd.len.dw_val > 16)
    //{
    //    wprintf(L"Incorrect SPI read length %d \n", gt_cmd_spi_rd.len.dw_val);
    //    n_rc = FALSE;
    //}
    t_cmd.uc_rd_len = (BYTE)gt_cmd_spi_rd.len.dw_val;

    if (!n_rc)
        goto cmd_io_spi_rd_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 3+t_cmd.uc_len;

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"SPI_RD");

    return;

cmd_io_spi_rd_error:

    wprintf(L"command error : SPI_RD\n");

    return;
}


void dle_cmd_io_apd_on(){

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_apd_on{
        BYTE    uc_mark;
        BYTE    uc_cmd;
        BYTE    uc_len;
        WORD    us_bias_ctrl_level;    // SPI value
    } t_cmd = { 0xD5, 0x21, 0x02};

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Check BIAS_CTRL (0x10 xx or 0x90 xx) if non zero value is specified
    if (gt_cmd_apd_on.bias_ctrl_level.dw_val != 0)
    {
        if ( (gt_cmd_apd_on.bias_ctrl_level.dw_val >> 8 != 0x10) &&
             (gt_cmd_apd_on.bias_ctrl_level.dw_val >> 8 != 0x90))
        {
            wprintf(L"Incorrect BIAS CTRL parameter - %d\n", gt_cmd_apd_on.bias_ctrl_level.dw_val);
            n_rc = FALSE;
        }
    }

    t_cmd.us_bias_ctrl_level = (WORD) gt_cmd_apd_on.bias_ctrl_level.dw_val;
    
    if (!n_rc)
        goto cmd_io_apd_on_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 3+t_cmd.uc_len;

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"APD_ON");

    return;

cmd_io_apd_on_error:
    wprintf(L"command error : APD_ON\n");

    return;
}

void dle_cmd_io_apd_off(){

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_apd_off{
        BYTE    uc_mark;
        BYTE    uc_cmd;
        BYTE    uc_len;
    } t_cmd = { 0xD5, 0x22, 0x00 };

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Verify command arguments
    // ...

    if (!n_rc)
        goto cmd_io_apd_off_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 3+t_cmd.uc_len;

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"APD_OFF");

    return;

cmd_io_apd_off_error:

    wprintf(L"command error : APD_OFF\n");
    return;
}


void dle_cmd_io_aim(){

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_aim{
        BYTE    uc_mark;
        BYTE    uc_cmd;
        BYTE    uc_len;
        BYTE    uc_act; // 0 - disable, 1 - enable, 2 - toggle
    } t_cmd = { 0xD5, 0x32, 0x01 };

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Verify command arguments
    // ...

    t_cmd.uc_act = (BYTE)gt_cmd_aim.act.dw_val;

    if (!n_rc)
        goto cmd_io_aim_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 3+t_cmd.uc_len;

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"aim");

    return;

cmd_io_aim_error:

    wprintf(L"command error : aim\n");
    return;
}

void dle_cmd_io_curtain(int n_curtain_act){

    int n_rc;
    DWORD dw_byte_to_write;
    const WCHAR *pc_cmd_details;

    #pragma pack(1)
    struct t_cmd_curtain{
        BYTE    uc_mark;
        BYTE    uc_cmd;
        BYTE    uc_len;
        BYTE    uc_act;
    } t_cmd = { 0xD5, 0x41, 0x01, 0x00 };

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Verify command arguments
    // ...

    t_cmd.uc_act = (BYTE)n_curtain_act;

    if (!n_rc)
        goto cmd_io_curtain_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 3+t_cmd.uc_len;


    switch (n_curtain_act)
    {
        case 0 : pc_cmd_details = L"CURTAIN_UP"; break;
        case 1 : pc_cmd_details = L"CURTAIN_DOWN"; break;

        default: 
        pc_cmd_details = L"CURTAIN_DIS"; break;
    }
    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, pc_cmd_details);

    return;

cmd_io_curtain_error:

    wprintf(L"command error : CURTAIN\n");
    return;
}

void dle_cmd_io_measure(){

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_measure{
        BYTE    uc_mark;
        BYTE    uc_cmd;
        BYTE    uc_len;
        BYTE    uc_measure_type;
    } t_cmd = { 0xD5, 0x51, 0x01, 0x00 };

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Verify command arguments
    // ...

    if (gt_cmd_measure.stream.dw_val)
    {
        t_cmd.uc_measure_type |= MEAS_TYPE_STREAM;
    }

    if (gt_cmd_measure.sincos.dw_val)
    {
        t_cmd.uc_measure_type |= MEAS_TYPE_SINCOS;
    }


    if (!n_rc)
        goto cmd_io_measure_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 3+t_cmd.uc_len;

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"MEASURE");

    return;

cmd_io_measure_error:

    wprintf(L"command error : MEASURE\n");
    return;
}

void dle_cmd_io_lcd(){

#define LCD_DIGITS_NUM 5

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_lcd{
        BYTE    uc_mark;
        BYTE    uc_cmd;
        BYTE    uc_len;
        BYTE    uca_dig0[LCD_DIGITS_NUM];
        BYTE    uca_dig1[LCD_DIGITS_NUM];
        DWORD   ul_signs;
    } t_cmd = { 0xD5, 0x61, 0x0E, {0x10, 0x10, 0x10, 0x10, 0x10}, {0x10, 0x10, 0x10, 0x10, 0x10}, 0 };

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Verify command arguments
    // ...


    // convert digits string to numbers
    //check hex or dec format
    int n_i, n_set;
    for (n_set = 0; n_set < 2; n_set ++)
    {
        WCHAR *pc_str; 
        BYTE  *pc_digit;

        if  (n_set == 0)
        {
            pc_digit = &t_cmd.uca_dig0[0];
            pc_str = &gt_cmd_lcd.dig0.pc_str[0];
        }
        else
        {
            pc_digit = &t_cmd.uca_dig1[0];
            pc_str = &gt_cmd_lcd.dig1.pc_str[0];
        }


        for (n_i = 0; n_i < LCD_DIGITS_NUM; n_i ++)
        {
            if (pc_str[n_i] == '/0') break;

            switch (pc_str[n_i])
            {
                case L'0' : pc_digit[n_i] = 0; break;
                case L'1' : pc_digit[n_i] = 1; break;
                case L'2' : pc_digit[n_i] = 2; break;
                case L'3' : pc_digit[n_i] = 3; break;
                case L'4' : pc_digit[n_i] = 4; break;
                case L'5' : pc_digit[n_i] = 5; break;
                case L'6' : pc_digit[n_i] = 6; break;
                case L'7' : pc_digit[n_i] = 7; break;
                case L'8' : pc_digit[n_i] = 8; break;
                case L'9' : pc_digit[n_i] = 9; break;
                case L'A' : pc_digit[n_i] = 0x0A; break;
                case L'B' : pc_digit[n_i] = 0x0B; break;
                case L'C' : pc_digit[n_i] = 0x0C; break;
                case L'D' : pc_digit[n_i] = 0x0D; break;
                case L'E' : pc_digit[n_i] = 0x0E; break;
                case L'F' : pc_digit[n_i] = 0x0F; break;
                case L' ' : pc_digit[n_i] = 0x10; break;
                case L'P' : pc_digit[n_i] = 0x11; break;
                case L'O' : pc_digit[n_i] = 0x12; break;
                case L'H' : pc_digit[n_i] = 0x13; break;
                case L'L' : pc_digit[n_i] = 0x14; break;
                case L'I' : pc_digit[n_i] = 0x15; break;
                case L'U' : pc_digit[n_i] = 0x16; break;
                case L'n' : pc_digit[n_i] = 0x17; break;
                case L'b' : pc_digit[n_i] = 0x18; break;
                case L'd' : pc_digit[n_i] = 0x19; break;
                case L'r' : pc_digit[n_i] = 0x1A; break;
                case L'o' : pc_digit[n_i] = 0x1B; break;
                case L'i' : pc_digit[n_i] = 0x1C; break;
                case L'h' : pc_digit[n_i] = 0x1D; break;
                case L'-' : pc_digit[n_i] = 0x1E; break;
                case L'_' : pc_digit[n_i] = 0x1F; break;
                default :
                pc_digit[n_i] = 0x10;

            } // End of switch

        } // End of digits FOR
    } // End of sets FOR


    // Set other signs bits
    t_cmd.ul_signs = gt_cmd_lcd.signs.dw_val;

    if (!n_rc)
        goto cmd_io_lcd_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 3+t_cmd.uc_len;

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"LCD");
    return;

cmd_io_lcd_error:

    wprintf(L"command error : LCD\n");
    return;

}

void dle_cmd_io_scan_start(){

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_scan_start{
        BYTE    uc_mark;
        BYTE    uc_cmd;
        BYTE    uc_len;
        WORD    us_apd_calib;
        WORD    us_apd_meas;
    } t_cmd = { 0xD5, 0x52, 0x04, 0x1000, 0x1000 };

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Verify command arguments
    // ...

    t_cmd.us_apd_calib = (WORD)gt_cmd_scan_start.calib_apd.dw_val;
    t_cmd.us_apd_meas = (WORD)gt_cmd_scan_start.meas_apd.dw_val;

    gt_dle_scan.e_state = E_SCAN_STATE_ENABLED;
    gt_dle_scan.n_apd_auto = (BOOL)gt_cmd_scan_start.apd_auto.dw_val;

    if (!n_rc)
        goto cmd_io_scan_start_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 3+t_cmd.uc_len;

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"SCAN_START");
    return;

cmd_io_scan_start_error:

    wprintf(L"command error : SCAN_START\n");
    return;

}

void dle_cmd_io_scan_stop(){
    // !!! SHORT COMMAND !!! 
    // Legal only while scaning

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_scan_stop{
        BYTE    uc_byte0;
        BYTE    uc_byte1;
    } t_cmd = { 0x04, 0x00};

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Verify command arguments
    // ...

    if (!n_rc)
        goto cmd_io_scan_stop_error;

    if (gt_dle_scan.e_state != E_SCAN_STATE_ENABLED)
    {
        wprintf(L"Scan mode not enabled\n");
        goto cmd_io_scan_stop_error;
    }

    gt_dle_scan.e_state = E_SCAN_STATE_DISABLED;
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 2;   // always two for short commands

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"SCAN_STOP");
    return;

cmd_io_scan_stop_error:

    wprintf(L"command error : SCAN_STOP\n");
    return;

}

void dle_cmd_io_scan_apd(){

    // !!! SHORT COMMAND !!! 
    // Legal only while scaning

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_scan_apd{
        BYTE    uc_byte0;
        BYTE    uc_byte1;
    } t_cmd = { 0x11, 0x00};

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Verify command arguments
    // ...

    t_cmd.uc_byte0 = (BYTE)((gt_cmd_scan_apd.apd.dw_val >> 8) & 0xF0);
    t_cmd.uc_byte1 = (BYTE)(gt_cmd_scan_apd.apd.dw_val);

    t_cmd.uc_byte0 |= 0x01;    // command header 4LSB of byte 0

    if (!n_rc)
        goto cmd_io_scan_apd_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 2;   // always two for short commands

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"SCAN_APD");
    return;

cmd_io_scan_apd_error:

    wprintf(L"command error : SCAN_APD\n");
    return;

}


void dle_cmd_io_scan_calib(){

    // !!! SHORT COMMAND !!! 
    // Legal only while scaning

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_scan_apd{
        BYTE    uc_byte0;
        BYTE    uc_byte1;
    } t_cmd = { 0x03, 0x00};

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Verify command arguments
    // ...

    t_cmd.uc_byte0 = (BYTE)((gt_cmd_scan_calib.apd.dw_val >> 8) & 0xF0);
    t_cmd.uc_byte1 = (BYTE)(gt_cmd_scan_calib.apd.dw_val);

    t_cmd.uc_byte0 |= 0x03;    // command header 4LSB of byte 0

    if (!n_rc)
        goto cmd_io_scan_calib_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 2;   // always two for short commands

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"SCAN_CALIB");
    return;

cmd_io_scan_calib_error:

    wprintf(L"command error : SCAN_CALIB\n");
    return;

}


#if  0   // Command template                    
void dle_cmd_io_???(){

    int n_rc;
    DWORD dw_byte_to_write;

    #pragma pack(1)
    struct t_cmd_???{
        BYTE    uc_mark;
        BYTE    uc_cmd;
        BYTE    uc_len;
        BYTE/WORD    uc/us_???;
    } t_cmd = { 0xD5, 0x???, 0x??? };

    // -----------------------------------------------
    // --- write data from UI command to MCU command
    // ------------------------------------------------
    n_rc = TRUE;

    // Verify command arguments
    // ...

    t_cmd.??? = (WORD/BYTE)gt_cmd_???.???.dw_val;

    if (!n_rc)
        goto cmd_io_???_error;
    
    // ----------------------------------------
    // --- Initiate data transfer to MCU
    // ----------------------------------------
    dw_byte_to_write = 3+t_cmd.uc_len;

    send_devcmd(dw_byte_to_write, (unsigned char *)&t_cmd, L"???");
    return;

cmd_io_???_error:

    wprintf(L"command error : ???\n");
    return;

}

#endif // #if 0
                       
