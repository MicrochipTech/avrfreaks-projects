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

    \file   driver.cpp

    \brief  Device driver/hardware abstraction layer
*/

#include "kerneltypes.h"
#include "mark3cfg.h"
#include "kernel_debug.h"
#include "driver.h"

//---------------------------------------------------------------------------
#if defined __FILE_ID__
	#undef __FILE_ID__
#endif
#define __FILE_ID__ 	DRIVER_CPP

//---------------------------------------------------------------------------
#if KERNEL_USE_DRIVER

DoubleLinkList DriverList::m_clDriverList;

/*!
	This class implements the "default" driver (/dev/null)
*/
class DevNull : public Driver
{
public:
	virtual void Init() { SetName("/dev/null"); };
	virtual K_UCHAR Open() { return 0; }
	virtual K_UCHAR Close() { return 0; }
		
	virtual K_USHORT Read( K_USHORT usBytes_,
	K_UCHAR *pucData_){ return 0; }
	
	virtual K_USHORT Write( K_USHORT usBytes_,
	K_UCHAR *pucData_) { return 0; }
	
	virtual K_USHORT Control( K_USHORT usEvent_,
		void *pvDataIn_,
		K_USHORT usSizeIn_,
		void *pvDataOut_,
		K_USHORT usSizeOut_ ) { return 0; }
	
};

//---------------------------------------------------------------------------
static DevNull clDevNull;

//---------------------------------------------------------------------------
static K_UCHAR DrvCmp( const K_CHAR *szStr1_, const K_CHAR *szStr2_ )
{
	K_CHAR *szTmp1 = (K_CHAR*) szStr1_;
	K_CHAR *szTmp2 = (K_CHAR*) szStr2_;

	while (*szTmp1 && *szTmp2)
	{
		if (*szTmp1++ != *szTmp2++)
		{
			return 0;
		}
	}

	// Both terminate at the same length
	if (!(*szTmp1) && !(*szTmp2))
	{
		return 1;
	}

	return 0;
}

//---------------------------------------------------------------------------
void DriverList::Init()
{
    // Ensure we always have at least one entry - a default in case no match
    // is found (/dev/null)
	clDevNull.Init();
	Add(&clDevNull);
}

//---------------------------------------------------------------------------
Driver *DriverList::FindByPath( const K_CHAR *m_pcPath )
{
	KERNEL_ASSERT( m_pcPath );
	Driver *pclTemp = static_cast<Driver*>(m_clDriverList.GetHead());
	
	while (pclTemp)
	{
		if(DrvCmp(m_pcPath, pclTemp->GetPath()))
		{
			return pclTemp;
		}
		pclTemp = static_cast<Driver*>(pclTemp->GetNext());
	}
	return &clDevNull;
}

#endif
