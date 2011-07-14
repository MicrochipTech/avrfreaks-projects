/***C*********************************************************************************************
**
** SRC-FILE     :   dle_cmd_proc.h
**                                        
** PROJECT      :   DLE user interface
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   DLE_CMD_PROC gloabal defines
**                  
** COPYRIGHT    :   
**
****C*E******************************************************************************************/
#ifndef _DLE_CMD_PROC_H
#define _DLE_CMD_PROC_H


#define IO_RX_MSG_LEN 1024
#define IO_TX_MSG_LEN 1024

#define FL_CLR  0
#define FL_RISE 1
#define FL_FALL 2
#define FL_SET  3
#define FL_UNDEF  4

typedef struct flags_tag{
    DWORD   verbose        : 8;
    DWORD   io_auto_start  : 8;
    DWORD   io_connected   : 8;
}CMD_PROC_FLAGS;

#define HANDLE_KEYBOARD         0
#define HANDLE_RX_IO_MSG        1
#define HANDLE_TX_IO_MSG        2
#define HANDLE_IO_PIPE          3
#define HANDLE_LAST_HANDLE      4

#define HANDLES_NUM HANDLE_LAST_HANDLE


#endif _DLE_CMD_PROC_H