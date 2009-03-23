;*************************************************************************
.include "compat.h"	; compatibility definitions
.include "protocol.h"
;-------------------------------------------------------------------------
;				Constant definitions
;-------------------------------------------------------------------------
.equ	VERSION		= 0x0109

.equ	XTAL		= 8000000	; 8MHz, not critical 
.equ	BootDelay	= XTAL / 3	; 0.33s

;------------------------------	select UART mode -------------------------
.if SRX == STX && SRX_PORT == STX_PORT
.equ	ONEWIRE		= 3
.else
.equ	ONEWIRE		= 0
.endif

.equ	SRX_PIN		= SRX_PORT - 2
.equ	STX_DDR		= STX_PORT - 1

;------------------------------	select bootloader size -------------------
.ifndef CRC
.equ	CRC		= 17
.endif

.ifndef VERIFY
.equ	VERIFY		= 15
.endif

.ifndef	WDTRIGGER
.equ	WDTRIGGER	= 8
.endif

.ifndef	SPH
.equ	MinSize		= 201
.else
.equ	MinSize		= 203
.endif

.equ	BootSize	= CRC + VERIFY + ONEWIRE + WDTRIGGER + MinSize

;------------------------------	UART delay loop value --------------------
.if CRC
.equ	UartLoop	= 28		; UART loop time
.else
.equ	UartLoop	= 24
.endif

;------------------------------	Bootloader fuse setting ------------------
.ifdef	FirstBootStart
.if	(FlashEnd - FirstBootStart) > 255	; 256 Words needed
.equ	BootStart	= FirstBootStart
.else
.equ	BootStart	= SecondBootStart
.endif
;------------------------------	max possible buffer size -----------------
.set    BufferSize      = SRAM_SIZE / 2 - PAGESIZE

.macro  testpage		; calculate Buffersize to fit into BootStart
.if     BootStart % BufferSize
.set    BufferSize = BufferSize - PAGESIZE
.endif
.endmacro
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage
	testpage

.equ	UserFlash	= (2*BootStart)
.equ	BootFuse	= 1
.else
.equ	BufferSize	= PageSize
.equ	BootStart	= ((FlashEnd - BootSize) / PageSize * PageSize)
.equ	UserFlash	= (2*BootStart - 2)
.endif
;-------------------------------------------------------------------------
;				Using register
;-------------------------------------------------------------------------
.def	zerol		= r2
.def	zeroh		= r3
.def	baudl		= r4		; baud divider
.def	baudh		= r5
.def	crcl		= r6
.def	crch		= r7
;-------------------------------------------------------------------------
.def	a0		= r16		; working registers
.def	a1		= r17
.def	appl		= r18		; rjmp to application
.def	apph		= r19
.def	polynoml	= r20		; CRC polynom 0xA001
.def	polynomh	= r21
.def	zx		= r22		; 3 byte Z pointer

.def	twl		= r24		; wait time
.def	twh		= r25
;-------------------------------------------------------------------------
;				Using SRAM
;-------------------------------------------------------------------------
.dseg
	.org		SRAM_START
PROGBUFF:		.byte 2*BufferSize
PROGBUFFEND:
.cseg
;-------------------------------------------------------------------------
;				Initialization Macros
;-------------------------------------------------------------------------
.if ONEWIRE
.macro	IOPortInit
	sbi	STX_PORT, SRX		; weak pullup on
	cbi	STX_DDR, SRX		; as input
.endmacro
.macro	TXD_0
	sbi	STX_DDR, SRX		; strong pullup = 0
.endmacro
.macro	TXD_1
	cbi	STX_DDR, SRX		; weak pullup = 1
.endmacro
.macro	SKIP_RXD_0
	sbis	SRX_PIN, SRX		; low = 1
.endmacro
.macro	SKIP_RXD_1
	sbic	SRX_PIN, SRX		; high = 0
.endmacro
.else
;------------------------------
.macro	IOPortInit
	sbi	SRX_PORT, SRX
	sbi	STX_PORT, STX
	sbi	STX_DDR, STX
.endmacro
.macro	TXD_0
	cbi	STX_PORT, STX
.endmacro
.macro	TXD_1
	sbi	STX_PORT, STX
.endmacro
.macro	SKIP_RXD_0
	sbic	SRX_PIN, SRX
.endmacro
.macro	SKIP_RXD_1
	sbis	SRX_PIN, SRX
.endmacro
.endif
;-------------------------------------------------------------------------
