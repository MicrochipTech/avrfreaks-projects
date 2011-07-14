// TODO: allow to insert symbol if command line reach maximum size
#include <windows.h>
#include <wchar.h>
#include "cmd_line_wc.h"
 
#define INPUT_REC_BUFF_SIZE 128
#define CMD_LINE_LEN 1024
#define HISTOR_DEPTH 50

typedef struct t_history_buff_tag{
    WCHAR   ca_template[CMD_LINE_LEN];
    WCHAR   caa_lines[HISTOR_DEPTH][CMD_LINE_LEN];
    int     n_rd_pos;
    int     n_wr_pos;
} T_HISTORY_BUFF;

typedef struct t_cmd_line_info_tag{
    WORD    w_prev_key;
    WCHAR   ca_line[CMD_LINE_LEN];
    WCHAR   ca_prompt[30];
    int     n_inp_max_len;
    int     n_cur_pos;
    int     n_end_pos;
    int     n_def_pos;
    HANDLE  h_stdin;
    HANDLE  h_stdout;

    CONSOLE_SCREEN_BUFFER_INFO t_console_info;
    T_HISTORY_BUFF  t_history;

}T_CMD_LINE_INFO;

void set_cmd_line(T_CMD_LINE_INFO *pt_info, WCHAR *pc_new_line); 
void add_to_history(T_CMD_LINE_INFO *pt_info); 

int proceed_key_event(T_CMD_LINE_INFO *pt_info, KEY_EVENT_RECORD t_event);  
int proceed_back_key(T_CMD_LINE_INFO *pt_info, int n_repeat); 
int proceed_delete_key(T_CMD_LINE_INFO *pt_info, int n_repeat); 
int proceed_left_key(T_CMD_LINE_INFO *pt_info, int n_repeat); 
int proceed_ctrl_left_key(T_CMD_LINE_INFO *pt_info, int n_repeat); 
int proceed_right_key(T_CMD_LINE_INFO *pt_info, int n_repeat); 
int proceed_ctrl_right_left_key(T_CMD_LINE_INFO *pt_info, int n_repeat); 
int proceed_end_key(T_CMD_LINE_INFO *pt_info); 
int proceed_home_key(T_CMD_LINE_INFO *pt_info); 
int proceed_escape_key(T_CMD_LINE_INFO *pt_info); 

int cursor_right(T_CMD_LINE_INFO *pt_info, int n_distance); 
int cursor_left(T_CMD_LINE_INFO *pt_info, int n_distance); 

void free_get_line(void *pv_info){
    if (pv_info) free(pv_info); 
}

void* init_get_line(WCHAR *pc_prompt, void** ppv_history, int *pn_hist_size){ 

    DWORD   dw_tmp;
    T_CMD_LINE_INFO *pt_info;

    pt_info = (T_CMD_LINE_INFO*)malloc(sizeof(T_CMD_LINE_INFO)); 
    if (pt_info == NULL) return FALSE;

    // Get the standard input handle. 
    pt_info->h_stdin = GetStdHandle(STD_INPUT_HANDLE);  
    if (pt_info->h_stdin == INVALID_HANDLE_VALUE) return FALSE; 

    pt_info->h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);  
    if (pt_info->h_stdout == INVALID_HANDLE_VALUE) return FALSE; 
 
    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    ); 

    // Set promt
    if (pc_prompt != NULL){
        wcsncpy(pt_info->ca_prompt, pc_prompt, wcs_sizeof(pt_info->ca_prompt)-1); 
        pt_info->ca_prompt[wcs_sizeof(pt_info->ca_prompt)-1] = 0;
    }else{
        pt_info->ca_prompt[0] = '\0';
    }

    // Enable the window and mouse input events. 
    dw_tmp = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT; 
    if (!SetConsoleMode(pt_info->h_stdin, dw_tmp)) return FALSE;

    // Init history
    memset(&pt_info->t_history, 0, sizeof(pt_info->t_history)); 

    if (ppv_history != NULL) *ppv_history = (void*)&pt_info->t_history;
    if (pn_hist_size != NULL) *pn_hist_size = sizeof(pt_info->t_history); 

    pt_info->n_cur_pos = 0;
    pt_info->n_end_pos = 0;
    pt_info->n_def_pos = CUR_POS_UNDEFINED;    // Defined position not defined.
    pt_info->ca_line[0] = '\0';
    pt_info->w_prev_key = 0;

    return (void*)pt_info;

} 

HANDLE get_stdin_handle(void *pv_info){

    T_CMD_LINE_INFO *pt_info = (T_CMD_LINE_INFO*)pv_info;
    return pt_info->h_stdin;
}

int get_line(void *pv_info, WCHAR *pc_arg_cmd, int n_arg_cmd_len, int n_flag){

    DWORD dw_ir_read;
    int   n_rc, n_i, n_char_written;

    T_CMD_LINE_INFO *pt_info = (T_CMD_LINE_INFO*)pv_info;
    INPUT_RECORD ta_inp_rec[INPUT_REC_BUFF_SIZE]; 

    // Detrmine max input length. max_len = min(n_arg_cmd_len, sizeof(pt_info->ca_line))
    pt_info->n_inp_max_len = wcs_sizeof(pt_info->ca_line); 
    if (pt_info->n_inp_max_len > n_arg_cmd_len) pt_info->n_inp_max_len = n_arg_cmd_len;

    // Check pretype flag
    if (n_flag & GL_FLAG_PRETYPE){
        // Consider current line as history scroll entry
        pt_info->t_history.ca_template[0] = 0;
        pt_info->w_prev_key = VK_UP;
    }

    // Init get line info if CONTINUE flag is clear
    if (n_flag & GL_FLAG_CONTINUE){
        set_cmd_line(pt_info, pc_arg_cmd); 
    }else{
        // Prompt
        WriteConsole(
            pt_info->h_stdout,
            pt_info->ca_prompt,
            (DWORD)wcslen(pt_info->ca_prompt),
            (LPDWORD)&n_char_written, 
            NULL
        ); 

        // copy new line -> curr_line
        wcsncpy(pt_info->ca_line, pc_arg_cmd, pt_info->n_inp_max_len-1); 
        pt_info->ca_line[pt_info->n_inp_max_len-1] = 0;
        
        WriteConsole(
            pt_info->h_stdout,
            pt_info->ca_line,
            (DWORD)wcslen(pt_info->ca_line),
            (LPDWORD)&n_char_written, 
            NULL
        ); 

        // Set end and current cursor position
        pt_info->n_end_pos = (int)wcslen(pt_info->ca_line); 
        pt_info->n_cur_pos = pt_info->n_end_pos;
    }

    // Cursor currently at the end of line

    // move cursor if position predefined 
    if (pt_info->n_def_pos > 0 && 
        pt_info->n_def_pos < pt_info->n_end_pos){

        pt_info->n_cur_pos -= cursor_left(pt_info, pt_info->n_end_pos - pt_info->n_def_pos); 
        
        SetConsoleCursorPosition(
            pt_info->h_stdout,
            pt_info->t_console_info.dwCursorPosition
        ); 
    }

    // Loop to read and handle the input events. 
    dw_ir_read = 0;
    while (1) { 
 
        // Wait for the events. 
         n_rc = ReadConsoleInput( 
                    pt_info->h_stdin,    // input buffer handle 
                    ta_inp_rec,          // buffer to read into 
                    INPUT_REC_BUFF_SIZE, // size of read buffer 
                    &dw_ir_read );        // number of records read 

        if (!n_rc) return FALSE;
 
        // Dispatch the events to the appropriate handler. 
        for (n_i = 0; n_i < (int)dw_ir_read; n_i++){
            if (ta_inp_rec[n_i].EventType != KEY_EVENT) continue;
            switch(ta_inp_rec[n_i].EventType){ 
                case KEY_EVENT: // keyboard input 

                    n_rc = proceed_key_event(pt_info, ta_inp_rec[n_i].Event.KeyEvent);  
                    if (n_rc == GL_RC_RETURN ||
                        n_rc == GL_RC_TAB    ||   
                        n_rc == GL_RC_TAB_TAB) {
                            
                        wcsncpy(pc_arg_cmd, pt_info->ca_line, n_arg_cmd_len-1); 
                        pc_arg_cmd[n_arg_cmd_len-1] = 0;
                        return n_rc;
                    }

                    break; 
                case MOUSE_EVENT: // mouse input 
                case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
                case FOCUS_EVENT:  // disregard focus events 
                case MENU_EVENT:   // disregard menu events 
                    break; 
                default: 
                    break; 
            } // Event switch

            if (!n_rc) return FALSE;

        } // Event for

    } // while (1)

    return FALSE;
}

void get_line_prompt(void *pv_info, WCHAR *pc_arg_cmd, int n_arg_cmd_len, int n_flag){

    int n_char_written;

    T_CMD_LINE_INFO *pt_info = (T_CMD_LINE_INFO*)pv_info;

    if (n_flag & GL_FLAG_NOPROMPT){
        return;
    }

    // Check command line reset flag
    if (n_flag & GL_FLAG_RESET){
        pc_arg_cmd[0] = L'\0';
    }

    // Check pretype flag
    if (n_flag & GL_FLAG_PRETYPE){
        // Consider current line as history scroll entry
        pt_info->t_history.ca_template[0] = 0;
        pt_info->w_prev_key = VK_UP;
    }

    // Detrmine max input length. max_len = min(n_arg_cmd_len, sizeof(pt_info->ca_line))
    pt_info->n_inp_max_len = wcs_sizeof(pt_info->ca_line); 
    if (pt_info->n_inp_max_len > n_arg_cmd_len) pt_info->n_inp_max_len = n_arg_cmd_len;

    // Init get line info if CONTINUE flag is clear
    if (n_flag & GL_FLAG_CONTINUE){
        set_cmd_line(pt_info, pc_arg_cmd); 
    }else{
        // Prompt
        WriteConsole(
            pt_info->h_stdout,
            pt_info->ca_prompt,
            (DWORD)wcslen(pt_info->ca_prompt),
            (LPDWORD)&n_char_written, 
            NULL
        ); 

        // copy new line -> curr_line
        wcsncpy(pt_info->ca_line, pc_arg_cmd, pt_info->n_inp_max_len-1); 
        pt_info->ca_line[pt_info->n_inp_max_len-1] = 0;
        
        WriteConsole(
            pt_info->h_stdout,
            pt_info->ca_line,
            (DWORD)wcslen(pt_info->ca_line),
            (LPDWORD)&n_char_written, 
            NULL
        ); 

        // Set end and current cursor position
        pt_info->n_end_pos = (int)wcslen(pt_info->ca_line); 
        pt_info->n_cur_pos = pt_info->n_end_pos;
    }

    // Cursor currently at the end of line

    // move cursor if position predefined 
    if (pt_info->n_def_pos > 0 && 
        pt_info->n_def_pos < pt_info->n_end_pos){

        pt_info->n_cur_pos -= cursor_left(pt_info, pt_info->n_end_pos - pt_info->n_def_pos); 
        
        SetConsoleCursorPosition(
            pt_info->h_stdout,
            pt_info->t_console_info.dwCursorPosition
        ); 
    }

    return;
} // End of Get_line_prompt

int get_line_input_events(void *pv_info, WCHAR *pc_arg_cmd, int n_arg_cmd_len){

    DWORD dw_ir_read;
    int   n_rc, n_i;

    T_CMD_LINE_INFO *pt_info = (T_CMD_LINE_INFO*)pv_info;
    INPUT_RECORD ta_inp_rec[INPUT_REC_BUFF_SIZE]; 

    // Loop to read and handle the input events. 
    dw_ir_read = 0;
 
    // Wait for the events. 
    n_rc = ReadConsoleInput( 
                pt_info->h_stdin,    // input buffer handle 
                ta_inp_rec,          // buffer to read into 
                INPUT_REC_BUFF_SIZE, // size of read buffer 
                &dw_ir_read );        // number of records read 

    if (!n_rc) return 0;
 
    // Dispatch the events to the appropriate handler. 
    n_rc = GL_RC_ANY_KEY;
    for (n_i = 0; n_i < (int)dw_ir_read; n_i++){
        if (ta_inp_rec[n_i].EventType != KEY_EVENT) continue;
        switch(ta_inp_rec[n_i].EventType){ 
            case KEY_EVENT: // keyboard input 

                n_rc = proceed_key_event(pt_info, ta_inp_rec[n_i].Event.KeyEvent);  
                if (n_rc == GL_RC_RETURN ||
                    n_rc == GL_RC_TAB    ||   
                    n_rc == GL_RC_TAB_TAB) {
                        
                    wcsncpy(pc_arg_cmd, pt_info->ca_line, n_arg_cmd_len-1); 
                    pc_arg_cmd[n_arg_cmd_len-1] = 0;
                    return n_rc;
                }

                break; 
            case MOUSE_EVENT: // mouse input 
            case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
            case FOCUS_EVENT:  // disregard focus events 
            case MENU_EVENT:   // disregard menu events 
                break; 
            default: 
                break; 
        } // Event switch

        if (!n_rc) return 0;

    } // Event for

    return n_rc;
}

int get_cur_pos(void *pv_info){
    return ((T_CMD_LINE_INFO*)pv_info)->n_cur_pos;
}

void set_cur_pos(void *pv_info, int n_def_pos){
    ((T_CMD_LINE_INFO*)pv_info)->n_def_pos = n_def_pos;
}

int cursor_left(T_CMD_LINE_INFO *pt_info, int n_distance){

    int   n_buff_pos;
    // Get current cursor position
    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    ); 

    n_buff_pos = pt_info->t_console_info.dwCursorPosition.X  + 
                 pt_info->t_console_info.dwCursorPosition.Y * pt_info->t_console_info.dwSize.X;

    if (n_distance > n_buff_pos) n_distance = n_buff_pos;


    // Update cursor position
    pt_info->t_console_info.dwCursorPosition.X -= n_distance;
    while (pt_info->t_console_info.dwCursorPosition.X < 0){
        pt_info->t_console_info.dwCursorPosition.X += pt_info->t_console_info.dwSize.X;
        pt_info->t_console_info.dwCursorPosition.Y -= 1;
    }

    if (pt_info->t_console_info.dwCursorPosition.Y < 0)
        pt_info->t_console_info.dwCursorPosition.Y = 0;

    return n_distance;
}
int cursor_right(T_CMD_LINE_INFO *pt_info, int n_distance){

    // Get current cursor position
    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    ); 

    pt_info->t_console_info.dwCursorPosition.X += (short)n_distance;
    while (pt_info->t_console_info.dwCursorPosition.X >= pt_info->t_console_info.dwSize.X){
        pt_info->t_console_info.dwCursorPosition.X -= pt_info->t_console_info.dwSize.X;
        pt_info->t_console_info.dwCursorPosition.Y += 1;
    }

    if (pt_info->t_console_info.dwCursorPosition.Y > pt_info->t_console_info.dwSize.Y)
        pt_info->t_console_info.dwCursorPosition.Y = pt_info->t_console_info.dwSize.Y;

    return n_distance;
}

int ins_chars(T_CMD_LINE_INFO *pt_info, WCHAR c_ascii, int n_count){
    
    int n_max_ins = pt_info->n_inp_max_len - pt_info->n_end_pos - 1 ;
    int n_move_block_size = pt_info->n_end_pos - pt_info->n_cur_pos + 1 ;

    n_count = (n_count > n_max_ins)? n_max_ins : n_count;
    wmemmove(&pt_info->ca_line[pt_info->n_cur_pos+n_count],
             &pt_info->ca_line[pt_info->n_cur_pos],
             n_move_block_size); 

    wmemset(&pt_info->ca_line[pt_info->n_cur_pos], c_ascii, n_count); 

    pt_info->n_cur_pos += n_count;
    pt_info->n_end_pos += n_count;

    return n_count;   
}

int proceed_end_key(T_CMD_LINE_INFO *pt_info){

    int   n_distance = pt_info->n_end_pos - pt_info->n_cur_pos;

    cursor_right(pt_info, n_distance); 

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    pt_info->n_cur_pos = pt_info->n_end_pos;

    return 0;
}

int proceed_ctrl_right_key(T_CMD_LINE_INFO *pt_info, int n_repeat){


    if (pt_info->n_cur_pos + n_repeat > pt_info->n_end_pos){
        n_repeat = pt_info->n_end_pos - pt_info->n_cur_pos;
    }

    n_repeat = cursor_right(pt_info, n_repeat); 

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    pt_info->n_cur_pos += n_repeat;

    return 0;
}

int proceed_right_key(T_CMD_LINE_INFO *pt_info, int n_repeat){


    if (pt_info->n_cur_pos + n_repeat > pt_info->n_end_pos){
        n_repeat = pt_info->n_end_pos - pt_info->n_cur_pos;
    }

    n_repeat = cursor_right(pt_info, n_repeat); 

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    pt_info->n_cur_pos += n_repeat;

    return 0;
}

int proceed_home_key(T_CMD_LINE_INFO *pt_info){

    int   n_distance;
    n_distance = cursor_left(pt_info, pt_info->n_cur_pos);  

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    pt_info->n_cur_pos -= n_distance;

    return 0;
}

int proceed_escape_key(T_CMD_LINE_INFO *pt_info){

    int   n_distance, n_char_written;
    
    n_distance = cursor_left(pt_info, pt_info->n_cur_pos);  

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    pt_info->n_cur_pos -= n_distance;

    wmemset(&pt_info->ca_line[pt_info->n_cur_pos], 0, pt_info->n_end_pos-pt_info->n_cur_pos); 

    WriteConsole(
        pt_info->h_stdout,
        pt_info->ca_line,
        pt_info->n_end_pos-pt_info->n_cur_pos,
        (LPDWORD)&n_char_written, 
        NULL
    ); 

    pt_info->n_end_pos = pt_info->n_cur_pos;

    // Restore cursor position
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    return 0;
}

int proceed_left_key(T_CMD_LINE_INFO *pt_info, int n_repeat){

    int   n_buff_pos;

    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    ); 
    
    n_repeat = (n_repeat > pt_info->n_cur_pos) ? pt_info->n_cur_pos : n_repeat;

    n_buff_pos = pt_info->t_console_info.dwCursorPosition.X  + 
                 pt_info->t_console_info.dwCursorPosition.Y * pt_info->t_console_info.dwSize.X;

    if (n_repeat > n_buff_pos) n_repeat = n_buff_pos;

    n_repeat = cursor_left(pt_info, n_repeat); 

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    pt_info->n_cur_pos -= n_repeat;

    return n_repeat;
}

int proceed_ctrl_left_key(T_CMD_LINE_INFO *pt_info, int n_repeat){

    int   n_buff_pos;

    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    ); 
    
    n_repeat = (n_repeat > pt_info->n_cur_pos) ? pt_info->n_cur_pos : n_repeat;

    n_buff_pos = pt_info->t_console_info.dwCursorPosition.X  + 
                 pt_info->t_console_info.dwCursorPosition.Y * pt_info->t_console_info.dwSize.X;

    if (n_repeat > n_buff_pos) n_repeat = n_buff_pos;

    n_repeat = cursor_left(pt_info, n_repeat); 

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    pt_info->n_cur_pos -= n_repeat;

    return n_repeat;
}

int proceed_back_key(T_CMD_LINE_INFO *pt_info, int n_repeat){

    int   n_i, n_char_written, n_buff_pos;

    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    ); 
    
    n_repeat = (n_repeat > pt_info->n_cur_pos) ? pt_info->n_cur_pos : n_repeat;

    n_buff_pos = pt_info->t_console_info.dwCursorPosition.X  + 
                 pt_info->t_console_info.dwCursorPosition.Y * pt_info->t_console_info.dwSize.X;

    if (n_repeat > n_buff_pos) n_repeat = n_buff_pos;


    wmemmove(&pt_info->ca_line[pt_info->n_cur_pos - n_repeat],
             &pt_info->ca_line[pt_info->n_cur_pos],
             pt_info->n_end_pos - pt_info->n_cur_pos + 1
    ); 

    wmemset(&pt_info->ca_line[pt_info->n_end_pos - n_repeat + 1], 0, n_repeat); 

    pt_info->n_cur_pos -= n_repeat;
    pt_info->n_end_pos -= n_repeat;

    n_repeat = cursor_left(pt_info, n_repeat); 

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    // Write characters
    WriteConsole(
        pt_info->h_stdout,
        &pt_info->ca_line[pt_info->n_cur_pos],
        pt_info->n_end_pos - pt_info->n_cur_pos + 1,
        (LPDWORD)&n_char_written, 
        NULL
    ); 

    // Remove trail
    for(n_i = 0; n_i < n_repeat; n_i++){
        WriteConsole(
            pt_info->h_stdout,
            &" ",
            1,
            (LPDWORD)&n_char_written, 
            NULL
        ); 
    }

    // Restore cursor position
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    return n_repeat;
}
int proceed_delete_key(T_CMD_LINE_INFO *pt_info, int n_repeat){

    int   n_i, n_char_written, n_tail_len;

    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    ); 
    
    n_tail_len = pt_info->n_end_pos - pt_info->n_cur_pos;

    n_repeat = (n_repeat > n_tail_len) ? n_tail_len : n_repeat;

    wmemmove(&pt_info->ca_line[pt_info->n_cur_pos],
             &pt_info->ca_line[pt_info->n_cur_pos] + n_repeat,
             n_tail_len
    ); 

    wmemset(&pt_info->ca_line[pt_info->n_end_pos - n_repeat + 1], 0, n_repeat); 

    pt_info->n_end_pos -= n_repeat;

    // Write characters
    WriteConsole(
        pt_info->h_stdout,
        &pt_info->ca_line[pt_info->n_cur_pos],
        pt_info->n_end_pos - pt_info->n_cur_pos + 1,
        (LPDWORD)&n_char_written, 
        NULL
    ); 

    // Remove trail
    for(n_i = 0; n_i < n_repeat; n_i++){
        WriteConsole(
            pt_info->h_stdout,
            &" ",
            1,
            (LPDWORD)&n_char_written, 
            NULL
        ); 
    }

    // Restore cursor position
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 


    return n_repeat;
}
void proceed_printable_key(T_CMD_LINE_INFO *pt_info, WCHAR c_ascii, int n_repeat){

    int n_ins, n_char_written;

    // Check is command fit to screen buffer
    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    ); 

    n_ins = ins_chars(pt_info, c_ascii, n_repeat); 

    // Write new characters
    WriteConsole(
        pt_info->h_stdout,
        &pt_info->ca_line[pt_info->n_cur_pos - n_ins],
        n_ins,
        (LPDWORD)&n_char_written, 
        NULL
    ); 

    // Write Old characters
    WriteConsole(
        pt_info->h_stdout,
        &pt_info->ca_line[pt_info->n_cur_pos],
        pt_info->n_end_pos - pt_info->n_cur_pos + 1,
        (LPDWORD)&n_char_written, 
        NULL
    ); 

    cursor_left(pt_info, pt_info->n_end_pos - pt_info->n_cur_pos + 1); 

    // Restore cursor position
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    ); 

    
}

void set_cmd_line(T_CMD_LINE_INFO *pt_info, WCHAR *pc_new_line){

    int n_char_written;
    T_HISTORY_BUFF  *pt_hist = &pt_info->t_history;

    // Clear current line
    proceed_escape_key(pt_info); 

    // copy new line -> curr_line
    wcsncpy(pt_info->ca_line, pc_new_line, pt_info->n_inp_max_len-1); 
    pt_info->ca_line[pt_info->n_inp_max_len-1] = 0;
    
    pt_info->n_cur_pos = (int)wcslen(pt_info->ca_line); 
    pt_info->n_end_pos = pt_info->n_cur_pos;

    WriteConsole(
        pt_info->h_stdout,
        pt_info->ca_line,
        (DWORD)wcslen(pt_info->ca_line),
        (LPDWORD)&n_char_written, 
        NULL
    ); 

}

void add_to_history(T_CMD_LINE_INFO *pt_info){

    T_HISTORY_BUFF  *pt_hist = &pt_info->t_history;
    
    int   n_pos, n_allow_add;

    n_allow_add = TRUE;
    // Add to history only if command line not equal to previous history line
    n_pos = pt_hist->n_wr_pos - 1;
    if (n_pos < 0) n_pos = HISTOR_DEPTH - 1;
    if (_wcsicmp(pt_hist->caa_lines[n_pos], pt_info->ca_line) == 0) n_allow_add = FALSE;

    // Do not add empty lines
    if (pt_info->ca_line[0] == '\0') n_allow_add = FALSE;

    if (n_allow_add){
        wcscpy(pt_hist->caa_lines[pt_hist->n_wr_pos], pt_info->ca_line); 
        pt_hist->n_wr_pos ++;
        if (pt_hist->n_wr_pos == HISTOR_DEPTH) pt_hist->n_wr_pos = 0;
        if (pt_hist->n_wr_pos  < 0) pt_hist->n_wr_pos = HISTOR_DEPTH - 1;
    }

    pt_hist->n_rd_pos = pt_hist->n_wr_pos;
}

int scroll_history(T_CMD_LINE_INFO *pt_info, WORD w_keycode){

    int n_rc, n_template_len, n_pos, n_found;
    T_HISTORY_BUFF  *pt_hist = &pt_info->t_history;

    n_template_len = (int)wcslen(pt_hist->ca_template); 
    
    n_pos  = pt_hist->n_rd_pos;

    do{
        switch(w_keycode){
            case VK_UP :
                n_pos --;
                break;
            case VK_DOWN:
                n_pos ++;
                break;
        }
        if ( n_pos == HISTOR_DEPTH) n_pos = 0;
        if ( n_pos < 0) n_pos = HISTOR_DEPTH - 1;
        
        if (n_template_len == 0){
            n_found = (*pt_hist->caa_lines[n_pos] == 0); 
        }else{
            n_found = (_wcsnicmp(pt_hist->caa_lines[n_pos], pt_hist->ca_template, n_template_len) != 0); 
        }

    // Scroll whole loop until not zero 
    }while(n_found && n_pos != pt_hist->n_wr_pos); 

    // Buffer roll over
    if (n_pos == pt_hist->n_wr_pos){
        if (n_template_len == 0){
            set_cmd_line(pt_info, L""); 
        }else{
            set_cmd_line(pt_info, pt_hist->ca_template); 
        }
    }else{
        set_cmd_line(pt_info, pt_hist->caa_lines[n_pos]); 
    }

    
    pt_hist->n_rd_pos = n_pos;

    n_rc = TRUE;
    return n_rc;
}

// ********************************************

int proceed_key_event(T_CMD_LINE_INFO *pt_info, KEY_EVENT_RECORD t_event){
    
    int   n_rc;
    
    n_rc = GL_RC_ANY_KEY;

    if (t_event.bKeyDown == TRUE){
        // If printable key pressed

        switch (t_event.wVirtualKeyCode){
            case VK_CANCEL  :
                n_rc = FALSE;
                break;
            case VK_TAB     :
                if (pt_info->w_prev_key != VK_TAB){
                    n_rc = GL_RC_TAB;
                }else{
                    n_rc = GL_RC_TAB_TAB;
                }
                break;
            case VK_UP      :
            case VK_DOWN    :
                if (pt_info->w_prev_key != VK_UP && pt_info->w_prev_key != VK_DOWN ){
                    wcscpy(pt_info->t_history.ca_template, pt_info->ca_line); 
                }

                scroll_history(pt_info, t_event.wVirtualKeyCode); 
                break;

            case VK_RETURN  :
                add_to_history(pt_info); 
                proceed_end_key(pt_info); 
                n_rc =  GL_RC_RETURN;
                break;
            case VK_ESCAPE  :
                    proceed_escape_key(pt_info); 
                break;
            case VK_DELETE  :
                    proceed_delete_key(pt_info, t_event.wRepeatCount); 
                break;
            case VK_BACK    :
                    proceed_back_key(pt_info, t_event.wRepeatCount); 
                break;
            case VK_END     :
                    proceed_end_key(pt_info); 
                break;
            case VK_HOME    :
                    proceed_home_key(pt_info); 
                break;
            case VK_LEFT    :
                if (t_event.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)){
                    proceed_ctrl_left_key(pt_info, t_event.wRepeatCount); 
                }else{
                    proceed_left_key(pt_info, t_event.wRepeatCount); 
                }

                break;
            case VK_RIGHT   :
                if (t_event.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)){
                    proceed_ctrl_right_key(pt_info, t_event.wRepeatCount); 
                }else{
                    proceed_right_key(pt_info, t_event.wRepeatCount); 
                }
                break;
            default :
                if (t_event.uChar.AsciiChar != 0){
                    proceed_printable_key(pt_info, t_event.uChar.AsciiChar, t_event.wRepeatCount); 
                    break;
                } // End of Printable char
        }// Printable character switch

        pt_info->w_prev_key = t_event.wVirtualKeyCode;

    } // Key Pressed

    return n_rc;
}

