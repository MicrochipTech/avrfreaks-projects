/*! \file
 * \brief eeOS 32-bit AVR UC3 Port
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

#ifndef __OS_PORT_H__
#define __OS_PORT_H__

#include "compiler.h"
#include "interrupt.h"

/*!
 * \ingroup os_scheduler_type
 * \brief Use the RTC peripheral to generate the scheduler interrupts
 */
#define CONFIG_OS_SCHEDULER_USE_RTC 1
/*!
 * \ingroup os_scheduler_type
 * \brief Use the internal compare feature to generate the scheduler interrupts.
 * Using this configuration, will affect the internal cycle counter value.
 */
#define CONFIG_OS_SCHEDULER_USE_COMPARE 2
/*!
 * \ingroup os_scheduler_type
 * \brief Use the TC peripheral to generate the scheduler interrupts
 */
#define CONFIG_OS_SCHEDULER_USE_TC 3

/*! \def CONFIG_OS_DEFAULT_SR_VALUE
 * \ingroup group_os_config
 * \brief This is the default value assigned to the Status Register (SR) before
 * running a new task
 */
#ifndef CONFIG_OS_DEFAULT_SR_VALUE
	//! SR: [M2:M0]=001 I1M=0 I0M=0, GM=0
	#define CONFIG_OS_DEFAULT_SR_VALUE 0x00400000
#endif

/*! \def CONFIG_OS_SCHEDULER_IRQ_PRIORITY
 * \ingroup group_os_config
 * \brief IRQ priority of the tick interrupt
 */
#ifndef CONFIG_OS_SCHEDULER_IRQ_PRIORITY
	#define CONFIG_OS_SCHEDULER_IRQ_PRIORITY 0
#endif

/*! \def CONFIG_OS_SCHEDULER_TC_CHANNEL
 * \ingroup group_os_config
 * \brief TC channel used to generate the tick interrupt
 * \pre \ref CONFIG_OS_SCHEDULER_USE_TC must be used
 */
#ifndef CONFIG_OS_SCHEDULER_TC_CHANNEL
	#define CONFIG_OS_SCHEDULER_TC_CHANNEL 0
#endif

static inline void os_enter_critical(void) {
	cpu_irq_disable();
};

static inline void os_leave_critical(void) {
	cpu_irq_enable();
};

static inline bool os_is_critical(void) {
	return !cpu_irq_is_enabled();
}

/* Declaration of the interrupt handler function
 */
#if __GNUC__
__attribute__((__interrupt__)) static void os_switch_context_int_handler(void);
#elif __ICCAVR32__
__interrupt static void os_switch_context_int_handler(void);
#endif

#define os_switch_context(bypass_context_saving) \
	do { \
		__asm__ __volatile__ ( \
			"mov r8, "ASTRINGZ(bypass_context_saving)"\n\t" \
			"scall\n\t" \
		); \
	} while (false)

#define OS_COMPILER_ALIGN 2
typedef void * os_ptr_t;
typedef uint32_t os_intptr_t;
typedef uint32_t os_reg_t;
typedef uint32_t os_cy_t;

/*! Benchmark ports
 */
static inline os_cy_t os_read_cycle_counter(void) {
	return Get_system_register(AVR32_COUNT);
}

/*! Get the entry point of the memory allocated for the application stack.
 */
static inline os_ptr_t os_get_app_stack(void) {
#if __GNUC__
	extern uint32_t _stack;
	return (os_ptr_t) _stack;
#elif __ICCAVR32__
	#pragma segment="SSTACK" __data21
	return (os_ptr_t) __segment_begin("SSTACK");
#endif
}

#endif // __OS_PORT_H__
