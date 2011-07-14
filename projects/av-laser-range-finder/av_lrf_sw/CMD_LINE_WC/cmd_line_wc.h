#define GL_RC_RETURN    0x0D
#define GL_RC_TAB       0x09
#define GL_RC_TAB_TAB   (GL_RC_TAB + GL_RC_TAB)
#define GL_RC_ANY_KEY   0x55
#define GL_FLAG_CONTINUE 1
#define GL_FLAG_PRETYPE  2
#define GL_FLAG_RESET    4
#define GL_FLAG_NOPROMPT 8
#define CUR_POS_UNDEFINED -1

#define wcs_sizeof(str) (sizeof(str)/sizeof(WCHAR))

void  free_get_line(void *pv_info);
void* init_get_line(WCHAR *pc_prompt, void** ppv_history, int *pn_hist_size); 
void  get_line_prompt(void *pv_info, WCHAR *pc_arg_cmd, int n_arg_cmd_len, int n_flag);
int   get_line_input_events(void *pv_info, WCHAR *pc_arg_cmd, int n_arg_cmd_len);
int   get_line(void *pv_info, WCHAR *pc_arg_cmd, int n_arg_cmd_len, int n_flag); 
int   get_cur_pos(void *pv_info); 
void  set_cur_pos(void *pv_info, int n_def_pos); 

HANDLE get_stdin_handle(void *pv_info);
