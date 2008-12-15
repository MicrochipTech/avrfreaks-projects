
; Filename: ISD16_AT90S8515.s
; Version: 1.6 (13/3/2002)
; Copyright 2001-2002 Flash Microsystems
; Added Changes to INFO-BLOCK (DD 30/11/2001)
; Added "R" Repeat Send (DD 4/12/2001)
; Changed modes to allow sending specified number of bytes (CB 4/12/2001)
; Added Changes to INFO-BLOCK, IC family & chip codes (CB 13/12/2001)
; Added Changes to INFO-BLOCK, SRAM & EEPROM size in bytes (CB 13/12/2001)
; Added INT0 vector, ISD Signature & made ISD routine relocatable (CB 1/2/2002)
; Changed first byte of Info-Block to length (CB 15/2/2002)
; Changed Info-Block Version number to a hex value (CB 15/2/2002)
; Removed last byte of Info-Block, not needed (CB 15/2/2002)
; Added CHIP_TYPE, INT0_BIT label/definitions (DD 22/2/2002)
; Moved "SetBreakHere" into the ISD signature (CB 7/3/2002)
; Removed "to PC" echoes to speed up "PC only" mode (CB 13/3/2002)
; Info-Block has a switch for Custom SRAM & EEPROM sizes (CB 13/3/2002)
; SRAM & EEPROM addresses are now 24 bit (CB 13/3/2002)
; Request codes are values now, not bit flags (CB 13/3/2002)

;******************************************************************************
;* Studio+ ISD - THE WORLD'S SMALLEST, FASTEST, NO-ICE IN-SYSTEM DEBUGGER     *
;******************************************************************************

; Studio+ In-System-Debugger (ISD) for use in the Atmel AT90S8515.

; ** LICENSE AGREEMENT **
; This program is made available FREE of CHARGE, providing it is used ONLY
; with the Flash PC WINDOWS STUDIO+ Software and in accordance with
; the license agreement terms of that software (refer to www.flash.co.uk web
; site for further details). You may also try out this program with a standard
; Windows ASCII Terminal Program.
; You may modify this program only for the purposes of merging-in your own
; Interrupt Routine as indicated, changing the AVR CPU program parameters,
; you may also remove sections of this program where indicated
; in order to reduce its size, any other user modifications are
; NOT permitted without the written consent of the Copyright Owner.
; Flash may offer variants of this program including a version which
; can simulate a UART via any two spare I/O pins of the AVR CPU. This license
; agreement also applies to any variants of this program which offer a
; SIMULATED UART.
; This program is protected by International Copyright Laws. 

.include "ISDAVR.inc"

VERSION = 0x16			; Version number (1.6)

; ****** Customizable settings START ******
; Adjust these settings to tailor the ISD for your target CPU.
; Then add this ISD source to your ImageCraft project or
; create a new ISD library by editing the MAKELIBS batch file and running it.

CHIP_TYPE = CHIP_AT90S8515 ; Refer to ISDAVR.inc file for complete list
INT0_BIT = 2			; INT0 Port D bit no. (0 for ATmega64/103/128 etc)
						;                     (2 for AT90S8515/Mega8 etc)
INSTR_SET = 0			; 1: Use "JMP/CALL"   (for ATmega64/103/128 etc)
						; 0: Use "RJMP/RCALL" (for 8515/Mega8 etc)
CUSTOM_MEMORY = 0		; 1: Use Custom SRAMSIZE & EEPROMSIZE below
						; 0: Use default memory sizes for CPU
SRAMSIZE = 512			; Size of SRAM
EEPROMSIZE = 512		; Size of EEPROM
; ****** Customizable settings END ******

.if CUSTOM_MEMORY
INFOBLOCKSIZE = 12		; Info-Block size (12 bytes)
.else
INFOBLOCKSIZE = 6		; Info-Block size (6 bytes)
.endif

STACKBYTES = 10			; Number of stack bytes needed by this debugger

; ** CPU RESOURCES REQUIRED **
; Stack Space: 10 Bytes (Included in SP value displayed).
; Flash Memory: 378 bytes.
; The UART: The UART is used to Tx/Rx data to/from Studio+ running on your PC.

; ** USER'S INTERRUPT ROUTINE **
; Using Highest Priority Maskable Interrupt eg INT0. (Pin PD2 on the AT90S8515)
; The User can merge in their own ISR by placing the label "UserINT0::" just
; before it and a "ret" instruction to exit it.  The User's ISR should only
; push registers it uses onto the stack.  Also, they should put a 220 ohm
; resistor in series with the INT0 pin of the AVR CPU.
; To add an ISR to this file, comment out the call instruction where
; indicated and place the ISR code between the markers:
;    ****** USER Custom ISR ******
; To make it easy to add an ISR, all the code labels defined within
; the ISD are prefixed with "isd_". To avoid conflicts, make sure the
; labels in the ISR start with something else.

; ** CPU STATUS TRANSMISSION **
; CPU Status Information is transmitted as follows:
;	ISD -> PC  'D'
;	If INFOBLOCK is required:
;		PC  -> ISD 'I'
;		ISD -> PC  <INFOBLOCK, see below>
;	PC  -> ISD <STUDIO+ REQUEST CODE, see below>
;		See "STUDIO+ REQUEST CODES" for info format.
;	ISD -> PC  'E' (End Condition)
;	PC  -> ISD <Continue Code, see below>
;	If more data is required (Continue Code = 'R'):
;		Start again!
 
; ** INFOBLOCK FORMAT **
;    Size (number of bytes inc. this one)
;    ISD Version
;    IC family code
;    IC code
;    Custom memory flag (1 = custom memory sizes follow)
;       SRAM size (Low byte)
;       SRAM size (Mid byte)
;       SRAM size (High byte)
;       EEPROM size (Low byte)
;       EEPROM size (Mid byte)
;       EEPROM size (High byte)
;    Number of stack bytes needed by debugger

; ** STUDIO+ REQUEST CODES **
	SENDPC = 0			; Get the PC
		   	 			; ISD Sends PC
	SENDREGISTERS = 1	; Get the Registers
						; ISD Sends Registers
	SENDSFRS = 2		; Get SFRs
						; ISD Sends SFRs (SRAM 0x20->0x5F)
	SENDSRAM = 3		; Get SRAM
			   			; PC Sends SRAMSTARTLOW (ISD echos)
						; PC Sends SRAMSTARTHIGH (ISD echos)
						; PC Sends SRAMENDLOW (ISD echos)
						; PC Sends SRAMENDHIGH (ISD echos)
						; ISD Sends SRAM DATA
	SENDEEPROM = 4		; Get EEPROM
						; PC Sends EEPROMLOW (ISD echos)
						; PC Sends EEPROMHIGH (ISD echos) 
						; PC Sends EEPROMENDLOW (ISD echos)
						; PC Sends EEPROMENDHIGH (ISD echos)
						; ISD Sends EEPROM DATA

; ** CONTINUE CODES **
;	'S' = Single Step
;	'C' = Continue to next breakpoint
;	'R' = Repeat (send more data)

; I/O Registers' Addresses Definitions
	SREG = 0x3f
	DDRD = 0x11			; Port D Data Direction Reg
	PORTD = 0x12
	UCR = 0x0A			; UART Registers
	UDR = 0x0C
	USR = 0x0B
	SPL = 0x3D			; Stack pointer (Low Byte)
	SPH = 0x3E			; Stack Pointer (High Byte)

; EEPROM Registers' Addresses Definitions
	EEARL = 0x1E		; EEPROM Address Register (Low Byte)
	EEARH = 0x1F		; EEPROM Address Register (High Byte)
	EEDR = 0x1D			; EEPROM Data Register
	EECR = 0x1C			; EEPROM Control Register
; EEPROM Control Register Bit Definitions
	EERE = 0			; Read Enable

.area isd_vector(abs)
.if INSTR_SET
.org $4
	jmp isd_ISD			; INT0 vector addr applies to ATmega103
.else
.org $2
	rjmp isd_ISD		; INT0 vector addr applies to 8515/Mega8
.endif

.area text(rom, con, rel)
isd_ISD::	
		rjmp isd_PreserveRegs ; Bypass ISD Signature
.asciz "ISD"			; ISD Signature,
						; *MUST* be here (2 bytes from the
						; beginning of the ISD routine)

						; The Set Breakpoint Routine
						; Use:
						;    SetBreakHere();
						; in C code to set a hard breakpoint.
_SetBreakHere::
		sbi DDRD, INT0_BIT ; Set INT0 data direction to output
		cbi PORTD, INT0_BIT ; Force INT0 low, cause an interrupt
		ret

isd_PreserveRegs:
						; PC pushed automatically
		push r24		; Preserve r24
		push r25		; Preserve r25
		push r26		; Preserve X
		push r27
		push r28		; Preserve Y
		push r29
		push r30		; Preserve Z
		push r31
		in r24, SREG	; Preserve SREG in r24

; Check the cause of the interrupt (INT0),
; Was it external (user) or internal (code breakpoint)?
.if INSTR_SET
						; ATmega103: Check the data direction register
		sbic DDRD, INT0_BIT	; Skip if bit is clear, user's interrupt
.else
						; AT90S8515: Check the port
		sbis PORTD, INT0_BIT ; Skip if bit is set, user's interrupt
.endif
		rjmp isd_DebugRoutine ; Jump to start of this debug routine

;    ****** USER Custom ISR ******
.if INSTR_SET
		call _INT0_ISR	; ATmega103 Call User's ISR
.else
		rcall _INT0_ISR	; AT90S8515/Mega8 Call User's ISR
.endif
						; Comment the above lines out if you're not using INT0
						; or when adding an ISR to this file here.

						; Add ISR here!!

		rjmp isd_RestoreStack ; Restore the stack & return from interrupt

;    ****** USER Custom ISR ******

isd_DebugRoutine:		; Flash Microsystems ISD debugger
		ldi r28, 'D'
		rcall isd_Put28	; Send "D"
isd_WaitForPC:
		sbis USR, 7		; Wait for response from PC
		rjmp isd_WaitForPC
		
		sbis USR, 4		; Check for framing error
		rjmp isd_PCAcked ; No error, send data
		in r29, UDR		; read UDR to clear RXC
		rjmp isd_DebugRoutine ; Try again		
				
isd_PCAcked:
		in r29, UDR		; Get Studio+ request code
		cpi r29, 'I'	; Check if Studio+ wants an INFOBLOCK
		brne isd_TryJustPC ; If not, bypass INFOBLOCK output

		ldi r28, INFOBLOCKSIZE
		rcall isd_Put28	; Send Info-Block size
		ldi r28, VERSION
		rcall isd_Put28	; Send ISD version
		ldi r28, FAMILY_ATMELAVR
		rcall isd_Put28	; Send IC family code
		ldi r28, CHIP_TYPE
		rcall isd_Put28	; Send CPU code
		ldi r28, CUSTOM_MEMORY
		rcall isd_Put28	; Send "Custom Memory" flag
.if CUSTOM_MEMORY 
		ldi r28, SRAMSIZE&0xff
		rcall isd_Put28
		ldi r28, (SRAMSIZE>>8)&0xff
		rcall isd_Put28
		ldi r28, SRAMSIZE>>16
		rcall isd_Put28	; Send SRAM size
		ldi r28, EEPROMSIZE&0xff
		rcall isd_Put28
		ldi r28, (EEPROMSIZE>>8)&0xff
		rcall isd_Put28
		ldi r28, EEPROMSIZE>>16
		rcall isd_Put28	; Send EEPROM size
.endif
		ldi r28, STACKBYTES
		rcall isd_Put28	; Send number of stack bytes used 
		in r29, UDR		; Read the request code	

isd_TryJustPC:
		clt				; Clear T Bit (send PC flag)
		cpi r29, SENDPC	; If other data is wanted
		brne isd_TryRegs ; Don't set the PC flag
		set				; Set the PC flag
		rjmp isd_SendRegs ; Send PC only

isd_TryRegs:
		cpi r29, SENDREGISTERS ; Does Studio+ want the Registers?
		brne isd_TrySFRs ; If not, try SRAM

isd_SendRegs:
		in r26, SPL		; Set the pointer
		in r27, SPH
		adiw r26, 1

		ldi r31, 8		; Sending (or skipping for PC only) 8 bytes
isd_SendReg1:
		brts isd_SkipReg ; If sending the registers
		rcall isd_PutXP	; Send the registers on the stack (R31->R24)
isd_SkipReg:
		brtc isd_NextReg ; If only sending the PC
		ld r28, X+		; Get the pointer ready for sending the PC
isd_NextReg:
		dec r31
		cpi r31, 0		; Done yet?
		brne isd_SendReg1 ; If not, send the next register
		brts isd_SendPC	; branch if T bit SET to output PC Only
						; Send Registers R23 to R0
		ldi	r30, 0x18	; Z points to R24 
isd_SendReg2:
		ld	r28, -Z		; Get next register
		rcall isd_Put28	; Send it
		cpse r30, r31	; Done ?
		rjmp isd_SendReg2
		
		mov r28, r24	; Get SREG
		rcall isd_Put28	; Send SREG

		in	r28, SPL
		rcall isd_Put28	; Send SPL
		in	r28, SPH
		rcall isd_Put28	; Send SPH

isd_SendPC:
		rcall isd_PutXP	; Send PC (High byte), X = SP+1
		rcall isd_PutXP	; Send PC (Low byte),  X = SP+2
		
		rjmp isd_End	; Go to End Condition

isd_TrySFRs:
		cpi r29, SENDSFRS ; Does Studio+ want the SFRs?
		brne isd_TrySram ; If not, try SRAM
						; Send the SFRs
		ldi r26, 0x20	; Start of SFR block
		ldi r27, 0x00
		ldi r25, 0x00
		ldi r30, 0x60	; End of SFR block
		ldi r31, 0x00
		ldi r29, 0x00
		rjmp isd_SendSram

isd_TrySram:
		cpi r29, SENDSRAM ; Does Studio+ want the SRAM?
		brne isd_TryEeprom ; If not, try EEPROM
						; Send SRAM
		rcall isd_Get28	; Read from PC SRAM start address (lo)
		mov r26, r28	; Copy r28 to r26
		rcall isd_Put28	; Echo back to PC
		rcall isd_Get28	; Read from PC SRAM start address (mid)
		mov r27, r28	; Copy r28 to r27
		rcall isd_Put28	; Echo back to PC
		rcall isd_Get28	; Read from PC SRAM start address (hi)
		mov r25, r28	; Copy r28 to r25
		rcall isd_Put28	; Echo back to PC
		rcall isd_Get28	; Read from PC SRAM end address (lo)
		mov r30, r28	; Copy r28 to r30
		rcall isd_Put28	; Echo back to PC
		rcall isd_Get28	; Read from PC SRAM end address (mid)
		mov r31, r28	; Copy r28 to r31
		rcall isd_Put28	; Echo back to PC
		rcall isd_Get28	; Read from PC SRAM end address (hi)
		mov r29, r28	; Copy r28 to r29
		rcall isd_Put28	; Echo back to PC
isd_SendSram:
		cpi r26, 0x2C	; Cannot read UDR because we are using it!!
		brne isd_NextSram
		cpi r27, 0x00
		brne isd_NextSram
		ldi r28, '?'	
		rcall isd_Put28	; Send a '?' character in place of UDR
		inc r26
		sbrc r27, 0		; Skip (Bit 0 will always be clear here!!)
isd_NextSram:
		rcall isd_PutXP	; Send next SRAM byte
		cp r26, r30		; If not done
		brne isd_SendSram ; Send next byte
		cp r27, r31		; If not done
		brne isd_SendSram ; Send next byte
		cp r25, r29		; If not done
		brne isd_SendSram ; Send next byte
		
		rjmp isd_End	; Go to End Condition

isd_TryEeprom:
		cpi r29, SENDEEPROM ; Does Studio+ want EEPROM?
		brne isd_End	; If not, done!!
						; Send EEPROM
		rcall isd_Get28	; Read from PC EEPROM start address (lo)
		mov r26, r28	; Copy r28 to r26
		rcall isd_Put28	; Echo back to PC
		rcall isd_Get28	; Read from PC EEPROM start address (mid)
		mov r27, r28	; Copy r28 to r27
		rcall isd_Put28	; Echo back to PC
		rcall isd_Get28	; Read from PC EEPROM start address (hi)
		mov r25, r28	; Copy r28 to r25
		rcall isd_Put28	; Echo back to PC
		rcall isd_Get28	; Read from PC EEPROM end address (lo)
		mov r30, r28	; Copy r28 to r30
		rcall isd_Put28	; Echo back to PC
		rcall isd_Get28	; Read from PC EEPROM end address (mid)
		mov r31, r28	; Copy r28 to r31
		rcall isd_Put28	; Echo back to PC
		rcall isd_Get28	; Read from PC EEPROM end address (hi)
		mov r29, r28	; Copy r28 to r29
		rcall isd_Put28	; Echo back to PC
isd_SendEeprom:
		out	EEARH, r26
		out	EEARL, r27	; EEPROM read Address
		sbi	EECR, EERE	; Read EEPROM
		cbi	EECR, EERE
		in	r28, EEDR	; Get EEPROM value
		rcall isd_Put28	; Send it
		adiw r26,1
		cp r26, r30		; If not done
		brne isd_SendEeprom ; Send next byte
		cp r27, r31		; If not done
		brne isd_SendEeprom ; Send next byte
		cp r25, r29		; If not done
		brne isd_SendEeprom ; Send next byte

isd_End:				; End Condition
						; Send 'E' to Studio+
						; then wait for continue code: 'S', 'C' or 'R' 
		ldi r28, 'E'
		rcall isd_Put28	; Send 'E'
isd_WaitForPC2:
		sbis USR, 7		; Wait for Studio+ to send continue code
		rjmp isd_WaitForPC2   
		sbis USR, 4		; If no framing error
		rjmp isd_GotContCode ; Continue
		in r28, UDR		; Read UDR to clear RXC
		rjmp isd_End	; Try again		
isd_GotContCode:
		in r28, UDR		; Continue Code
		sbrs r28, 0		; If Studio+ wants more data (sent an "R")
		rjmp isd_DebugRoutine ; Loop back and repeat
		sbrc r28, 4
		rjmp isd_RestoreStack ; Don't reset INT0 if Single-Stepping

						; Continue to next break
		sbi PORTD, INT0_BIT	; Turn on INT0's Pull-Up
		cbi DDRD, INT0_BIT ; Set data direction to input

isd_RestoreStack:
		out SREG, r24	; Restore SREG
		pop r31			; Restore preserved registers
		pop r30
		pop r29
		pop r28
		pop r27
		pop r26
		pop r25
		pop r24
		reti			; Return from interrupt

						; Send Data Subroutines
isd_PutXP:
		ld r28, X+		; Get data via pointer (X)
isd_Put28:
		out UDR,r28		; Send it 
isd_WaitToSend:
		sbis USR, 5		; Wait for it to send
 		rjmp isd_WaitToSend
		ret				; Done

isd_Get28:
		sbis USR, 7		; Wait for the PC to send
		rjmp isd_Get28
		in r28, UDR		; Read from PC
		ret				; Done
