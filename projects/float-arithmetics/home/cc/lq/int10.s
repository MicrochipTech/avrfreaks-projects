/* This is file CC\LQ\INT10.S */
	.data
	.comm   _int10_bp_save_area,4
	.text
	.align  4
	.globl  _int10__FP4REGS

// Invication : 
// union REGS inregs
// int10 (&inregs) ;
// where inregs is a union data structure, which contains all the registers
// used by the 8086 CPU, as defined in <dos.h>
// Offset  Len  long register
//  0      4    edi
//  4      4    esi
//  8      4    ebp
// 12      4    cflag
// 16      4    ebx
// 20      4    edx
// 24      4    ecx
// 28      4    eax
// 32      2    elags
//
_int10__FP4REGS:
	pushl   %ebp				// save callers Frame pointer
	movl    %esp,%ebp			// setup own frame pointer
								// BP+ 0.. 3 = old BP
								// BP+ 4.. 7 = return addresse
								// BP+ 8..11 = Parameter
	movl    %ebp,_int10_bp_save_area

	pushl   %ebx				// save all registers of caller
	pushl   %ecx
	pushl   %edx
	pushl   %esi
	pushl   %edi

	movl    8(%ebp),%edi	// get start adresse of union REGS

	pushl     (%edi)        // EDI
	pushl    8(%edi)        // EBP
	pushl    4(%edi)        // ESI
//	pushl   12(%edi)        // cflags
	pushl   16(%edi)        // EBX
	pushl   20(%edi)        // EDX
	pushl   24(%edi)        // ECX
	pushl   28(%edi)        // EAX
//	pushl	32(%edi)		// eflags

//	popl    %eflags
	popl    %eax
	popl    %ecx
	popl	%edx
	popl    %ebx
//	popl    %cflags
	popl	%esi
	popl	%ebp
	popl	%edi

// perform call with registers, defined by caller
	int     $0x10
// push result registers into stack

	pushl   %ebp
	pushl   %edi
	pushl   %esi
//	pushl   %cflags
	pushl   %ebx
	pushl   %edx
	pushl   %ecx
	pushl   %eax
//	pushl   %eflags
// All return registers are pushed into stack

	movl    _int10_bp_save_area,%ebp

	movl    8(%ebp),%edi	// get start adresse of union REGS

// restore them from stack into register area
//	popl	32(%edi)		// eflags
	popl   28(%edi)        // EAX
	popl   24(%edi)        // ECX
	popl   20(%edi)        // EDX
	popl   16(%edi)        // EBX
//	popl   12(%edi)        // cflags
	popl    4(%edi)        // ESI
	popl     (%edi)        // EDI
	popl    8(%edi)        // EBP

	popl    %edi			// restore all registers of caller
	popl    %esi
	popl    %edx
	popl    %ecx
	popl    %ebx

	popl	%ebp

	ret

	.align  4
	.data
	.text
