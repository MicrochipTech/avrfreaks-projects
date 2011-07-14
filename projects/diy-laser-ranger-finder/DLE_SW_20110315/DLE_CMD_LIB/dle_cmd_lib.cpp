/***C*********************************************************************************************
**
** SRC-FILE     :   dle_cmd_lib.cpp
**                                        
** PROJECT      :   DLE
**                                                                
** SRC-VERSION  :   
**              
** DATE         :   
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   
**                  
** FUNCTIONS    :   
**                  
** COMPILER     :   
**
** REFERENCES   :   
**
** COPYRIGHT    :   
**
****C*E******************************************************************************************/

#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include "dle_cmd_lib.h"

// ---------------- Internal Functions ----------------------
int update_dle_cmd_int(WCHAR *pc_cmd_arg, T_DLE_CMD_FIELD *pt_fields, int n_update);

//---------------------------------------------------------------------------
t_cmd_init_tag gt_cmd_init = {
    {NULL, CFT_LAST, 0, 0}
};

//---------------------------------------------------------------------------
t_cmd_dev_sign_tag gt_cmd_dev_sign = {
    {NULL, CFT_LAST, 0, 0}
};

//---------------------------------------------------------------------------
t_cmd_spi_rd_tag gt_cmd_spi_rd = {
    {L"ADDR"    ,  CFT_NUM,      0,            0},
    {L"LEN"     ,  CFT_NUM,      0,           14},
    {NULL, CFT_LAST, 0, 0}
};

//---------------------------------------------------------------------------
#define SPI_WR_DATA_LEN 128
WCHAR ca_spi_wr_data[SPI_WR_DATA_LEN];
t_cmd_spi_wr_tag gt_cmd_spi_wr = {
    {L"DATA"    ,  CFT_TXT,      SPI_WR_DATA_LEN,  (DWORD)ca_spi_wr_data},
    {NULL, CFT_LAST, 0, 0}
};

//---------------------------------------------------------------------------
t_cmd_apd_on_tag gt_cmd_apd_on = {
    {L"BIAS_VALUE"      ,  CFT_NUM,      0,           0},
    {L"BIAS_CTRL_LEVEL" ,  CFT_NUM,      0,           0},
    {NULL, CFT_LAST, 0, 0}
};

//---------------------------------------------------------------------------
t_cmd_apd_off_tag gt_cmd_apd_off = {
    {NULL, CFT_LAST, 0, 0}
};

//---------------------------------------------------------------------------
t_cmd_aim_tag gt_cmd_aim = {
    {L"ACT"      ,  CFT_NUM,      0,           2},  // Default value is TOGGLE
    {NULL, CFT_LAST, 0, 0}
};

//---------------------------------------------------------------------------
t_cmd_curtain_tag gt_cmd_curtain = {
    {NULL, CFT_LAST, 0, 0}
};

//---------------------------------------------------------------------------
t_cmd_measure_tag gt_cmd_measure = {
    {L"STREAM"      ,  CFT_NUM,      0,           0},
    {L"SINCOS"      ,  CFT_NUM,      0,           0},
    {NULL, CFT_LAST, 0, 0}
};

//---------------------------------------------------------------------------
#define LCD_DIGITS_NUM 5
#define LCD_DIGITS_STR_LEN (LCD_DIGITS_NUM + 1)
WCHAR ca_lcd_dig0[LCD_DIGITS_STR_LEN];
WCHAR ca_lcd_dig1[LCD_DIGITS_STR_LEN];

t_cmd_lcd_tag gt_cmd_lcd = {
    {L"dig0"      ,  CFT_TXT,      LCD_DIGITS_NUM,           (DWORD)ca_lcd_dig0},
    {L"dig1"      ,  CFT_TXT,      LCD_DIGITS_NUM,           (DWORD)ca_lcd_dig1},
    {L"signs"     ,  CFT_NUM,      4,           0},
    {NULL, CFT_LAST, 0, 0}
};

#if 0
//---------------------------------------------------------------------------
t_cmd_???_tag gt_cmd_??? = {
    {L"???"      ,  CFT_NUM,      0,           0},
    {NULL, CFT_LAST, 0, 0}
};

#endif // #if 0


T_DLE_CMD gta_cmd_lib[] = {
    { L"DEV_SIGN"                 , (T_DLE_CMD_FIELD*)&gt_cmd_dev_sign},
    { L"INIT"                     , (T_DLE_CMD_FIELD*)&gt_cmd_init},
    { L"SPI_RD"                   , (T_DLE_CMD_FIELD*)&gt_cmd_spi_rd},
    { L"SPI_WR"                   , (T_DLE_CMD_FIELD*)&gt_cmd_spi_wr},
    { L"APD_ON"                   , (T_DLE_CMD_FIELD*)&gt_cmd_apd_on},
    { L"APD_OFF"                  , (T_DLE_CMD_FIELD*)&gt_cmd_apd_off},
    { L"AIM"                      , (T_DLE_CMD_FIELD*)&gt_cmd_aim},
    { L"CURTAIN_UP"               , (T_DLE_CMD_FIELD*)&gt_cmd_curtain},
    { L"CURTAIN_DOWN"             , (T_DLE_CMD_FIELD*)&gt_cmd_curtain},
    { L"CURTAIN_DIS"              , (T_DLE_CMD_FIELD*)&gt_cmd_curtain},
    { L"MEASURE"                  , (T_DLE_CMD_FIELD*)&gt_cmd_measure},
    { L"CALIB"                    , (T_DLE_CMD_FIELD*)&gt_cmd_measure},
    { L"LCD"                      , (T_DLE_CMD_FIELD*)&gt_cmd_lcd},
    {0, 0}
};

//---------------------------------------------------------------------------

T_DLE_CMD* lookup_dle_cmd(WCHAR *pc_cmd_arg, T_DLE_CMD *pt_cmd_lib){

    while(pt_cmd_lib->pc_name){
        if (_wcsicmp(pc_cmd_arg, pt_cmd_lib->pc_name) == 0){
            break;
        }
        pt_cmd_lib++;
    } // end of command scan while

    return pt_cmd_lib;
}

T_DLE_CMD* decomposite_dle_cmd(WCHAR *pc_cmd_arg, T_DLE_CMD *pt_cmd, int n_update){

    WCHAR   ca_cmd[1024];
    WCHAR   *pc_cmd_token, *pc_cmd_line_end;

    int n_rc = FALSE;

    // Make command local copy to use STRTOK
    if (!(*pc_cmd_arg)) return NULL;
    wcscpy(ca_cmd, pc_cmd_arg);

    // Read command
    pc_cmd_token = wcstok(ca_cmd, L" ");
    if (!(*pc_cmd_token)) return NULL;

    pt_cmd = lookup_dle_cmd(pc_cmd_token, pt_cmd);

    // Check is command was found, update fields if so
    if (pt_cmd->pt_fields){

        // Calc last element address
        pc_cmd_line_end = &ca_cmd[wcslen(pc_cmd_arg)];

        // Move pointer over command if not end of line
        pc_cmd_token += wcslen(pc_cmd_token);
        if (pc_cmd_token != pc_cmd_line_end) pc_cmd_token ++;

        // Parse command WITH fields update
        n_rc = update_dle_cmd_int(pc_cmd_token, pt_cmd->pt_fields, n_update);
    }else{
        fwprintf(stderr, L"Command not found %s\n", pc_cmd_token);
    }

    return (n_rc ? pt_cmd : NULL);
}

int update_dle_cmd_int(WCHAR *pc_cmd_arg, T_DLE_CMD_FIELD *pt_fields, int n_update){

    int  n_rc;
    int  n_value;
    WCHAR  *pc_cmd, *pc_value;
    int n_skip_tokenization_fl;
    T_DLE_CMD_FIELD *pt_curr_field;


    n_rc = TRUE;

    if (!(*pc_cmd_arg)) return n_rc;

    pc_cmd = wcstok(pc_cmd_arg, L" ");
    
    n_skip_tokenization_fl = 0;

    while(pc_cmd){
        if (pc_cmd[0] != L'-') {
            fwprintf(stderr, L"Command field name must follow '-' symbol\n");
            n_rc = FALSE;
            break;
        }else {
            pc_cmd++; // Move pointer over '-'
        }

        // Scan fields table and find cmd field in fields table
        pt_curr_field = pt_fields;

        n_rc = FALSE;
        while(pt_curr_field->pc_name){
            if (_wcsicmp(pt_curr_field->pc_name, pc_cmd) == 0){
                // Field name found here

                // Get Field value
                pc_value = wcstok(NULL, L" ");
                if (pc_value ==  NULL){
                    fwprintf(stderr, L"Value not specified \n", pc_value);
                    break;
                }

                // Field name was found and valu exist

                // Update field if requried
                switch (pt_curr_field->e_type){
                    case CFT_NUM: 
                        if (swscanf_s( pc_value, L"%i", &n_value) != 1){
                            fwprintf(stderr, L"Cannot recognize value %s \n", pc_value);
                            break;
                        }
                        
                        if (n_update) {
                            pt_curr_field->dw_val = (DWORD)n_value;
                        }
                        break;

                    case CFT_TXT: 
                        WCHAR *pc_dst = pt_curr_field->pc_str;
                        int n_dst_size = pt_curr_field->n_len;

                        while(pc_value){
                            if (n_update){
                                wcsncpy_s(pc_dst, n_dst_size+1, pc_value, _TRUNCATE);
                            }
                            pc_dst += wcslen(pc_value);
                            n_dst_size -= wcslen(pc_value);

                            pc_value = wcstok(NULL, L" ");

                            // check is another token started
                            if (pc_value && pc_value[0] == L'-') 
                            {
                                n_skip_tokenization_fl = true;
                                pc_cmd = pc_value;
                                break;
                            }

                            if (pc_value && n_dst_size > 0){
                                *pc_dst++ = L' ';
                                n_dst_size--;
                            }
                        }
                        break;

                } // End of field type switch

                n_rc = TRUE;
                break;  // break from field scan

            }// End of field match
            pt_curr_field++;
        } // End of field scan parse
 
        // Check field not found
        if (!pt_curr_field->pc_name){
            fwprintf(stderr, L"Field %s not recognized\n", pc_cmd);
        }

        // l_rc == TRUE only if field found and specified value fit to field
        if (!n_rc) break;

        // Read next field from argument string
        if (!n_skip_tokenization_fl)
            pc_cmd = wcstok(NULL, L" ");

        n_skip_tokenization_fl = false;

    }// End of Command parse while

    return n_rc;
}
