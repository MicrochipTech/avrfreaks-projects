#ifndef MAIN_TEST_H_
#define MAIN_TEST_H_

/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

#include <sst_timer.h>
#include "ring_buf.h"

extern SST_TaskID_T timer_10ms_tid;
extern SST_TaskID_T timer_100ms_tid;
extern SST_TaskID_T btn_task_id;
extern SST_TaskID_T rx_task_id;
extern SST_TaskID_T interp_task_id;


SST_TICKER_DECLARE(ticker_10ms);
SST_TICKER_DECLARE(ticker_100ms);

extern volatile RingBuffer_T tx_buf;


#endif /*MAIN_TEST_H_*/
