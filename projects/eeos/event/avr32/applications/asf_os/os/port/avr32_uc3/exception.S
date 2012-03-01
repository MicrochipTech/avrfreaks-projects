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

#if !__AVR32_UC__ && !__AVR32_AP__
  #error Implementation for the AVR32 architecture only.
#endif


#include <avr32/io.h>


//! @{
//! \verbatim


.section  .exception, "ax", @progbits


// Start of Exception Vector Table.

/*
 * EVBA must be aligned with a power of two strictly greater than the 
 * EVBA-relative offset of the last vector.
 */
.balign 0x200

// Export symbol.
.global _evba
.type _evba, @function
_evba:

	.org  0x000
	// Unrecoverable Exception.
_handle_Unrecoverable_Exception:
	rjmp $

	.org  0x004
	// TLB Multiple Hit.
_handle_TLB_Multiple_Hit:
	rjmp $

	.org  0x008
	// Bus Error Data Fetch.
_handle_Bus_Error_Data_Fetch:
	rjmp $

	.org  0x00C
	// Bus Error Instruction Fetch.
_handle_Bus_Error_Instruction_Fetch:
	rjmp $

	.org  0x010
	// NMI.
_handle_NMI:
	rjmp $

	.org  0x014
	// Instruction Address.
_handle_Instruction_Address:
	rjmp $

	.org  0x018
	// ITLB Protection.
_handle_ITLB_Protection:
	rjmp $

	.org  0x01C
	// Breakpoint.
_handle_Breakpoint:
	rjmp $

	.org  0x020
	// Illegal Opcode.
_handle_Illegal_Opcode:
	rjmp $

	.org  0x024
	// Unimplemented Instruction.
_handle_Unimplemented_Instruction:
	rjmp $

	.org  0x028
	// Privilege Violation.
_handle_Privilege_Violation:
	rjmp $

	.org  0x02C
	// Floating-Point: UNUSED IN AVR UC3.
_handle_Floating_Point:
	rjmp $

	.org  0x030
	// Coprocessor Absent: UNUSED IN AVR UC3.
_handle_Coprocessor_Absent:
	rjmp $

	.org  0x034
	// Data Address (Read).
_handle_Data_Address_Read:
	rjmp $

	.org  0x038
	// Data Address (Write).
_handle_Data_Address_Write:
	rjmp $

	.org  0x03C
	// DTLB Protection (Read).
_handle_DTLB_Protection_Read:
	rjmp $

	.org  0x040
	// DTLB Protection (Write).
_handle_DTLB_Protection_Write:
	rjmp $

	.org  0x044
	// DTLB Modified: UNUSED IN AVR32UC.
_handle_DTLB_Modified:
	rjmp $

	.org  0x050
	// ITLB Miss.
_handle_ITLB_Miss:
	rjmp $

	.org  0x060
	// DTLB Miss (Read).
_handle_DTLB_Miss_Read:
	rjmp $

	.org  0x070
	// DTLB Miss (Write).
_handle_DTLB_Miss_Write:
	rjmp $

	.org  0x100
	// Supervisor Call.
_handle_Supervisor_Call:
	lda.w   pc, _os_switch_context


/*
 * Interrupt support.
 * The interrupt controller must provide the offset address relative to EVBA.
 * Important note:
 * All interrupts call a C function named _get_interrupt_handler.
 * This function will read group and interrupt line number to then return in
 *R12 a pointer to a user-provided interrupt handler.
 */

.balign 4

.irp    priority, 0, 1, 2, 3
.global _int\priority
.type   _int\priority, @function
_int\priority:
	/*
	 * R8-R12, LR, PC and SR are automatically pushed onto the system stack
	 * by the CPU upon interrupt entry. No other register is saved by 
	 * hardware.
	 */
	// Pass the int_level parameter to the _get_interrupt_handler function.
	mov     r12, \priority
	call    _get_interrupt_handler
	// Get the pointer to the interrupt handler returned by the function.
	cp.w    r12, 0
	/* 
	 * If this was not a spurious interrupt (R12 != NULL), jump to the 
	 * handler.
	 */
	movne   pc, r12
	/*
	 * If this was a spurious interrupt (R12 == NULL), return from event 
	 * handler.
	 */
	rete
.endr


//! \endverbatim
//! @}
