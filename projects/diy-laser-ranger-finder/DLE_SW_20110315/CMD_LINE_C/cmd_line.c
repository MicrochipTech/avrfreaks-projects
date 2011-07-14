// TODO: allow to insert char if command line reach maximum size
#include <windows.h>
#include <stdio.h>
#include "cmd_line.h"
 
#define INPUT_REC_BUFF_SIZE 128
#define CMD_LINE_LEN 1024
#define HISTOR_DEPTH 50

typedef struct t_history_buff_tag{
    char    ca_template[CMD_LINE_LEN];
    char    caa_lines[HISTOR_DEPTH][CMD_LINE_LEN];
    int32   l_rd_pos;
    int32   l_wr_pos;
} T_HISTORY_BUFF;

typedef struct t_cmd_line_info_tag{
    int16   s_prev_key;
    char    ca_line[CMD_LINE_LEN];
    char    ca_prompt[30];
    int32   l_inp_max_len;
    int32   l_cur_pos;
    int32   l_end_pos;
    int32   l_def_pos;
    HANDLE  h_stdin;
    HANDLE  h_stdout;

    CONSOLE_SCREEN_BUFFER_INFO t_console_info;
    T_HISTORY_BUFF  t_history;

}T_CMD_LINE_INFO;

void set_cmd_line(T_CMD_LINE_INFO *pt_info, char *pc_new_line);
void add_to_history(T_CMD_LINE_INFO *pt_info);

int32 proceed_key_event(T_CMD_LINE_INFO *pt_info, KEY_EVENT_RECORD t_event); 
int32 proceed_back_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat);
int32 proceed_delete_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat);
int32 proceed_left_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat);
int32 proceed_ctrl_left_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat);
int32 proceed_right_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat);
int32 proceed_ctrl_right_left_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat);
int32 proceed_end_key(T_CMD_LINE_INFO *pt_info);
int32 proceed_home_key(T_CMD_LINE_INFO *pt_info);
int32 proceed_escape_key(T_CMD_LINE_INFO *pt_info);

int32 cursor_right(T_CMD_LINE_INFO *pt_info, int32 l_distance);
int32 cursor_left(T_CMD_LINE_INFO *pt_info, int32 l_distance);

void free_get_line(void *pv_info){
    if (pv_info) free(pv_info);
}

void* init_get_line(char *pc_prompt, void** ppv_history, uint32 *pul_hist_size){ 

    uint32  ul_tmp;
    T_CMD_LINE_INFO *pt_info;

    pt_info = malloc(sizeof(T_CMD_LINE_INFO));
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
        strncpy(pt_info->ca_prompt, pc_prompt, sizeof(pt_info->ca_prompt)-1);
        pt_info->ca_prompt[sizeof(pt_info->ca_prompt)-1] = 0;
    }else{
        pt_info->ca_prompt[0] = '\0';
    }

    // Enable the window and mouse input events. 
    ul_tmp = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT; 
    if (!SetConsoleMode(pt_info->h_stdin, ul_tmp)) return FALSE;

    // Init history
    memset(&pt_info->t_history, 0, sizeof(pt_info->t_history));

    if (ppv_history != NULL) *ppv_history = (void*)&pt_info->t_history;
    if (pul_hist_size != NULL) *pul_hist_size = sizeof(pt_info->t_history);

    pt_info->l_cur_pos = 0;
    pt_info->l_end_pos = 0;
    pt_info->l_def_pos = CUR_POS_UNDEFINED;    // Defined position not defined.
    pt_info->ca_line[0] = '\0';
    pt_info->s_prev_key = 0;

    return (void*)pt_info;

} 

int32 get_line(void *pv_info, char *pc_arg_cmd, uint32 ul_arg_cmd_len, uint32 ul_flag){

    uint32  ul_ir_read;
    int32   l_rc, l_i, l_char_written;

    T_CMD_LINE_INFO *pt_info = pv_info;
    INPUT_RECORD ta_inp_rec[INPUT_REC_BUFF_SIZE]; 

    // Detrmine max input length. max_len = min(ul_arg_cmd_len, sizeof(pt_info->ca_line))
    pt_info->l_inp_max_len = sizeof(pt_info->ca_line);
    if (pt_info->l_inp_max_len > (int32)ul_arg_cmd_len) pt_info->l_inp_max_len = ul_arg_cmd_len;

    // Check pretype flag
    if (ul_flag & GL_FLAG_PRETYPE){
        // Consider current line as history scroll entry
        pt_info->t_history.ca_template[0] = 0;
        pt_info->s_prev_key = VK_UP;
    }

    // Init get line info if CONTINUE flag is clear
    if (ul_flag & GL_FLAG_CONTINUE){
        set_cmd_line(pt_info, pc_arg_cmd);
    }else{
        // Prompt
        WriteConsole(
            pt_info->h_stdout,
            pt_info->ca_prompt,
            (int32)strlen(pt_info->ca_prompt),
            &l_char_written, 
            NULL
        );

        // copy new line -> curr_line
        strncpy(pt_info->ca_line, pc_arg_cmd, pt_info->l_inp_max_len-1);
        pt_info->ca_line[pt_info->l_inp_max_len-1] = 0;
        
        WriteConsole(
            pt_info->h_stdout,
            pt_info->ca_line,
            (int32)strlen(pt_info->ca_line),
            &l_char_written, 
            NULL
        );

        // Set end and current cursor position
        pt_info->l_end_pos = (int32)strlen(pt_info->ca_line);
        pt_info->l_cur_pos = pt_info->l_end_pos;
    }

    // Cursor currently at the end of line

    // move cursor if position predefined 
    if (pt_info->l_def_pos > 0 && 
        pt_info->l_def_pos < pt_info->l_end_pos){

        pt_info->l_cur_pos -= cursor_left(pt_info, pt_info->l_end_pos - pt_info->l_def_pos);
        
        SetConsoleCursorPosition(
            pt_info->h_stdout,
            pt_info->t_console_info.dwCursorPosition
        );
    }

    // Loop to read and handle the input events. 
    ul_ir_read = 0;
    while (1) { 
 
        // Wait for the events. 
         l_rc = ReadConsoleInput( 
                    pt_info->h_stdin,    // input buffer handle 
                    ta_inp_rec,          // buffer to read into 
                    INPUT_REC_BUFF_SIZE, // size of read buffer 
                    &ul_ir_read );       // number of records read 

        if (!l_rc) return FALSE;
 
        // Dispatch the events to the appropriate handler. 
        for (l_i = 0; l_i < (int32)ul_ir_read; l_i++){
            switch(ta_inp_rec[l_i].EventType){ 
                case KEY_EVENT: // keyboard input 

                    l_rc = proceed_key_event(pt_info, ta_inp_rec[l_i].Event.KeyEvent); 
                    if (l_rc == GL_RC_RETURN ||
                        l_rc == GL_RC_TAB    ||   
                        l_rc == GL_RC_TAB_TAB) {
                            
                        strncpy(pc_arg_cmd, pt_info->ca_line, ul_arg_cmd_len-1);
                        pc_arg_cmd[ul_arg_cmd_len-1] = 0;
                        return l_rc;
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

            if (!l_rc) return FALSE;

        } // Event for

    } // while (1)

    return FALSE;
}

int32 get_cur_pos(void *pv_info){
    return ((T_CMD_LINE_INFO*)pv_info)->l_cur_pos;
}

void set_cur_pos(void *pv_info, int32 l_def_pos){
    ((T_CMD_LINE_INFO*)pv_info)->l_def_pos = l_def_pos;
}

int32 cursor_left(T_CMD_LINE_INFO *pt_info, int32 l_distance){

    int32   l_buff_pos;
    // Get current cursor position
    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    );

    l_buff_pos = pt_info->t_console_info.dwCursorPosition.X  + 
                 pt_info->t_console_info.dwCursorPosition.Y * pt_info->t_console_info.dwSize.X;

    if (l_distance > l_buff_pos) l_distance = l_buff_pos;


    // Update cursor position
    pt_info->t_console_info.dwCursorPosition.X -= (int16)l_distance;
    while (pt_info->t_console_info.dwCursorPosition.X < 0){
        pt_info->t_console_info.dwCursorPosition.X += pt_info->t_console_info.dwSize.X;
        pt_info->t_console_info.dwCursorPosition.Y -= 1;
    }

    if (pt_info->t_console_info.dwCursorPosition.Y < 0)
        pt_info->t_console_info.dwCursorPosition.Y = 0;

    return l_distance;
}
int32 cursor_right(T_CMD_LINE_INFO *pt_info, int32 l_distance){

    // Get current cursor position
    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    );

    pt_info->t_console_info.dwCursorPosition.X += (int16)l_distance;
    while (pt_info->t_console_info.dwCursorPosition.X >= pt_info->t_console_info.dwSize.X){
        pt_info->t_console_info.dwCursorPosition.X -= pt_info->t_console_info.dwSize.X;
        pt_info->t_console_info.dwCursorPosition.Y += 1;
    }

    if (pt_info->t_console_info.dwCursorPosition.Y > pt_info->t_console_info.dwSize.Y)
        pt_info->t_console_info.dwCursorPosition.Y = pt_info->t_console_info.dwSize.Y;

    return l_distance;
}

int32 ins_chars(T_CMD_LINE_INFO *pt_info, char c_ascii, int32 l_count){
    
    int32 l_max_ins = pt_info->l_inp_max_len - pt_info->l_end_pos - 1 ;
    int32 l_move_block_size = pt_info->l_end_pos - pt_info->l_cur_pos + 1 ;

    l_count = (l_count>l_max_ins)? l_max_ins : l_count;
    memmove(&pt_info->ca_line[pt_info->l_cur_pos+l_count],
            &pt_info->ca_line[pt_info->l_cur_pos],
            l_move_block_size);

    memset(&pt_info->ca_line[pt_info->l_cur_pos], c_ascii, l_count);

    pt_info->l_cur_pos += l_count;
    pt_info->l_end_pos += l_count;

    return l_count;   
}

int32 proceed_end_key(T_CMD_LINE_INFO *pt_info){

    int32   l_distance = pt_info->l_end_pos - pt_info->l_cur_pos;

    cursor_right(pt_info, l_distance);

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    pt_info->l_cur_pos = pt_info->l_end_pos;

    return 0;
}

int32 proceed_ctrl_right_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat){


    if (pt_info->l_cur_pos + l_repeat > pt_info->l_end_pos){
        l_repeat = pt_info->l_end_pos - pt_info->l_cur_pos;
    }

    l_repeat = cursor_right(pt_info, l_repeat);

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    pt_info->l_cur_pos += l_repeat;

    return 0;
}

int32 proceed_right_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat){


    if (pt_info->l_cur_pos + l_repeat > pt_info->l_end_pos){
        l_repeat = pt_info->l_end_pos - pt_info->l_cur_pos;
    }

    l_repeat = cursor_right(pt_info, l_repeat);

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    pt_info->l_cur_pos += l_repeat;

    return 0;
}

int32 proceed_home_key(T_CMD_LINE_INFO *pt_info){

    int32   l_distance;
    l_distance = cursor_left(pt_info, pt_info->l_cur_pos); 

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    pt_info->l_cur_pos -= l_distance;

    return 0;
}

int32 proceed_escape_key(T_CMD_LINE_INFO *pt_info){

    int32   l_distance, l_char_written;
    
    l_distance = cursor_left(pt_info, pt_info->l_cur_pos); 

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    pt_info->l_cur_pos -= l_distance;

    memset(&pt_info->ca_line[pt_info->l_cur_pos], 0, pt_info->l_end_pos-pt_info->l_cur_pos);

    WriteConsole(
        pt_info->h_stdout,
        pt_info->ca_line,
        pt_info->l_end_pos-pt_info->l_cur_pos,
        &l_char_written, 
        NULL
    );

    pt_info->l_end_pos = pt_info->l_cur_pos;

    // Restore cursor position
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    return 0;
}

int32 proceed_left_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat){

    int32   l_buff_pos;

    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    );
    
    l_repeat = (l_repeat > pt_info->l_cur_pos) ? pt_info->l_cur_pos : l_repeat;

    l_buff_pos = pt_info->t_console_info.dwCursorPosition.X  + 
                 pt_info->t_console_info.dwCursorPosition.Y * pt_info->t_console_info.dwSize.X;

    if (l_repeat > l_buff_pos) l_repeat = l_buff_pos;

    l_repeat = cursor_left(pt_info, l_repeat);

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    pt_info->l_cur_pos -= l_repeat;

    return l_repeat;
}

int32 proceed_ctrl_left_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat){

    int32   l_buff_pos;

    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    );
    
    l_repeat = (l_repeat > pt_info->l_cur_pos) ? pt_info->l_cur_pos : l_repeat;

    l_buff_pos = pt_info->t_console_info.dwCursorPosition.X  + 
                 pt_info->t_console_info.dwCursorPosition.Y * pt_info->t_console_info.dwSize.X;

    if (l_repeat > l_buff_pos) l_repeat = l_buff_pos;

    l_repeat = cursor_left(pt_info, l_repeat);

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    pt_info->l_cur_pos -= l_repeat;

    return l_repeat;
}

int32 proceed_back_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat){

    int32   l_i, l_char_written, l_buff_pos;

    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    );
    
    l_repeat = (l_repeat > pt_info->l_cur_pos) ? pt_info->l_cur_pos : l_repeat;

    l_buff_pos = pt_info->t_console_info.dwCursorPosition.X  + 
                 pt_info->t_console_info.dwCursorPosition.Y * pt_info->t_console_info.dwSize.X;

    if (l_repeat > l_buff_pos) l_repeat = l_buff_pos;


    memmove(&pt_info->ca_line[pt_info->l_cur_pos - l_repeat],
            &pt_info->ca_line[pt_info->l_cur_pos],
            pt_info->l_end_pos - pt_info->l_cur_pos + 1
    );

    memset(&pt_info->ca_line[pt_info->l_end_pos - l_repeat + 1], 0, l_repeat);

    pt_info->l_cur_pos -= l_repeat;
    pt_info->l_end_pos -= l_repeat;

    l_repeat = cursor_left(pt_info, l_repeat);

    // Move cursor
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    // Write characters
    WriteConsole(
        pt_info->h_stdout,
        &pt_info->ca_line[pt_info->l_cur_pos],
        pt_info->l_end_pos - pt_info->l_cur_pos + 1,
        &l_char_written, 
        NULL
    );

    // Remove trail
    for(l_i = 0; l_i < l_repeat; l_i++){
        WriteConsole(
            pt_info->h_stdout,
            &" ",
            1,
            &l_char_written, 
            NULL
        );
    }

    // Restore cursor position
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    return l_repeat;
}
int32 proceed_delete_key(T_CMD_LINE_INFO *pt_info, int32 l_repeat){

    int32   l_i, l_char_written, l_tail_len;

    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    );
    
    l_tail_len = pt_info->l_end_pos - pt_info->l_cur_pos;

    l_repeat = (l_repeat > l_tail_len) ? l_tail_len : l_repeat;

    memmove(&pt_info->ca_line[pt_info->l_cur_pos],
            &pt_info->ca_line[pt_info->l_cur_pos] + l_repeat,
            l_tail_len
    );

    memset(&pt_info->ca_line[pt_info->l_end_pos - l_repeat + 1], 0, l_repeat);

    pt_info->l_end_pos -= l_repeat;

    // Write characters
    WriteConsole(
        pt_info->h_stdout,
        &pt_info->ca_line[pt_info->l_cur_pos],
        pt_info->l_end_pos - pt_info->l_cur_pos + 1,
        &l_char_written, 
        NULL
    );

    // Remove trail
    for(l_i = 0; l_i < l_repeat; l_i++){
        WriteConsole(
            pt_info->h_stdout,
            &" ",
            1,
            &l_char_written, 
            NULL
        );
    }

    // Restore cursor position
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );


    return l_repeat;
}
proceed_printable_key(T_CMD_LINE_INFO *pt_info, char c_ascii, int32 l_repeat){

    int32 l_ins, l_char_written;

    // Check is command fit to screen buffer
    GetConsoleScreenBufferInfo(
        pt_info->h_stdout,
        &pt_info->t_console_info
    );

    l_ins = ins_chars(pt_info, c_ascii, l_repeat);

    // Write new characters
    WriteConsole(
        pt_info->h_stdout,
        &pt_info->ca_line[pt_info->l_cur_pos - l_ins],
        l_ins,
        &l_char_written, 
        NULL
    );

    // Write Old characters
    WriteConsole(
        pt_info->h_stdout,
        &pt_info->ca_line[pt_info->l_cur_pos],
        pt_info->l_end_pos - pt_info->l_cur_pos + 1,
        &l_char_written, 
        NULL
    );

    cursor_left(pt_info, pt_info->l_end_pos - pt_info->l_cur_pos + 1);

    // Restore cursor position
    SetConsoleCursorPosition(
        pt_info->h_stdout,
        pt_info->t_console_info.dwCursorPosition
    );

    
}

void set_cmd_line(T_CMD_LINE_INFO *pt_info, char *pc_new_line){

    int32 l_char_written;
    T_HISTORY_BUFF  *pt_hist = &pt_info->t_history;

    // Clear current line
    proceed_escape_key(pt_info);

    // copy new line -> curr_line
    strncpy(pt_info->ca_line, pc_new_line, pt_info->l_inp_max_len-1);
    pt_info->ca_line[pt_info->l_inp_max_len-1] = 0;
    
    pt_info->l_cur_pos = (int32)strlen(pt_info->ca_line);
    pt_info->l_end_pos = pt_info->l_cur_pos;

    WriteConsole(
        pt_info->h_stdout,
        pt_info->ca_line,
        (int32)strlen(pt_info->ca_line),
        &l_char_written, 
        NULL
    );

}

void add_to_history(T_CMD_LINE_INFO *pt_info){

    T_HISTORY_BUFF  *pt_hist = &pt_info->t_history;
    
    int32   l_pos, l_allow_add;

    l_allow_add = TRUE;
    // Add to history only if command line not equal to previous history line
    l_pos = pt_hist->l_wr_pos - 1;
    if (l_pos < 0) l_pos = HISTOR_DEPTH - 1;
    if (_stricmp(pt_hist->caa_lines[l_pos], pt_info->ca_line) == 0) l_allow_add = FALSE;

    // Do not add empty lines
    if (pt_info->ca_line[0] == '\0') l_allow_add = FALSE;

    if (l_allow_add){
        strcpy(pt_hist->caa_lines[pt_hist->l_wr_pos], pt_info->ca_line);
        pt_hist->l_wr_pos ++;
        if (pt_hist->l_wr_pos == HISTOR_DEPTH) pt_hist->l_wr_pos = 0;
        if (pt_hist->l_wr_pos  < 0) pt_hist->l_wr_pos = HISTOR_DEPTH - 1;
    }

    pt_hist->l_rd_pos = pt_hist->l_wr_pos;
}

int32 scroll_history(T_CMD_LINE_INFO *pt_info, int16 s_keycode){

    int32 l_rc, l_template_len, l_pos, l_found;
    T_HISTORY_BUFF  *pt_hist = &pt_info->t_history;

    l_template_len = (int32)strlen(pt_hist->ca_template);
    
    l_pos  = pt_hist->l_rd_pos;

    do{
        switch(s_keycode){
            case VK_UP :
                l_pos --;
                break;
            case VK_DOWN:
                l_pos ++;
                break;
        }
        if ( l_pos == HISTOR_DEPTH) l_pos = 0;
        if ( l_pos < 0) l_pos = HISTOR_DEPTH - 1;
        
        if (l_template_len == 0){
            l_found = (*pt_hist->caa_lines[l_pos] == 0);
        }else{
            l_found = (_strnicmp(pt_hist->caa_lines[l_pos], pt_hist->ca_template, l_template_len) != 0);
        }

    // Scroll whole loop until not zero 
    }while(l_found && l_pos != pt_hist->l_wr_pos);

    // Buffer roll over
    if (l_pos == pt_hist->l_wr_pos){
        if (l_template_len == 0){
            set_cmd_line(pt_info, "");
        }else{
            set_cmd_line(pt_info, pt_hist->ca_template);
        }
    }else{
        set_cmd_line(pt_info, pt_hist->caa_lines[l_pos]);
    }

    
    pt_hist->l_rd_pos = l_pos;

    l_rc = TRUE;
    return l_rc;
}

// ********************************************

int32 proceed_key_event(T_CMD_LINE_INFO *pt_info, KEY_EVENT_RECORD t_event){
    
    int32   l_rc;
    
    l_rc = TRUE;

    if (t_event.bKeyDown == TRUE){
        // If printable key pressed

        switch (t_event.wVirtualKeyCode){
            case VK_CANCEL  :
                l_rc = FALSE;
                break;
            case VK_TAB     :
                if (pt_info->s_prev_key != VK_TAB){
                    l_rc = GL_RC_TAB;
                }else{
                    l_rc = GL_RC_TAB_TAB;
                }
                break;
            case VK_UP      :
            case VK_DOWN    :
                if (pt_info->s_prev_key != VK_UP && pt_info->s_prev_key != VK_DOWN ){
                    strcpy(pt_info->t_history.ca_template, pt_info->ca_line);
                }

                scroll_history(pt_info, t_event.wVirtualKeyCode);
                break;

            case VK_RETURN  :
                add_to_history(pt_info);
                proceed_end_key(pt_info);
                l_rc =  GL_RC_RETURN;
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

        pt_info->s_prev_key = t_event.wVirtualKeyCode;

    } // Key Pressed

    return l_rc;
}

