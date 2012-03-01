/*! \file
 * \brief eeOS Software Interrupts
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

void __os_interrupt_handler(os_ptr_t args)
{
	struct os_interrupt *interrupt = (struct os_interrupt *) args;

	/* Disable scheduler interrupts */
	os_enter_critical();
	/* Remove the interrupt from the chain list to prevent another execution
	 */
	__os_process_disable_naked(__os_interrupt_get_process(interrupt));
	/* Execute the interrupt handler */
	interrupt->int_ptr(interrupt->args);
	/* Manually call the scheduler */
	os_switch_context(true);
}

void os_interrupt_create(struct os_interrupt *interrupt, os_proc_ptr_t int_ptr,
		os_ptr_t args)
{
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_INTERRUPT_CREATE, interrupt);

	/* Create the process */
	__os_process_create(__os_interrupt_get_process(interrupt), NULL,
			OS_PROCESS_TYPE_INTERRUPT);
	/* Fill the structure */
	interrupt->int_ptr = int_ptr;
	interrupt->args = args;
	/* Set default priority for the interrupt */
#if CONFIG_OS_USE_PRIORITY == true
	os_interrupt_set_priority(interrupt,
			CONFIG_OS_INTERRUPT_DEFAULT_PRIORITY);
#endif
}
