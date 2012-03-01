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

#include "os_core.h"

/*! \brief Push a 32-bit value onto the stack of the task
 * \param proc The current process holding the stack we want to update
 * \param value The 32-bit value to be pushed
 */
#define PUSH(proc, value) \
		do { \
			(proc)->sp = (uint32_t *) (proc)->sp - 1; \
			*((uint32_t *) (proc)->sp) = (uint32_t) (value); \
		} while (false)

#if CONFIG_OS_SCHEDULER_TYPE == CONFIG_OS_SCHEDULER_USE_COMPARE
	/* Setup functions to use the compare interrupt
	 */
	void os_setup_scheduler(uint32_t cpu_freq_hz)
	{
		cpu_irq_disable();
		irq_initialize_vectors();
		irq_register_handler((__int_handler) os_switch_context_int_handler,
				AVR32_CORE_COMPARE_IRQ, CONFIG_OS_SCHEDULER_IRQ_PRIORITY);
		Set_system_register(AVR32_COMPARE, cpu_freq_hz / CONFIG_OS_TICK_HZ);
		Set_system_register(AVR32_COUNT, 0);
		cpu_irq_enable();
	}

	static inline void os_scheduler_clear_int(void) {
		Set_system_register(AVR32_COMPARE, Get_system_register(AVR32_COMPARE));
	}

	#define OS_SCHEDULER_IRQ_GROUP AVR32_CORE_IRQ_GROUP

#elif CONFIG_OS_SCHEDULER_TYPE == CONFIG_OS_SCHEDULER_USE_RTC
	/* Setup functions to use the RTC interrupt
	 */
	#include "rtc.h"

	void os_setup_scheduler(uint32_t pba_freq_hz)
	{
		cpu_irq_disable();
		irq_initialize_vectors();
		irq_register_handler((__int_handler) os_switch_context_int_handler,
				AVR32_RTC_IRQ, CONFIG_OS_SCHEDULER_IRQ_PRIORITY);
		// fRTC = 115000 / 4 = 28750
		rtc_init(&AVR32_RTC, RTC_OSC_RC, 1);
		rtc_set_top_value(&AVR32_RTC, 28750 / CONFIG_OS_TICK_HZ);
		rtc_enable_interrupt(&AVR32_RTC);
		rtc_enable(&AVR32_RTC);
		cpu_irq_enable();
	}

	static inline void os_scheduler_clear_int(void) {
		(&AVR32_RTC)->icr = AVR32_RTC_ICR_TOPI_MASK;
	}

	#define OS_SCHEDULER_IRQ_GROUP AVR32_RTC_IRQ_GROUP

#elif CONFIG_OS_SCHEDULER_TYPE == CONFIG_OS_SCHEDULER_USE_TC
	/* Setup functions to use the TC interrupt
	 */
	#include "tc.h"

	#define OS_SCHEDULER_TC_IRQ AVR32_TC_IRQ0

	void os_setup_scheduler(uint32_t pba_freq_hz)
	{
		volatile avr32_tc_t *tc = &AVR32_TC;
		tc_waveform_opt_t waveform_opt = {
			.channel = CONFIG_OS_SCHEDULER_TC_CHANNEL,
			.bswtrg = TC_EVT_EFFECT_NOOP,
			.beevt = TC_EVT_EFFECT_NOOP,
			.bcpc = TC_EVT_EFFECT_NOOP,
			.bcpb = TC_EVT_EFFECT_NOOP,
			.aswtrg = TC_EVT_EFFECT_NOOP,
			.aeevt = TC_EVT_EFFECT_NOOP,
			.acpc = TC_EVT_EFFECT_NOOP,
			.acpa = TC_EVT_EFFECT_NOOP,
			.wavsel = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
			.enetrg = false,
			.eevt = 0,
			.eevtedg = TC_SEL_NO_EDGE,
			.cpcdis = false,
			.cpcstop = false,
			.burst = false,
			.clki = false,
			.tcclks = TC_CLOCK_SOURCE_TC3
		};
		tc_interrupt_t tc_interrupt = {
			.etrgs = 0,
			.ldrbs = 0,
			.ldras = 0,
			.cpcs = 1,
			.cpbs = 0,
			.cpas = 0,
			.lovrs = 0,
			.covfs = 0,
		};
		cpu_irq_disable();
		irq_initialize_vectors();
		irq_register_handler((__int_handler) os_switch_context_int_handler,
				OS_SCHEDULER_TC_IRQ, CONFIG_OS_SCHEDULER_IRQ_PRIORITY);
		tc_init_waveform(tc, &waveform_opt);
		tc_write_rc(tc, CONFIG_OS_SCHEDULER_TC_CHANNEL,
				(pba_freq_hz + 4 * CONFIG_OS_TICK_HZ) / (8 * CONFIG_OS_TICK_HZ));
		tc_configure_interrupts(tc, CONFIG_OS_SCHEDULER_TC_CHANNEL,
				&tc_interrupt);
		tc_start(tc, CONFIG_OS_SCHEDULER_TC_CHANNEL);
		cpu_irq_enable();
	}

	static inline void os_scheduler_clear_int(void) {
		AVR32_TC.channel[CONFIG_OS_SCHEDULER_TC_CHANNEL].sr;
	}

	#define OS_SCHEDULER_IRQ_GROUP AVR32_TC_IRQ_GROUP

#else

	static inline void os_scheduler_clear_int(void) {
	}
	#define OS_SCHEDULER_IRQ_GROUP 0
#endif

#if __GNUC__
__attribute__((__naked__))
#elif __ICCAVR32__
	#pragma shadow_registers = full
#endif
ISR(os_switch_context_int_handler, OS_SCHEDULER_IRQ_GROUP,
		CONFIG_OS_SCHEDULER_IRQ_PRIORITY)
{
	extern struct os_process *__os_current_process;

	__asm__ __volatile__ (
		// Save context
        	"pushm r0-r7\n\t"

		// Save the stack pointer
		"mov r0, __os_current_process\n\t"
		"ld.w r1, r0[0]\n\t"
		"st.w r1[0], sp\n\t"
	);

	__HOOK_OS_STATISTICS_SWITCH_CONTEXT_TICK_HANDLER_START(20);

	// Clear the interrupt flag
	os_scheduler_clear_int();
	__os_switch_context_int_handler_hook();

	__asm__ __volatile__ (
		// Update the stack pointer
		"ld.w sp, r12\n\t"
	);

	__HOOK_OS_STATISTICS_SWITCH_CONTEXT_TICK_HANDLER_STOP(18);

	__asm__ __volatile__ (
		// Restore context
		"popm r0-r7\n\t"
		"rete\n\t"
	);
#if __ICCAVR32__
	#pragma diag_suppress=Pe174
#endif
	__os_current_process = __os_current_process;
#if __ICCAVR32__
	#pragma diag_default=Pe174
#endif
}

#if __GNUC__
__attribute__((__naked__))
void _os_switch_context(void);
void _os_switch_context(void)
#elif __ICCAVR32__
	#pragma shadow_registers = full
	#pragma exception=0x100,0
__exception void _os_switch_context(void)
#endif
{
	extern struct os_process *__os_current_process;

	__asm__ __volatile__ (
#if CONFIG_OS_USE_SW_INTERRUPTS == true
		// Test if we need to save the context
		"cp.w r8, 0\n\t"
	#if __ICCAVR32__
		"brne bypass_context_save:C\n\t"
	#else
		"brne bypass_context_save\n\t"
	#endif
#endif
		// Save context
		"pushm r10-r12, lr\n\t" // r10-r12, lr

		"ld.d r10, sp[4*4]\n\t"
		"st.d sp[4*4], r8\n\t" // r8-r9
		"st.d --sp, r10\n\t" // pc, sr
        	"pushm r0-r7\n\t" // r0-r7

		// Save the stack pointer
		"mov r0, __os_current_process\n\t"
		"ld.w r1, r0[0]\n\t"
		"st.w r1[0], sp\n"

		// Bypass the context saving
		"bypass_context_save:"
	);

	__HOOK_OS_STATISTICS_SWITCH_CONTEXT_START(34);

	__os_switch_context_hook();

	__asm__ __volatile__ (
		// Update the stack pointer
		"ld.w sp, r12\n\t"
	);

	__HOOK_OS_STATISTICS_SWITCH_CONTEXT_STOP(32);

	__asm__ __volatile__ (
		// Restore context
		"popm r0-r7\n\t" // r0-r7
		"ld.d r6, sp++\n\t" // r6 = sr; r7 = pc
		"popm lr, r8-r12\n\t" // lr, r8-r12
		"st.d --sp, r6\n\t" // stack sr and pc
		"ld.d r6, sp[-14*4]\n\t" // restore r6-r7

		// Restore context
		"rets\n\t"
	);

#if __ICCAVR32__
	#pragma diag_suppress=Pe174
#endif
	__os_current_process = __os_current_process;
#if __ICCAVR32__
	#pragma diag_default=Pe174
#endif
}

bool os_process_context_load(struct os_process *proc, os_proc_ptr_t proc_ptr,
		os_ptr_t args)
{
	PUSH(proc, 0); // R8
	PUSH(proc, 0); // R9
	PUSH(proc, 0); // R10
	PUSH(proc, 0); // R11
	PUSH(proc, args); // R12
	PUSH(proc, 0); // LR
	PUSH(proc, (void *) proc_ptr); // PC
	PUSH(proc, CONFIG_OS_DEFAULT_SR_VALUE);
	PUSH(proc, 0); // R0
	PUSH(proc, 0); // R1
	PUSH(proc, 0); // R2
	PUSH(proc, 0); // R3
	PUSH(proc, 0); // R4
	PUSH(proc, 0); // R5
	PUSH(proc, 0); // R6
	PUSH(proc, 0); // R7

	return true;
}
