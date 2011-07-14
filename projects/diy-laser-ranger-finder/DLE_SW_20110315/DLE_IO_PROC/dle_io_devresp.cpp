/***C*********************************************************************************************
**
** SRC-FILE     :   dle_io_devresp.cpp
**                                        
** PROJECT      :   DLE
**                                                                
** SRC-VERSION  :   
**              
** DATE         :   22/01/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   Device's responses process routines
**                  Typicall function parses incoming data packet from DLE device, convert it to 
**                  user friendly form and sends to user interface (IO pipe) as a text string.
**                  
** COPYRIGHT    :   (c) 2011 Andrejs Vasiljevs. All rights reserved.
**
****C*E******************************************************************************************/
#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <math.h>
#include "FTD2XX.H"
#include "dle_io_measure.h"
#include "dle_io_proc.h"
#include "dle_io_devcmd.h"

WCHAR gca_io_cmd_sub_resp1[RESP_STR_LEN];
WCHAR gca_io_cmd_sub_resp2[RESP_STR_LEN];
WCHAR gca_io_cmd_sub_resp3[RESP_STR_LEN];
WCHAR gca_io_cmd_sub_resp4[RESP_STR_LEN];

void proceed_dev_sign_resp();
void proceed_adc_ctrl_resp();
void proceed_spi_wr_resp();
void proceed_spi_rd_resp();
void proceed_curtain_resp();

void proceed_aim_resp();
void proceed_apd_on_resp();
void proceed_apd_off_resp();
void proceed_measure_resp();

void proceed_lcd_resp();

int dle_dev_response_processing(){

    // parse system command from global sys_info buffer
    if (gdw_dev_bytes_rcv == 0)
    {
        wprintf(L"No response\n");
    }
    else if (gdw_dev_bytes_rcv >= 2)
    {
        switch(guca_dev_resp[0])
        {
            case 0x11 : // Device signature
                proceed_dev_sign_resp();
                break;
            case 0x15 :                         // SPI WR 
                proceed_spi_wr_resp();
            case 0x16 :                         // SPI RD 
                proceed_spi_rd_resp();
                break;
            case 0x41 :                         // CURTAIN
                proceed_curtain_resp();
                break;
            case 0x32 :                         // Laser AIM
                proceed_aim_resp();
                break;
            case 0x21 :                         // APD_ON
                proceed_apd_on_resp();
                break;
            case 0x22 :                         // APD_OFF
                proceed_apd_off_resp();
                break;
            case 0x51 :                         // MEASURE
                proceed_measure_resp();
                break;
            case 0x61 :                         // LCD
                proceed_lcd_resp();
                break;
            default:
                wprintf(L"Shit happens");
                while(1);
        }

        // Send procssed device response to IO
        dle_io_pipe_tx();
    }
    else
    { // gdw_dev_bytes_rcv < 2
        wprintf(L"Garbage received\n");
    }

    // Initiate another IO pipe RX 
    gn_dev_resp_timeout = 0;
    dle_io_pipe_rx_init();

    return TRUE;
}

void proceed_dev_sign_resp(){

    WCHAR   ca_sign[256] = L"";

    {   // Convert signature to Multibyte string
        size_t n_chars;
        mbstowcs_s(
                &n_chars,
                ca_sign,
                sizeof(ca_sign)>>1,         // sign len
                (char*)&guca_dev_resp[4],   // sign text
                _TRUNCATE);

        ca_sign[n_chars-1] = 0;   // asure null termination

    }

    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"Device signature:\n%s. Hardware version %d.%d\n", ca_sign, guca_dev_resp[2], guca_dev_resp[3]);

}

void proceed_spi_rd_resp(){

    int n_i;

    gca_io_cmd_resp[0] = '\0';

    // check response length
    //if (guca_dev_resp[1] > 16)
    //{
    //    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"SPI RD response error\n");
    //    return;
    //}

    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"SPI RD: ", gca_io_cmd_resp);
    for (n_i = 0; n_i < guca_dev_resp[1]; n_i++)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s %02X", gca_io_cmd_resp, guca_dev_resp[n_i+2]);
    }
    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s\n", gca_io_cmd_resp);

    return;

}

void proceed_spi_wr_resp(){

    int n_i;

    gca_io_cmd_resp[0] = '\0';

    // check response length
    if (guca_dev_resp[1] > 14)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"SPI WR response error\n");
        return;
    }

    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"SPI WR: ", gca_io_cmd_resp);
    for (n_i = 0; n_i < guca_dev_resp[1]; n_i++)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s %02X", gca_io_cmd_resp, guca_dev_resp[n_i+2]);
    }
    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s\n", gca_io_cmd_resp);

    return;

}

void proceed_curtain_resp(){

    const WCHAR *pc_details;
    gca_io_cmd_resp[0] = '\0';

    if (guca_dev_resp[1] != 1)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"CURTAIN response length error (%d) \n", guca_dev_resp[1]);
        return;
    }

    switch (guca_dev_resp[2])
    {
        case 0 : pc_details = L"DOWN"; break;
        case 1 : pc_details = L"UP"; break;

        default: 
        pc_details = L"DISABLED"; break;
    }
    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"CURTAIN %s \n",  pc_details);

    return;

}

void proceed_aim_resp(){
    if (guca_dev_resp[1] != 1)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"Laser aim response length error (%d) \n", guca_dev_resp[1]);
        return;
    }

    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"Laser aim: %s \n", guca_dev_resp[2] ? L"ON":L"OFF");
}


void proceed_apd_on_resp(){

    double db_power;
    signed short s_cos, s_sin;
    if (guca_dev_resp[1] != 6)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"APD ON response length error (%d) \n", guca_dev_resp[1]);
        return;
    }

    s_cos = guca_dev_resp[4] | (guca_dev_resp[5]<<8); 
    s_sin = guca_dev_resp[6] | (guca_dev_resp[7]<<8);

    db_power = sqrt((double)(s_sin * s_sin) + (double)(s_cos * s_cos))/2*3.22;

    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"\nAPD ON: BIAS CTRL 0x%04X\n        Signal Amax ~%4.0fmV\n\n", 
        guca_dev_resp[2] | (guca_dev_resp[3]<<8), 
        db_power);
}

void proceed_apd_off_resp(){

    if (guca_dev_resp[1] != 0)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"APD OFF response length error (%d) \n", guca_dev_resp[1]);
        return;
    }
    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"APD OFF\n");
}

void proceed_measure_resp(){

    int n_burst;
    int n_burst_num, n_burst_size, n_burst_delay, n_measure_type;
    int n_payload_len;
    unsigned short *pus_payload;

    T_FREQ_TABLE    *pt_freq_table;

    if (guca_dev_resp[1] != 6)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"Measure response error (%d) \n", guca_dev_resp[1]);
        return;
    }

    n_burst_num = guca_dev_resp[2];
    n_burst_size = guca_dev_resp[3];
    n_burst_delay = guca_dev_resp[4];
    n_measure_type = guca_dev_resp[5];

    // Calculate payload length
    n_payload_len = 0;
    if (n_measure_type & MEAS_TYPE_STREAM)
    {
        n_payload_len += n_burst_size * 2 * n_burst_num;
    }
    if (n_measure_type & MEAS_TYPE_SINCOS)
    {
        n_payload_len += 4 * n_burst_num;
    }
    pus_payload = (unsigned short*)&guca_dev_resp[0];
    
    { // Read out payload 
        DWORD dw_bytes_rcv;
        int n_rc;

        n_rc = FT_W32_ReadFile(
            gh_dle_dev,
            guca_dev_resp, 
            n_payload_len,
            &dw_bytes_rcv, 
            &gt_rx_dev_overlapped);

        if (n_rc == 0)
        {
            n_rc = FT_W32_GetLastError(gh_dle_dev);
            if ( n_rc != ERROR_IO_PENDING) 
            {
                // log an error
                // sent something to UI
                while(1);
            }

            dw_bytes_rcv = 0;
            n_rc = FT_W32_GetOverlappedResult(
                gh_dle_dev,         
                &gt_rx_dev_overlapped,
                &dw_bytes_rcv,
                TRUE);
                           
            if (dw_bytes_rcv != n_payload_len)
            {  
                wprintf(L"Error in command response ");
                swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s\nError in command response\n", gca_io_cmd_resp);
                return;
            }
        }
    } // End of Read payload block

    { // DBG dump
        DWORD dw_temp;
        int n_rc;

        n_rc = WriteFile(
            gh_dump_file,
            pus_payload,
            n_payload_len,
            &dw_temp,
            NULL
        );
    }

    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"\nMEASURE: Burst = %dx%d, Burst delay = %d", n_burst_num, n_burst_size, n_burst_delay);

    swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"\n         STREAM(deg): ");
    swprintf(gca_io_cmd_sub_resp2, RESP_STR_LEN, L"\n         ST_PWR(mV) : ");
    swprintf(gca_io_cmd_sub_resp3, RESP_STR_LEN, L"\n         SINCOS(deg): ");
    swprintf(gca_io_cmd_sub_resp4, RESP_STR_LEN, L"\n         SC_PWR(mV) : ");

    pt_freq_table = gpt_active_table;
    for (n_burst = 0; n_burst < n_burst_num;  n_burst ++)
    {
        if (n_measure_type & MEAS_TYPE_STREAM)
        {
            measure_stream2sincos(pt_freq_table, n_burst_size, pus_payload);

            measure_sincos2phase(pt_freq_table);

            swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"%s %-07.1f,",gca_io_cmd_sub_resp1, pt_freq_table->db_angle_grad);
            swprintf(gca_io_cmd_sub_resp2, RESP_STR_LEN, L"%s %-07.1f,",gca_io_cmd_sub_resp2, pt_freq_table->db_power);

            pus_payload += n_burst_size;
        }

        if (n_measure_type & MEAS_TYPE_SINCOS)
        {
            // get SIN, COS from payload
            pt_freq_table->s_cos = (INT16)*pus_payload++;
            pt_freq_table->s_sin = (INT16)*pus_payload++;

            measure_sincos2phase(pt_freq_table);

            swprintf(gca_io_cmd_sub_resp3, RESP_STR_LEN, L"%s %-07.1f,",gca_io_cmd_sub_resp3, pt_freq_table->db_angle_grad);
            swprintf(gca_io_cmd_sub_resp4, RESP_STR_LEN, L"%s %-07.1f,",gca_io_cmd_sub_resp4, pt_freq_table->db_power);

        }

        pt_freq_table++;
    }

    if (n_measure_type & MEAS_TYPE_STREAM)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s%s", gca_io_cmd_resp, gca_io_cmd_sub_resp1);
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s%s", gca_io_cmd_resp, gca_io_cmd_sub_resp2);
    }

    if (n_measure_type & MEAS_TYPE_SINCOS)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s%s", gca_io_cmd_resp, gca_io_cmd_sub_resp3);
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s%s", gca_io_cmd_resp, gca_io_cmd_sub_resp4);
    }

    wprintf(L"%s\n", gca_io_cmd_resp);

    measure_phases2distance(gpt_active_table, n_burst_num, NULL);

    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"\n         DISTAN(mm) : ", gca_io_cmd_resp);
    for (n_burst = 0; n_burst < n_burst_num;  n_burst ++)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s%-07.1f,  ", gca_io_cmd_resp, gta_meas_table[n_burst].db_distance);
    }

    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s\n", gca_io_cmd_resp);

    return;

}

void proceed_lcd_resp(){

//    const WCHAR *pc_details;
    gca_io_cmd_resp[0] = '\0';

    if (guca_dev_resp[1] != 0)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"LCD response length error (%d) \n", guca_dev_resp[1]);
        return;
    }

    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"LCD OK\n");

    return;

}
