/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact. 
 * 
 ******************************************************************************/

    .global main
    .global _start
	
    /* Symbols defined by the linker script */
    .global _etext
    .global _data
    .global _edata
    .global _bss
    .global _ebss
    
    .text
    .arm

/* ----------------------------------------------------------------
 * Exception vectors
 * ----------------------------------------------------------------
 */
_start:

Vectors:
    b reset /* reset */
    b loop  /* undefined instruction */
    b loop  /* software interrupt */
    b loop  /* prefetch abort */
    b loop  /* data abort */
    nop     /* reserved for the bootloader checksum */
	ldr	pc, [pc, #-0x0FF0]		/* vector from VIC */
	b loop	/* FIQ */

/* ----------------------------------------------------------------
 * LPC21xx PLL setup
 * ----------------------------------------------------------------
 */
reset:
/* ----------------------------------------------------------------
 * LPC21xx PLL setup
 * ----------------------------------------------------------------
 */
    /* Use r0 for indirect addressing */
    ldr	r0, PLLBASE

    /* PLLCFG = PLLCFG_VALUE */
    mov	r3, #PLLCFG_VALUE
    str r3, [r0, #PLLCFG_OFFSET]

    /* PLLCON = PLLCON_PLLE */
    mov	r3, #PLLCON_PLLE
    str r3, [r0, #PLLCON_OFFSET]

    /* PLLFEED = PLLFEED1, PLLFEED2 */
    mov	r1, #PLLFEED1
    mov	r2, #PLLFEED2
    str r1, [r0, #PLLFEED_OFFSET]
    str r2, [r0, #PLLFEED_OFFSET]

    /* 	while ((PLLSTAT & PLLSTAT_PLOCK) == 0); */
pll_loop:
    ldr r3, [r0, #PLLSTAT_OFFSET]
    tst	r3, #PLLSTAT_PLOCK
    beq	pll_loop

    /* PLLCON = PLLCON_PLLC|PLLCON_PLLE */
    mov	r3, #(PLLCON_PLLC|PLLCON_PLLE)
    str r3, [r0, #PLLCON_OFFSET]

    /* PLLFEED = PLLFEED1, PLLFEED2 */
    str r1, [r0, #PLLFEED_OFFSET]
    str r2, [r0, #PLLFEED_OFFSET]

/* ----------------------------------------------------------------
 * LPC21xx MAM setup
 * ----------------------------------------------------------------
 */
mam_init:
    /* Use r0 for indirect addressing */
    ldr	r0, MAMBASE

    /* MAMTIM = MAMTIM_VALUE */
    mov	r1, #MAMTIM_VALUE
    str r1, [r0, #MAMTIM_OFFSET]

    /* MAMCR = MAMCR_VALUE */
    mov	r1, #MAMCR_VALUE
    str r1, [r0, #MAMCR_OFFSET]

/* ----------------------------------------------------------------
 * LPC21xx stacks setup
 * ----------------------------------------------------------------
 */
stacks_init:
    ldr r0, STACK_START

    /* Undefined mode stack */
    msr CPSR_c, #UND_MODE|IRQ_DISABLE|FIQ_DISABLE
    mov sp, r0
    sub r0, r0, #UND_STACK_SIZE

    /* Abort mode stack */
    msr CPSR_c, #ABT_MODE|IRQ_DISABLE|FIQ_DISABLE
    mov sp, r0
    sub r0, r0, #ABT_STACK_SIZE

    /* FIQ mode stack */
    msr CPSR_c, #FIQ_MODE|IRQ_DISABLE|FIQ_DISABLE
    mov sp, r0
    sub r0, r0, #FIQ_STACK_SIZE

    /* IRQ mode stack */
    msr CPSR_c, #IRQ_MODE|IRQ_DISABLE|FIQ_DISABLE
    mov sp, r0
    sub r0, r0, #IRQ_STACK_SIZE

    /* Supervisor mode stack */
    msr CPSR_c, #SVC_MODE|IRQ_DISABLE|FIQ_DISABLE
    mov sp, r0
    sub r0, r0, #SVC_STACK_SIZE

    /* System mode stack */
#if 1
    msr CPSR_c, #SYS_MODE|IRQ_DISABLE|FIQ_DISABLE
#else
    msr CPSR_c, #SYS_MODE
#endif
    mov sp, r0

    /* Leave the processor in system mode */

/* ----------------------------------------------------------------
 * C runtime setup
 * ----------------------------------------------------------------
 */
runtime_init:
    /* Copy .data */
    ldr r0, data_source
    ldr r1, data_start
    ldr r2, data_end
copy_data:
    cmp   r1, r2
    ldrne r3, [r0], #4
    strne r3, [r1], #4
    bne   copy_data
	
    /* Clear .bss */
    mov r0, #0
    ldr r1, bss_start
    ldr r2, bss_end
clear_bss:
    cmp   r1, r2
    strne r0, [r1], #4
    bne   clear_bss

	// Run application in THUMB or ARM mode (depends on the _main address)
	// Jump to main function
	ldr	r0, =main
	bx	r0

/* Catch return from main */
loop:   b   loop

/* ----------------------------------------------------------------
 * 32-bit constants (and storage)
 * ----------------------------------------------------------------
 *
 * These 32-bit constants are used in ldr statements.
 */

/* LPC SRAM starts at 0x40000000, and there is 16Kb = 4000h */ 
STACK_START:	.word   0x40004000
PLLBASE:        .word   0xE01FC080
MAMBASE:        .word   0xE01FC000

/* Linker symbols */
data_source:    .word   _etext
data_start:     .word   _data
data_end:       .word   _edata
bss_start:      .word   _bss
bss_end:        .word   _ebss

/* ----------------------------------------------------------------
 * 8-bit constants
 * ----------------------------------------------------------------
 *
 * These 8-bit constants are used as immediate values and offsets.
 */

/* PLL configuration */
    .equ PLLCON_OFFSET,   0x0
    .equ PLLCFG_OFFSET,   0x4
    .equ PLLSTAT_OFFSET,  0x8
    .equ PLLFEED_OFFSET,  0xC

    .equ PLLCON_PLLE,    (1 << 0)
    .equ PLLCON_PLLC,    (1 << 1)
    .equ PLLSTAT_PLOCK,  (1 << 10)
    .equ PLLFEED1,        0xAA
    .equ PLLFEED2,        0x55

    .equ PLLCFG_VALUE,    0x24

/* MAM configuration */
    .equ MAMCR_OFFSET,   0x0
    .equ MAMTIM_OFFSET,  0x4

    .equ MAMCR_VALUE,    0x2  /* fully enabled */
    .equ MAMTIM_VALUE,   0x4  /* fetch cycles  */

/* Stack configuration */
    /* Processor modes (see pA2-11 ARM-ARM) */
    .equ FIQ_MODE,       0x11
    .equ IRQ_MODE,       0x12
    .equ SVC_MODE,       0x13  /* reset mode */
    .equ ABT_MODE,       0x17
    .equ UND_MODE,       0x1B
    .equ SYS_MODE,       0x1F

    /* Stack sizes */
    .equ FIQ_STACK_SIZE, 0x00000010
    .equ IRQ_STACK_SIZE, 0x00000100
    .equ SVC_STACK_SIZE, 0x00000004
    .equ ABT_STACK_SIZE, 0x00000004
    .equ UND_STACK_SIZE, 0x00000004
    .equ SYS_STACK_SIZE, 0x00000800

    /* CPSR interrupt disable bits */
    .equ IRQ_DISABLE,    (1 << 7)
    .equ FIQ_DISABLE,    (1 << 6)

    .end
