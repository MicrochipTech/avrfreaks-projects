/***C*********************************************************************************************
**
** SRC-FILE     :   dle_io_proc.cpp
**                                        
** PROJECT      :   DLE
**                                                                
** SRC-VERSION  :   
**              
** DATE         :   22/01/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   DLE IO processor 
**                  Provides control logic between IO pipe (commands from user) and DLE device.
**                  Main function are pass data from UI to DLE device and back.
**                  Also, keep device and IO pipe connections alive.
**                  
** COPYRIGHT    :   (c) 2011 Andrejs Vasiljevs. All rights reserved.
**
****C*E******************************************************************************************/
#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <math.h>
#include "FTD2XX.H"
#include "cmd_line_wc.h"
#include "dle_cmd_lib.h"
#include "dle_io_devcmd.h"
#include "dle_io_devresp.h"
#include "dle_io_measure.h"
#include "dle_io_proc.h"

T_DLE_IO_FLAGS gt_flags;

HANDLE gha_events[HANDLES_NUM];
HANDLE gh_DLE_IO_rd;

FT_HANDLE gh_dle_dev;

OVERLAPPED gt_dle_io_rx_overlap;
OVERLAPPED gt_dle_io_tx_overlap;

#define DLE_CMD_BUFF_LEN 1024
WCHAR gca_dle_cmd_buff[DLE_CMD_BUFF_LEN];

WCHAR gca_io_cmd_resp[RESP_STR_LEN];

#define IO_PIPENAME_LEN 64
WCHAR gca_pipe_name[IO_PIPENAME_LEN];

OVERLAPPED gt_rx_dev_overlapped = { 0 };
DWORD gdw_dev_bytes_rcv;
BYTE guca_dev_resp[1024];

int gn_dev_index = 0;
int gn_dev_resp_timeout;

HANDLE gh_dump_file;
HANDLE gh_meas_log_file;

int dle_io_init(WCHAR *pc_pipe_name);
int main_loop_wait();

int open_device(int n_dev_indx, FT_HANDLE *ph_device);

//T_FREQ_TABLE  *gpt_active_table;

// dbg temp
void proceed_scan_resp(int n_calibration);
void proceed_scan_meas_resp();
unsigned short poly_atan2(short s_sampl_cos, short s_sampl_sin);
int scan_measure(double *pdb_distance);

int wmain(int argc, WCHAR *argv[]){

    unsigned short gusa_trace[16];

// ------------------
    /* connection with xFloorer using windows messaging
    #define WM_DISTANCE (WM_USER + 0)
    HWND hWnd;
    while(!gn_dev_index)
    {
        hWnd  = FindWindow( NULL, L"AV xFloorer");
        PostMessage (hWnd, WM_DISTANCE, (WORD)200, (WORD)0xDEADBEEF);
        Sleep(20);
    }
    */
// ------------------

    double db_tmp;

// 1358.8 =>  146.3, 129.7, 95.9, 83.4, 238.6 =>  26633, 23611, 17458, 15183, 43436
//  515.1 => 11.7f;   9.0f; 359.7f; 354.6f;  311.0f;
// 3100.1 =>  11.9, 355.4, 258.5, 206.6, 139.2

    //gta_meas_info[0].db_phase =  11.9f;
    //gta_meas_info[1].db_phase = 355.4f;
    //gta_meas_info[2].db_phase = 258.5f;
    //gta_meas_info[3].db_phase = 206.6f;
    //gta_meas_info[4].db_phase = 139.2f;

    //gta_meas_info[0].db_phase =  13.7f;
    //gta_meas_info[1].db_phase = 323.7f;
    //gta_meas_info[2].db_phase = 175.1f;
    //gta_meas_info[3].db_phase = 108.9f;
    //gta_meas_info[4].db_phase = 299.6f;

    gta_meas_info[0].db_phase =  30.4f;
    gta_meas_info[1].db_phase =  29.7f;
    gta_meas_info[2].db_phase = 344.1f;
    gta_meas_info[3].db_phase = 318.1f;
    gta_meas_info[4].db_phase =  83.7f;

    gta_meas_info[0].n_valid = 1;
    gta_meas_info[1].n_valid = 1;
    gta_meas_info[2].n_valid = 1;
    gta_meas_info[3].n_valid = 1;
    gta_meas_info[4].n_valid = 1;

    //scan_measure(&db_tmp);

    if (!dle_io_init(argv[1])) 
        return FALSE;

    gha_events[HADLE_DEV_RESP] = CreateEvent(
        NULL,  // __in_opt  LPSECURITY_ATTRIBUTES lpEventAttributes,
        FALSE, // __in      BOOL bManualReset,
        FALSE, // __in      BOOL bInitialState,
        NULL   // __in_opt  LPCTSTR lpName
    );
    gt_rx_dev_overlapped.hEvent = gha_events[HADLE_DEV_RESP];

    gha_events[HANDLE_NOT_IDLE] = CreateEvent(
        NULL,  // __in_opt  LPSECURITY_ATTRIBUTES lpEventAttributes,
        FALSE, // __in      BOOL bManualReset,
        TRUE,  // __in      BOOL bInitialState,
        NULL   // __in_opt  LPCTSTR lpName
    );


    gh_dump_file = CreateFile( L"dump.bin",    // File name
                          GENERIC_READ | GENERIC_WRITE,	    // Access mode
                          FILE_SHARE_READ | FILE_SHARE_WRITE,  // Share Mode
                          NULL,				// Security Attributes
                          CREATE_ALWAYS,	// Creation Disposition
                          0,				// File Flags&Attr
                          NULL);			// hTemplate

    gh_meas_log_file = CreateFile( L"meas.log",    // File name
                          GENERIC_READ | GENERIC_WRITE,	    // Access mode
                          FILE_SHARE_READ | FILE_SHARE_WRITE,  // Share Mode
                          NULL,				// Security Attributes
                          CREATE_ALWAYS,	// Creation Disposition
                          0,				// File Flags&Attr
                          NULL);			// hTemplate

    // TODO: extract device index from command line arguments
    gn_dev_index = 0;
    gn_dev_resp_timeout = 0;

    gt_flags.dev_conn = FL_UNDEF;   // in order to report very first connection FAIL
    gt_flags.dev_conn_req = FL_REQ_SHOULD;
    
    gt_flags.io_conn = FL_UNDEF;
    gt_flags.io_conn_req = FL_REQ_SHOULD;

    gt_rx_dev_overlapped.hEvent = gha_events[HADLE_DEV_RESP];

    wprintf(L" ---      DLE IO proccesor      ---\n");
    wprintf(L" ---    Copyright AV (c) 2010   ---\n");
    wprintf(L"                                   \n");
    while(gt_flags.exit != FL_SET){

        // ---------------------------------------------------
        // --- Wait until something happens
        // ---------------------------------------------------
        if (!main_loop_wait())
            continue;

        // ---------------------------------------------------
        // --- Check device connection
        // ---------------------------------------------------
        if (!dle_dev_connection_check()) 
            continue;

        // ---------------------------------------------------
        // --- Check IO Pipe connection
        // ---------------------------------------------------
        if (!dle_io_connection_check()) 
            continue;

    }// End of while

    dle_dev_close();
    dle_io_close();

    wprintf(L"---------------- End of Session ----------------");

    if ( gha_events[HANDLE_NOT_IDLE] != INVALID_HANDLE_VALUE){
	    CloseHandle(gha_events[HANDLE_NOT_IDLE]);
    }

    return TRUE;

}
/***C*F******************************************************************************************
**
** FUNCTION       : Main's loop waiting.
** DATE           : 
** AUTHOR         : AV
**
** DESCRIPTION    : Function wait until something happens in main loop. 
**                  Following events are supported:
**                      DLE command from user
**                      DLE response from device
**                      Timeout occured
**                      Not idle event set by internal routines
**
** PARAMETERS     : 
**
** RETURN-VALUE   : Returns TRUE if everything is OK, return FALSE if further main loop processing
**                  not reasonanble. Upon FASLE return, Main loop should proceed with "continue".
**
** NOTES          : 
**
***C*F*E***************************************************************************************************/
int main_loop_wait(){

    int n_rc;

    n_rc = WaitForMultipleObjects(
        HANDLES_NUM,                 // __in  DWORD nCount,
        gha_events,                  // __in  const HANDLE *lpHandles,
        FALSE,                       // __in  BOOL bWaitAll,
        100                          // __in  DWORD dwMilliseconds
    );


    if (n_rc == WAIT_TIMEOUT)
    {
        //// Check DEV Response TIME OUT
        //// Reinit IO_RX if timer expired
        //if (gn_dev_resp_timeout > 0)
        //{
        //    gn_dev_resp_timeout-- ;
        //    if (gn_dev_resp_timeout == 0)
        //    {
        //        dle_io_pipe_rx_init();  
        //    }
        //}

        // Nothing to do
        return TRUE;

    }
    else if (n_rc == WAIT_OBJECT_0 + HADLE_IO_CMD)    
    { // Incoming command from IO pipe
        // dle_dev_clear_fifos();  // TODO: execute only if command cause dev transaction(s)
        
        dle_io_command_processing();

        //if (dw_dev_resp_req_len) 
        //{ // Initiate device response read if requested by command processing routine
        //    dle_dev_resp_rx_init(dw_dev_resp_req_len);
        //}

        //// Initiate another IO pipe rx if device response timeout is not set
        //// Otherwise another IO pipe RX will be initiated upon dev response or timeout expired
        //if (gn_dev_resp_timeout == 0)
        //{
        //    dle_io_pipe_rx_init();
        //}
        dle_io_pipe_rx_init();

        return n_rc;
    }
    else if (n_rc == WAIT_OBJECT_0 + HADLE_DEV_RESP)
    {
        n_rc = FT_W32_GetOverlappedResult(
            gh_dle_dev,         
            &gt_rx_dev_overlapped,
            &gdw_dev_bytes_rcv,
            TRUE);
            
        if (n_rc)
        {
            n_rc = dle_dev_response_processing();
        }
        else
        {
            gt_flags.dev_conn = FL_FALL;
            SetEvent(gha_events[HANDLE_NOT_IDLE]);
        }

        dle_dev_resp_rx_init(2);
        return TRUE;
    }
    else if (n_rc == WAIT_OBJECT_0 + HANDLE_NOT_IDLE)
    {
        return TRUE;
    }

    return TRUE;

}

int dle_io_init(WCHAR *pc_pipe_name){

    wcscpy_s(gca_pipe_name, IO_PIPENAME_LEN, pc_pipe_name);

    // Init input buffer
    wmemset(gca_dle_cmd_buff, 0, DLE_CMD_BUFF_LEN);

    // Creat command received event
    gha_events[HADLE_IO_CMD] = CreateEvent( 
         NULL,    // default security attribute 
         FALSE,   // manual-reset event 
         FALSE,   // initial state
         NULL);   // unnamed event object 
    
    gt_dle_io_rx_overlap.hEvent = gha_events[HADLE_IO_CMD];

    return TRUE;
}

void dle_dev_close(){

    if ( gh_dle_dev != INVALID_HANDLE_VALUE)
        FT_W32_CloseHandle(gh_dle_dev);

    if (gha_events[HADLE_DEV_RESP] != INVALID_HANDLE_VALUE) 
        CloseHandle(gha_events[HADLE_DEV_RESP]);

    return;
}
void dle_io_close(){

    // Close IO pipe
    if (gh_DLE_IO_rd != INVALID_HANDLE_VALUE) 
        CloseHandle(gh_DLE_IO_rd);

    if (gha_events[HADLE_IO_CMD] != INVALID_HANDLE_VALUE) 
        CloseHandle(gha_events[HADLE_IO_CMD]);

    return;
}


int dle_io_command_processing(){

    T_DLE_CMD   *pt_dle_cmd;

    int n_rc;
    DWORD dw_bytes_transf;

    // Get Overlapped result
    n_rc = GetOverlappedResult(
        gh_DLE_IO_rd,       //  __in   HANDLE hFile,
        &gt_dle_io_rx_overlap, //  __in   LPOVERLAPPED lpOverlapped,
        &dw_bytes_transf,   //  __out  LPDWORD lpNumberOfBytesTransferred,
        FALSE               //  __in   BOOL bWait
        );

    if (!n_rc){
        wprintf(L"Can't read from IO pipe\n");
        gt_flags.io_conn = FL_FALL;
        return FALSE;
    }

    // ----------------------------------
    // --- Process command just read
    // ----------------------------------
    // Add command null termination
    gca_dle_cmd_buff[dw_bytes_transf>>1] = '\0';

    // Lookup command within library & decomposite it's values
    pt_dle_cmd = decomposite_dle_cmd(gca_dle_cmd_buff, gta_cmd_lib, TRUE);

    // If command not found check for exit or unknow command
    if (!pt_dle_cmd || !pt_dle_cmd->pt_fields){
        wprintf(L"Unrecognized command %s. Try again\n", gca_dle_cmd_buff);
    }else{
        wprintf(L"cmd --> io : %s\n", gca_dle_cmd_buff);
               
        // Lookup & execute command functor
        // TODO: replace for registered functions lookup
        if (_wcsicmp(pt_dle_cmd->pc_name, L"DEV_SIGN") == 0){
            dle_cmd_io_dev_sign();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"INIT") == 0){
            wcscpy_s(gt_cmd_spi_wr.data.pc_str, gt_cmd_spi_wr.data.n_len, L"0x02  0xC4 0x93  0x04  0xF0 0x78 0x0F 0x0F  0x00 0x10 0x00 0xF6  0x80 0x3C");
            dle_cmd_io_spi_wr();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"SPI_WR") == 0){
            dle_cmd_io_spi_wr();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"SPI_RD") == 0){
            dle_cmd_io_spi_rd();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"APD_ON") == 0){
            dle_cmd_io_apd_on();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"APD_OFF") == 0){
            dle_cmd_io_apd_off();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"aim") == 0){
            dle_cmd_io_aim();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"CURTAIN_UP") == 0){
            dle_cmd_io_curtain(0);
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"CURTAIN_DOWN") == 0){
            dle_cmd_io_curtain(1);
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"CURTAIN_DIS") == 0){
            dle_cmd_io_curtain(2);
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"MEASURE") == 0){
//            gpt_active_table = gta_meas_table;
//            dle_cmd_io_measure();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"CALIB") == 0){
//            gpt_active_table = gta_calib_table;
//            dle_cmd_io_measure();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"LCD") == 0){
            dle_cmd_io_lcd();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"SCAN_START") == 0){
            measure_init_scan((WORD)gt_cmd_scan_start.meas_apd.dw_val);
            dle_cmd_io_scan_start();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"SCAN_STOP") == 0){
            dle_cmd_io_scan_stop(); 
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"SCAN_APD") == 0){
            dle_cmd_io_scan_apd();
        }else if (_wcsicmp(pt_dle_cmd->pc_name, L"SCAN_CALIB") == 0){
            dle_cmd_io_scan_calib();
        }else{
            wprintf(L"Command not registered %s\n", pt_dle_cmd->pc_name);
        }
    }

    // check command send succesfully, 
    // proceed returned error code, initiate another command read otherwise in case of error
    // ...

    return TRUE;
}



int dle_dev_connect(){
    
    int n_rc;

    // Try to locate and open device on USB bus
    n_rc = open_device(gn_dev_index, &gh_dle_dev);

    if (!n_rc) {
        goto dle_dev_connect_cleanup;
    }

    // Read device signature
    // ...

    // Blink with led
    // ...


dle_dev_connect_cleanup:

    // Log activity only if device connection status changed
    if (n_rc){
        if (gt_flags.dev_conn != FL_SET){
            gt_flags.dev_conn = FL_RISE;
            SetEvent(gha_events[HANDLE_NOT_IDLE]);
        }
    }else{
        if (gt_flags.dev_conn != FL_CLR){
            gt_flags.dev_conn = FL_FALL;
            SetEvent(gha_events[HANDLE_NOT_IDLE]);
        }
    }
    
    return n_rc;

}


/***C*F********************************************************************************************
**
** FUNCTION       : open_device
** DATE           : 
** AUTHOR         : AV
**
** DESCRIPTION    : Open programmer (device) and write device handle to ph_device.
**                  If more than one device found, particular device may be explicitly specified
**                  by index from command line argument, or if not specified user will be asked for
**
** PARAMETERS     : 
**
** RETURN-VALUE   : Return TRUE if everythink is OK, FALSE otherwise
**
** NOTES          : 
**
***C*F*E*******************************************************************************************/
int open_device(int n_dev_indx, FT_HANDLE *ph_device){

	FT_STATUS ft_status;
    DWORD   dw_num_devs;
	LONG	devLocation;
	char	devDescriptor[64]; 
	char	devSerial[64]; 


	ft_status = FT_ListDevices(&dw_num_devs, NULL, FT_LIST_NUMBER_ONLY);
	if (ft_status != FT_OK) return FALSE;
        
    if (dw_num_devs == 0){
        // No devices were found
        return FALSE;
    }

	ft_status  = FT_ListDevices((void*)gn_dev_index, &devLocation,   FT_LIST_BY_INDEX | FT_OPEN_BY_LOCATION);
    if (ft_status != FT_OK) {
		return FALSE;
	}

    ft_status |= FT_ListDevices((void*)gn_dev_index, &devDescriptor, FT_LIST_BY_INDEX | FT_OPEN_BY_DESCRIPTION);
	ft_status |= FT_ListDevices((void*)gn_dev_index, &devSerial,     FT_LIST_BY_INDEX | FT_OPEN_BY_SERIAL_NUMBER);

	if (ft_status != FT_OK){
		return FALSE;
	}

#if FT_Classic
    ft_status |= FT_OpenEx((void*)devLocation, FT_OPEN_BY_LOCATION, ph_device);

    ft_status |= FT_SetTimeouts(*ph_device, 500, 500);
    ft_status |= FT_SetLatencyTimer(*ph_device, 2);

    // Divisor selection
    // BAUD = 3000000 / Divisor
    // Divisor = (N + 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875)
    // Divisor = 24 ==> Baud 125000 
    ft_status |= FT_SetDivisor(*ph_device, 3000000/125000);

    // Set UART format 8N1
    ft_status |= FT_SetDataCharacteristics(*ph_device, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);

	if (ft_status != FT_OK){
		return FALSE;
	}


    // Just in case
	FT_Purge(*ph_device, FT_PURGE_TX | FT_PURGE_RX);
#else


    // Open a device for overlapped I/O using its serial number
    *ph_device = FT_W32_CreateFile(
        (char*)devLocation,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FT_OPEN_BY_LOCATION,
        0);

    if (*ph_device == INVALID_HANDLE_VALUE)
    {
        // FT_W32_CreateDevice failed
        return FALSE;
    }

    // ----------------------------------------
    // --- Set comm parameters
    // ----------------------------------------

    FTDCB ftDCB;     
    FTTIMEOUTS	ftTimeouts;
    FTCOMSTAT	ftPortStatus;
    DWORD   dw_port_error;

    if (!FT_W32_GetCommState(*ph_device, &ftDCB))
    {
        return FALSE;
    }

    // Divisor selection
    // BAUD = 3000000 / Divisor
    // Divisor = (N + 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875)
    // Divisor = 24 ==> Baud 125000 

    ftDCB.BaudRate = 76800;
    ftDCB.fBinary = TRUE;                       /* Binary Mode (skip EOF check)    */
    ftDCB.fParity = FALSE;                      /* Enable parity checking          */
    ftDCB.fOutxCtsFlow = FALSE;                 /* CTS handshaking on output       */
    ftDCB.fOutxDsrFlow = FALSE;                 /* DSR handshaking on output       */
    ftDCB.fDtrControl = DTR_CONTROL_DISABLE;    /* DTR Flow control                */
    ftDCB.fTXContinueOnXoff = FALSE;

    ftDCB.fErrorChar = FALSE;			// enable error replacement 
	ftDCB.fNull = FALSE;				// enable null stripping 
	ftDCB.fRtsControl = RTS_CONTROL_DISABLE;       // RTS flow control 
	ftDCB.fAbortOnError = TRUE;			// abort reads/writes on error 

    ftDCB.fOutX = FALSE;                        /* Enable output X-ON/X-OFF        */
    ftDCB.fInX = FALSE;                         /* Enable input X-ON/X-OFF         */
    ftDCB.fNull = FALSE;                        /* Enable Null stripping           */
    ftDCB.fRtsControl = RTS_CONTROL_DISABLE;    /* Rts Flow control                */
    ftDCB.fAbortOnError = TRUE;                 /* Abort all reads and writes on Error */

    // 8N1
    ftDCB.ByteSize = 8;                 /* Number of bits/byte, 4-8        */
    ftDCB.Parity = NOPARITY;            /* 0-4=None,Odd,Even,Mark,Space    */
    ftDCB.StopBits = ONESTOPBIT;        /* 0,1,2 = 1, 1.5, 2               */

    if (!FT_W32_SetCommState(*ph_device, &ftDCB))
    {
        return FALSE;
    }

    FT_W32_GetCommState(*ph_device, &ftDCB);

	// Set serial port Timeout values
	FT_W32_GetCommTimeouts(*ph_device, &ftTimeouts);

	ftTimeouts.ReadIntervalTimeout = 0;
	ftTimeouts.ReadTotalTimeoutMultiplier = 0;
	ftTimeouts.ReadTotalTimeoutConstant = 200;
	ftTimeouts.WriteTotalTimeoutConstant = 0;
	ftTimeouts.WriteTotalTimeoutMultiplier = 0;

	FT_W32_SetCommTimeouts(*ph_device, &ftTimeouts);

	FT_W32_ClearCommError(*ph_device, &dw_port_error, &ftPortStatus);
	FT_W32_PurgeComm(*ph_device, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

#endif  // End of W32 device init

    return TRUE;
}


/***C*F******************************************************************************************
**
** FUNCTION       : Device connection main's loop subroutine
** DATE           : 
** AUTHOR         : AV
**
** DESCRIPTION    : 
**
** PARAMETERS     : 
**
** RETURN-VALUE   : Returns TRUE if everything is OK, return FALSE if further main loop processing
**                  not reasonanble. Upon FASLE return Main loop should proceed with "continue".
**
** NOTES          : 
**
***C*F*E***************************************************************************************************/
int dle_dev_connection_check(){

    // Connect to device, Check board presence, ping UI
    if ( (gt_flags.dev_conn == FL_CLR || gt_flags.dev_conn == FL_UNDEF) && gt_flags.dev_conn_req & FL_REQ_SHOULD){
        dle_dev_connect();
    }

    //// Try to connect if requested
    //if ( (gt_flags.dev_conn == FL_CLR || gt_flags.dev_conn == FL_UNDEF) && gt_flags.dev_conn_req & FL_REQ_MUST){
    //    // Show operation window
    //    wprintf(L"Connecting to device ...\n");
    //    dle_dev_connect();
    //} // End of Not Connected but required

    // Check device status transition
    if (gt_flags.dev_conn == FL_RISE){
        // DEV CONNECT 0->1
        wprintf(L"Device connected\n");
        gt_flags.dev_conn = FL_SET;

        dle_dev_resp_rx_init(2);

    }else if (gt_flags.dev_conn == FL_FALL){
        // DEV CONNECT 1->0

        // Clear connection flag
        gt_flags.dev_conn = FL_CLR;

        wprintf(L"Device disconnected\n");

        // End of Device connection FALL

    } else if (gt_flags.dev_conn == FL_SET){
        // check device connection status
        int n_rc;
        DWORD   dw_port_err; 
        FTCOMSTAT ftPortStatus;

        n_rc = FT_W32_ClearCommError(gh_dle_dev, &dw_port_err, &ftPortStatus);
        if (!n_rc)
        {
            gt_flags.dev_conn = FL_FALL;
            SetEvent(gha_events[HANDLE_NOT_IDLE]);
        }

        // analyze dw_port_err & ftPortStatus here
        // clear device connection flag in case of error detected

        // ???

    }

    return TRUE;
}


int dle_io_pipe_rx_init(){

    int n_rc, n_gle;

    n_rc = ReadFile(
        gh_DLE_IO_rd,                               //__in         HANDLE hFile,
        gca_dle_cmd_buff,                           //__out        LPVOID lpBuffer,
        DLE_CMD_BUFF_LEN,                           //__in         DWORD nNumberOfBytesToRead,
        NULL,                                       //__out_opt    LPDWORD lpNumberOfBytesRead,
        &gt_dle_io_rx_overlap);                        //__inout_opt  LPOVERLAPPED lpOverlapped

    if (!n_rc)
    {
        n_gle = GetLastError();
        if ( n_gle != ERROR_IO_PENDING && n_gle != ERROR_IO_INCOMPLETE)
        {
            //wprintf(L"something wrong @ %d. GLE=%d", __LINE__, n_gle);
            gt_flags.io_conn = FL_FALL;
            return FALSE;
        }
    }

    return TRUE;

}

int dle_io_connect(){
    
    int n_rc;
    DWORD   dwMode;

    // Try to connect to DLE IO pipe
    gh_DLE_IO_rd = CreateFile( 
        gca_pipe_name,                                          // pipe name 
        GENERIC_READ | GENERIC_WRITE | FILE_WRITE_ATTRIBUTES,   // read and write access 
        0,                                                      // no sharing 
        NULL,                                                   // default security attributes
        OPEN_EXISTING,                                          // opens existing pipe 
        FILE_FLAG_OVERLAPPED,                                   //attributes 
        NULL);                                                  // no template file 
 
    if (gh_DLE_IO_rd == INVALID_HANDLE_VALUE) 
    {
        n_rc = 0;
        goto dle_io_connect_cleanup;
    }

    // The pipe connected; change to message-read mode. 
    dwMode = PIPE_READMODE_MESSAGE; 
    n_rc = SetNamedPipeHandleState( 
        gh_DLE_IO_rd,     // pipe handle 
        &dwMode,          // new pipe mode 
        NULL,             // don't set maximum bytes 
        NULL);            // don't set maximum time 

    if (!n_rc){
        // Critical error! terminate application
        wprintf( L"SetNamedPipeHandleState failed. GLE=%d\n", GetLastError() ); 
        gt_flags.exit = FL_SET; // Critical error. Terminate application
        return FALSE;
    }

    // Initiate very first ReadEvent

dle_io_connect_cleanup:

    // Log activity only if IO pipe connection status changed
    if (n_rc){
        if (gt_flags.io_conn != FL_SET){
            gt_flags.io_conn = FL_RISE;
            SetEvent(gha_events[HANDLE_NOT_IDLE]);
        }
    }else{
        if (gt_flags.io_conn != FL_CLR){
            gt_flags.io_conn = FL_FALL;
            SetEvent(gha_events[HANDLE_NOT_IDLE]);
        }
    }
    
    return TRUE;

}

int dle_io_connection_check(){

    int n_rc, n_gle;

    // Try to connect to IO pipe if not connected and connection is required
    if ( (gt_flags.io_conn == FL_CLR || gt_flags.io_conn == FL_UNDEF) && gt_flags.io_conn_req & FL_REQ_SHOULD)
    {
        n_rc = dle_io_connect();            
        if (!n_rc)
        {
            return FALSE;   // Critical error
        }

    }

    // Check device status transition
    if (gt_flags.io_conn == FL_RISE){
        // IO pipe connected. Try to initiate very first rad transaction
        n_rc = dle_io_pipe_rx_init();
        if (n_rc)
        {
            wprintf(L"IO pipe connected\n");
            gt_flags.io_conn = FL_SET;
            return TRUE;
        }
        else
        {
            gt_flags.io_conn = FL_CLR;
        }
    }

    if (gt_flags.io_conn == FL_FALL)
    {
        wprintf(L"IO pipe disconnected\n");
        gt_flags.io_conn = FL_CLR;
        CloseHandle(gh_DLE_IO_rd);
    }

    if (gt_flags.io_conn == FL_SET)
    {
        HANDLE h_temp;

        // Try to connect to DLE IO pipe again
        // If everything is OK, CreateFiles have to return ERROR_PIPE_BUSY
        h_temp = CreateFile( 
            gca_pipe_name,                                          // pipe name 
            GENERIC_READ | GENERIC_WRITE | FILE_WRITE_ATTRIBUTES,   // read and write access 
            0,                                                      // no sharing 
            NULL,                                                   // default security attributes
            OPEN_EXISTING,                                          // opens existing pipe 
            FILE_FLAG_OVERLAPPED,                                   //attributes 
            NULL);                                                  // no template file 

        if (h_temp == INVALID_HANDLE_VALUE) 
        {
            n_gle = GetLastError();
            if (n_gle == ERROR_PIPE_BUSY)
            {
                return TRUE;
            }
        }
        else
        {
            CloseHandle(h_temp);
        }

        gt_flags.io_conn = FL_FALL;
    }

    return TRUE;
}


void dle_io_disconnect(){
    CloseHandle(gh_DLE_IO_rd);
}

int dle_dev_clear_fifos(){

    int n_rc;

    if (gt_flags.dev_conn != FL_SET)
    {
        return TRUE;
    }

    n_rc = FT_W32_PurgeComm(gh_dle_dev, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
    if (!n_rc)
    {
        gt_flags.dev_conn = FL_FALL;
    }

    return n_rc;
}

int dle_dev_resp_rx_init(DWORD dw_dev_resp_req_len ){

    int n_rc, n_gle;

    gdw_dev_bytes_rcv = 0;

    n_rc = FT_W32_ReadFile(
        gh_dle_dev,
        guca_dev_resp, 
        dw_dev_resp_req_len ,
        &gdw_dev_bytes_rcv, 
        &gt_rx_dev_overlapped);

    if (n_rc == 0)
    {
        n_gle = FT_W32_GetLastError(gh_dle_dev);
        if ( n_gle != ERROR_IO_PENDING) 
        {
            gt_flags.dev_conn = FL_FALL;
            SetEvent(gha_events[HANDLE_NOT_IDLE]);
            return FALSE;
        }
        //// Reset response for N idle periods
        //// Initiate another IO rx if dev response not received within timeout period
        //gn_dev_resp_timeout = 5;
    }
    else
    {
        // SetEvent(gha_events[HADLE_DEV_RESP]);  // ??? manualy set response received event ???
    }

    return TRUE;
}


int dle_io_pipe_tx(){

    int n_rc, n_gle;

    if (wcslen(gca_io_cmd_resp) == 0)
        return TRUE;

    DWORD dw_n;
    // Write back command response to UI
    n_rc = WriteFile(
        gh_DLE_IO_rd,                   //__in         HANDLE hFile,
        gca_io_cmd_resp,                //__in         LPCVOID lpBuffer,
        wcslen(gca_io_cmd_resp)*2+2,    //__in         DWORD nNumberOfBytesToWrite,
        &dw_n,                          //__out_opt    LPDWORD lpNumberOfBytesWritten,
        &gt_dle_io_tx_overlap);         //__inout_opt  LPOVERLAPPED lpOverlapped

    if (!n_rc)
    {
        n_gle = GetLastError();
        if (n_gle != ERROR_IO_PENDING)
        {
            gt_flags.io_conn = FL_FALL;
        }
    }

    return TRUE;
}

