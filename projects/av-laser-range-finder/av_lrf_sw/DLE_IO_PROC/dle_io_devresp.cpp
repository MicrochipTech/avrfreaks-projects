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
#include <float.h>
#include "FTD2XX.H"
#include "dle_io_measure.h"
#include "dle_io_proc.h"
#include "dle_io_devcmd.h"
#include "dle_cmd_lib.h"


WCHAR gca_io_cmd_sub_resp1[RESP_STR_LEN];
WCHAR gca_io_cmd_sub_resp2[RESP_STR_LEN];
WCHAR gca_io_cmd_sub_resp3[RESP_STR_LEN];
WCHAR gca_io_cmd_sub_resp4[RESP_STR_LEN];

WCHAR gca_pwr_range_str[6][20] = 
{
    {L"PWR_TOOLOW     "},
    {L"PWR_LOW        "},
    {L"PWR_OK         "},
    {L"PWR_HIGH       "},
    {L"PWR_TOOHIGH    "},
    {L"PWR_OVERVOLTAGE"},
};

WCHAR gca_err_code_str[5][20] = 
{
       {L"ERR_OK    "},
       {L"ERR_MAX0  "},
       {L"ERR_AVG0  "},
       {L"ERR_NOSIG "},
       {L"ERR_PWR   "},
};


extern void log_meas(WCHAR *pc_str);
extern WORD gus_apd_calib;
extern WORD gus_apd_meas;
extern WORD gus_apd;


void proceed_dev_sign_resp();
void proceed_adc_ctrl_resp();
void proceed_spi_wr_resp();
void proceed_spi_rd_resp();
void proceed_curtain_resp();

void proceed_aim_resp();
void proceed_apd_on_resp();
void proceed_apd_off_resp();
//void proceed_measure_resp();
void proceed_scan_start_resp();

void proceed_lcd_resp();

void proceed_scan_meas_resp();
void proceed_scan_calib_resp();
void proceed_scan_dbg_meas_resp();
void proceed_scan_dbg_calib_resp();

void apd_auto();
void apd_calibr();

int dle_dev_resp_read_payload()
{
    
   // Read out payload 
    DWORD dw_bytes_rcv;
    int n_rc;

    int n_payload_len;
    unsigned short *pus_payload;

    n_payload_len = guca_dev_resp[1];

    if (n_payload_len == 0)
        return TRUE;

    pus_payload = (unsigned short*)&guca_dev_resp[2];

    n_rc = FT_W32_ReadFile(
        gh_dle_dev,
        pus_payload, 
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
                       
    }

    if (dw_bytes_rcv != n_payload_len)
    {  
        wprintf(L"Error in command response ");
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"%s\nError in command response\n", gca_io_cmd_resp);
        return FALSE;
    }

    return TRUE;
}

int dle_dev_response_processing(){

    // parse system command from global sys_info buffer
    if (gdw_dev_bytes_rcv == 0)
    {
        wprintf(L"No response\n");
    }
    else if (gdw_dev_bytes_rcv >= 2)
    {

        // read out response payload
        if (!dle_dev_resp_read_payload())
        {
            dle_dev_clear_fifos(); 
            return TRUE;
        }
        gca_io_cmd_resp[0] = L'\0';

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
//                proceed_measure_resp();
                break;
            case 0x52 :                         // SCAN_START
                proceed_scan_start_resp();
                break;
            case 0x61 :                         // LCD
                proceed_lcd_resp();
                break;
            case 0x71 :                         // SCAN_MEASURE
                proceed_scan_dbg_meas_resp();
                break;
            case 0x72 :                         // SCAN_CALIBRATION
                proceed_scan_dbg_calib_resp();
                break;
            case 0x73 :                         // SCAN_MEASURE
                proceed_scan_meas_resp();
                break;
            case 0x74 :                         // SCAN_CALIBRATION
                proceed_scan_calib_resp();
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

    //// Initiate another IO pipe RX 
    //gn_dev_resp_timeout = 0;
    //dle_io_pipe_rx_init();

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

        ca_sign[n_chars-1] = 0;   // assure null termination

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
        case 0 : pc_details = L"UP"; break;
        case 1 : pc_details = L"DOWN"; break;

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
#if 0
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

            swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"%s %-07.1f,",gca_io_cmd_sub_resp1, pt_freq_table->db_angle_deg);
            swprintf(gca_io_cmd_sub_resp2, RESP_STR_LEN, L"%s %-07.1f,",gca_io_cmd_sub_resp2, pt_freq_table->db_power);

            pus_payload += n_burst_size;
        }

        if (n_measure_type & MEAS_TYPE_SINCOS)
        {
            // get SIN, COS from payload
            pt_freq_table->s_cos = (INT16)*pus_payload++;
            pt_freq_table->s_sin = (INT16)*pus_payload++;

            measure_sincos2phase(pt_freq_table);

            swprintf(gca_io_cmd_sub_resp3, RESP_STR_LEN, L"%s %-07.1f,",gca_io_cmd_sub_resp3, pt_freq_table->db_angle_deg);
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
#endif // 0

void proceed_scan_start_resp(){

//    const WCHAR *pc_details;
    gca_io_cmd_resp[0] = '\0';

    if (guca_dev_resp[1] != 0)
    {
        swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"SCAN_START response length error (%d) \n", guca_dev_resp[1]);
        return;
    }

    swprintf(gca_io_cmd_resp, RESP_STR_LEN, L"SCAN_START OK\n");
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
void proceed_scan_resp(int n_calibration){

    gus_timestamp = *((unsigned short*)&guca_dev_resp[2]);
    gus_apd = *((unsigned short*)&guca_dev_resp[4]);

    if (n_calibration)
    {
        swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"\n c------------------");
        gus_apd_calib = gus_apd;
    }
    else
    {
        swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"\n -------------------");
        gus_apd_meas = gus_apd;
    }

    gus_distance = *((WORD*)&guca_dev_resp[6]);
    guc_err_code = *((BYTE*)&guca_dev_resp[8]);
    guc_pwr_range = *((BYTE*)&guca_dev_resp[9]);

    
    gca_io_cmd_sub_resp1[4+guc_pwr_range] = L'+';
    gca_io_cmd_sub_resp1[4+8+guc_err_code] = L'+';


    WCHAR ca_dist[10];

    if (guc_err_code != 0)
    {
        wcscpy(ca_dist, L"....");
    }
    else
    {
        _ultow(gus_distance, ca_dist, 10);
    }


    swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"%s   Distance = %-5smm (%s, %s), APD = 0x%04X, Times stamp = 0x%04X", gca_io_cmd_sub_resp1,
        ca_dist, 
        gca_pwr_range_str[guc_pwr_range],
        gca_err_code_str[guc_err_code],
        gus_apd,
        gus_timestamp);

    log_meas(gca_io_cmd_sub_resp1);

    wprintf(L"%s", gca_io_cmd_sub_resp1);
}


void proceed_scan_dbg_resp(int n_calibration){

    unsigned short *pus_payload;
    unsigned short us_apd, us_timestamp;
    unsigned short us_phase_ext;
    
    T_FREQ_INFO *pt_freq;
    T_SIG_PARAM *pt_sig_param;

    pt_freq = &gta_meas_info[0];

    us_apd = *((unsigned short*)&guca_dev_resp[4]);
    us_timestamp = *((unsigned short*)&guca_dev_resp[2]);

    if (n_calibration)
    {
        swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"\n    -!------- calibration --------- ");
        gus_apd_calib = us_apd;
    }
    else
    {
        swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"\n    ------------------------------- ");
        gus_apd_meas = us_apd;
    }

    gus_apd = us_apd;

    swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"%s\n         Times stamp = 0x%04X, APD level = 0x%04X", gca_io_cmd_sub_resp1, us_timestamp, us_apd);


    swprintf(gca_io_cmd_sub_resp2, RESP_STR_LEN, L"\n         Phase(deg): ");
    swprintf(gca_io_cmd_sub_resp3, RESP_STR_LEN, L"\n         Ampl.(mV) : ");

    // write sin&cos to measure table
    pus_payload = (unsigned short*)&guca_dev_resp[6];
    
    while(pt_freq->us_freq)             
    {

        // -------------------------------------------------------
        // --- Convert sin & cos -> phase & pwr (internals)
        // -------------------------------------------------------
        if (n_calibration)
        {
            pt_sig_param = &pt_freq->t_calib_sig;
        }
        else
        {
            pt_sig_param = &pt_freq->t_meas_sig;
        }

        pt_sig_param->s_cos = (INT16)*pus_payload++;
        pt_sig_param->s_sin = (INT16)*pus_payload++;
             
        measure_sincos2param(pt_sig_param);
        
        // phase calculated by PC
        pt_freq->db_phase_int = ( pt_freq->t_meas_sig.db_angle_deg -  pt_freq->t_calib_sig.db_angle_deg);
        if (pt_freq->db_phase_int < 0) pt_freq->db_phase_int += 360;

        // Validate internal phase & power
        if(
           (pt_freq->t_calib_sig.us_phase_int != 0xFFFF) &&
           (pt_freq->t_meas_sig.us_phase_int  != 0xFFFF) &&
           (pt_freq->t_calib_sig.uc_pwr_range_int > 0 && pt_freq->t_calib_sig.uc_pwr_range_int < 5) &&
           (pt_freq->t_meas_sig.uc_pwr_range_int  > 0 && pt_freq->t_meas_sig.uc_pwr_range_int  < 5)
           )
        {
            pt_freq->n_valid_int = TRUE;
        }
        else
        {
            pt_freq->n_valid_int = FALSE;
        }

        pt_freq->uc_pwr_range_int = pt_sig_param->uc_pwr_range_int;

        
        // -------------------------------------------------------
        // --- Read external phase & pwr from incoming packet
        // -------------------------------------------------------
        pt_freq->t_calib_sig.us_phase_ext = (INT16)*pus_payload++;
        pt_freq->t_meas_sig.us_phase_ext  = (INT16)*pus_payload++;
        us_phase_ext = (UINT16)*pus_payload++;
        pt_freq->us_phase_ext = us_phase_ext;
//        pt_freq->db_phase_ext = ((double)us_phase_ext) /8192 / (2*3.141593) * 360;
        pt_freq->db_phase_ext = ((double)us_phase_ext) / 65536 * 360;
        pt_freq->uc_pwr_range_ext = (UINT8)*pus_payload++;

        // Validate external phase & pwr
        if(
           (us_phase_ext != 0xFFFF ) &&
           (pt_freq->uc_pwr_range_ext > 0 && pt_freq->uc_pwr_range_ext  < 5)
           )
        {
            pt_freq->n_valid_ext = TRUE;
        }
        else
        {
            pt_freq->n_valid_ext = FALSE;
        }

        // -------------------------------------------------------
        // --- choose phase & power calculated by PC(int) or MCU (ext)
        // -------------------------------------------------------
        pt_freq->db_phase = pt_freq->db_phase_ext;
        pt_freq->uc_pwr_range = pt_freq->uc_pwr_range_ext;
        pt_freq->n_valid = pt_freq->n_valid_ext;

        // -------------------------------------------------------
        // --- debug log & traps
        // -------------------------------------------------------
        swprintf(gca_io_cmd_sub_resp2, RESP_STR_LEN, L"%s\n int (meas: %5.1d, calib: %5.1d, delta:%5.1f) \n ext (meas: %5.1d, calib: %5.1d, delta:%5.1f, %d)", gca_io_cmd_sub_resp2, 
            pt_freq->t_meas_sig.us_phase_int, 
            pt_freq->t_calib_sig.us_phase_int, 
            pt_freq->db_phase_int, 
            pt_freq->t_meas_sig.us_phase_ext, 
            pt_freq->t_calib_sig.us_phase_ext, 
            pt_freq->db_phase_ext,
            pt_freq->us_phase_ext);

        //swprintf(gca_io_cmd_sub_resp2, RESP_STR_LEN, L"%s (meas: %5.1f, calib: %5.1f, int:%5.1f, ext:%5.1f) \n", gca_io_cmd_sub_resp2, 
        //    pt_freq->t_meas_sig.db_angle_deg, 
        //    pt_freq->t_calib_sig.db_angle_deg, 
        //    pt_freq->db_phase_int, 
        //    pt_freq->db_phase_ext);
//        swprintf(gca_io_cmd_sub_resp2, RESP_STR_LEN, L"%s (%-07.1f),", gca_io_cmd_sub_resp2, pt_freq->db_phase_int - pt_freq->db_phase_ext);

        swprintf(gca_io_cmd_sub_resp3, RESP_STR_LEN, L"%s %-07.1f",gca_io_cmd_sub_resp3, pt_sig_param->db_power_mV);
        swprintf(gca_io_cmd_sub_resp3, RESP_STR_LEN, L"%s (ext:%d, int:%d),",gca_io_cmd_sub_resp3, pt_freq->uc_pwr_range_ext, pt_freq->uc_pwr_range_int);

        // debug trap: compare signal validation results
        if (pt_freq->n_valid_int != pt_freq->n_valid_ext)
        {
            pt_freq->n_valid_int = pt_freq->n_valid_ext;
        }
            
        // debug trap: compare phases calculated by PC & MCU
        if (pt_freq->n_valid_int)
        {
            if (fabs(pt_freq->db_phase_int - pt_freq->db_phase_ext) > 1)
            {
                pt_freq->db_phase_ext = pt_freq->db_phase_int;
            }
        }

        // debug trap: compare power range calculated by PC & MCU
        if (pt_freq->db_phase_int != pt_freq->db_phase_ext)
        {
            pt_freq->db_phase_int = pt_freq->db_phase_ext;
        }

        pt_freq++;
    }

    gus_distance = 0;
    guc_err_code = 3;

    // get final distance
    if (!n_calibration)
    {
        gus_distance = (INT16)*pus_payload++;
//        guc_err_code = (UINT8)*pus_payload++;
        swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"%s, Distance = %d, ErrCode = %d", gca_io_cmd_sub_resp1, gus_distance, guc_err_code);
    }

    log_meas(gca_io_cmd_sub_resp1);
    log_meas(gca_io_cmd_sub_resp2);
    log_meas(gca_io_cmd_sub_resp3);

}


void proceed_scan_dbg_calib_resp(){

    proceed_scan_dbg_resp(1);

    // apd_dbg_calibr();
   
}

void proceed_scan_dbg_meas_resp(){

    double db_distance;
    proceed_scan_dbg_resp(0);

    if (gt_cmd_scan_start.apd_auto.dw_val)
    {
        //apd_dbg_auto();
    }

    wprintf(L"\n    0x%04X      ", gus_apd);
    if (scan_measure(&db_distance))
    {
        wprintf(L"    %-07.1f     %d   %-07.1f", db_distance, gus_distance, (double)(db_distance - (double)gus_distance));
    }
    else
    {
        wprintf(L"    Err.     ");
    }


}

void proceed_scan_meas_resp(){

    proceed_scan_resp(0);

    if (gt_cmd_scan_start.apd_auto.dw_val)
    {
        //apd_auto();
    }

}

void proceed_scan_calib_resp(){

    proceed_scan_resp(1);

    if (gt_cmd_scan_start.apd_auto.dw_val)
    {
        //apd_calibr();
    }


}

void apd_dbg_calibr()
{ 
    T_FREQ_INFO *pt_freq;
    double db_sig_min;
    WORD us_apd;

    // Check input signal level modify measurement APD value if necessary
    // find min signal
    pt_freq = &gta_meas_info[0];
    db_sig_min = DBL_MAX;
    while(pt_freq->us_freq)
    {
        if (pt_freq->t_calib_sig.db_power_mV < db_sig_min)
        {
            db_sig_min = pt_freq->t_calib_sig.db_power_mV;
        }
        pt_freq++;
    }

    if (db_sig_min < 200)
    { // Too low
        if (gus_apd_calib > 0x9044)
        {
            us_apd = 0x9010;
        }
        else
        {
            us_apd = gus_apd_calib + 1;
        }
    }

    else if (db_sig_min > 500)
    { // Too high
        us_apd = gus_apd_calib - 1;
    }
    else
    { // OK, Stop calibration
        us_apd = 0;
    }
    
    gt_cmd_scan_calib.apd.dw_val = us_apd;
    dle_cmd_io_scan_calib();


    swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"\n Calibration APD = 0x%04X -> 0x%04X", gus_apd_calib, us_apd);
    log_meas(gca_io_cmd_sub_resp1);

    wprintf(L"\n Calibration APD = 0x%04X -> 0x%04X", gus_apd_calib, us_apd);

    return;
}


void apd_dbg_auto()
{ 
    T_FREQ_INFO *pt_freq;
    double db_sig_min;
    int n_sig_idx;
    int n_apd_change;

    WORD us_new_apd;

    // Check input signal level modify measurement APD value if necessary
    // find min signal
    pt_freq = &gta_meas_info[0];
    db_sig_min = DBL_MAX;
    while(pt_freq->us_freq)
    {
        if (pt_freq->t_meas_sig.db_power_mV < db_sig_min)
        {
            db_sig_min = pt_freq->t_meas_sig.db_power_mV;
        }
        pt_freq++;
    }

    // find min signal level in table
    for(n_sig_idx = 0; n_sig_idx < APD_AUTO_SIG_PARAM_NUM; n_sig_idx++)
    {
        if (db_sig_min < gt_apd_auto.ta_sig_apd_table[n_sig_idx].db_sig_level)
        {
            break;            
        }
    }

    swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"\n        %-7.1fmV (%s), ", db_sig_min, gt_apd_auto.ta_sig_apd_table[gt_apd_auto.n_curr_sig_idx].pc_sig_level_str);


    // Get APD change value from table according to crrent APD level
    n_apd_change = 0;
    
    if (gt_apd_auto.n_curr_apd_idx != -1)
    {
        n_apd_change = gt_apd_auto.ta_sig_apd_table[gt_apd_auto.n_curr_sig_idx].t_apd_param[gt_apd_auto.n_curr_apd_idx].n_change;
    }
    us_new_apd = gt_apd_auto.us_apd_value + n_apd_change;

    if (us_new_apd < APD_AUTO_MIN) us_new_apd = APD_AUTO_MIN;
    if (us_new_apd > APD_AUTO_MAX) us_new_apd = APD_AUTO_MAX;

    if (us_new_apd != gt_apd_auto.us_apd_value || gt_apd_auto.n_curr_apd_idx == -1)
    { // APD level changed

        gt_apd_auto.us_apd_value = us_new_apd;

        // find new APD index
        for(int n_apd_idx = 0; n_apd_idx < APD_AUTO_APD_PARAM_NUM; n_apd_idx++)
        {
            if (gt_apd_auto.us_apd_value <  gt_apd_auto.ta_sig_apd_table[gt_apd_auto.n_curr_sig_idx].t_apd_param[n_apd_idx].us_level)
            {
                gt_apd_auto.n_curr_apd_idx = n_apd_idx;
                break;
            }
        }

        // send modfiy APD command
        gt_cmd_scan_apd.apd.dw_val = gt_apd_auto.us_apd_value;
        dle_cmd_io_scan_apd();
    }


    swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"%s APD = 0x%04X", gca_io_cmd_sub_resp1, gt_apd_auto.us_apd_value);
    if (n_apd_change)
    {
        swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"%s(%+d)", gca_io_cmd_sub_resp1, n_apd_change);
    }
    if (gt_apd_auto.n_curr_apd_idx != -1)
    {
        swprintf(gca_io_cmd_sub_resp1, RESP_STR_LEN, L"%s (0x%04X)", gca_io_cmd_sub_resp1, gt_apd_auto.ta_sig_apd_table[gt_apd_auto.n_curr_sig_idx].t_apd_param[gt_apd_auto.n_curr_apd_idx]);
    }

    log_meas(gca_io_cmd_sub_resp1);

    return;

}
