/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!

    \file   drvUART.h

    \brief  SAM D20s serial port driver

*/
#ifndef __DRVUART_H_
#define __DRVUART_H_

#include "kerneltypes.h"
#include "driver.h"
#include "sys_uart.h"

//---------------------------------------------------------------------------
// UART defines - user-configurable for different targets
//---------------------------------------------------------------------------
#define UART_DEFAULT_BAUD       ((K_ULONG)57600)

//---------------------------------------------------------------------------
typedef enum
{
    CMD_SET_BAUDRATE = 0x80,
    CMD_SET_BUFFERS,    
    CMD_SET_RX_ESCAPE,
    CMD_SET_RX_CALLBACK,
    CMD_SET_RX_ECHO,
    CMD_SET_RX_ENABLE,
    CMD_SET_RX_DISABLE
} CMD_UART;

class D20_UART;
//---------------------------------------------------------------------------
typedef struct _UartData_t UartData_t;

//---------------------------------------------------------------------------
typedef void (*UART_Rx_Callback_t)( D20_UART *pclUART );

//---------------------------------------------------------------------------
class D20_UART : public Driver
{
    
public:        
    virtual void Init();
    virtual K_UCHAR Open();
    virtual K_UCHAR Close();
    virtual K_USHORT Read( K_USHORT usBytes_, 
                                 K_UCHAR *pucData_ );
                                 
    virtual K_USHORT Write( K_USHORT usBytes_, 
                                  K_UCHAR *pucData_ );
                                  
    virtual K_USHORT Control( K_USHORT usEvent_, 
                                    void *pvIn_, 
                                    K_USHORT usSizeIn_, 
                                    void *pvOut_, 
                                    K_USHORT usSizeOut_ );

private:

    SysUART m_clUART;

};

#endif 
