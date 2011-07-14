/***C*********************************************************************************************
**
** SRC-FILE     :   dle_cmd_proc_inp.cpp
**                                        
** PROJECT      :   DLE user interface
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   Collateral UI function - autocomplete, history buffer etc.
**                  
** COPYRIGHT    :   
**
****C*E******************************************************************************************/

#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <time.h>
#include "cmd_line_wc.h"
#include "dle_cmd_lib.h"
//#include "dle_cmd_proc.h"
#include "dle_cmd_proc_inp.h"

void cmd_auto_complete(WCHAR *pc_cmd_line, int n_cmd_len, int *pn_cur_pos);
int cmd_suggest(WCHAR *pc_cmd_line, int *pn_cur_pos);

void match_case(WCHAR *pc_str, WCHAR *pc_template){

    if (iswlower(*pc_template)){
        _wcslwr(pc_str);
    }else{
        _wcsupr(pc_str);
    }
}

int strcharins(WCHAR *pc_dst, WCHAR c_src, int n_pos, int n_dst_max_len){

    wmemmove(pc_dst + n_pos + 1, pc_dst + n_pos, n_dst_max_len - n_pos - 1);
    *(pc_dst + n_pos) = c_src;
    *(pc_dst + n_dst_max_len-1) = 0;
    n_pos ++;
    if (n_pos == n_dst_max_len) n_pos--;
    return n_pos;
}

void cmd_auto_complete(WCHAR *pc_cmd_line, int n_cmd_len, int *pn_cur_pos){

    WCHAR   ca_inp_cmd[CMD_LINE_LENGTH];
    WCHAR   ca_last_token[30];
    WCHAR   ca_cmd_token[30];

    WCHAR   *pc_next_tok;
    WCHAR   *pc_last_tok;
    WCHAR   ca_char[2] = {0,0};

    int     n_cmd_tok_len;
    int     n_last_tok_len;
    int     n_exact_match, n_unique;

    T_DLE_CMD_FIELD  *pt_fields;
    T_DLE_CMD  *pt_cmds;
    T_DLE_CMD  *pt_exact_cmds;

    pt_fields = NULL;

    // Make input string local copy & Truncate input line after *pn_cur_pos 
    wcsncpy(ca_inp_cmd, pc_cmd_line, *pn_cur_pos);
    ca_inp_cmd[*pn_cur_pos] = 0;

    // Get First token
    pc_next_tok = wcstok(ca_inp_cmd, L" ");
    
    // Find token in command set
    ca_cmd_token[0] = 0;
    if (pc_next_tok){
        wcsncat(ca_cmd_token, pc_next_tok, wcs_sizeof(ca_last_token)-1);
    }

    pt_cmds = gta_cmd_lib;
    while(pt_cmds->pc_name){
        // exit on command match
        if (_wcsicmp(pt_cmds->pc_name, ca_cmd_token) == 0)  break;
        pt_cmds++;
    }

    ca_last_token[0] = 0;
    // Find last token
    while(pc_next_tok){
        pc_next_tok = wcstok(NULL, L" ");
        if (pc_next_tok){
            ca_last_token[0] = 0;
            wcsncat(ca_last_token, pc_next_tok, wcs_sizeof(ca_last_token)-1);
        }
    }

    if (pt_cmds->pt_fields){
        // Command recognized
        // Autocomplete command's fields

        // Add " -" if no tokens (command only)
        if (ca_last_token[0] == 0){
            *pn_cur_pos = strcharins(pc_cmd_line, L' ', *pn_cur_pos, n_cmd_len);
            *pn_cur_pos = strcharins(pc_cmd_line, L'-', *pn_cur_pos, n_cmd_len);
        }

        // Skip '-' sign in fields
        pc_last_tok = ca_last_token;
        if (*pc_last_tok == '-') pc_last_tok ++;
        
        do{
            *ca_char = 0;
            pt_fields = pt_cmds->pt_fields;
            n_last_tok_len = (int)wcslen(pc_last_tok);
            n_exact_match = FALSE;

            while(pt_fields->pc_name){

                if (_wcsnicmp(pt_fields->pc_name, pc_last_tok, n_last_tok_len) == 0){

                    // init template for first time
                    if (*ca_char == 0 && !n_exact_match){
                        *ca_char = pt_fields->pc_name[n_last_tok_len];
                        n_unique = TRUE;
                    }else{
                        n_unique = FALSE;
                    }

                    // Check field exact match
                    if (n_last_tok_len == wcslen(pt_fields->pc_name)){
                        n_exact_match = TRUE;
                    }

                    // is character is same
                    if (*ca_char != pt_fields->pc_name[n_last_tok_len]) {
                        // Clear template (ca_char) and break if template WCHAR not unique
                        *ca_char = 0;
                        break;
                    }

                }
                pt_fields++;
            } // Scan all fields

            if (*ca_char){
                // Same WCHAR found
                // Insert it to field and to result string
                match_case(ca_char, pc_last_tok);
                
                if ( n_last_tok_len + 1 < wcs_sizeof(ca_last_token)){
                    wcsncat(pc_last_tok, ca_char, 1);

                    // Insert new WCHAR and advance cursor position
                    *pn_cur_pos = strcharins(pc_cmd_line, ca_char[0], *pn_cur_pos, n_cmd_len);
                }else{
                    *ca_char = 0;
                }

            }

            if (n_exact_match && n_unique){
                // Add space and exit on exact match
                *pn_cur_pos = strcharins(pc_cmd_line, ' ', *pn_cur_pos, n_cmd_len);
                break;
            }


        }while(*ca_char);// while same chars in possible commands

        if (*ca_last_token == 0){
            // If command exact match and no other token exist
            // Confirm command by adding " -"
            *pn_cur_pos = strcharins(pc_cmd_line, ' ', *pn_cur_pos, n_cmd_len);
            *pn_cur_pos = strcharins(pc_cmd_line, '-', *pn_cur_pos, n_cmd_len);
        }

    }else{
        // Command not recognized and no other token exist
        if (ca_last_token[0] == 0){

            // Only command (full or aprtial) exist. Autocomplete commands
            do{
                *ca_char = 0;
                pt_cmds = gta_cmd_lib;
                n_cmd_tok_len = (int)wcslen(ca_cmd_token);
                pt_exact_cmds = NULL;

                while(pt_cmds->pc_name){
                    if (_wcsnicmp(pt_cmds->pc_name, ca_cmd_token, n_cmd_tok_len) == 0){

                        // init template for first time
                        if (*ca_char == 0 && !pt_exact_cmds){
                            *ca_char = pt_cmds->pc_name[n_cmd_tok_len];
                            n_unique = TRUE;
                        }else{
                            n_unique = FALSE;
                        }

                        // Check command exact match
                        if (n_cmd_tok_len == wcslen(pt_cmds->pc_name)){
                            pt_exact_cmds = pt_cmds;
                        }

                        // is character is same
                        if (*ca_char != pt_cmds->pc_name[n_cmd_tok_len]) {
                            *ca_char = 0;
                            break;
                        }
                    }
                    pt_cmds++;
                }// compare all commands with cmd_token
                
                if (*ca_char){
                    // Same char found
                    // Add it to command and to result string
                    match_case(ca_char, ca_cmd_token);
                    
                    if (wcslen(ca_cmd_token)+1 < wcs_sizeof(ca_cmd_token)){
                        wcsncat(ca_cmd_token, ca_char, 1);
                        // Insert new WCHAR and advance cursor position
                        *pn_cur_pos = strcharins(pc_cmd_line, ca_char[0], *pn_cur_pos, n_cmd_len);

                    }else
                        *ca_char = 0;
                }

                if (pt_exact_cmds && n_unique){
                    // If atleast one field exist, add space and exit on exact match
                    if (pt_exact_cmds->pt_fields->pc_str != NULL){
                        *pn_cur_pos = strcharins(pc_cmd_line, ' ', *pn_cur_pos, n_cmd_len);
                        *pn_cur_pos = strcharins(pc_cmd_line, '-', *pn_cur_pos, n_cmd_len);
                    }
                    break;
                }

            }while(*ca_char);   // while same chars is in possible commands

        }// only command token exist

    }// command not recognizes

    return;
}

void close_cmd_proc(void *pv_info){

    T_CMD_PROC_INFO   *pt_info;

    if (pv_info == NULL)
        return;

    pt_info = (T_CMD_PROC_INFO*)pv_info;

    // save history buffer
    rewind(pt_info->pf_hist);
    fwrite(pt_info->pv_cl_hist_buff, 1, pt_info->n_cl_hist_size, pt_info->pf_hist);
    fclose(pt_info->pf_hist);

    free_get_line(pt_info->pv_cmd_line_info);

    free(pv_info); 
}

void *init_cmd_proc(WCHAR *pc_cmd_hist_file_name){

    T_CMD_PROC_INFO   *pt_info;
    void *pv_cl_info;

    pt_info = (T_CMD_PROC_INFO*)malloc(sizeof(T_CMD_PROC_INFO));
    if (pt_info == NULL) return FALSE;

    // Init get line
    pv_cl_info = init_get_line(L">>", &pt_info->pv_cl_hist_buff, &pt_info->n_cl_hist_size);

    if (pv_cl_info == NULL)
        return NULL;

    // Init history buffer
    pt_info->pf_hist = _wfopen(pc_cmd_hist_file_name, L"rb+");
    if (!pt_info->pf_hist){
        pt_info->pf_hist = _wfopen(pc_cmd_hist_file_name, L"wb+");
        if (!pt_info->pf_hist){
            wprintf(L"Error opening history file\n");
            return NULL;
        }
    }

    // Fill history buffer
    fread(pt_info->pv_cl_hist_buff, 1, pt_info->n_cl_hist_size, pt_info->pf_hist);

    pt_info->pv_cmd_line_info = pv_cl_info;
    pt_info->n_cur_pos = 0;
    pt_info->n_flag = GL_FLAG_RESET;
    pt_info->ca_cmd_line[0] = '\0';
    
    return pt_info;
}

void cmd_proc_prompt(void *pv_cmd_proc_info){

    T_CMD_PROC_INFO   *pt_info;
    pt_info = (T_CMD_PROC_INFO*)pv_cmd_proc_info;

    // Preset cursor position
    set_cur_pos(pt_info->pv_cmd_line_info, pt_info->n_cur_pos);
    get_line_prompt(pt_info->pv_cmd_line_info, pt_info->ca_cmd_line, wcs_sizeof(pt_info->ca_cmd_line), pt_info->n_flag);

    pt_info->n_flag &= ~(GL_FLAG_NOPROMPT | GL_FLAG_RESET);
}

WCHAR* cmd_keys_pressed(void *pv_cmd_info, int *pn_prompt_restore){

    int n_rc, n_cur_pos, n_flag;

    T_CMD_PROC_INFO   *pt_info;
    void *pv_cl_info;
    WCHAR *pc_ret_val;
    
    pt_info = (T_CMD_PROC_INFO*)pv_cmd_info;

    pv_cl_info = pt_info->pv_cmd_line_info;
    n_cur_pos = pt_info->n_cur_pos;

    n_rc = get_line_input_events(pv_cl_info, pt_info->ca_cmd_line, wcs_sizeof(pt_info->ca_cmd_line));

    pc_ret_val = NULL;

    // Get cursor position 
    n_cur_pos = get_cur_pos(pv_cl_info);
    n_flag = 0;
    switch (n_rc){
        case GL_RC_ANY_KEY:
            //n_flag = GL_FLAG_NOPROMPT;          // skip promt on regular event
            break;
        case GL_RC_RETURN:
            wprintf(L"\n");
            n_flag = GL_FLAG_RESET;             // Reset command line at next prompt call
            pc_ret_val = pt_info->ca_cmd_line;        
            *pn_prompt_restore = TRUE;
            break;
        case GL_RC_TAB:
            cmd_auto_complete(&pt_info->ca_cmd_line[0], wcs_sizeof(pt_info->ca_cmd_line)-1, &n_cur_pos);
            n_flag = GL_FLAG_CONTINUE;
            *pn_prompt_restore = TRUE;
            break;

        case GL_RC_TAB_TAB:
            wprintf(L"\n");
            cmd_suggest(&pt_info->ca_cmd_line[0], &n_cur_pos);
            *pn_prompt_restore = TRUE;
            n_flag = 0;
            break;
    } // End of switch

    pt_info->n_cur_pos = n_cur_pos;
    pt_info->n_flag = n_flag;        

    return pc_ret_val;
}


void print_next_suggest(WCHAR *pc_str){

    CONSOLE_SCREEN_BUFFER_INFO t_console_info;
    int n_till_border;

    GetConsoleScreenBufferInfo(
        GetStdHandle(STD_OUTPUT_HANDLE),
        &t_console_info
    );

    n_till_border = t_console_info.dwSize.X - t_console_info.dwCursorPosition.X;
    
    if (n_till_border <= (int)wcslen(pc_str)) 
        wprintf(L"\n %-20s", pc_str);
    else if (n_till_border < 20) 
        wprintf(L" %s", pc_str);
    else 
        wprintf(L" %-20s", pc_str);

    return;
}

void print_col_adj(WCHAR *pc_str, int n_col_size){

    CONSOLE_SCREEN_BUFFER_INFO t_console_info;
    int n_till_border;
    int n_str_len;
    WCHAR  ca_out_str[80];

    GetConsoleScreenBufferInfo(
        GetStdHandle(STD_OUTPUT_HANDLE),
        &t_console_info
    );

    n_str_len = (int)wcslen(pc_str);
    if (n_str_len > n_col_size) n_str_len = n_col_size;
    if (n_col_size >= wcs_sizeof(ca_out_str)) n_col_size = wcs_sizeof(ca_out_str)-1;

    wmemset(ca_out_str, ' ', n_col_size);
    wmemcpy(ca_out_str, pc_str, n_str_len);
    ca_out_str[n_col_size] = 0;

    n_till_border = t_console_info.dwSize.X - t_console_info.dwCursorPosition.X;
    
    if (n_till_border <= n_col_size) 
        wprintf(L"\n%s", ca_out_str);
    else 
        wprintf(L"%s", ca_out_str);

    return;
}

int cmd_suggest(WCHAR *pc_cmd_line, int *pn_cur_pos){

    int   n_rc, n_str_max_len, n_str_len, n_cmd_hit_cnt, n_tok_len;

    WCHAR    ca_inp_cmd[CMD_LINE_LENGTH];
    WCHAR    ca_last_token[30];
    WCHAR    ca_cmd_token[30];

    WCHAR    *pc_next_tok;
    WCHAR    *pc_last_tok;

    T_DLE_CMD_FIELD  *pt_fields;
    T_DLE_CMD  *pt_cmds, *pt_hit_cmds;

    n_rc = FALSE;   // Return TRUE if something suggested FALSE otherwise

    pt_fields = NULL;

    // Truncate input line after *pn_cur_pos
    wcsncpy(ca_inp_cmd, pc_cmd_line, *pn_cur_pos);
    ca_inp_cmd[*pn_cur_pos] = 0;

    _wcsupr(ca_inp_cmd);

    // Get First token
    pc_next_tok = wcstok(ca_inp_cmd, L" ");
    
    // Find token in command set
    pt_hit_cmds = NULL;
    n_cmd_hit_cnt = 0;

    ca_cmd_token[0] = 0;
    if (pc_next_tok){
        wcsncat(ca_cmd_token, pc_next_tok, wcs_sizeof(ca_last_token)-1);
        n_tok_len = (int)wcslen(ca_cmd_token);
        pt_cmds = gta_cmd_lib;
        while(pt_cmds->pc_name){
            if (_wcsnicmp(pt_cmds->pc_name, ca_cmd_token, n_tok_len) == 0){
                n_cmd_hit_cnt ++;
                // Check exact match
                if (n_tok_len == wcslen(pt_cmds->pc_name)){
                    pt_hit_cmds = pt_cmds;
                }
            }
            pt_cmds++;
        }
    }

    ca_last_token[0] = 0;

    // Find last token
    while(pc_next_tok){
        pc_next_tok = wcstok(NULL, L" ");
        if (pc_next_tok){
            ca_last_token[0] = 0;
            wcsncat(ca_last_token, pc_next_tok, wcs_sizeof(ca_last_token)-1);
        }
    }

    // If command only
    if (ca_last_token[0] == 0){
        // Discard search result if not uniq
        if (n_cmd_hit_cnt > 1){
            pt_hit_cmds = NULL;
        }
    }

    if (pt_hit_cmds){
        pt_fields = pt_hit_cmds->pt_fields;    
        // Command recognized
        // Suggest command's fields
        // Skip '-' sign in fields
        pc_last_tok = ca_last_token;
        if (*pc_last_tok == '-') pc_last_tok ++;
        n_str_max_len = -1;

        // Calculate max field string length
        n_tok_len = (int)wcslen(pc_last_tok);
        while(pt_fields->pc_name){
            if (wcsncmp(pt_fields->pc_name, pc_last_tok, n_tok_len) == 0 &&
                pt_fields->e_type < CFT_RESERVED){

                n_str_len = (int)wcslen(pt_fields->pc_name);
                if ( n_str_len > n_str_max_len) n_str_max_len = n_str_len;
            }
            pt_fields++;
        }
        // Printout fields
        if (n_str_max_len > 0){
            n_rc = TRUE;
            pt_fields = pt_hit_cmds->pt_fields;
            // Calculate max field string length
            n_tok_len = (int)wcslen(pc_last_tok);
            while(pt_fields->pc_name){
                if (wcsncmp(pt_fields->pc_name, pc_last_tok, n_tok_len) == 0 &&
                    pt_fields->e_type < CFT_RESERVED){

                    print_col_adj(pt_fields->pc_name, n_str_max_len+2);
                }
                pt_fields++;
            }
        }

    }else{
        if (ca_last_token[0]){
            // Command not recognized and another token exist
            wprintf(L"Command %s not recognized.\n", ca_cmd_token);
        }else{

            // Only command (full or partial) exist. Suggest commands
            // Calculate max string length
            n_str_max_len = -1;
            pt_cmds = gta_cmd_lib;
            n_tok_len = (int)wcslen(ca_cmd_token);
            while(pt_cmds->pc_name){
                if (wcsncmp(pt_cmds->pc_name, ca_cmd_token, n_tok_len) == 0){
                    n_str_len = (int)wcslen(pt_cmds->pc_name);
                    if ( n_str_len > n_str_max_len) n_str_max_len = n_str_len;
                }
                pt_cmds++;
            }

            if (n_str_max_len > 0){
                n_rc = TRUE;
                // Printout
                pt_cmds = gta_cmd_lib;
                n_tok_len = (int)wcslen(ca_cmd_token);
                while(pt_cmds->pc_name){
                    if (wcsncmp(pt_cmds->pc_name, ca_cmd_token, n_tok_len) == 0){
                        print_col_adj(pt_cmds->pc_name, n_str_max_len+2);
                    }
                    pt_cmds++;
                }// End of command suggestion scan while
            } // End of something found for suggest 

        }// Command (not fields) partially or fully exist
    } // End of only first token exist

    if (n_rc) 
        wprintf(L"\n");
    else
        wprintf(L"Nothing to suggest\n");

    return n_rc;
}

T_DLE_CMD *get_next_cmd(T_DLE_CMD *pt_cmd, int n_mod){

/*
    T_DLE_CMD     *pt_fields;

    while(pt_cmd->pt_fields){
        pt_fields = pt_cmd->pt_fields;
        while(pt_fields->pc_name){
            if (wcscmp(pt_fields->pc_name, L"MOD") == 0) break;
            pt_fields++;
        }
        // If MOD field found compare value
        if (pt_fields->pc_name){
            // if (pt_fields->ul_default == ul_mod) break;
        }
        pt_cmd++;
    }
*/
    return pt_cmd;
}

void show_options_help(){

    printf(" This tool sends DLE IO controlcommands to dedicated (named)\n");
    printf(" IO pipe.                                                   \n"); 
    printf("                                                            \n");
    printf(" Input arguments format:                                    \n");
    printf("    to be defined                                           \n");
    printf(" Options: -v --verbose                                      \n");
    printf("   -v      TBD                                              \n");                 
    printf("                                                             \n");
    printf(" Actions:  -c --command;                                     \n");
    printf("                                                             \n");
    printf(" If action not specified you'll be kindly asked to enter     \n");
    printf(" DLI IO commands from console.                               \n");
    printf("                                                             \n");
    printf(" Any comments and suggestions send to:                       \n");
    printf(" solit@bk.ru                                                 \n");
    printf("                                                             \n");
    printf(" Enjoy!                                                      \n");

}

void show_cmd_help(){

    T_DLE_CMD *pt_cmd_lib;
    WCHAR    *pc_cmd_token;

    wprintf(L"\n Not implemented yet                   \n");
    return;

    pc_cmd_token = wcstok(NULL, L" ");
    if (pc_cmd_token){
        if (_wcsicmp(pc_cmd_token, L"XXX") == 0){
            wprintf(L"\n X3 commands:                   \n");
            pt_cmd_lib = gta_cmd_lib;
            while(1){
                // pt_cmd_lib = get_next_cmd(pt_cmd_lib , FW_CMD_MOD_SYS );
                if (!pt_cmd_lib->pc_name) break;
                print_next_suggest(pt_cmd_lib->pc_name);
                pt_cmd_lib++;
            }

        }else{
            wprintf(L"\n %s not recognized. ??? expected instead\n", pc_cmd_token);
        }
    // End of Command group help list
    }else{
        // Details not specified
        wprintf(L"\n");
        wprintf(L"                                                            \n");
        wprintf(L" Command format:                                            \n");
        wprintf(L"    COMMAND -OPTION_1 VALUE ... -OPTION_N VALUE             \n");
        wprintf(L"    Default values are used for unspecified options.        \n");
        wprintf(L"                                                            \n");
        wprintf(L" All commands are split to several main groups:             \n");
        wprintf(L"    xxx, xxx                                                \n");
        wprintf(L" Use HELP GROUP_NAME for more detailed list                 \n");
        wprintf(L"                                                            \n");
        wprintf(L" You may enjoy autocomplete and history buffer features     \n");
        wprintf(L" while typing. History buffer saved on graceful program     \n");
        wprintf(L" close only.                                                \n");
        wprintf(L"                                                            \n");
        wprintf(L" Any comments and suggestions send to:                      \n");
        wprintf(L" solit@bk.ru                                                \n");
        wprintf(L"                                                            \n");
        wprintf(L" Enjoy!                                                     \n");
    }

     _putws(L"\n");

}

