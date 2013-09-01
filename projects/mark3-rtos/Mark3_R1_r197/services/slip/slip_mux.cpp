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
    \file slip_mux.cpp
    \brief FunkenSlip Channel Multiplexer
    
    Demultiplexes FunkenSlip packets transmitted over a single serial channel,
    and provides an abstraction to attach handlers for each event type. 
*/

#include "kerneltypes.h"
#include "driver.h"
#include "drvUART.h"
#include "slip.h"
#include "slip_mux.h"
#include "message.h"

//---------------------------------------------------------------------------
MessageQueue *SlipMux::m_pclMessageQueue;
K_UCHAR SlipMux::m_aucData[SLIP_BUFFER_SIZE];
Driver *SlipMux::m_pclDriver;
Slip_Channel SlipMux::m_apfChannelHandlers[SLIP_CHANNEL_COUNT] = {0};
Semaphore SlipMux::m_clSlipSem;
Slip SlipMux::m_clSlip;

//---------------------------------------------------------------------------
/*!
    \fn static void SlipMux_CallBack(Driver *pclDriver_)
    \param pclDriver_ Pointer to the driver data for the port triggering
                    the callback
*/
static void SlipMux_CallBack( Driver *pclDriver_)
{
    Message *pclMsg = GlobalMessagePool::Pop();
    if (pclMsg)
    {        
        pclDriver_->Control(CMD_SET_RX_DISABLE, 0, 0, 0, 0);
        
        // Send a message to the queue, letting it know that there's a 
        // pending slip message that needs to be processed
        pclMsg->SetCode(SLIP_RX_MESSAGE_ID);
        pclMsg->SetData(NULL);
        SlipMux::GetQueue()->Send(pclMsg);
    }
}

//---------------------------------------------------------------------------
void SlipMux::Init(const K_CHAR *pcDriverPath_, K_USHORT usRxSize_, K_UCHAR *aucRx_, K_USHORT usTxSize_, K_UCHAR *aucTx_)
{
    m_pclDriver = DriverList::FindByPath(pcDriverPath_);
    m_pclMessageQueue = NULL;

    m_clSlip.SetDriver(m_pclDriver);
    m_clSlipSem.Init(0, 1);
    
    m_pclDriver->Control(CMD_SET_BUFFERS, (void*)aucRx_, usRxSize_, (void*)aucTx_, usTxSize_);    
    m_pclDriver->Control(CMD_SET_RX_CALLBACK, (void*)SlipMux_CallBack, 0, 0, 0);
    {
        K_UCHAR ucEscape = 192;
        m_pclDriver->Control(CMD_SET_RX_ESCAPE, (void*)&ucEscape, 1, 0, NULL);
    }
}

//---------------------------------------------------------------------------
void SlipMux::InstallHandler( K_UCHAR ucChannel_, Slip_Channel pfHandler_ )
{
    if (pfHandler_)
    {
        m_apfChannelHandlers[ucChannel_] = pfHandler_;
    }
}

//---------------------------------------------------------------------------
void SlipMux::MessageReceive(void)
{    
    K_USHORT usLen;
    K_UCHAR ucChannel;

    usLen = m_clSlip.ReadData( &ucChannel, (K_CHAR*)m_aucData, SLIP_BUFFER_SIZE );
    if (usLen && (m_apfChannelHandlers[ucChannel] != NULL))
    {
        m_apfChannelHandlers[ucChannel]( m_pclDriver, ucChannel, &(m_aucData[3]), usLen);
    }
        
    // Re-enable the driver once we're done.
    m_pclDriver->Control( CMD_SET_RX_ENABLE, 0, 0, 0, 0 );    
}

