/*! \file
 * \brief eeOS Debug
 * \author Blaise Lengrand (blaise.lengrand@gmail.com)
 * \version 0.1
 * \date 2011
 *
 * \section eeos_license License
 * \ref group_os is provided in source form for FREE evaluation, for
 * educational use or for peaceful research. If you plan on using \ref group_os
 * in a commercial product you need to contact the author to properly license
 * its use in your product. The fact that the  source is provided does
 * NOT mean that you can use it without paying a licensing fee.
 */

#include "os_core.h"

/*! \brief Flag set to true if the trace is activated, false otherwise.
 */
bool os_debug_trace_flag = false;
/*! \brief Trace pointer, to keep track of the current trace entry.
 */
struct os_trace *os_debug_trace_ptr;
/*! \brief Trace buffer starting point.
 */
static struct os_trace *os_debug_trace_start;
/*! \brief Trace buffer ending point.
 */
static struct os_trace *os_debug_trace_end;

#warning TODO - fis the trace log function
#if 0
void __os_debug_trace_log(enum os_debug_trace_event event, os_ptr_t data)
{
	/* The following code is only activated if the trace is enabled */
	if (os_debug_trace_flag) {
		/* Save the critical region status */
		bool is_critical = os_is_critical();
		/* The trace log is a shared variable so this snippet of code
		 * must run as in a critical region.
		 */
		if (!is_critical) {
			os_enter_critical();
		}
		/* Fill in the trace information */
		os_debug_trace_ptr->time = os_read_cycle_counter();
		os_debug_trace_ptr->event = event;
		os_debug_trace_ptr->data = data;
		/* Increase the trace pointer. The trace is saved in a circular
		 * buffer, handle the wrapping if any.
		 */
		os_debug_trace_ptr++;
		/* If the new trace pointer is equal to the last trace address,
		 * we need to wrap.
		 */
		if (os_debug_trace_ptr == os_debug_trace_end) {
			os_debug_trace_ptr = os_debug_trace_start;
		}
		/* Leave the critical region if the rest of the code does not
		 * requires to run inside a critical code region.
		 */
		if (!is_critical) {
			os_leave_critical();
		}
	}
}

void os_debug_start_trace(os_ptr_t buffer, int size)
{
	/* Save the first aligned byte of the buffer */
	os_debug_trace_start = OS_ALIGN(buffer);
	/* Update the size which might have changed after the alignement */
	size -= ((int) os_debug_trace_start - (int) buffer);
	/* Compute the last trace entry ending address */
	os_debug_trace_end = os_debug_trace_start
			+ size / sizeof(struct os_trace);
	/* Setup the trace pointer */
	os_debug_trace_ptr = os_debug_trace_start;
	/* Activate the trace */
	os_debug_trace_flag = true;
}

#endif