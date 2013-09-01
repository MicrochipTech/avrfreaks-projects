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
    \file shiftreg.cpp
    \brief Shift register functionality to allow for efficient I/O expansion
*/
#include "kerneltypes.h"
#include "driver.h"

#ifndef __SHIFTREG_H__
#define __SHIFTREG_H__

//---------------------------------------------------------------------------
typedef enum
{
    SHIFT_SET_CLOCK = 0x80,
    SHIFT_SET_DATA,
    SHIFT_SET_STROBE,
    SHIFT_SET_ENABLE
} ShiftDriverCommands;

//---------------------------------------------------------------------------
/*!
    Shift register driver class
*/
class ShiftDriver : public Driver
{
public:
/*!
    Driver functions
*/
    virtual void Init() {}    //Stubbed
    virtual K_UCHAR Open();
    virtual K_UCHAR Close();
    virtual K_USHORT Read( K_USHORT usBytes_, 
                                 K_UCHAR *pucData_) {return usBytes_;}    //Stubbed
    virtual K_USHORT Write( K_USHORT usBytes_, 
                                  K_UCHAR *pucData_);
    
    virtual K_USHORT Control( K_USHORT usEvent_, 
                                    void *pvDataIn_, 
                                    K_USHORT usSizeIn_, 
                                    void *pvDataOut_, 
                                    K_USHORT usSizeOut_ );
    
    void SetClock( volatile K_UCHAR *pucPort_, K_UCHAR ucBit_ );
    void SetData( volatile K_UCHAR *pucPort_, K_UCHAR ucBit_ );
    void SetStrobe( volatile K_UCHAR *pucPort_, K_UCHAR ucBit_ );
    void SetEnable( volatile K_UCHAR *pucPort_, K_UCHAR ucBit_ );
    
private:
    
    void WriteByte( K_UCHAR ucByte_ );

    volatile K_UCHAR *m_pucClockPort;
    K_UCHAR m_ucClockBit;

    volatile K_UCHAR *m_pucDataPort;
    K_UCHAR m_ucDataBit;

    volatile K_UCHAR *m_pucStrobePort;
    K_UCHAR m_ucStrobeBit;
    
    volatile K_UCHAR *m_pucEnablePort;
    K_UCHAR m_ucEnableBit;    
};

#endif
