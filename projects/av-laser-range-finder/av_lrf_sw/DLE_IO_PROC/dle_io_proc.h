
#define HANDLE_NOT_IDLE         0
#define HADLE_IO_CMD            1       // incoming command from IO pipe
#define HADLE_DEV_RESP          2       // incoming response from device
#define HANDLE_LAST_HANDLE      3

#define HANDLES_NUM HANDLE_LAST_HANDLE

extern HANDLE gha_events[HANDLES_NUM];
extern FT_HANDLE gh_dle_dev;

#define FL_CLR  0
#define FL_RISE 1
#define FL_FALL 2
#define FL_SET  3
#define FL_UNDEF  4

#define FL_REQ_NONE   0
#define FL_REQ_SHOULD 1
#define FL_REQ_MUST   2

#define RESP_STR_LEN 1024

typedef struct t_dle_io_flags_tag{
    DWORD   dev_conn       : 8;
    DWORD   dev_conn_req   : 8;
    DWORD   io_conn        : 8;
    DWORD   io_conn_req    : 8;
    DWORD   exit           : 8;
}T_DLE_IO_FLAGS;

extern T_DLE_IO_FLAGS gt_flags;

extern DWORD gdw_dev_bytes_rcv;
extern BYTE guca_dev_resp[1024];
extern int gn_dev_resp_timeout;

extern OVERLAPPED gt_rx_dev_overlapped;

extern WCHAR gca_io_cmd_resp[RESP_STR_LEN];

extern HANDLE gh_dump_file;

//extern T_FREQ_TABLE  *gpt_active_table;

void dle_io_close();
int  dle_io_command_processing();
void dle_io_disconnect();
int  dle_io_connection_check();
int  dle_io_pipe_tx();
int  dle_io_pipe_rx_init();

void dle_dev_close();
int  dle_dev_connect();
int  dle_dev_response_processing();
int  dle_dev_connection_check();
int  dle_dev_resp_rx_init(DWORD dw_dev_resp_req_len);
int  dle_dev_clear_fifos();



