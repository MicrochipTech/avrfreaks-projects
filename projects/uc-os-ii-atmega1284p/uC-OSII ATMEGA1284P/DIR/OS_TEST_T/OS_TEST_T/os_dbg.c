/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                           DEBUGGER CONSTANTS
*
*                              (c) Copyright 1992-2007, Micrium, Weston, FL
*                                           All Rights Reserved
*
* File    : OS_DBG.C
* By      : Jean J. Labrosse
* Version : V2.86
*
* LICENSING TERMS:
* ---------------
*   uC/OS-II is provided in source form for FREE evaluation, for educational use or for peaceful research.  
* If you plan on using  uC/OS-II  in a commercial product you need to contact Micri�m to properly license 
* its use in your product. We provide ALL the source code for your convenience and to help you experience 
* uC/OS-II.   The fact that the  source is provided does  NOT  mean that you can use it without  paying a 
* licensing fee.
*********************************************************************************************************
*/

#include <ucos_ii.h>

/*
*********************************************************************************************************
*                                             DEBUG DATA
*********************************************************************************************************
*/

INT16U  const  OSDebugEn           = OS_DEBUG_EN;               /* Debug constants are defined below   */

#if OS_DEBUG_EN > 0

INT32U  const  OSEndiannessTest    = 0x12345678L;               /* Variable to test CPU endianness     */

INT16U  const  OSEventEn           = OS_EVENT_EN;
INT16U  const  OSEventMax          = OS_MAX_EVENTS;             /* Number of event control blocks      */
INT16U  const  OSEventNameSize     = OS_EVENT_NAME_SIZE;        /* Size (in bytes) of event names      */
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0)
INT16U  const  OSEventSize         = sizeof(OS_EVENT);          /* Size in Bytes of OS_EVENT           */
INT16U  const  OSEventTblSize      = sizeof(OSEventTbl);        /* Size of OSEventTbl[] in bytes       */
#else
INT16U  const  OSEventSize         = 0;
INT16U  const  OSEventTblSize      = 0;
#endif
INT16U  const  OSEventMultiEn      = OS_EVENT_MULTI_EN;


INT16U  const  OSFlagEn            = OS_FLAG_EN;
#if (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
INT16U  const  OSFlagGrpSize       = sizeof(OS_FLAG_GRP);       /* Size in Bytes of OS_FLAG_GRP        */
INT16U  const  OSFlagNodeSize      = sizeof(OS_FLAG_NODE);      /* Size in Bytes of OS_FLAG_NODE       */
INT16U  const  OSFlagWidth         = sizeof(OS_FLAGS);          /* Width (in bytes) of OS_FLAGS        */
#else
INT16U  const  OSFlagGrpSize       = 0;
INT16U  const  OSFlagNodeSize      = 0;
INT16U  const  OSFlagWidth         = 0;
#endif
INT16U  const  OSFlagMax           = OS_MAX_FLAGS;
INT16U  const  OSFlagNameSize      = OS_FLAG_NAME_SIZE;         /* Size (in bytes) of flag names       */

INT16U  const  OSLowestPrio        = OS_LOWEST_PRIO;

INT16U  const  OSMboxEn            = OS_MBOX_EN;

INT16U  const  OSMemEn             = OS_MEM_EN;
INT16U  const  OSMemMax            = OS_MAX_MEM_PART;           /* Number of memory partitions         */
INT16U  const  OSMemNameSize       = OS_MEM_NAME_SIZE;          /* Size (in bytes) of partition names  */
#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
INT16U  const  OSMemSize           = sizeof(OS_MEM);            /* Mem. Partition header sine (bytes)  */
INT16U  const  OSMemTblSize        = sizeof(OSMemTbl);
#else
INT16U  const  OSMemSize           = 0;
INT16U  const  OSMemTblSize        = 0;
#endif
INT16U  const  OSMutexEn           = OS_MUTEX_EN;

INT16U  const  OSPtrSize           = sizeof(void *);            /* Size in Bytes of a pointer          */

INT16U  const  OSQEn               = OS_Q_EN;
INT16U  const  OSQMax              = OS_MAX_QS;                 /* Number of queues                    */
#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
INT16U  const  OSQSize             = sizeof(OS_Q);              /* Size in bytes of OS_Q structure     */
#else
INT16U  const  OSQSize             = 0;
#endif

INT16U  const  OSRdyTblSize        = OS_RDY_TBL_SIZE;           /* Number of bytes in the ready table  */

INT16U  const  OSSemEn             = OS_SEM_EN;

INT16U  const  OSStkWidth          = sizeof(OS_STK);            /* Size in Bytes of a stack entry      */

INT16U  const  OSTaskCreateEn      = OS_TASK_CREATE_EN;
INT16U  const  OSTaskCreateExtEn   = OS_TASK_CREATE_EXT_EN;
INT16U  const  OSTaskDelEn         = OS_TASK_DEL_EN;
INT16U  const  OSTaskIdleStkSize   = OS_TASK_IDLE_STK_SIZE;
INT16U  const  OSTaskProfileEn     = OS_TASK_PROFILE_EN;
INT16U  const  OSTaskMax           = OS_MAX_TASKS + OS_N_SYS_TASKS; /* Total max. number of tasks      */
INT16U  const  OSTaskNameSize      = OS_TASK_NAME_SIZE;             /* Size (in bytes) of task names   */
INT16U  const  OSTaskStatEn        = OS_TASK_STAT_EN;
INT16U  const  OSTaskStatStkSize   = OS_TASK_STAT_STK_SIZE;
INT16U  const  OSTaskStatStkChkEn  = OS_TASK_STAT_STK_CHK_EN;
INT16U  const  OSTaskSwHookEn      = OS_TASK_SW_HOOK_EN;

INT16U  const  OSTCBPrioTblMax     = OS_LOWEST_PRIO + 1;        /* Number of entries in OSTCBPrioTbl[] */
INT16U  const  OSTCBSize           = sizeof(OS_TCB);            /* Size in Bytes of OS_TCB             */
INT16U  const  OSTicksPerSec       = OS_TICKS_PER_SEC;
INT16U  const  OSTimeTickHookEn    = OS_TIME_TICK_HOOK_EN;
INT16U  const  OSVersionNbr        = OS_VERSION;

INT16U  const  OSTmrEn             = OS_TMR_EN;
INT16U  const  OSTmrCfgMax         = OS_TMR_CFG_MAX;
INT16U  const  OSTmrCfgNameSize    = OS_TMR_CFG_NAME_SIZE;
INT16U  const  OSTmrCfgWheelSize   = OS_TMR_CFG_WHEEL_SIZE;
INT16U  const  OSTmrCfgTicksPerSec = OS_TMR_CFG_TICKS_PER_SEC;

#if (OS_TMR_EN > 0) && (OS_TMR_CFG_MAX > 0)
INT16U  const  OSTmrSize           = sizeof(OS_TMR);
INT16U  const  OSTmrTblSize        = sizeof(OSTmrTbl);
INT16U  const  OSTmrWheelSize      = sizeof(OS_TMR_WHEEL);
INT16U  const  OSTmrWheelTblSize   = sizeof(OSTmrWheelTbl);
#else
INT16U  const  OSTmrSize           = 0;
INT16U  const  OSTmrTblSize        = 0;
INT16U  const  OSTmrWheelSize      = 0;
INT16U  const  OSTmrWheelTblSize   = 0;
#endif

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                             DEBUG DATA
*                            TOTAL DATA SPACE (i.e. RAM) USED BY uC/OS-II
*********************************************************************************************************
*/
#if OS_DEBUG_EN > 0

INT16U  const  OSDataSize = sizeof(OSCtxSwCtr)
#if (OS_EVENT_EN) && (OS_MAX_EVENTS > 0)
                          + sizeof(OSEventFreeList)
                          + sizeof(OSEventTbl)
#endif
#if (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
                          + sizeof(OSFlagTbl)
                          + sizeof(OSFlagFreeList)
#endif
#if OS_TASK_STAT_EN > 0
                          + sizeof(OSCPUUsage)
                          + sizeof(OSIdleCtrMax)
                          + sizeof(OSIdleCtrRun)
                          + sizeof(OSStatRdy)
                          + sizeof(OSTaskStatStk)
#endif
#if OS_TICK_STEP_EN > 0
                          + sizeof(OSTickStepState)
#endif
#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
                          + sizeof(OSMemFreeList)
                          + sizeof(OSMemTbl)
#endif
#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
                          + sizeof(OSQFreeList)
                          + sizeof(OSQTbl)
#endif
#if OS_TIME_GET_SET_EN > 0   
                          + sizeof(OSTime)
#endif
#if (OS_TMR_EN > 0) && (OS_TMR_CFG_MAX > 0)
                          + sizeof(OSTmrFree)
                          + sizeof(OSTmrUsed)
                          + sizeof(OSTmrTime)
                          + sizeof(OSTmrSem)
                          + sizeof(OSTmrSemSignal)
                          + sizeof(OSTmrTbl)
                          + sizeof(OSTmrFreeList)
                          + sizeof(OSTmrTaskStk)
                          + sizeof(OSTmrWheelTbl)
#endif
                          + sizeof(OSIntNesting)
                          + sizeof(OSLockNesting)
                          + sizeof(OSPrioCur)
                          + sizeof(OSPrioHighRdy)
                          + sizeof(OSRdyGrp)
                          + sizeof(OSRdyTbl)
                          + sizeof(OSRunning)
                          + sizeof(OSTaskCtr)
                          + sizeof(OSIdleCtr)
                          + sizeof(OSTaskIdleStk)
                          + sizeof(OSTCBCur)
                          + sizeof(OSTCBFreeList)
                          + sizeof(OSTCBHighRdy)
                          + sizeof(OSTCBList)
                          + sizeof(OSTCBPrioTbl)
                          + sizeof(OSTCBTbl);

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                        OS DEBUG INITIALIZATION
*
* Description: This function is used to make sure that debug variables that are unused in the application
*              are not optimized away.  This function might not be necessary for all compilers.  In this
*              case, you should simply DELETE the code in this function while still leaving the declaration
*              of the function itself.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : (1) This code doesn't do anything, it simply prevents the compiler from optimizing out
*                  the 'const' variables which are declared in this file.
*              (2) You may decide to 'compile out' the code (by using #if 0/#endif) INSIDE the function 
*                  if your compiler DOES NOT optimize out the 'const' variables above.
*********************************************************************************************************
*/

#if OS_DEBUG_EN > 0
void  OSDebugInit (void)
{
    void  *ptemp;

    
    ptemp = (void *)&OSDebugEn;

    ptemp = (void *)&OSEndiannessTest;

    ptemp = (void *)&OSEventMax;
    ptemp = (void *)&OSEventNameSize;
    ptemp = (void *)&OSEventEn;
    ptemp = (void *)&OSEventSize;
    ptemp = (void *)&OSEventTblSize;
    ptemp = (void *)&OSEventMultiEn;

    ptemp = (void *)&OSFlagEn;
    ptemp = (void *)&OSFlagGrpSize;
    ptemp = (void *)&OSFlagNodeSize;
    ptemp = (void *)&OSFlagWidth;
    ptemp = (void *)&OSFlagMax;
    ptemp = (void *)&OSFlagNameSize;

    ptemp = (void *)&OSLowestPrio;

    ptemp = (void *)&OSMboxEn;

    ptemp = (void *)&OSMemEn;
    ptemp = (void *)&OSMemMax;
    ptemp = (void *)&OSMemNameSize;
    ptemp = (void *)&OSMemSize;
    ptemp = (void *)&OSMemTblSize;

    ptemp = (void *)&OSMutexEn;

    ptemp = (void *)&OSPtrSize;

    ptemp = (void *)&OSQEn;
    ptemp = (void *)&OSQMax;
    ptemp = (void *)&OSQSize;

    ptemp = (void *)&OSRdyTblSize;

    ptemp = (void *)&OSSemEn;

    ptemp = (void *)&OSStkWidth;

    ptemp = (void *)&OSTaskCreateEn;
    ptemp = (void *)&OSTaskCreateExtEn;
    ptemp = (void *)&OSTaskDelEn;
    ptemp = (void *)&OSTaskIdleStkSize;
    ptemp = (void *)&OSTaskProfileEn;
    ptemp = (void *)&OSTaskMax;
    ptemp = (void *)&OSTaskNameSize;
    ptemp = (void *)&OSTaskStatEn;
    ptemp = (void *)&OSTaskStatStkSize;
    ptemp = (void *)&OSTaskStatStkChkEn;
    ptemp = (void *)&OSTaskSwHookEn;

    ptemp = (void *)&OSTCBPrioTblMax;
    ptemp = (void *)&OSTCBSize;

    ptemp = (void *)&OSTicksPerSec;
    ptemp = (void *)&OSTimeTickHookEn;

#if OS_TMR_EN > 0
    ptemp = (void *)&OSTmrTbl[0];
    ptemp = (void *)&OSTmrWheelTbl[0];
    
    ptemp = (void *)&OSTmrEn;
    ptemp = (void *)&OSTmrCfgMax;
    ptemp = (void *)&OSTmrCfgNameSize;
    ptemp = (void *)&OSTmrCfgWheelSize;
    ptemp = (void *)&OSTmrCfgTicksPerSec;
    ptemp = (void *)&OSTmrSize;
    ptemp = (void *)&OSTmrTblSize;

    ptemp = (void *)&OSTmrWheelSize;
    ptemp = (void *)&OSTmrWheelTblSize;
#endif

    ptemp = (void *)&OSVersionNbr;

    ptemp = (void *)&OSDataSize;

    ptemp = ptemp;                             /* Prevent compiler warning for 'ptemp' not being used! */
}
#endif
