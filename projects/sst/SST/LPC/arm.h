#ifndef ARM_H
#define ARM_H

/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact. 
 * 
 ******************************************************************************/

//
// (S)CPSR Register defines
//
 
#define CPSR_IRQ_DISABLE        0x80
#define CPSR_FIQ_DISABLE        0x40
#define CPSR_THUMB_ENABLE       0x20
#define CPSR_USER_MODE          0x10
#define CPSR_FIQ_MODE           0x11
#define CPSR_IRQ_MODE           0x12
#define CPSR_SUPERVISOR_MODE    0x13
#define CPSR_UNDEF_MODE         0x1B
#define CPSR_SYSTEM_MODE        0x1F

#define CPSR_MODE_BITS          0x1F


#ifndef __ASSEMBLER__

/*
 Macro for enabling interrupts, moving to System mode and relevant stack operations
 Nested Interrupts Entry
    Copy SPSR_irq to LR 
    Save SPSR_irq
    Enable IRQ (Sys Mode)
    Save LR
*/ 
#define ENABLE_NEST_INTS()  \
    asm volatile (      \
        "mrs     lr, SPSR       \n\t"\
        "stmfd   sp!, {lr}      \n\t"\
        "msr     CPSR_c, #0x9F  \n\t"\
        "stmfd   sp!, {lr}      \n\t"\
        "msr     CPSR_c, #0x1F  \n\t"\
        )

/*
 Macro for disabling interrupts, switching back to IRQ and relevant stack operations
 Nested Interrupts Exit
    Restore LR
    Disable IRQ (IRQ Mode)
    Restore SPSR_irq to LR
    Copy LR to SPSR_irq
*/
#define DISABLE_NEST_INTS() \
    asm volatile ( \
        "ldmfd   sp!, {lr}      \n\t"\
        "msr     CPSR_c, #0x92  \n\t"\
        "ldmfd   sp!, {lr}      \n\t"\
        "msr     SPSR_cxsf, lr  \n\t"\
        )

#define DISABLE_INTS_SAVE(_old_)\
    asm volatile (		\
        "mrs %0,CPSR        \n\t"\
        "mrs r4,CPSR		\n\t"\
        "orr r4,r4,#0x80	\n\t"\
        "msr CPSR_c,r4		\n\t"\
        : "=r"(_old_)		\
        :			\
        : "r4"			\
        );

#define DISABLE_INTS()\
    asm volatile (      \
        "mrs r4,CPSR        \n\t"\
        "orr r4,r4,#0x80    \n\t"\
        "msr CPSR_c,r4      \n\t"\
        :               \
        :               \
        : "r4"          \
        );

#define ENABLE_INTS()	\
    asm volatile (		\
        "mrs r3,CPSR		\n\t"\
        "bic r3,r3,#0x80	\n\t"\
        "msr CPSR_c,r3		\n\t"\
        :			\
        :			\
        : "r3"			\
        );

#define RESTORE_INTS(_old_)\
    asm volatile (		\
        "mrs r3,CPSR		\n\t"\
        "and r4,%0,#0x80	\n\t"\
        "bic r3,r3,#0x80	\n\t"\
        "orr r3,r3,r4		\n\t"\
        "msr CPSR_c,r3		\n\t"\
        :			\
        : "r"(_old_)		\
        : "r3", "r4"		\
        );

#define QUERY_INTS(_old_)	\
    asm volatile (		\
        "mrs r4,CPSR		\n\t"\
        "and r4,r4,#0x80	\n\t"\
        "eor %0,r4,#0x80	\n\t"\
        : "=r"(_old_)		\
        :			\
        : "r4"			\
        );

#endif	// __ASSEMBLER__

#endif // ARM_H
