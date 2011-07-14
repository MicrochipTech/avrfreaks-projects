/***C*********************************************************************************************
**
** SRC-FILE     :   dle_cmd_proc.cpp
**                                        
** PROJECT      :   DLE user interface
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   Command line tool for DLE IO commands with autocomplete & history support.
**                  DLE IO commands define in DLE_CMD_LIB project.
**                  
** COPYRIGHT    :   
**
****C*E******************************************************************************************/
#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include "cmd_line_wc.h"
#include "dle_cmd_lib.h"
#include "dle_cmd_proc.h"
#include "dle_cmd_proc_inp.h"

// --- Internal function ---------------------------------------------
int start_dle_io();
int send_single_command(int argc, WCHAR *argv[], int n_arg_num);
int proceed_cmd(WCHAR *pc_cmd);
int get_command_line_args(int argc, WCHAR *argv[], int n_arg_num);
int check_io_pipe_connection(int *pn_prompt_restore);
int proceed_rx_msg(int *pn_prompt_restore);
BOOL CtrlHandler( DWORD fdwCtrlType );
int proceed_rx_msg(int *pn_prompt_restore);

// --- Synchronization vars  -----------------------------------------
HANDLE gh_DLE_IO_PIPE;
HANDLE gha_events[HANDLES_NUM];
OVERLAPPED gt_dle_io_pipe_overlap;
OVERLAPPED gt_dle_io_rx_overlap;
OVERLAPPED gt_dle_io_tx_overlap;

// --- Other globals -------------------------------------------------
CMD_PROC_FLAGS gt_flags;
int gn_action;
WCHAR gca_cmd_io_resp[IO_RX_MSG_LEN];

/***C*F******************************************************************************************
**
** FUNCTION       : wmain
** DATE           : 01/20/2011
** AUTHOR         : AV
**
** DESCRIPTION    : 
**
** PARAMETERS     : 
**
** RETURN-VALUE   : 
**
** NOTES          : 
**
***C*F*E***************************************************************************************************/
void wmain(int argc, WCHAR *argv[]){

    int n_arg_num;

    WCHAR   *pc_cmd;
    int     n_rc, n_evt;
    int     n_prompt_restore;
    void    *pv_cmd_proc_info;

    // Default options
    gn_action = CMD_LINE_MODE;
    gt_flags.io_auto_start = FL_SET;    

    n_arg_num = get_command_line_args(argc, argv, 1);
 
    if (gn_action == SHOW_OPTIONS_HELP){
        show_options_help();
        return;
    }

    // Creat pipe IO connection event
    gha_events[HANDLE_IO_PIPE] = CreateEvent( 
         NULL,    // default security attribute 
         FALSE,   // manual-reset event 
         FALSE,   // initial state
         NULL);   // unnamed event object 

    // Creat command rRX event
    gha_events[HANDLE_RX_IO_MSG] = CreateEvent( 
         NULL,    // default security attribute 
         FALSE,   // manual-reset event 
         FALSE,   // initial state
         NULL);   // unnamed event object 

    // Creat command TX event
    gha_events[HANDLE_TX_IO_MSG] = CreateEvent( 
         NULL,    // default security attribute 
         FALSE,   // manual-reset event 
         FALSE,   // initial state
         NULL);   // unnamed event object 

    gha_events[HANDLE_KEYBOARD] = GetStdHandle(STD_INPUT_HANDLE);

    gt_dle_io_pipe_overlap.hEvent = gha_events[HANDLE_IO_PIPE];
    gt_dle_io_rx_overlap.hEvent = gha_events[HANDLE_RX_IO_MSG];
    gt_dle_io_tx_overlap.hEvent = gha_events[HANDLE_TX_IO_MSG];

    wprintf(L" --- DLE command procesing tool ---\n");
    wprintf(L" --- Copyright AV (c) 2010      ---\n");
    wprintf(L"                                   \n");

    // --------------------------
    // --- Start DLE IO module 
    // --------------------------
    if (!start_dle_io()) return;
    
    if (gn_action == SEND_COMMAND){
        send_single_command(argc, argv, n_arg_num);
        return;
    }

    // Catch console control events
    SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );

    // Init command input processor
    pv_cmd_proc_info = init_cmd_proc(L"cmd_hist.dat");
    if (pv_cmd_proc_info == NULL)
        return;

    // Show initial prompt
    n_prompt_restore = TRUE;

    // --------------------------
    // --- Main wait loop
    // --------------------------
    while(1){

        n_evt = WaitForMultipleObjects(
            HANDLES_NUM,                 // __in  DWORD nCount,
            gha_events,                  // __in  const HANDLE *lpHandles,
            FALSE,                       // __in  BOOL bWaitAll,
            100                          // __in  DWORD dwMilliseconds
        );

        if (n_evt == WAIT_TIMEOUT)
        {
            // Nothing to do
            continue;
        }
        else if (n_evt == WAIT_OBJECT_0 + HANDLE_IO_PIPE)
        {
            n_rc = check_io_pipe_connection(&n_prompt_restore);
            if (!n_rc) return;
        }
        else if (n_evt == WAIT_OBJECT_0 + HANDLE_RX_IO_MSG)
        {
            n_rc = proceed_rx_msg(&n_prompt_restore);
            if (!n_rc) return;
        }
        else if (n_evt == WAIT_OBJECT_0 + HANDLE_TX_IO_MSG)
        {
            // Check TX message complete
            // ...
        }
        else if (n_evt == WAIT_OBJECT_0 + HANDLE_KEYBOARD)
        {
            pc_cmd = cmd_keys_pressed(pv_cmd_proc_info, &n_prompt_restore);

            if (pc_cmd)
            {
                n_rc = proceed_cmd(pc_cmd);
                if (!n_rc) return;
            }
        }
        else
        {
            wprintf(L"?\n");
        }

        if (n_prompt_restore)
        {
            cmd_proc_prompt(pv_cmd_proc_info);
            n_prompt_restore = FALSE;
        }
    } // End of While forever

    close_cmd_proc(pv_cmd_proc_info);

    return;
}

/***C*F******************************************************************************************
**
** FUNCTION       : CtrlHandler
** DATE           : 01/20/2011
** AUTHOR         : AV
**
** DESCRIPTION    : Program termination hook
**
** PARAMETERS     : 
**
** RETURN-VALUE   : 
**
** NOTES          : 
**
***C*F*E***************************************************************************************************/
BOOL CtrlHandler( DWORD fdwCtrlType ){ 

	wprintf(L"\nUse 'quit' or just 'q' to save command history");
	wprintf(L"\nHave a nice day!\n");

    return FALSE;
} 

/***C*F******************************************************************************************
**
** FUNCTION       : get_command_line_args
** DATE           : 01/20/2011
** AUTHOR         : AV
**
** DESCRIPTION    : Parses command line argument
**
** PARAMETERS     : 
**
** RETURN-VALUE   : 
**
** NOTES          : Not tested
**
***C*F*E***************************************************************************************************/
int get_command_line_args(int argc, WCHAR *argv[], int n_arg_num){

	while(n_arg_num < argc){

        if ( wcscmp(argv[n_arg_num], L"-h") == 0 ||
			 wcscmp(argv[n_arg_num], L"--help") == 0 ){
             gn_action  = SHOW_OPTIONS_HELP;
             break;
		}


		if ( wcscmp(argv[n_arg_num], L"-v") == 0 ||
			 wcscmp(argv[n_arg_num], L"--verbose") == 0 ){
			n_arg_num++;
            gt_flags.verbose = FL_SET;
            continue;
		}

		if ( wcscmp(argv[n_arg_num], L"-c") == 0 ||
			 wcscmp(argv[n_arg_num], L"--command") == 0 ){
			n_arg_num++;
            gn_action = SEND_COMMAND;
            continue;
		}

		if ( wcscmp(argv[n_arg_num], L"-no_io_start") == 0){
			n_arg_num++;
            gt_flags.io_auto_start = FL_CLR;
            continue;
		}


        // If argument not regonized, then single command supposed
        break;
	}

    return n_arg_num;
}
/***C*F******************************************************************************************
**
** FUNCTION       : send_single_command
** DATE           : 01/20/2011
** AUTHOR         : AV
**
** DESCRIPTION    : Send single command from command line and exit
**
** PARAMETERS     : 
**
** RETURN-VALUE   : 
**
** NOTES          : Not tested. Some problems with IO pipe expected
**
***C*F*E*************************************************************************************************/
int send_single_command(int argc, WCHAR *argv[], int n_arg_num){

    WCHAR    ca_cmd_line[CMD_LINE_LENGTH];
    WCHAR    *pc_cmd;

    pc_cmd = ca_cmd_line;
    pc_cmd[0] = 0;

    // combine all remaining command line tokens to one string
    while(n_arg_num < argc ){
        wcsncat_s(pc_cmd, wcs_sizeof(ca_cmd_line), argv[n_arg_num], wcs_sizeof(ca_cmd_line));
        n_arg_num ++;    
    }

    return proceed_cmd(pc_cmd);
}
/***C*F******************************************************************************************
**
** FUNCTION       : init_rx_msg_read
** DATE           : 01/20/2011
** AUTHOR         : AV
**
** DESCRIPTION    : Initiate DLE IO asynchronous read
**
** PARAMETERS     : 
**
** RETURN-VALUE   : returns TRUE if OK, FALSE otherwise
**
** NOTES          : 
**
***C*F*E***************************************************************************************************/
int init_rx_msg_read(){

    int n_rc, n_gle;
    DWORD dw_n;

    n_rc = ReadFile(
        gh_DLE_IO_PIPE,             //__in         HANDLE hFile,
        gca_cmd_io_resp,            //__out        LPVOID lpBuffer,
        IO_RX_MSG_LEN,              //__in         DWORD nNumberOfBytesToRead,
        &dw_n,                      //__out_opt    LPDWORD lpNumberOfBytesRead,
        &gt_dle_io_rx_overlap       //__inout_opt  LPOVERLAPPED lpOverlapped
        );

    if(n_rc != 0)
    {
        wprintf(L"\nCan't read from IO pipe. Error @ %d", __LINE__);
        return FALSE;
    }
    n_gle = GetLastError();

    if (n_gle == ERROR_IO_PENDING)
    {
        gt_flags.io_connected = FL_SET;
    }
    else
    {
        gt_flags.io_connected = FL_FALL;
    }

    return TRUE;
}

/***C*F******************************************************************************************
**
** FUNCTION       : proceed_rx_msg
** DATE           : 01/20/2011
** AUTHOR         : AV
**
** DESCRIPTION    : Receives & printout DLE IO incoming messages
**
** PARAMETERS     : 
**
** RETURN-VALUE   : returns TRUE if OK, FALSE otherwise
**
** NOTES          : 
**
***C*F*E***************************************************************************************************/
int proceed_rx_msg(int *pn_prompt_restore){

    int n_rc, n_gle;
    DWORD   dw_bytes_received;

    n_rc = GetOverlappedResult(
        gh_DLE_IO_PIPE,           // __in  HANDLE hFile,
        &gt_dle_io_rx_overlap,    //__in   LPOVERLAPPED lpOverlapped,
        &dw_bytes_received,       //__out  LPDWORD lpNumberOfBytesTransferred,
        FALSE                     //__in   BOOL bWait
    );

    if (n_rc)
    {
        n_rc = init_rx_msg_read();
        if(!n_rc) return n_rc;
    }
    else
    {
        n_gle = GetLastError();
        if (n_gle != ERROR_IO_INCOMPLETE){
            gt_flags.io_connected = FL_CLR;
            SetEvent(gha_events[HANDLE_IO_PIPE]);
            wprintf(L"\nError reading IO pipe\n");
            *pn_prompt_restore = TRUE;
            return TRUE;
        }
    }
    wprintf(L"\n%s\n", gca_cmd_io_resp);
    *pn_prompt_restore = TRUE;

    return TRUE;
}

/***C*F******************************************************************************************
**
** FUNCTION       : check_io_pipe_connection
** DATE           : 01/20/2011
** AUTHOR         : AV
**
** DESCRIPTION    : DLE IO pipe state machine. Automatically connect/disconects from IO pipe
**
** PARAMETERS     : 
**
** RETURN-VALUE   : returns TRUE if OK, FALSE otherwise
**
** NOTES          : 
**
***C*F*E***************************************************************************************************/
int check_io_pipe_connection(int *pn_prompt_restore){

    int n_rc, n_gle;

    if (gt_flags.io_connected == FL_CLR)
    {
        // Wait until DLE_IO connects to pipe
        n_rc = ConnectNamedPipe(gh_DLE_IO_PIPE, &gt_dle_io_pipe_overlap);
        if (n_rc != 0)
        {
            wprintf(L"\nCan't connect to IO pipe. Error @ %d", __LINE__);
            return FALSE;
        }

        n_gle = GetLastError();
        if (n_gle == ERROR_PIPE_CONNECTED)
        {
            gt_flags.io_connected = FL_RISE;
            SetEvent(gha_events[HANDLE_IO_PIPE]);
            wprintf(L"\nIO pipe connected\n");
            *pn_prompt_restore = TRUE;
        }
        else if (n_gle == ERROR_IO_PENDING)
        {
            wprintf(L"\nConnecting to IO pipe\n");
            *pn_prompt_restore = TRUE;
        }
        else //(n_gle == ERROR_NO_DATA)
        {
            gt_flags.io_connected = FL_FALL;
            SetEvent(gha_events[HANDLE_IO_PIPE]);
        }

        return TRUE;
    } // End of Connection CLR

    if (gt_flags.io_connected == FL_SET)
    {
        return TRUE;
    } // End of Connection SET

    if (gt_flags.io_connected == FL_RISE)
    {
        n_rc = init_rx_msg_read();
        if (!n_rc) return n_rc;
        return TRUE;
    } // End of Connection RISE

    if (gt_flags.io_connected == FL_FALL)
    {
        wprintf(L"\nDisconnecting IO pipe\n");
        *pn_prompt_restore = TRUE;

        n_rc = DisconnectNamedPipe(gh_DLE_IO_PIPE);
        if(n_rc == 0)
        {
            wprintf(L"\nCan't disconnect IO pipe. Error @ %d", __LINE__);
            return FALSE;
        }

        gt_flags.io_connected = FL_CLR;
        SetEvent(gha_events[HANDLE_IO_PIPE]);

        return TRUE;
    } // End of Connection FALL

    // PC should never hits here
    return FALSE;
}

/***C*F******************************************************************************************
**
** FUNCTION       : proceed_cmd
** DATE           : 01/20/2011
** AUTHOR         : AV
**
** DESCRIPTION    : Check user input (parse command), write  to DLE IO pipe if command OK
**
** PARAMETERS     : 
**
** RETURN-VALUE   : returns TRUE if OK, FALSE otherwise
**
** NOTES          : 
**
***C*F*E***************************************************************************************************/
int proceed_cmd(WCHAR *pc_cmd_arg){

    int     n_rc, n_gle;
    T_DLE_CMD   *pt_curr_cmd;

    WCHAR   ca_cmd[CMD_LINE_LENGTH];
    WCHAR   *pc_cmd_token;

    // Make command local copy to use STRTOK
    wcscpy(ca_cmd, pc_cmd_arg);

    // Read command
    pc_cmd_token = wcstok(ca_cmd, L" ");
    if (!pc_cmd_token) return TRUE;

    pc_cmd_token = _wcsupr(pc_cmd_token);

    if (wcscmp(pc_cmd_token, L"QUIT") == 0 || 
        wcscmp(pc_cmd_token, L"EXIT") == 0 ||
        wcscmp(pc_cmd_token, L"Q")    == 0 ) return FALSE;

    if (wcscmp(pc_cmd_token, L"HELP") == 0) {
        show_cmd_help();
        return TRUE;
    }

    // DLE command processor starts here
    pt_curr_cmd = decomposite_dle_cmd(pc_cmd_arg, gta_cmd_lib, FALSE);
    
    if (pt_curr_cmd == NULL)
    { // Command not found or error in parameters
        return TRUE;
    }
    
    { // Send command to IO processor
        DWORD dw_n;
        // Send command 
        n_rc = WriteFile(
            gh_DLE_IO_PIPE,             //__in         HANDLE hFile,
            pc_cmd_arg,                 //__in         LPCVOID lpBuffer,
            (int)wcslen(pc_cmd_arg)<<1, //__in         DWORD nNumberOfBytesToWrite,
            &dw_n,                      //__out_opt    LPDWORD lpNumberOfBytesWritten,
            &gt_dle_io_tx_overlap       //__inout_opt  LPOVERLAPPED lpOverlapped
        );

        if(!n_rc){
            n_gle = GetLastError();
            if (n_gle != ERROR_IO_PENDING)
            {
                wprintf(L"Something wrong GLE=%d @ %d", n_rc, __LINE__);
                return FALSE;
            }
        }
    } // End of TX MSG block

    return TRUE;
}
/***C*F******************************************************************************************
**
** FUNCTION       : start_dle_io
** DATE           : 01/20/2011
** AUTHOR         : AV
**
** DESCRIPTION    : Create DLE IO socket and start up DLE IO process
**
** PARAMETERS     : 
**
** RETURN-VALUE   : returns TRUE if OK, FALSE otherwise
**
** NOTES          : Pipe connction performed in pipe's FSM. see check_io_pipe_connection()
**
***C*F*E***************************************************************************************************/
int start_dle_io(){

    WCHAR ca_pipe_name[] = L"\\\\.\\pipe\\dle_io";

    gh_DLE_IO_PIPE = CreateNamedPipe(
        ca_pipe_name,           //__in      LPCTSTR lpName,
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,     //__in      DWORD dwOpenMode, ??? FILE_FLAG_OVERLAPPED ???
        PIPE_TYPE_MESSAGE,      //__in      DWORD dwPipeMode, 
        2,                      //__in      DWORD nMaxInstances, ???1???
        8*IO_RX_MSG_LEN,      //__in      DWORD nOutBufferSize,
        8*IO_RX_MSG_LEN,      //__in      DWORD nInBufferSize,
        0,                      //__in      DWORD nDefaultTimeOut,
        NULL                    //__in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes
        );

    if (gh_DLE_IO_PIPE == INVALID_HANDLE_VALUE){
        wprintf(L"Can't create IO pipe. Error @ %d", __LINE__);
        return FALSE;
    }

    gt_flags.io_connected = FL_CLR;
    SetEvent(gha_events[HANDLE_IO_PIPE]);

    
    if (gt_flags.io_auto_start == FL_SET)
    { // Start DLE_IO process and pass outgoing pipe as STDIN
        WCHAR ca_cmd_line[1024];
        PROCESS_INFORMATION piProcInfo; 
        STARTUPINFO siStartInfo;
        BOOL bSuccess = FALSE; 
 
        // Set up members of the PROCESS_INFORMATION structure. 
        ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
        // Set up members of the STARTUPINFO structure. 
        // This structure specifies the STDIN and STDOUT handles for redirection.
 
        ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
        siStartInfo.cb = sizeof(STARTUPINFO); 

        wsprintf(ca_cmd_line, L"dle_io_proc.exe %s", ca_pipe_name);
        // Create the child process. 
        bSuccess = CreateProcess(
            NULL, 
            ca_cmd_line,            // command line 
            NULL,                   // process security attributes 
            NULL,                   // primary thread security attributes 
            FALSE,                  // handles are inherited 
            CREATE_NEW_CONSOLE,     // creation flags 
            NULL,                   // use parent's environment 
            NULL,                   // use parent's current directory 
            &siStartInfo,           // STARTUPINFO pointer 
            &piProcInfo);           // receives PROCESS_INFORMATION 
   
        // If an error occurs, exit the application. 
        if ( !bSuccess ){
            wprintf(L"Can't create IO process. Error @ %d", __LINE__);
            return FALSE;
        }
    }


    return TRUE;
}
