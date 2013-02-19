/************************************************************
The contents of this file are subject to the Pumpkin Salvo
License (the "License").  You may not use this file except
in compliance with the License. You may obtain a copy of
the License at http://www.pumpkininc.com, or from
warranty@pumpkininc.com.

Software distributed under the License is distributed on an
"AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express
or implied. See the License for specific language governing
the warranty and the rights and limitations under the
License.

The Original Code is Salvo - The RTOS that runs in tiny
places(TM). Copyright (C) 1995-2008 Pumpkin, Inc. and its
Licensor(s). All Rights Reserved.

$Source: C:\\RCS\\D\\Pumpkin\\Salvo\\Src\\salvomem.c,v $
$Author: aek $
$Revision: 3.38 $
$Date: 2008-04-27 14:45:34-07 $

Global variable declarations.

************************************************************/

#if !defined(__SALVOMEM_C)
#define __SALVOMEM_C

#include <salvo.h>

/************************************************************
****                                                     ****
**                                                         **
This file works together with ../inc/salvoprg.h, which
"inserts" compiler-specific #pragmas where necessary.

The pragmas are generally used to control the location of
Salvo's globals variables in the target processor's RAM
space.

**                                                         **
****                                                     ****
************************************************************/

/* by placing OScTcbP as the first Salvo object, we're  */
/*  able to avoid problems that crop up with targets    */
/*  that allow placement of variables at address 0      */
/*  (e.g. PIC18+MPLAB-C18, 8051+xdata, etc.). This      */
/*  works because we never point to OScTcbP.            */
#if OSENABLE_TASKS
#include <salvoprg.h>
OSgltypeCTcbP OScTcbP;
#endif


#if OSUSE_ARRAYS
#if OSARRAY_SIZE_IS_BYTE
const OStypePrioA OSBits[8] =
        { 0x0001, 0x0002, 0x0004, 0x0008,
          0x0010, 0x0020, 0x0040, 0x0080 };
#else
const OStypePrioA OSBits[16] =
        { 0x0001, 0x0002, 0x0004, 0x0008,
          0x0010, 0x0020, 0x0040, 0x0080,
          0x0100, 0x0200, 0x0400, 0x0800,
          0x1000, 0x2000, 0x4000, 0x8000 };
#endif /* #if OSTASKS < 9 */
#endif


#if OSENABLE_TASKS
#if OSOBJECT_PRAGMAS_REQUIRED
#define __OSTCBAREA_MEM_C
#include <salvoprg.h>
#endif
OSgltypeTcb OStcbArea[OSTASKS];
#undef  __OSTCBAREA_MEM_C
#endif


#if OSENABLE_TASKS && !OSUSE_ARRAYS
#if OSOBJECT_PRAGMAS_REQUIRED
#include <salvoprg.h>
#endif
OSgltypeTcbP OSeligQP;
#endif


#if OSENABLE_TASKS && OSUSE_ARRAYS
#if OSOBJECT_PRAGMAS_REQUIRED
#include <salvoprg.h>
#endif
OSgltypePrioA OSeligQP;
#endif


#if OSENABLE_EVENTS
#if OSOBJECT_PRAGMAS_REQUIRED
#define __OSECBAREA_MEM_C
#include <salvoprg.h>
#endif
OSgltypeEcb OSecbArea[OSEVENTS];
#undef  __OSECBAREA_MEM_C
#endif


#if OSENABLE_SIGQ
#if OSOBJECT_PRAGMAS_REQUIRED
#include <salvoprg.h>
#endif
OSgltypeSigQP OSsigQinP;
#endif


#if OSENABLE_SIGQ
#if OSOBJECT_PRAGMAS_REQUIRED
#include <salvoprg.h>
#endif
OSgltypeSigQP OSsigQoutP;
#endif


#if OSENABLE_EVENT_FLAGS && OSEVENT_FLAGS
#if OSOBJECT_PRAGMAS_REQUIRED
#define __OSEFCBAREA_MEM_C
#include <salvoprg.h>
#endif
OSgltypeEfcb OSefcbArea[OSEVENT_FLAGS];
#undef  __OSEFCBAREA_MEM_C
#endif


#if OSENABLE_MESSAGE_QUEUES && OSMESSAGE_QUEUES
#if OSOBJECT_PRAGMAS_REQUIRED
#define __OSMQCBAREA_MEM_C
#include <salvoprg.h>
#endif
OSgltypeMqcb OSmqcbArea[OSMESSAGE_QUEUES];
#undef  __OSMQCBAREA_MEM_C
#endif


#if OSENABLE_DELAYS || OSENABLE_TIMEOUTS
#if OSOBJECT_PRAGMAS_REQUIRED
#include <salvoprg.h>
#endif
OSgltypeTcbP OSdelayQP;
#endif


#if OSENABLE_STACK_CHECKING
#include <salvoprg.h>
OSgltypeDepth OSstkDepth, OSmaxStkDepth;
#endif


#if OSGATHER_STATISTICS && OSENABLE_COUNTS
#include <salvoprg.h>
OSgltypeCount OSctxSws;
#endif


#if OSGATHER_STATISTICS && OSENABLE_COUNTS && OSENABLE_IDLE_COUNTER
#include <salvoprg.h>
OSgltypeCount OSidleCtxSws;
#endif


#if OSGATHER_STATISTICS && OSENABLE_TIMEOUTS
#include <salvoprg.h>
OSgltypeErr OStimeouts;
#endif


#if OSLOGGING
#include <salvoprg.h>
OSgltypeErr OSerrs, OSwarns;
#endif


#if OSLOG_MESSAGES > OSLOG_NONE
#include <salvoprg.h>
OSgltypeLogMsg OSlogMsg[80];
#endif


#if OSENABLE_TICKS
#if OSOBJECT_PRAGMAS_REQUIRED
#include <salvoprg.h>
#endif
OSgltypeTick OStimerTicks;
#endif


#if OSENABLE_PRESCALAR
#include <salvoprg.h>
OSgltypePS OStimerPS;
#endif


#if OSCTXSW_METHOD == OSRTNADDR_IS_VAR
#include <salvoprg.h>
OStypeRtnAddr OSrtnAddr;
#endif


#if OSENABLE_DELAYS
#if OSOBJECT_PRAGMAS_REQUIRED
#include <salvoprg.h>
#endif
OSgltypeLostTick OSlostTicks;
#endif


#if ( (OSCTXSW_METHOD == OSVIA_OSDISPATCH) \
  ||  (OSCTXSW_METHOD == OSVIA_OSDISPATCH_WLABEL) \
  ||  (OSCTXSW_METHOD == OSVIA_OSDISPATCH_WPARAM)  )
#if OSOBJECT_PRAGMAS_REQUIRED
#include <salvoprg.h>
#endif
OSgltypeFrameP OSframeP;
#endif


/* eight levels are more than enough ... */
#if ( OSCOMPILER == OSIAR_ICC ) && ( OSTARGET == OSPIC18 )

#define OSLOC_SAVEPIC18INTS __nonbanked /* always */

#if OSPIC18_INTERRUPT_MASK & 0x80
#include <salvoprg.h>
OSLOC_SAVEPIC18INTS OStypeInt8u OSsavePIC18GIE;
#endif

#if OSPIC18_INTERRUPT_MASK & 0x40
#include <salvoprg.h>
OSLOC_SAVEPIC18INTS OStypeInt8u OSsavePIC18PEIE;
#endif

#undef OSLOC_SAVEPIC18INTS

#endif


/* allow us to "clean up" / reset any memory qualifiers  */
/*  in use                        */
#if OSOBJECT_PRAGMAS_REQUIRED
#define __OSENDSALVOVARS_MEM_C
#include <salvoprg.h>
#endif
#undef  __OSENDSALVOVARS_MEM_C


#endif /* __SALVOMEM_C */
