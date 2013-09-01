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
    \file slip_hid.h
    
    \brief HID device abstraction over FunkenSlip channel    
*/

#ifndef __SLIP_HID_H__
#define __SLIP_HID_H__

#include "kerneltypes.h"
#include "driver.h"

//---------------------------------------------------------------------------
typedef enum
{
    CMD_SET_SURFACE,    // Set the event surface to attach to (output)
    CMD_SET_SLIPMUX,    // Set the slip mux object to attach to (input)
} SlipDriverCommands_t;

//---------------------------------------------------------------------------
class SlipHid : public Driver 
{
public:
    virtual void Init() {}
    
    virtual K_UCHAR Open() 
        { return 0; }
    
    virtual K_UCHAR Close() 
        { return 0; }
    
    virtual K_USHORT Read( K_USHORT usBytes_, K_UCHAR *pucData_) 
        { return usBytes_; }
    
    virtual K_USHORT Write( K_USHORT usBytes_, K_UCHAR *pucData_) 
        { return usBytes_; }
    
    virtual K_USHORT Control( K_USHORT usEvent_, void *pvDataIn_,
                     K_USHORT usSizeIn_, void *pvDataOut_, K_USHORT usSizeOut_ );
private:
};


#endif
