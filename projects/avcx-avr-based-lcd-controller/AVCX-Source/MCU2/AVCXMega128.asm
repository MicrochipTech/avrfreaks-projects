.NOLIST

.INCLUDE "m128def.inc"
.INCLUDE "Macros.inc"

.LIST
.LISTMAC

;
; Software based on the Grand Prize winning Entry for the
; Circuit Cellar 2004 AVR Contest
; Project AVCX: http://www.circuitcellar.com/avr2004/grand.html
;
; It is re-used here with permission from Circuit Cellar
;
; Modified 2008 by Jan de Rie
;
; Besides a bug fix, the runtime design of MCU2 was completely redesigned
; because the original design was very timing sensitive. For example, a large
; delay was used in the MCU1 line scanning timing to accomodate MCU2
;
; In general, the only time MCU2 can access the video memory is between
; Int6 and Int7 events (the RAM window). Incoming graphics commands are
; stored in a buffer which is processed during the RAM window, when it
; is safe to write to the video memory).
;
; 1. the incoming bytes buffer was only 256 bytes but the CLRSCR command
;	 for example takes 57 mSecs to execute, which equates to 663 incoming
;	 bytes at 115,200 bps. A lot of these bytes could be discarded.
;	 The full bitmap commands expect 76,800 data bytes.
;	 With multibyte commands, the state machine would get out of sync
;
; 2. The graphics command processing during Int6 was not always completed
;	 before Int7 occured, requiring a clever and tricky recovery mechanism
;	 to make sure the previous processing was completed on the next Int6.
;	 This mechanism required that no functions were called while doing
;	 graphic commands processing.
;
; 3. While graphics processing occured during the Int6 ISR, no incoming bytes
;	 were accepted.
;
; 4. the RAM window is fairly long at the end of each screen. From lines 160 to 200,
;	 the RAM window is open all the time because this is "overscan" time. Even though
;	 the HSYNC signal is still properly issues, no data bytes are used by the LCD.
;	 In the original design, the RAM window was closed at line 185, probably because
;	 too many serial input bytes were lost. (the serial input was disabled during
;	 the RAM window in the original design)
;
; The new design uses a 2K input buffer, plenty for the most demanding input.
; It also accepts incoming characters at all times. There are a few short moments
; where interrupts are disabled, mostly to ensure atomic operations on 16 bit pointers
; or when toggling the memory controls
;
; Graphics processing is now done in the forever loop, not in an ISR. The forever loop gets interrupted
; when an Int7 occurs. The Int7 ISR releases the memory bus, sets the state to FROZEN
; and waits until the state changes to UNFREEZE. This state change occurs when Int6 hits.
; Int6 does nothing but changing the state. Int7 grabs the memory bus again and finishes,
; allowing the forever loop to continue graphics processing at the point it was interrupted.
; In summary, when the forever loops runs, it is safe to access the video memory. When it is
; not safe, the code loops inside the Int7 ISR. Incoming bytes are accepted at all times.
; The performance of MCU2, specially wrt incoming bytes, is now guaranteed, independant of
; the specific timing of Int6 and Int7 (as long as they alternate)
; Well almost. There were also screen artifacts at the beginning of each line when MCU2 accessed
; the memory. These artefacts were caused by the fact the MCU2 didn't release the memory bus fast
; enough, causing bus contention. This was fixed by sending the RAM_STOP signal from MCU1 to MCU2
; earlier (from Dot clock 262 to Dot clock 252). This however leaves only 8 dot clocks (16 cycles)
; for MCU2 to access the memory, which is not enough, causing even a 3K input buffer to overflow
; So some freeze (12 uSecs, value = 15, original value was 100) is still present.
;
; During the VSYNC period, the RAM window is now closed at line 200, allowing for a longer RAM window.
;
; Part of the reason why INT7 is slow to respond on MCU2 is the fact that SerialInput interrupts are
; on. When the INT7 interrupt occurs while a serial input is active, the INT7 interrupt is pended
; and won't get triggered until the serial input interrupt is completed. For this reason, the
; Serial Input ISR processing must be a short as possible.
; The same artifacts were showing in the original design even though the serial input interrupt was disabled.
; Need more testing, e.g. by disabling serial input interrupts while the RAM window is open. TODO
; On MCU1, the timing of INT7 is now advanced to minimize this effect, making the RAM window very short
; specially without the Freeze hack.
;
; The modified source files result in an LCD refresh rate of 66.6 Hz, double the original design,
; with little, if any, serial input rate limitations.
;
; The Sony LCD supports a 9-bit colordepth per pixel. 9 bits is an awkward number for an 8 bit processor.
; To send a complete bitmap over the serial port in 9 bits, the original design used 2 bytes per pixel,
; for a total of 76,800 bytes. Even at 115200 bps, this still takes over 6 seconds. Therefore I added a
; "Short bitmap" command, which only sends 8 bits, ignoring the lowest BLUE bit. Sending the short bitmap
; still takes 3.3 seconds.
;
; It would be nice to have a way to resetting the Graphics commands parser, There are multi-byte commands plus
; the bitmaps, which expect 38,400 or 76,800 bytes. Once the parser gets out of sync, it may gobble the next 76,800
; input bytes as it expects bitmap data.
;
; The pixel set functions have not been tested
;
; The current design relies on registers X and Y to be dedicated to the Serial Input buffer, which makes
; it difficult to convert the code to C.
; I have tried to use memory based pointers but this results in memory bus contention at the beginning
; of the scan lines when serial data are coming in to MCU2, because it can take too long for MCU2 to respond
; to the INT7 request from MCU1 (SerialIn ISR is too slow)
; If this program needs to be converted to C, the timing of INT7 in MCU1 must be advanced, at least to DotClock 248.
; The freeze window may have to be extended too in that case.
;
;##########################################################
;##                     ###################################
;## GENERAL DEFINITIONS ###################################
;##                     ###################################
;##########################################################

.EQU IN_PROGRESS = 0   ;Int_6 Processing in progress
                       ;For Int_6 use only
.EQU NULL = 0          ;Null string termination

.EQU STANDBY      = $00
.EQU COLOR        = $88
.EQU MOVE         = $90
.EQU PUT_CHAR     = $A0
.EQU PRINT_STRING = $B0
.EQU PICTURE      = $C0
.EQU	SHORTPICTURE = 0xC1
.EQU CLEAR_SCR    = $43
.EQU PSET         = 4
.EQU ARG_X        = 1
.EQU ARG_Y        = 2
.EQU CRD_INC      = 0
.EQU CRD_DEC      = 8
.EQU LIGHT_ON     = $7F
.EQU LIGHT_OFF    = $40
.EQU FONT_HEIGH   = 1
.EQU FONT_WIDTH   = 1
.EQU ODD_BYTE     = 1

#define		DEBUG 0

#define	State_FREEZING		10
#define	State_FROZEN		11
#define	State_UNFREEZING	12
#define	State_ACTIVE		13

;##########################################################
;##                  ######################################
;## GLOBAL VARIABLES ######################################
;##                  ######################################
;##########################################################

;Register Variables
;==================
;Lower Registers
.def	_0		=	r15
.def	_min1	=	r14
.def	RxData	=	r13
.def	RxFlags	=	r12

;Upper Registers
.DEF Vx        = R24  ;Video X coordinate
.DEF Vy        = R25   ;Video Y coordinate
;
; registers X & Y are currently reserved for the serial input buffer handling
; MEMPTRS will use ram based pointers
;
#define	MEMPTRS	0

.DSEG
.ORG $100
;
; At Serial Input speed of 115200, time per byte is 86 uSec
; Longest instruction is ClrScr, which takes 57 millisecs -> 662 character buffer needed
;
#define UART_115200 	3
#define UART_38400		11
#define	UART_SPEED		UART_115200		;with larger RxBuffer


;RAM Variables
;=============
RxBuffer: .BYTE 2048    ;Serial In Receive Buffer
RxBufferEnd:

#if DEBUG
Toggle:			.byte	1
#endif

GrStateMem:		.byte	1
GrOldCmdMem:	.byte	1
State:			.byte	1
#if MEMPTRS
Tailptr:		.byte	2
Headptr:		.byte	2
#endif

;##########################################################
;##             ###########################################
;## STACK SPACE ###########################################
;##             ###########################################
;##########################################################

.EQU STACK_SIZE = 256

.DSEG

.ORG (RAMEND - STACK_SIZE)

StackBottom: .BYTE STACK_SIZE

.CSEG
;##########################################################
;##                   #####################################
;## INTERRUPT VECTORS #####################################
;##                   #####################################
;##########################################################

;INTERRUPT VECTORS
.ORG 0x000
jmp  Reset
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  Int_6
jmp  Int_7
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  IntUart_0_Rx
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0
jmp  0

#if DEBUG
;
; PD0..PD7 are debug leds
;
InitDebug:
	ser		r16				;all bits
	out		DDRD,r16		;output
	clr		r16				;no bits
	out		PORTD,r16		;all low
	sts		Toggle,r16		;start low
	ret

DebugL0:
	sbi	PORTD,0
	rjmp	PC

DebugL1:
	sbi	PORTD,1
	rjmp	PC

DebugL2:
	sbi	PORTD,2
	rjmp	PC

DebugL3:
	sbi	PORTD,3
	rjmp	PC

ToggleL4:
	push r16
	lds	r16,Toggle
	tst	r16
	brne ToggleL410
	sbi	PORTD,4
	inc	r16
	rjmp ToggleL420
ToggleL410:
	cbi	PORTD,4
	dec	r16
ToggleL420:
	sts	Toggle,r16
	pop	r16
	ret

L4On:
	sbi	PORTD,4
	ret

L4Off:
	cbi	PORTD,4
	ret

L5On:
	sbi	PORTD,5
	ret

L5Off:
	cbi	PORTD,5
	ret
#else
InitDebug:
DebugL0:
DebugL1:
DebugL2:
DebugL3:
L4On:
L4Off:
	ret
#endif

;##########################################################
;##             ###########################################
;## ENTRY POINT ###########################################
;##             ###########################################
;##########################################################

Reset:
  ; Disable Interrupts during initializations
  cli

;##########################################################
;##                              ##########################
;## STACK POINTER INITIALIZATION ##########################
;##                              ##########################
;##########################################################
  ldi r16, HIGH(RAMEND)
  out SPH, r16
  ldi r16, LOW(RAMEND)
  out SPL, r16
;
; registers initialization
;
	clr		r16
	mov		_0,r16
	dec		r16
	mov		_min1,r16

;##########################################################
;##                          ##############################
;## I/O PORTS INITIALIZATION ##############################
;##                          ##############################
;##########################################################

  ; Initialize PORT G
  ; =================
  ; 4 3210 Bit No
  ; 0 0000 Value
  ; o ioii Direction
  ldi r16, 0b01011
  sts PORTG, r16
  ldi r16, 0b10100
  sts DDRG, r16

  ; Initialize PORT E
  ; =================
  ; 7654 3210 Bit No
  ; 1111 0000 Value
  ; iioo ooii Direction	sb: iioiooii but E4 is n.c.
  ldi r16, 0b11111111
  out PORTE, r16
  ldi r16, 0b00101100
  out DDRE,  r16

  ; Initialize PORT A
  ; =================
  ; 7654 3210 Bit No
  ; ---- ---- Value
  ; oooo oooo Direction
  ldi r16, 0b11111111
  out DDRA, r16

  ; Initialize PORT C
  ; =================
  ; 7654 3210 Bit No
  ; ---- ---- Value
  ; oooo oooo Direction
  out DDRC, r16

  ; Initialize PORT B
  ; =================
  ; 7654 3210 Bit No
  ; ---- ---- Value
  ; oooo oooo Direction
  out DDRB, r16

  ; Initialize all other ports
  ; as inputs with pull-up resistors
  ; 7654 3210 Bit No
  ; 1111 1111 Value
  ; iiii iiii Direction
  ldi r16, $FF
  out PORTD, r16
  sts PORTF,r16
  ldi r16, 0
  out DDRD, r16
  sts DDRF, r16

	rcall	InitDebug

;##########################################################
;##                                    ####################
;## EXTERNAL INTERRUPTS INITIALIZATION ####################
;##                                    ####################
;##########################################################

  ; INT0: Off
  ; INT1: Off
  ; INT2: Off
  ; INT3: Off
  ; INT4: Off
  ; INT5: Off
  ; INT6: On, Falling Edge
  ; INT7: On, Falling Edge

  ldi r16,  $00
  sts EICRA, r16
  ldi r16,  0b10100000			;Falling edge int7 & int6
  out EICRB, r16
  ldi r16,  $C0
  out EIFR,  r16
  ; Mask off all external interrupts for now
  ldi r16,  $00
  out EIMSK, r16

;##########################################################
;##                        ################################
;## USART 0 INITIALIZATION ################################
;##                        ################################
;##########################################################

  ; Receiver:    On, Interrupt: On
  ; Transmitter: On, Interrupt: Off
  ; Mode:        Asynchronous
  ; Parameters:  8 Data, 1 Stop, No Parity
  ; Baud rate:   UART_SPEED
  ldi r16,   $00
  out UCSR0A, r16
  ldi r16,   $98
  out UCSR0B, r16
  ldi r16,   $06
  sts UCSR0C, r16
  ldi r16,   (UART_SPEED & 0xff)
  out UBRR0L, r16
  ldi r16,   (UART_SPEED >> 8)
  sts UBRR0H, r16

;##########################################################
;##                                  ######################
;## ANALOG COMPARATOR INITIALIZATION ######################
;##                                  ######################
;##########################################################

  ;Analog Comparator: Off
  ldi r16,  $80
  out ACSR,  r16
  ldi r16,  $00
  out SFIOR, r16


;##########################################################
;##                          ##############################
;## VARIABLES INITIALIZATION ##############################
;##                          ##############################
;##########################################################

  sts	GrStateMem,_0		; GrState = STANDBY
  sts	GrOldCmdMem,_0		;nothing

#if DEBUG
	ldi		r16,LOW(RxBuffer)
	tst		r16
	breq	Main_L01
	rjmp	PC
Main_L01:
#endif

	ldi		r16,HIGH(RxBuffer)
#if MEMPTRS
	sts		HeadPtr,_0
	sts		HeadPtr+1,r16
	sts		TailPtr,_0
	sts		TailPtr+1,r16
#else
	mov		YL,_0
	mov		XL,_0
	mov		YH,r16
	mov		XH,r16
#endif

;##########################################################
;##                               #########################
;## VARIOUS FINAL INITIALIZATIONS #########################
;##                               #########################
;##########################################################

  ; Select Upper Page of Flash for fonts, bitmaps, etc.
  ldi r17, 1
  out RAMPZ, r17

  ;Clear Screen
  SET_COLOR 7, 7, 7
#if 0
;  CLR_SCREEN
  ldi Vy, 159   ;
  cls1:
  out PORTC, Vy ;<-----+
  ldi Vx, 239   ;      |
  cls0:
  out PORTA, Vx ;<--+  |
  cbi PORTE, 3  ;   |  |
  sbi PORTE, 3  ;   |  |
  subi Vx, 1    ;   |  |
  brcc cls0     ;---+  |
  subi Vy, 1    ;      |
  brcc cls1     ;------+
  ldi Vx, 0     ;
  ldi Vy, 0     ;
#endif
;
;  PrintString("Ready")
;

	ldi		r17,0x43		;clear Screen
	rcall	Serial_In

	ldi		r17,0x88		;Set Color 0,0,0
	rcall	Serial_In
	ldi		r17,0
	rcall	Serial_In

	ldi		r17,0xB0		;String
	rcall	Serial_In

	ldi r17, 'R'
  	rcall	Serial_In
	ldi r17, 'e'
	rcall	Serial_In
	ldi r17, 'a'
	rcall	Serial_In
	ldi r17, 'd'
	rcall	Serial_In
	ldi r17, 'y'
	rcall	Serial_In
	ldi r17, 0
	rcall	Serial_In

  ;Clear Int_6 & Int_7 flags
  ldi r16,  0b11000000
  out EIFR,  r16
;
; enable both Int_6 and Int_7
;
	ldi r16, 0b11000000
	out EIMSK, r16
	ldi	r16,State_ACTIVE
	sts		State,r16
	sei
	rcall	Int_7			;wait for INT6
;
; we come back here after INT6 has been received
;
MainLoop:
;
; process queued graphic commands from SerialIn
;
	rcall		ProcessCommands
	rjmp	 	MainLoop

;##########################################################
;##                           #############################
;## VRAM ACCESS START (Int_6) #############################
;##                           #############################
;##########################################################
Int_6:
	push	r16
	in		r16,SREG
	push	r16
	lds		r16,State
	cpi		r16,State_FROZEN
	breq	Int_6_L100
	rcall	DebugL0				;error
Int_6_L100:
	ldi		r16,State_UNFREEZING
	sts		State,r16
	pop		r16
	out		SREG,r16
	pop		r16
	reti

;##########################################################
;##                           #############################
;## VRAM ACCESS END (Int_7)   #############################
;##                           #############################
;##########################################################

Int_7:
;
;	Disconnect VRAM
;
	VRAM_RELEASE
	push	r16
	in		r16,SREG
	push	r16
	rcall	L4On
;
; Check Logic
;
	lds		r16,State
	cpi		r16,State_ACTIVE
	breq	Int_7_L100
	rcall	DebugL1				;error
Int_7_L100:
	ldi		r16,State_FROZEN
	sts		State,r16
	sei							;allow interrups for Serial In, Int_6

Int_7_Loop:
	lds		r16,State
	cpi		r16,State_UNFREEZING		;wait for Int_6 interrupt
	brne	Int_7_Loop

	cli
;
; State is now State_UNFREEZING
; Take control over Video RAM
;
  	VRAM_CONNECT
	ldi		r16,State_ACTIVE
	sts		State,r16
		
	rcall	L4Off
	pop		r16
	out		SREG,r16
	pop		r16
	reti


;####################################
;# Graphic Command Processing BEGIN #
;####################################
;
; only while doing processing
;
.DEF GrCmd     = R21  ;Current Graphic Command
.DEF GrState   = R22  ;Graphic Command state machine variable
.DEF GrOldCmd  = R23   ;Last Graphic Command

ProcessCommands:
	lds		GrState,GrStateMem
	lds		GrOldCmd,GrOldCmdMem
CommandLoop:
  ;BEGIN while (RxReadPtr != RxWritePtr)
	cli
#if MEMPTRS
	lds		XL,TailPtr		;not changed in SerialIn ISR
	lds		XH,TailPtr+1
	lds		YL,HeadPtr		;changed in SerialIn ISR
	lds		YH,HeadPtr+1
#endif
	cp		YH,XH
	cpc		YL,XL
	sei
	brne	Int6_L01
	sts		GrStateMem,GrState
	sts		GrOldCmdMem,GrOldCmd
	ret

Int6_L01:
  ;Get next Graphic Command
  ;GrCmd = RxReadPtr
	ld	GrCmd,X


  ;#######################
  ;# State Machine BEGIN #
  ;#######################

;------------------------------------------------------------------
GrState_STANDBY:
  cpi GrState, STANDBY
  breq PC+2
  rjmp GrState_PICTURE

  ;Decode Command
  mov  ZL, GrCmd
  andi ZL, 0b11000000
  cpi  ZL, 0b01000000
  brne MultiByteCommand
  rjmp SingleByteCommand

MultiByteCommand:
  mov  GrOldCmd, GrCmd
  mov  GrState, GrCmd
  andi GrState, $F8
  jmp GetNextCommand

SingleByteCommand:

ClrScreen:
  cpi GrCmd, CLEAR_SCR
  brne LightOn
  CLR_SCREEN
  jmp GetNextCommand

LightOn:
  cpi GrCmd, LIGHT_ON
  brne LightOff
  ldi GrCmd, 0b10000
  sts PORTG, GrCmd
  jmp GetNextCommand

LightOff:
  cpi GrCmd, LIGHT_OFF
  brne GrPset
  ldi GrCmd, 0b00000
  sts PORTG, GrCmd
  jmp GetNextCommand

GrPset:
  sbrs GrCmd, 2
  rjmp CrdDec
  VRAM_WR

CrdDec:
  sbrs GrCmd, 3
  rjmp CrdInc

  sbrs GrCmd, 0
  rjmp CrdDecVy
  dec Vx
  cpi Vx, $FF
  brne PC+2
  ldi Vx, 239
  out PORTA, Vx
  rjmp GetNextCommand

CrdDecVy:
  sbrs GrCmd, 1
  jmp GetNextCommand

  dec Vy
  cpi Vy, $FF
  brne PC+2
  ldi Vy, 159
  out PORTC, Vy
  jmp GetNextCommand

CrdInc:
  sbrs GrCmd, 0
  rjmp CrdIncVy
  inc Vx
  cpi Vx, 240
  brne PC+2
  ldi Vx, 0
  out PORTA, Vx
  jmp GetNextCommand

CrdIncVy:
  sbrs GrCmd, 1
  jmp GetNextCommand
  inc Vy
  cpi Vy, 160
  brne PC+2
  ldi Vy, 0
  out PORTC, Vy
  jmp GetNextCommand

;------------------------------------------------------------------


GrState_PICTURE:
  cpi GrState, (PICTURE & 0xC0)
  breq PC+2
  rjmp GrState_COLOR

  ; PICTURE Commands just issued?
  mov	r16,GROldCmd
  andi	r16,0xC0
  cpi	r16,PICTURE
  brne GrPict_00
  ldi Vx, 0
  ldi Vy, 159
  ldi r17, 0
  andi GrOldCmd, 0x01

GrPict_00:
	cpi		GrOldCmd, (SHORTPICTURE & 0x01)
	brne	GrPict_DblByte
	out		PORTC,Vy
	out		PORTA,Vx
	out		PORTB,GrCmd
	VRAM_WR
	inc		Vx
	cpi		Vx,240
	breq	GrPict_003
	rjmp	GetNextCommand
GrPict_003:
	mov		Vx,_0
	dec		Vy
	cpi		Vy,255
	breq	GrPict_004
	rjmp	GetNextCommand
GrPict_004:
	mov		Vy,_0
	rjmp	GrState_DEFAULT		;done

GrPict_DblByte:
  cpi  r17, 0
  breq GrPict_EvenByte
  rjmp GrPict_OddByte

GrPict_EvenByte:
  ldi r17, 7
  out PORTC, Vy
  out PORTA, Vx
  out PORTB, GrCmd
  jmp GetNextCommand

GrPict_OddByte:
  ldi r17, 0
  cpi GrCmd, 0
  brne GrPict_01
  cbi  PORTE, 2
  rjmp GrPict_02
GrPict_01:
  sbi  PORTE, 2
GrPict_02:
  VRAM_WR
  inc Vx
  cpi Vx, 240
  breq GrPict_03
  jmp GetNextCommand

GrPict_03:
  ldi Vx, 0
  dec Vy
  cpi Vy, 255
  breq GrPict_04
  jmp GetNextCommand
GrPict_04:
  ldi Vy, 0
  jmp GrState_DEFAULT

GrState_COLOR:
  cpi GrState, COLOR
  breq PC+2
  rjmp GrState_MOVE

  lsl  GrCmd
  lsl  GrCmd
  lsr  GrOldCmd
  ror  GrCmd
  lsr  GrOldCmd
  ror  GrCmd
  cbi  PORTE, 2
  lsr  GrOldCmd
  brcc Color_00
  sbi PORTE, 2
Color_00:
  out PORTB, GrCmd

  jmp GrState_DEFAULT

;------------------------------------------------------------------
GrState_MOVE:
  cpi GrState, MOVE
  breq PC+2
  rjmp GrState_PRINT_STRING

  sbrs GrOldCmd, 0
  rjmp GrMoveVy
  mov Vx, GrCmd
  out PORTA, Vx
  rjmp GrMovePset

GrMoveVy:
#if 1	;BUG FIX
  sbrs GrOldCmd, 1
#else
  sbrs GrOldCmd, 0
#endif
  rjmp GrMoveEnd
  mov Vy, GrCmd
  out PORTC, Vy

GrMovePset:
  sbrs GrOldCmd, 2
  rjmp GrMoveEnd
  VRAM_WR

GrMoveEnd:
  jmp GrState_DEFAULT

;------------------------------------------------------------------
GrState_PRINT_STRING:
  cpi GrState, PRINT_STRING
  breq GrL_00

GrState_PUT_CHAR:
  cpi GrState, PUT_CHAR
  breq PC+2
  rjmp GrState_DEFAULT
  clr GrState      ; GrState = STANDBY
  rjmp GrL_01


; Check if NULL
GrL_00:
  cpi GrCmd, NULL
  brne GrL_01
  rjmp GrState_DEFAULT
GrL_01:
	rcall	PutChar				;print GrCmd
GrL_02:
  jmp GetNextCommand
;------------------------------------------------------------------

GrState_DEFAULT:
  clr GrState      ; GrState = STANDBY

  ;#######################
  ;# State Machine END   #
  ;#######################

GetNextCommand:
	cli
#if MEMPTRS
	lds		XL,TailPtr
	lds		XH,TailPtr+1
#endif
	adiw	X,1					;increment queue tail ptr
	cpi		XH,HIGH(RxBufferEnd)
	brne	GRL_104
#if DEBUG
	tst		XL					;expected to be 0
	breq	GRL_103
	rjmp	PC
GRL_103:
#endif
	ldi		XH,HIGH(RxBuffer)
GRL_104:
#if MEMPTRS
	sts		TailPtr,XL
	sts		TailPtr+1,XH
#endif
	sei
	jmp		CommandLoop
  ;END while (RxReadPtr != RxWritePtr)


;##########################################################
;##                                         ###############
;## PUTCHAR - Character printing subroutine ###############
; IN:	GrCmd - char to print
;##                                         ###############
;##########################################################
PutChar:
  ldi	r18,FONT_WIDTH*8	;column counter
  ; Get Font8x8[GrCmd][0]
  ldi ZL,  LOW(Font8x8*2)
  ldi ZH, HIGH(Font8x8*2)
  ldi r17, FONT_WIDTH*FONT_HEIGH*8
  mov	r16,GrCmd
  mul r16, r17
  add ZL, R0					;table position
  adc ZH, R1
  mov r16, Vy					;Y bit position

GrLoop_01:
  ldi r19, FONT_HEIGH*8			;row counter
  elpm r17, Z+					;get bit pattern
GrLoop_02:
  lsr r17						;highest bit into carry
  brcc GrJump_01
  out PORTC, Vy					;bit was high
  cli
  cbi PORTE, 3					;toggle WR line
  sbi PORTE, 3
  sei
GrJump_01:
  inc Vy						;next Y position (row)
  cpi	Vy,160					;overflow?
  brlo	GrJump_03
  ldi	Vy,0					;to top row. No need to update Vx
GrJump_03:
  dec	r19						;next row
  brne GrLoop_02

  mov Vy, r16					;restore Y position
  inc Vx						;next X position
  cpi Vx, 240					;overflow?
  brlo GrJump_02				;no
  ldi Vx, 0						;wrap to next line
  subi Vy, (255-FONT_HEIGH*8)+1
  mov r16, Vy					;also update restore value
GrJump_02:
  out PORTA, Vx					;new memory address
  out PORTC, Vy

	dec  r18						;next column
	brne GrLoop_01
	ret

.UNDEF GrCmd
.UNDEF GrState
.UNDEF GrOldCmd


;##########################################################
;##                             ###########################
;## RS232 RECEIVE EVENT HANDLER ###########################
;##                             ###########################
;##########################################################
;
; Try to make this as short as possible since the Int7 interrupt
; may occur while we're processing this request. Delaying Int7
; handling delays releasing the SRAM bus, causing bus contention.
; Interrupt priorities would be useful here.
;
; Use 2 permanently reserved registers: RxFlags, RxData
;
; Registers X and Y are permanently reserved as Serial Input Buffer pointers
; MEMPTRS replaces that convention with memory based pointers.
;
IntUart_0_Rx:
	in		RxFlags,SREG
#if MEMPTRS
	push	r16
	push	YL
	push	YH
#endif
;
; Get Data from Rs232
;
	in RxData, UDR0
;	out UDR0, GrState
#if 0
; interpret all input as ascii characters to be displayed
;

	push	r17
	ldi		r17,PUT_CHAR
	rcall	Serial_In
	pop		r17

#endif
#if MEMPTRS
	lds		YL,HeadPtr
	lds		YH,HeadPtr+1
#endif
	st		Y+,RxData				;store and increment queue head ptr
	cpi		YH,HIGH(RxBufferEnd)
	brne	IntUart_0_L100
	ldi		YH,HIGH(RxBuffer)
IntUart_0_L100:
#if MEMPTRS
	lds		r16,TailPtr				;could be in operation!!!!
	lds		RxData,TailPtr+1
	cp		YH,RxData				;overflow test
	cpc		YL,r16
#else
	cp		YH,XH					;overflow test
	cpc		YL,XL
#endif
	brne	IntUart_0_Out
;
; input buffer overflow
; consider sending Xoff-Xon to the sender. Xoff when say < 256 bytes left, Xon when >256 bytes in buffer
;
#if DEBUG
	rcall	DebugL3				;error
#else
	sbiw	Y,1
#endif
IntUart_0_Out:
#if MEMPTRS
	sts		HeadPtr,YL
	sts		HeadPtr+1,YH
	pop		YH
	pop		YL
	pop		r16
#endif
	out		SREG,RxFlags
	reti

;##########################################################
;##                             ###########################
;## Serial In Queue HANDLER     ###########################
;##                             ###########################
; IN: r17
;
; USES: r17, Y
;
;##########################################################
Serial_In:
;
; Store Data in r17 to HeadPtr
; This code is always running with ints disabled
;
; Y points to first free position (head)
; X points to oldest entry (tail)
;
#if MEMPTRS
	lds		YL,HeadPtr
	lds		YH,HeadPtr+1
#endif
	st		Y+,r17				;store and increment queue head ptr
	cpi		YH,HIGH(RxBufferEnd)
	brne	Serial_InL100
#if DEBUG
	tst		YL					;expected to be 0
	breq	Serial_InL50
	rjmp	PC
Serial_InL50:
#endif
	ldi		YH,HIGH(RxBuffer)
Serial_InL100:
#if MEMPTRS
	lds		r16,TailPtr
	lds		r17,TailPtr+1
	cp		YH,r17				;overflow test
	cpc		YL,r16
#else
	cp		YH,XH				;overflow test
	cpc		YL,XL
#endif
	brne	Serial_Out
;
; input buffer overflow
; consider sending Xoff-Xon to the sender. Xoff when say < 256 bytes left, Xon when >256 bytes in buffer
;
#if DEBUG
	rcall	DebugL3				;error
#else
	sbiw	Y,1
#endif
Serial_Out:
#if MEMPTRS
	sts		HeadPtr,YL
	sts		HeadPtr+1,YH
#endif
	ret


;##########################################################
;##                     ###################################
;## FONTS & BITMAPS     ###################################
;##                     ###################################
;##########################################################
;##                     ###################################
;## Located in the      ###################################
;## Upper Page of Flash ###################################
;##                     ###################################
;##########################################################

.NOLIST
.CSEG
.ORG $8000

.INCLUDE "Font8x8.inc"
