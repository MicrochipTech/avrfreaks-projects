/*



*/
/*********************************************************/
/*             SCHWED-OSEK operating system              */
/*                                                       */
/*   (C) by Joerg Schwedes 2009, All rights reserved     */
/*********************************************************/
#ifndef OS_CNF_H
#define OS_CNF_H

#include <target.h>

/* from target.h */
#define OS_EnaInt() EnaInt()  /* enables global interrupt flag */
#define OS_DisInt() DisInt()  /* disables global interrupt flag */
#define OS_SaveInt() SaveInt()
#define OS_RestoreInt(x) RestoreInt(x)

#define __OS_CONST __CONST 

#define __OS_NO_RETURN(x) __NO_RETURN(x)

/* ************************************************************************************/
/*                      OS option configuration                                       */

#define OS_USE_PREEMP_TASK_OPTION  /* Preemptive tasks option is active */

#define OS_USE_16BIT_TIMETABLE_OPTION /* tasks can be auto scheduled between 1ms..65535ms */
                                      /* otherwise scheduling is 8Bit-> 1ms..255ms */

#define OS_USE_MAX_16TASKS_OPTION /* A maximum of 16 tasks are possible */
                           /* otherwise only 8 tasks are supported */
#endif
