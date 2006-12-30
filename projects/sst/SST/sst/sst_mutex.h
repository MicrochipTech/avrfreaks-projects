#ifndef SST_MUTEX_H_
#define SST_MUTEX_H_

/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

/** \file sst_mutex.h
 *  \brief A header file with the SST mutex API.
 *
 *  Requires \c sst.h (from sst folder) headers.
 */

#include <sst.h>


/** \typedef SST_Mutex_T
 *  \brief The datatype used to keep a SST mutex data.
 *
 * It is used internally by API and should be not changed directly by user. 
 */ 
typedef CPU_Base_T SST_Mutex_T;


/** \fn SST_Mutex_T SST_MutexLock(CPU_Base_T prio_ceiling)
 *  \brief Locks mutex.
 *
 * Because of the main idea of the SST the mutex functionality is implemented by
 * raisng of the current task priority.
 * 
 *  \param prio_ceiling - a priority for mutex, any taks with priority lower than 
 * or equal to \a prio_ceiling will be suspended (blocked) until call to 
 * the #SST_MutexUnlock  
 *  \return a #SST_Mutex_T object
 */
SST_Mutex_T SST_MutexLock(CPU_Base_T prio_ceiling);


/** \fn void SST_MutexUnlock(SST_Mutex_T mutex)
 *  \brief Unlocks mutex.
 *
 *  \param mutex - a #SST_Mutex_T object returned by #SST_MutexLock function
 */
void SST_MutexUnlock(SST_Mutex_T mutex);

#endif /*SST_MUTEX_H_*/
