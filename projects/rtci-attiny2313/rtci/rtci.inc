;*****************************************************************************
; Run Time Changeable Interrupts for ATTINY2313 (rtci.inc)
;	By: gogamoga@gmail.com License: GPL2.0
;	Version: 0.1
;	AVRASM 2.1 is required to be able to use this include!!! (for .overlap)
;
; Description:
;	This include allows to change interrupt entry points during runtime
;	it maintains a table of entry point per requested interrupt.
;	A simple loader is implented to take care of initial data in ram
;	This set of macros can be used to simplify definition of static interrupt
;	table (when rt-change functionality is not needed)
;	Takes 2*(N+1) bytes in SRAM, where N is the number of rtc Interrupts
;	Rtc interrupt entry is delayed by additional 6 clocks (load and jump)
;	Registers: R27, R26 and R16 are used in sHANDLER calls !!!(push them first)
;	Registers: R31 and R30 are used when rtc Interrupt is called !!!!
;
; User macros, general:
;	rtciCseg	- change default offset for loader & jump table (optional)
;	rtciDseg	- change default offset for entry point table (optional)
;	enableRtci - enable rtci functionality
;	rtciLoader - execute rtci loader. Must be called before any i/sHandler calls
;
; User macros, enable rtci functionality for each HANDLER:
; (rtciRESET is not in the list!)
; rtciINT0, rtciINT1, rtciCAPT1, rtciCOMPA1, rtciOVF1, rtciOVF0, rtciRX0,
; rtciUDRE0, rtciTX0, rtciACOMP, rtciPCINT, rtciCOMPB1, rtciCOMPA0, rtciCOMPB0,
; rtciUSISTART, rtciUSIOVF, rtciEEREADY, rtciWDTOVF
;
; User macros, set initial entry point for each HANDLER	
; iRESET, iINT0, iINT1, iCAPT1, iCOMPA1, iOVF1, iOVF0, iRX0,
; iUDRE0, iTX0, iACOMP, iPCINT, iCOMPB1, iCOMPA0, iCOMPB0,
; iUSISTART, iUSIOVF, iEEREADY, iWDTOVF
;
; User macros, change entry point for each HANDLER during runtime	
; sHANDLER OFFSET - change entry point to OFFSET (label or immediate)
; sRESET, sINT0, sINT1, sCAPT1, sCOMPA1, sOVF1, sOVF0, sRX0,
; sUDRE0, sTX0, sACOMP, sPCINT, sCOMPB1, sCOMPA0, sCOMPB0,
; sUSISTART, sUSIOVF, sEEREADY, sWDTOVF

; Usage, Case 1.:
; The following example enables rtci functionality, declares INT0 as rtci,
; defines initial entry point for RESET, INT0 and INT1 (int 1 is static!)
; the INT0 handler routine changes the entrypoint of INT0 during runtime - sINT0
;
;	.include "path to rtci.inc"
;	rtciCseg 0x20	; (optional to change default offset for loader & jumptable)
;	rtciDseg 0x70	; (optional to change default offset for entry point table)
;	enableRtci		; to enable rtci functionality, (skip if rtci is not needed)
;	rtciINT0		; declare INT0 as rtc interrupt - works with enableRtci
;	rtciLoader		; MUST BE CALLED to enable any macros
;	
;	RESET:			; (you can either use labels or skip them on entry points)
;	iRESET			; Define program entry point - because of enableRtci call
;	nop				; this entry point will go into entry point table in SRAM
;	nop				; and at chip reset, the loader will be called
;	nop				; loader size and execution time depends
;	nop				; on number of rtciHANDLER calls

;	nop				; (do your usefull stuff here)
;	nop				; ( such as... configure and enable interrupts, etc.)
;	nop				; (and program execution loop)

;	iINT0			; (define initial entry point for rtciINT0)
;	nop				; (do some stuff here)
;	nop		
;	sINT0 NEW_INT0_ENTRY	; set new INT0 entry point (in runtime)
;	reti
;
;	NEW_INT0_ENTRY:
;	nop				; (do something here)
;	nop
;	reti

;	iINT1			; (define initial entry point for static INT1)
;	nop				; (INT1 was not declared as rtci - rtciINT1 was not called)
;	nop				; (so INT1 entry point will go into static table - .org 0x0002)
;	reti

; Usage, Case 2.:
; The following example does not involve rtci functionality.
; defines initial entry point for RESET, INT0 and INT1 (all static),
; so, the purpose of that form is to save the effort of writing the interrupt 
; table by hand. Just use iHANDLER at any line to set the current offset 
; as the entrypoint and it will be written into the interurpt table.
;
;	.include "path to rtci.inc"
;	rtciLoader		; MUST BE CALLED to enable any macros
;	
;	iRESET			; Program entry point
;	nop				; some stuff
;
;	iINT0			; INT0 entry point
;	nop
;	reti
;
;	iINT1			; INT1 entry point
;	nop
;	reti

	
;*****************************************************************************
; Definitions

.set _rtciEnabled	= 0
.set _rtciCounter	= 0
.set _rtciLoaded	= 0

.set _rtciCsegStart = 0x0013
.set _rtciDsegStart = 0x0060

.equ vRESET		= 0x0000
.equ vINT0		= 0x0001
.equ vINT1		= 0x0002
.equ vCAPT1		= 0x0003
.equ vCOMPA1	= 0x0004
.equ vOVF1		= 0x0005
.equ vOVF0		= 0x0006
.equ vRX0		= 0x0007
.equ vUDRE0		= 0x0008
.equ vTX0		= 0x0009
.equ vACOMP		= 0x000A
.equ vPCINT		= 0x000B
.equ vCOMPB1	= 0x000C
.equ vCOMPA0	= 0x000D
.equ vCOMPB0	= 0x000E
.equ vUSISTART	= 0x000F
.equ vUSIOVF	= 0x0010
.equ vEEREADY	= 0x0011
.equ vWDTOVF	= 0x0012


; Options macros (call early if need to change)

.macro rtciCseg
	.set _rtciCsegStart = @0
.endm

.macro rtciDseg
	.set _rtciDsegStart = @0
.endm

; Internal macros

.macro _fixorg				; internal - fix .org to higher one
	.cseg
	_this:
	.if _this < _orgfix
		.org _orgfix
	.else
		.org _this
	.endif
.endm

.macro _rtciHandler			; write handler for rtc interrupt
	lds R31, @0
	lds R30, @0+1
	ijmp
	.dseg
	.org @0
	.db 0x00, 0x00
	.cseg
.endm

.macro _rtciLoader			; write loader for interrupt entry
	.overlap
	.if _loaderCounter == 0
			ldi R27, 0x00
			ldi R26, _rtciDsegStart
	.endif
	.if _loaderCounter < _rtciCounter
		.set _rtciCsegOffset = _rtciCsegStart + _loaderCounter
		.set _loaderCounter = _loaderCounter + 1
		ldi R16, high(_rtciCsegOffset)
		st X+, R16
		ldi R16, low(_rtciCsegOffset)
		st X+, R16
		_rtciLoader
	.endif
	.nooverlap
.endm

.macro _rtciUpdateLoader	; update rtci entry at runtime
	.ifdef _rtci@0
		.set _rtciDsegOffset = _rtciDsegStart + _rtci@0 * 2
		ldi R27, high(_rtciDsegOffset)
		ldi R26, low(_rtciDsegOffset)
		ldi R16, high(@1)
		st X+, R16
		ldi R16, low(@1)
		st X+, R16
	.else
		.error "_rtciUpdateLoader called for static handler"
	.endif
.endm

.macro _rtcInterrupt		; rtc interrupt initialization
	.if _rtciLoaded == 1
		.error "_rtcInterrupt called after loader"
	.endif
	.if _rtciEnabled == 0
		.error "_rtcInterrupt called with rtci not enabled"
	.endif
	.ifndef _rtci@0
		.cseg
		_this:
		.set _rtci@0 = _rtciCounter
		.set _rtciCounter = _rtciCounter + 1
		.set _rtciCsegOffset = _rtciCsegStart + _rtci@0 * 5
		.set _rtciDsegOffset = _rtciDsegStart + _rtci@0 * 2
		.org _rtciCsegOffset
		_rtciHandler _rtciDsegOffset	
		.overlap	
		.org @0
		rjmp _rtciCsegOffset
		.nooverlap
		.set _orgfix = _this
		_fixorg
	.else
		.error "duplicate call to _rtcInterrupt"
	.endif
.endm

.macro _staticInt	; define static interrupt entry
			.cseg
			_this:
			.org @0
			rjmp _this
			.set _orgfix = _this
			_fixorg
.endm

.macro _rtcInt		; define rtci interrupt entry				
	.cseg
	_this:
	.if _rtciEnabled == 1
		.ifdef _rtci@0
			.set _loaderCsegOffset = _rtciCsegStart + _rtciCounter * 5 + _rtci@0 * 4
			.org _loaderCsegOffset  + 2
			ldi R16, high(_this)
			.org _loaderCsegOffset + 4
			ldi R16, low(_this)
		.endif
	.else
		.error "rtci functionality is not enabled"
	.endif
	.set _orgfix = _this
	_fixorg
.endm

.macro _defaultIntVectors	; internal - setup default vector table
	.cseg
	_this:
	.overlap
	.org vRESET
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	.nooverlap
	.set _orgfix = _this
	_fixorg
.endm

;*****************************************************************************
;	User macros

.macro enableRtci	; enable runtime-change interrups
	.set _rtciEnabled = 1
	_defaultIntVectors
	_rtcInterrupt vRESET
.endm

.macro rtciLoader		; initialize the rtci loader (must be called!)
	.cseg
	_this:
	.if _rtciLoaded == 0
		.set _rtciLoaded = 1
		.if _rtciEnabled == 1
			.set _loaderCsegOffset = _rtciCsegStart + _rtciCounter * 5
			.set _loaderCounter = 0
			.org vRESET
			rjmp _loaderCsegOffset
			.overlap
			.org _loaderCsegOffset
			_rtciLoader
			.nooverlap
			rjmp _rtciCsegStart
		.else
			_defaultIntVectors
		.endif
	.else
		.error "Duplicate call to rtciLoader"
	.endif
	.set _orgfix = _this
	_fixorg
.endm

;*****************************************************************************
;	Next set of macros enables rtci functionality for interrupts

.macro rtciINT0
	_rtcInterrupt vINT0
.endm

.macro rtciINT1
	_rtcInterrupt vINT1
.endm

.macro rtciCAPT1
	_rtcInterrupt vCAPT1
.endm

.macro rtciCOMPA1
	_rtcInterrupt vCOMPA1
.endm

.macro rtciOVF1
	_rtcInterrupt vOVF1
.endm

.macro rtciOVF0
	_rtcInterrupt vOVF0
.endm

.macro rtciRX0
	_rtcInterrupt vRX0
.endm

.macro rtciUDRE0
	_rtcInterrupt vUDRE0
.endm

.macro rtciTX0
	_rtcInterrupt vTX0
.endm

.macro rtciACOMP
	_rtcInterrupt vACOMP
.endm

.macro rtciPCINT
	_rtcInterrupt vPCINT
.endm

.macro rtciCOMPB1
	_rtcInterrupt vCOMPB1
.endm

.macro rtciCOMPA0
	_rtcInterrupt vCOMPA0
.endm

.macro rtciCOMPB0
	_rtcInterrupt vCOMPB0
.endm

.macro rtciUSISTART
	_rtcInterrupt vUSISTART
.endm

.macro rtciUSIOVF
	_rtcInterrupt vUSIOVF
.endm

.macro rtciEEREADY
	_rtcInterrupt vEEREADY
.endm

.macro rtciWDTOVF
	_rtcInterrupt vWDTOVF
.endm


;*****************************************************************************
;	Next set of macros are used to define initial interrupt entry point

.macro iRESET	
	.ifdef _rtcivRESET
		_rtcInt vRESET
	.else
		_staticInt vRESET
	.endif
.endm

.macro iINT0
	.ifdef _rtcivINT0
		_rtcInt vINT0	
	.else
		_staticInt vINT0
	.endif
.endm

.macro iINT1
	.ifdef _rtcivINT1
		_rtcInt vINT1	
	.else
		_staticInt vINT1
	.endif 
.endm

.macro iCAPT1
	.ifdef _rtcivCAPT1
		_rtcInt vCAPT1	
	.else
		_staticInt vCAPT1
	.endif 
.endm

.macro iCOMPA1
	.ifdef _rtcivCOMPA1
		_rtcInt vCOMPA1	
	.else
		_staticInt vCOMPA1
	.endif 
.endm

.macro iOVF1
	.ifdef _rtcivOVF1
		_rtcInt vOVF1	
	.else
		_staticInt vOVF1
	.endif 
.endm

.macro iOVF0
	.ifdef _rtcivOVF0
		_rtcInt vOVF0	
	.else
		_staticInt vOVF0
	.endif 
.endm

.macro iRX0
	.ifdef _rtcivRX0
		_rtcInt vRX0	
	.else
		_staticInt vRX0
	.endif 
.endm

.macro iUDRE0
	.ifdef _rtcivUDRE0
		_rtcInt vUDRE0	
	.else
		_staticInt vUDRE0
	.endif 
.endm

.macro iTX0
	.ifdef _rtcivTX0
		_rtcInt vTX0	
	.else
		_staticInt vTX0
	.endif 
.endm

.macro iACOMP
	.ifdef _rtcivACOMP
		_rtcInt vACOMP	
	.else
		_staticInt vACOMP
	.endif 
.endm

.macro iPCINT
	.ifdef _rtcivPCINT
		_rtcInt vPCINT	
	.else
		_staticInt vPCINT
	.endif 
.endm

.macro iCOMPB1
	.ifdef _rtcivCOMPB1
		_rtcInt vCOMPB1	
	.else
		_staticInt vCOMPB1
	.endif 
.endm

.macro iCOMPA0
	.ifdef _rtcivCOMPA0
		_rtcInt vCOMPA0	
	.else
		_staticInt vCOMPA0
	.endif 
.endm

.macro iCOMPB0
	.ifdef _rtcivCOMPB0
		_rtcInt vCOMPB0	
	.else
		_staticInt vCOMPB0
	.endif 
.endm

.macro iUSISTART
	.ifdef _rtcivUSISTART
		_rtcInt vUSISTART	
	.else
		_staticInt vUSISTART
	.endif 
.endm

.macro iUSIOVF
	.ifdef _rtcivUSIOVF
		_rtcInt vUSIOVF	
	.else
		_staticInt vUSIOVF
	.endif 
.endm

.macro iEEREADY
	.ifdef _rtcivEEREADY
		_rtcInt vEEREADY	
	.else
		_staticInt vEEREADY
	.endif 
.endm

.macro iWDTOVF
	.ifdef _rtcivWDTOVF
		_rtcInt vWDTOVF	
	.else
		_staticInt vWDTOVF
	.endif 
.endm


;*****************************************************************************
;	Next set of macros change interrupt entry point in runtime

.macro sINT0
	_rtciUpdateLoader vINT0, @0
.endm

.macro sINT1
	_rtciUpdateLoader vINT1, @0
.endm

.macro sCAPT1
	_rtciUpdateLoader vCAPT1, @0
.endm

.macro sCOMPA1
	_rtciUpdateLoader vCOMPA1, @0
.endm

.macro sOVF1
	_rtciUpdateLoader vOVF1, @0
.endm

.macro sOVF0
	_rtciUpdateLoader vOVF0, @0
.endm

.macro sRX0
	_rtciUpdateLoader vRX0, @0
.endm

.macro sUDRE0
	_rtciUpdateLoader vUDRE0, @0
.endm

.macro sTX0
	_rtciUpdateLoader vTX0, @0
.endm

.macro sACOMP
	_rtciUpdateLoader vACOMP, @0
.endm

.macro sPCINT
	_rtciUpdateLoader vPCINT, @0
.endm

.macro sCOMPB1
	_rtciUpdateLoader vCOMPB1, @0
.endm

.macro sCOMPA0
	_rtciUpdateLoader vCOMPA0, @0
.endm

.macro sCOMPB0
	_rtciUpdateLoader vCOMPB0, @0
.endm

.macro sUSISTART
	_rtciUpdateLoader vUSISTART, @0
.endm

.macro sUSIOVF
	_rtciUpdateLoader vUSIOVF, @0
.endm

.macro sEEREADY
	_rtciUpdateLoader vEEREADY, @0
.endm

.macro sWDTOVF
	_rtciUpdateLoader vWDTOVF, @0
.endm
