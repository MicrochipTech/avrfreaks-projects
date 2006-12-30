#ifndef SST_BASE_DEF_H_
#define SST_BASE_DEF_H_

/**************************************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *************************************************************************************************/

#include <lpc2114.h>
#include <arm.h>


#define SST_MAX_NO_OF_TASKS     5
#define SST_MAX_EV_QUEUE_LEN    0
#define SST_MAX_TASK_PRIO       100

#define SST_USE_SIGNAL_MASK     0
#define SST_USE_LAZY_TIMER      1


typedef uint32_t CPU_Base_T;
typedef uint32_t CPU_SR_T;

#define SST_Start_Nested_Ints()         do { VICVectAddr = 0xFF; ENABLE_NEST_INTS(); } while (0)
#define SST_Stop_Nested_Ints()          DISABLE_NEST_INTS()

#define SST_disable_ints_save(_old_)    DISABLE_INTS_SAVE(_old_)
#define SST_disable_ints()              DISABLE_INTS()
#define SST_enable_ints()               ENABLE_INTS()
#define SST_restore_ints(_old_)         RESTORE_INTS(_old_)

#endif /*SST_BASE_DEF_H_*/
