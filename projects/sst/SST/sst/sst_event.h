#ifndef SST_EVENT_H_
#define SST_EVENT_H_

/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

/** \file sst_event.h
 *  \brief A header with definition of the SST event.
 *
 *  Requires \c sst_base_def.h (user supplied) header.
 */
 
#include <sst_base_def.h>

/** \typedef SST_Signal_T
 *  \brief The datatype used to carry signal value in the SST event.
 * 
 * To achieve maximum efficiency it is defined as a \c CPU_Base_T type.
 */
typedef CPU_Base_T SST_Signal_T;

/** \typedef SST_Param_T
 *  \brief The datatype used to carry parameter value in the SST event.
 * 
 * To achieve maximum efficiency it is defined as a \c CPU_Base_T type.
 */
typedef CPU_Base_T SST_Param_T;

/** \typedef SST_Event_T
 *  \brief The datatype used to carry SST event information.
 *
 *  This is a structure with two fields:
 *      - sig - signal value of the event of type #SST_Signal_T
 *      - par - parameter value of the event of type #SST_Param_T
 */
typedef struct SST_Event_Tag {
    SST_Signal_T sig;
    SST_Param_T par;
} SST_Event_T;

#endif /* SST_EVENT_H_ */
