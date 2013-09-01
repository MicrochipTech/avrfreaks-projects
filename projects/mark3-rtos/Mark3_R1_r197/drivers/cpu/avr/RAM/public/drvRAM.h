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

    \file   drvRAM.h

    \brief  Generic RAM-buffer driver

*/
#ifndef __DRVEEPROM_H_
#define __DRVEEPROM_H_

#include "kerneltypes.h"
#include "driver.h"

typedef enum
{
    EEPROM_CMD_SEEK = 0x80,    //!< Set the current EEPROM address, used for read/write
    EEPROM_CMD_SET_BUFFER    //!< Set the data buffer and buffer size before use
} EEPROM_Cmd_t;

class RAMDriver;

class RAMDriver : public Driver
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
    K_USHORT m_usAddress;        
    K_USHORT m_usSize;
    K_UCHAR *m_pcData;
};

#endif 
