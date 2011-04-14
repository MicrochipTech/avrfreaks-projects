;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                                          AVR Specific code
;                                           (AVR-GCC 3.x)
;
;
; File         : OS_CPU_A.ASM
; By           : Ole Saether
; Port Version : V1.01
;
; AVR-GCC port version : 1.0 	2001-04-02 modified/ported to avr-gcc by Jesper Hansen (jesperh@telia.com)
;
; Modifications by Julius Luukko 2003-02-19 (Julius.Luukko@lut.fi):
;
; - Minor modification to OSTickISR's declaration to global and addition of #include <avr/io.h> 
;   in order to work with gcc 3.3.
; - Moved 'sei' from the beginning of OSTickISR after the increasing of OSIntNesting (as the book suggests)
; - Changed the order of reloading the timer counter and calling of OSIntExit
; - Changed OSIntCtxSw to the way it is supposed to be with OS_VERSION >= 251
; - Changed OSTickISR so that is saves the stack pointer into the current task's OS_TCB if OSTickISR is the
;   first level ISR
; - os_cfg.h must be included (definitions of CPU_CLOCK_HZ and OS_TICKS_PER_SEC)
; - avr/io.h is included to get SIG_OVERFLOW0 to produce the code to the interrupt vector table
;
; Modifications by Julius Luukko 2003-03-06 (Julius.Luukko@lut.fi):
;
; - RAMPZ is also saved to the stack
;
; Modifications by Julius Luukko 2003-03-18 (Julius.Luukko@lut.fi):
;
; - RAMPZ and SREG were popped out of the stack in the wrong order! Corrected this.
; - Divided macros PUSHRS and POPRS into to macros PUSHRS, PUSHSREG, POPRS and POPSREG. Additionally, 
; in ISRs macro called PUSHSREGISR must be used. It sets the I bit i SREG, since its always unset, when
; ISR is entered. It must be, however, set to enable interrupts when the task is run again.
; - RCALLs changed to CALLs. This allows the code of called functions to be further away from the calls.
;   (fixes the "relocating truncated to fit" error from the linker)
; 
;
; Modifications by Julius Luukko 2003-03-19 (Julius.Luukko@lut.fi):
;
; - timer interrupt is changed so that the timer counter register is updated right after _not_first_int
;   (was AFTER the call to OSIntExit!!)
;
; Modifications by Julius Luukko 2003-06-24 (Julius.Luukko@lut.fi):
;
; - I/O port addressing is now done using the macro _SFR_IO_ADDR from avr-libc and the address
; definitions are removed from here
; - RAMPZ is pushed and popped only if it is defined, i.e. with chips that have it
;
; Modifications by Julius Luukko 2003-07-21 (Julius.Luukko@lut.fi) for V2.70
;
; - OSTaskSwHook is not called if OS_TASK_SW_HOOK_EN == 0
; - defines typedef to ; so that os_cfg_r.h can be used more easily as a starting point for
;   the application specific os_cfg.h
;
;********************************************************************************************************

;********************************************************************************************************
;                                      C PREPROCESSOR DIRECTIVES
;********************************************************************************************************

#include <avr/io.h>

#define OS_CPU_A
#include "os_cpu.h"
#define typedef	; 
#include "os_cfg.h"

#ifndef OS_TASK_SW_HOOK_EN
#define OS_TASK_SW_HOOK_EN 1
#endif
	
;********************************************************************************************************
;                                          PUBLIC DECLARATIONS
;********************************************************************************************************

                .global OSStartHighRdy
                .global OSCtxSw
                .global OSIntCtxSw
                .global	OSTickISR

;********************************************************************************************************
;                                         EXTERNAL DECLARATIONS
;********************************************************************************************************

                .extern	OSIntExit
                .extern	OSIntNesting
                .extern	OSPrioCur
                .extern	OSPrioHighRdy
                .extern	OSRunning
#if OS_TASK_SW_HOOK_EN > 0
                .extern	OSTaskSwHook
#endif
                .extern	OSTCBCur
                .extern	OSTCBHighRdy
                .extern	OSTimeTick

;********************************************************************************************************
;                                         MACROS
;********************************************************************************************************

; Push all registers and the status register	
.macro	PUSHRS

                push	r0
                push	r1
                push	r2
                push	r3
                push	r4
                push	r5
                push	r6
                push	r7
                push	r8
                push	r9
                push	r10
                push	r11
                push	r12
                push	r13
                push	r14
                push	r15
                push	r16
                push	r17
                push	r18
                push	r19
                push	r20
                push	r21
                push	r22
                push	r23
                push	r24
                push	r25
                push	r26
                push	r27
                push	r28
                push	r29
                push	r30
                push	r31
#ifdef RAMPZ
                in      r16,_SFR_IO_ADDR(RAMPZ)
                push	r16
#endif
.endm

; Pop all registers and the status registers
.macro	POPRS

#ifdef RAMPZ
                pop     r16
                out     _SFR_IO_ADDR(RAMPZ),r16
#endif
                pop     r31
                pop     r30
                pop     r29
                pop     r28
                pop     r27
                pop     r26
                pop     r25
                pop     r24
                pop     r23
                pop     r22
                pop     r21
                pop     r20
                pop     r19
                pop     r18
                pop     r17
                pop     r16
                pop     r15
                pop     r14
                pop     r13
                pop     r12
                pop     r11
                pop     r10
                pop     r9
                pop     r8
                pop     r7
                pop     r6
                pop     r5
                pop     r4
                pop     r3
                pop     r2
                pop     r1
                pop     r0

.endm

.macro	POPSREG

                pop     r16
                out     _SFR_IO_ADDR(SREG),r16

.endm

.macro	PUSHSREG

                in      r16,_SFR_IO_ADDR(SREG)
                push	r16

.endm

.macro	PUSHSREGISR

                in      r16,_SFR_IO_ADDR(SREG)
                sbr     r16,0x80
                push	r16

.endm


                	.text
                	.section	.text
			

;********************************************************************************************************
;                               START HIGHEST PRIORITY TASK READY-TO-RUN
;
; Description : This function is called by OSStart() to start the highest priority task that was created
;               by your application before calling OSStart().
;
; Note(s)     : 1) The (data)stack frame is assumed to look as follows:
;
;                  OSTCBHighRdy->OSTCBStkPtr --> LSB of (return) stack pointer           (Low memory)
;                                                SPH of (return) stack pointer
;                                                Flags to load in status register
;                                                R31
;                                                R30
;                                                R7
;                                                .
;                                                .
;                                                .
;                                                R0
;                                                PCH
;                                                PCL                                     (High memory)
;
;                  where the stack pointer points to the task start address.
;
;
;               2) OSStartHighRdy() MUST:
;                      a) Call OSTaskSwHook() then,
;                      b) Set OSRunning to TRUE,
;                      c) Switch to the highest priority task.
;********************************************************************************************************

OSStartHighRdy: 
#if OS_TASK_SW_HOOK_EN > 0
	        call    OSTaskSwHook                ; Invoke user defined context switch hook
#endif
                lds     R16,OSRunning               ; Indicate that we are multitasking
                inc     R16                         ;
                sts     OSRunning,R16               ;

                lds     R30,OSTCBHighRdy            ; Let Z point to TCB of highest priority task
                lds     R31,OSTCBHighRdy+1          ; ready to run

                ld      R28,Z+                      ; Load stack L pointer
                out     _SFR_IO_ADDR(SPL),R28
                ld      R29,Z+                      ;
                out     _SFR_IO_ADDR(SPH),R29

                POPSREG                             ; Pop the status register
                POPRS                               ; Pop all registers
                ret                                 ; Start task

;********************************************************************************************************
;                                       TASK LEVEL CONTEXT SWITCH
;
; Description : This function is called when a task makes a higher priority task ready-to-run.
;
; Note(s)     : 1) Upon entry,
;                  OSTCBCur     points to the OS_TCB of the task to suspend
;                  OSTCBHighRdy points to the OS_TCB of the task to resume
;
;               2) The stack frame of the task to suspend looks as follows:
;
;                                       SP+0 --> LSB of task code address
;                                         +1     MSB of task code address                (High memory)
;
;               3) The saved context of the task to resume looks as follows:
;
;                  OSTCBHighRdy->OSTCBStkPtr --> Flags to load in status register (Low memory)
;                                                R31
;                                                R30
;                                                R7
;                                                .
;                                                .
;                                                .
;                                                R0
;                                                PCH
;                                                PCL                                     (High memory)
;********************************************************************************************************

OSCtxSw:        PUSHRS                              ; Save current tasks context
                PUSHSREG                            ; Save SREG

                lds     R30,OSTCBCur                ; Z = OSTCBCur->OSTCBStkPtr
                lds     R31,OSTCBCur+1              ;

                in      r28,_SFR_IO_ADDR(SPL)
                st      Z+,R28                      ; Save Y (R29:R28) pointer
                in      r29,_SFR_IO_ADDR(SPH)
                st      Z+,R29                      ;

#if OS_TASK_SW_HOOK_EN > 0
                call    OSTaskSwHook                ; Call user defined task switch hook
#endif

                lds     R16,OSPrioHighRdy           ; OSPrioCur = OSPrioHighRdy
                sts     OSPrioCur,R16

                lds     R30,OSTCBHighRdy            ; Let Z point to TCB of highest priority task
                lds     R31,OSTCBHighRdy+1          ; ready to run
                sts     OSTCBCur,R30                ; OSTCBCur = OSTCBHighRdy
                sts     OSTCBCur+1,R31              ;

                ld      R28,Z+                      ; Restore Y pointer
                out     _SFR_IO_ADDR(SPL),R28
                ld      R29,Z+                      ;
                out     _SFR_IO_ADDR(SPH),R29

                POPSREG                             ; Restore the status register
                POPRS                               ; Restore all registers
                ret


;*********************************************************************************************************
;                                INTERRUPT LEVEL CONTEXT SWITCH
;
; Description : This function is called by OSIntExit() to perform a context switch to a task that has
;               been made ready-to-run by an ISR.
;
; Note(s)     : 1) Upon entry,
;                  OSTCBCur     points to the OS_TCB of the task to suspend
;                  OSTCBHighRdy points to the OS_TCB of the task to resume
;
;               2) The stack frame of the task to suspend looks as follows:
;
;                  OSTCBCur->OSTCBStkPtr ------> Flags to load in status register        (Low memory)
;                                                R31
;                                                R30
;                                                R27
;                                                .
;                                                .
;                                                R0
;                                                PCH
;                                                PCL                                     (High memory)
;
;               3) The saved context of the task to resume looks as follows:
;
;                  OSTCBHighRdy->OSTCBStkPtr --> Flags to load in status register         (Low memory)
;                                                R31
;                                                R30
;                                                R7
;                                                .
;                                                .
;                                                .
;                                                R0                                      (High memory)
;*********************************************************************************************************

OSIntCtxSw:     
;*********************************************************************************************************
; You should enable the lines between #if 0 and #endif only if you use uC/OS-II version 2.51 or below!
; (Unfortunately OS_VERSION is not defined here (how to do it elegantly?)
;*********************************************************************************************************
#if 0
                in      R28,_SFR_IO_ADDR(SPL)       ; Z = SP
                in      R29,_SFR_IO_ADDR(SPH)

#if      OS_CRITICAL_METHOD == 1
                adiw    R28,4                       ; Use if OS_CRITICAL_METHOD is 1, see OS_CPU.H
#endif
#if      OS_CRITICAL_METHOD == 2
                adiw    R28,5                       ; Use if OS_CRITICAL_METHOD is 2, see OS_CPU.H
#endif

                lds     R30,OSTCBCur                ; Z = OSTCBCur->OSTCBStkPtr
                lds     R31,OSTCBCur+1              ;
                st      Z+,R28                      ; Save Y pointer
                st      Z+,R29                      ;
#endif
;*********************************************************************************************************

#if OS_TASK_SW_HOOK_EN > 0
                call    OSTaskSwHook                ; Call user defined task switch hook
#endif
                lds     R16,OSPrioHighRdy           ; OSPrioCur = OSPrioHighRdy
                sts     OSPrioCur,R16               ;

                lds     R30,OSTCBHighRdy            ; Z = OSTCBHighRdy->OSTCBStkPtr
                lds     R31,OSTCBHighRdy+1          ;
                sts     OSTCBCur,R30                ; OSTCBCur = OSTCBHighRdy
                sts     OSTCBCur+1,R31              ;

                ld      R28,Z+                      ; Restore Y pointer
                out     _SFR_IO_ADDR(SPL),R28
                ld      R29,Z+                      ;
                out     _SFR_IO_ADDR(SPH),R29

                POPSREG                             ; Restore the status register
                POPRS                               ; Restore all registers
                ret

;********************************************************************************************************
;                                           SYSTEM TICK ISR
;
; Description : This function is the ISR used to notify uC/OS-II that a system tick has occurred.
;
; (Julius Luukko/2003-02-14): Changed the name of vector to SIG_OVERFLOW0. In order to the corresponding 
; interrupt vector to be set to point to SIG_OVERFLOW0, one has to also #include <avr/io.h> in this file. 
; Note that no error message or warning will be given, if the include is not present, the interupt just
; won't get vectored.
;
;********************************************************************************************************
.global	SIG_OVERFLOW0
SIG_OVERFLOW0:  PUSHRS                              ; Save all registers
                PUSHSREGISR                         ; Save SREG with I bit set!

                lds     R16,OSIntNesting            ; Notify uC/OS-II of ISR
                inc     R16                         ;
                sts     OSIntNesting,R16            ;
                
;*********************************************************************************************************
; You should disable the lines between #if 1 and #endif if you use uC/OS-II version 2.51 or below!
; (Unfortunately OS_VERSION is not defined here (how to do it elegantly? On possibility would be to
;  code this ISR with C in os_cpu_c.c or use inline assembler in os_cpu_c.c)
;*********************************************************************************************************
#if 1
                cpi     R16,0x01
                brne    _not_first_int

                in      R28,_SFR_IO_ADDR(SPL)       ; Z = SP
                in      R29,_SFR_IO_ADDR(SPH)
                lds     R30,OSTCBCur                ; Z = OSTCBCur->OSTCBStkPtr
                lds     R31,OSTCBCur+1              ;
                st      Z+,R28                      ; Save Y pointer
                st      Z+,R29                      ;
_not_first_int:
#endif
                ldi     R16,256-(CPU_CLOCK_HZ/OS_TICKS_PER_SEC/1024)   
                                                    ; Reload timer to overflow at a rate of OS_TICKS_PER_SEC
                out     _SFR_IO_ADDR(TCNT0),R16     ; at a prescaler of 1024 and CPU_CLOCK_HZ AVR clock
	
                sei                                 ; Enable interrupts
                
                call    OSTimeTick                  ; Call uC/OS-IIs tick updating function
                nop
                call    OSIntExit                   ; Notify uC/OS-II about end of ISR

                POPSREG
                POPRS                               ; Restore all registers and status register
                ret                                 ; Note: RET instead of RETI


