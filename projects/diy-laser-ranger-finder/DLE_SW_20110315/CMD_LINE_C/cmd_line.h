typedef DWORD	uint32;
typedef	int 	int32;
typedef WORD	uint16;
typedef short	int16;
typedef char	char8;
typedef unsigned char	uint8;
typedef signed char	    int8;

#define GL_RC_RETURN    0x0D
#define GL_RC_TAB       0x09
#define GL_RC_TAB_TAB   (GL_RC_TAB + GL_RC_TAB)
#define GL_FLAG_CONTINUE 1
#define GL_FLAG_PRETYPE  2
#define CUR_POS_UNDEFINED -1

void  free_get_line(void *pv_info);
void* init_get_line(char *pc_prompt, void** ppv_history, uint32 *pul_hist_size);
int32 get_line(void *pv_info, char *pc_arg_cmd, uint32 ul_arg_cmd_len, uint32 ul_flag);
int32 get_cur_pos(void *pv_info);
void set_cur_pos(void *pv_info, int32 l_def_pos);
