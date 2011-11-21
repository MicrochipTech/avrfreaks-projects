; sdrschd_0.01.inc - Single Delay Request Dual Tasking Scheduler version 0.01
; Switches between 2 tasks, cpu time is shared 1/3 - 1/3 - 1/3 
; between 2 processes and the scheduler itself
; can supply 1 short delay (precision +- 1 clock tick)
; designed to be used without crystal on internal 1 mhz on ATTiny2313
; wasnt tested on anything but ATTiny2313
; No support, warranty, license etc shit.
;
; Algorithm and timing:
;
; Switching without Delay Request:
;
;             +------------------ tSVD -------------------+
; time:  tXC  tTEN   tSWR    tCR   tXC  tTEN   tSWR    tCR  tXC
; flow: Proc1 ----> Switcher ---> Proc2 ----> Switcher ---> Proc1
; tTEN + tCR + tTEN + tCR = 24 Clocks LOST IN TRANSITION! (SAD)
;
; Switching with Delay Request:
;
;             +------------------ DELAY ----------------------+
; time: t<tXC  tE      tDSR    tCR    ???  tTEN   tSWR    tCR    tXC
; flow: Proc1 ----> DelaySetup ----> Proc2 ----> Switcher ----> Proc1
; tE + tCR + tTEN + tCR = 19 Clocks LOST IN TRANSITION! (SAD AGAIN)
;
; usage:
;
; .INCLUDE "sdrschd_0.01.inc"
;
; sdrsSetup proc1, proc2
;
; proc1:
;	nop
;	rjmp proc1

; proc2:
;	nop
;	isdrsDelay 100
;	rjmp proc2
;
;
; This one is really simple one (to make it as fast as possible)
; switching between tasks in 23 clock ticks,
; Total size of scheduler is only 98 bytes.
; The scheduler provides 1 persistent register - PRR -
; it is guaranteed to go unchanged between process execution times
; The scheduler also provides the routine for making short delays 
; Max delay possible is 255 + [switching time] - about 300 clocks
; Min delay possible is [switching time] - delays shorter than 
; that will not be honored, so, minimal would be 45 Clocks
; need less than that - do it yourself (cli, delay, do something, sei)
; also, as the name of the library says, its Single Delay Request
; which means, that if delay request comes, and the previous one 
; isnt over yet -  the previous will be canceled and returned immediately, 
; thus failing to provide the requested delay

; Constant description:
; 	tE		- Entry Time
;	tTen 	- Timer interrupt Entry Time
;	tCR		- Control Return Time
;	tSWR	- Switcher Routine Time
;	tDSR	- Delay Setup Time
;	tSVD	- Switching Time
;	tRVD	- Switching via Delay Setup Routine
;	tXC		- Execution Cycle length
;	tMD		- Minimal Delay
;	ccRD	- Delay Routine Timer Correction
;	ccXC	- Switcher Routine Timer Correction

;	ORAM	- RAM address Offset
;	MRAM	- Max RAM
;	VRAM	- RAM used by Scheduler
;	PRAM	- RAM left for Processes
;	DPP1	- Data Pointer for Process 1
;	SPP1	- Stack Pointer for Process 1
;	DPP2	- Data Pointer for Process 2
;	SPP2	- Stack Pointer for Process 2
;	PRD		- Pointer to Scheduler Process Data

; Setup routine, usage: sdrsSetup Process1, Process2
; The scheduler will switch between Processes after one of them
; Was running for tXC clock ticks

; FLAME AT: gogamoga@gmail.com

.MACRO sdrsSetup	

.IFNDEF PRR
	.DEF PRR			= R16
.ENDIF

.IFNDEF WRR
	.DEF WRR			= R17
.ENDIF


.EQU _sdrschd_tE	= 3
.EQU _sdrschd_tTEN	= 8
.EQU _sdrschd_tCR	= 4
.EQU _sdrschd_tSWR	= 12
.EQU _sdrschd_tDSR	= 14
.EQU _sdrschd_tSVD	= 2 * (_sdrschd_tTEN + _sdrschd_tCR + _sdrschd_tSWR)
.EQU _sdrschd_tRVD	= _sdrschd_tE + _sdrschd_tTEN + _sdrschd_tSWR + _sdrschd_tDSR + _sdrschd_tCR * 2
.EQU _sdrschd_tXC	= -_sdrschd_tSVD
.EQU _sdrschd_tCRD	= _sdrschd_tSVD + _sdrschd_tXC
.EQU _sdrschd_tMD	= _sdrschd_tRVD
.EQU _sdrschd_ccRD	= -32
.EQU _sdrschd_ccXC	= -4


.EQU _sdrschd_ORAM	= 96
.EQU _sdrschd_MRAM	= 128
.EQU _sdrschd_VRAM	= 4
.EQU _sdrschd_PRAM	= _sdrschd_MRAM-_sdrschd_VRAM

.EQU _sdrschd_DPP1	= 0 + _sdrschd_ORAM
.EQU _sdrschd_DPP2	= _sdrschd_PRAM/2 + _sdrschd_ORAM
.EQU _sdrschd_SPP1	= _sdrschd_DPP2-1
.EQU _sdrschd_SPP2	= _sdrschd_PRAM-1 + _sdrschd_ORAM
.EQU _sdrschd_PRD		= _sdrschd_PRAM + _sdrschd_ORAM

.CSEG
.ORG $0000
	rjmp _sdrschd_init ; Reset vector

.ORG $000D
	rjmp _sdrschd_scheduler; T/C0 Compare Match A vector

.ORG $0013
_sdrschd_init:

ldi WRR, _sdrschd_SPP2
out SPL, WRR

ldi WRR, low(@0)
push WRR
ldi WRR, high(@0)
push WRR

ldi PRR, 0
push PRR

in WRR, SPL
sts _sdrschd_PRD, WRR

ldi WRR, _sdrschd_SPP1
out SPL, WRR

ldi WRR, low(@1)
push WRR
ldi WRR, high(@1)
push WRR

ldi WRR, 1
out TIMSK, WRR
out TCCR0B, WRR

neg WRR
out TCNT0, WRR

reti

_sdrschd_scheduler:
	push PRR
	in PRR, SPL
	lds WRR, _sdrschd_PRD
	out SPL, WRR
	sts _sdrschd_PRD, PRR
	
	ldi WRR, _sdrschd_tXC + _sdrschd_ccXC
	out TCNT0, WRR

	pop PRR	
	reti


__sdrschd_delay:
	push PRR
	subi WRR, _sdrschd_ccRD
	out TCNT0, WRR
	ser WRR			; If those 2 lines are not present - 2 clocks faster
	out TIFR, WRR	; but if counter kicks in while here - delay is lost!
	in PRR, SPL		; so we have to clear the TIFR to workaround that
	lds WRR, _sdrschd_PRD
	out SPL, WRR
	sts _sdrschd_PRD, PRR
	pop PRR

	reti 

.EQU _sdrschd_delay = __sdrschd_delay	; TRICK!

.ENDMACRO

; Delay request, param in REGISTER: sdrsDelay REGISTER
.MACRO sdrsDelay	; 
	cli
	neg @0		; Counter runs from n to 0, - delay should be negative
	rcall _sdrschd_delay
.ENDMACRO

; Delay request, immediate param: sdrsDelay IMMEDIATE
.MACRO isdrsDelay
	cli
	ldi WRR, -@0 ; Counter runs from n to 0, - delay should be negative
	rcall _sdrschd_delay
.ENDMACRO
