/***C*********************************************************************************************
**
** SRC-FILE     :   dle_cmd_proc_inp.h
**                                        
** PROJECT      :   DLE user interface
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   Interface file for collateral UI functions
**                  
** COPYRIGHT    :   
**
****C*E******************************************************************************************/
#ifndef _DLE_CMD_PROC_INP_H
#define _DLE_CMD_PROC_INP_H

#define CMD_LINE_LENGTH 1024

typedef struct cmd_proc_tag{
    void    *pv_cmd_line_info;
    int     n_cur_pos;
    int     n_flag;
    WCHAR   ca_cmd_line[CMD_LINE_LENGTH];

    FILE    *pf_hist;
    void    *pv_cl_hist_buff;
    int     n_cl_hist_size;

} T_CMD_PROC_INFO;

#define CMD_LINE_MODE       1
#define SEND_COMMAND        5
#define SHOW_OPTIONS_HELP   8

void show_options_help();

void show_cmd_help(void);
void show_options_help(void);

void *init_cmd_proc(WCHAR *pc_hist_filename);
void close_cmd_proc(void *pv_info);

void cmd_proc_prompt(void *pv_cmd_proc_info);
WCHAR* cmd_keys_pressed(void *pv_cmd_info, int *pn_prompt_restore);

#endif _DLE_CMD_PROC_INP_H