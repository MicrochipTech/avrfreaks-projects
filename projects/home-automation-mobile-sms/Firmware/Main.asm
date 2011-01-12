; ####################################################################################################### ;
; #
; # © Portia CyberLogic.
; # www.portia-cyberlogic.com
; # mail@portia-cyberlogic.com
; # 
; ####################################################################################################### ;
; #
; # PROJECT INFORMATION:
; # Project	 name:		DMX512 to D54 converter.
; # Project version:	0.9.0.0
; #
; # File name:		Main.inc
; # File title:		Firmware main module include.
; # Target MCU:		ATmega32
; # Clock freq.:	Check MCU_CLOCK_FREQ in Main.inc.
; # File version:	1.0.0.0
; # Last modefied:	2010.12.09
; #
; # FILE DESCRIPTION:
; # Firmware main module.
; #
; # Routines:
; # 001:	IHReset: Reset Handler.
; # 002:	MainLoop: Main program loop
; #
; # FILE HISTORY:
; #
; # 2010.12.01
; # File creation.
; # By: Mahmoud Taher, portia@portia-cyberlogic.com
; #
; ####################################################################################################### ;

; ####################################################################################################### ;
; # Includes
; ####################################################################################################### ;

.include "Main.inc"

; ####################################################################################################### ;
; # Code defs.
; ####################################################################################################### ;

;Defs..
#define D54Buffer (DMXBuffer + 1) ;D54Buffer = DMXBuffer execluding start byte.

;Register names..
.def Flags = r25 ; General flags.
.def D54_State = r24 ; D54 state.
.def D54_cTS = r23 ; D54 current time slice.
.def D54_cChn = r22 ; D54 current channel.

; ####################################################################################################### ;
; # Code segment
; ####################################################################################################### ;

.cseg

.org 0

; ------------------------------------------------------------------------------------------------------- ;
; -  Interrupt vector:

.org 0x000 jmp ISRReset ; Reset Handler
.org 0x002 reti ; IRQ0 Handler
.org 0x004 reti ; IRQ1 Handler
.org 0x006 reti ; IRQ2 Handler
.org 0x008 reti ; Timer2 Compare Handler
.org 0x00A reti ; Timer2 Overflow Handler
.org 0x00C reti ; Timer1 Capture Handler
.org 0x00E reti ; Timer1 CompareA Handler
.org 0x010 reti ; Timer1 CompareB Handler
.org 0x012 reti ; Timer1 Overflow Handler
.org 0x014 jmp ISRTimer0Compare ; Timer0 Compare Handler
.org 0x016 reti ; Timer0 Overflow Handler
.org 0x018 reti ; SPI Transfer Complete Handler
.org 0x01A jmp ISRRXComplete ; USART RX Complete Handler
.org 0x01C reti ; UDR Empty Handler
.org 0x01E reti ; USART TX Complete Handler
.org 0x020 reti ; ADC Conversion Complete Handler
.org 0x022 reti ; EEPROM Ready Handler
.org 0x024 reti ; Analog Comparator Handler
.org 0x026 reti ; Two-wire Serial Interface Handler
.org 0x028 reti ; Store Program Memory Ready Handler
; ------------------------------------------------------------------------------------------------------- ;

; ------------------------------------------------------------------------------------------------------- ;
; - Strings:

;T_BOARD_SERIAL: .db 0, 0, 0, 0 ; 4 bytes serial no.
;T_BOARD_ID_PTR: .db T_BOARD_ID, 0
;T_BOARD_VER_PTR: .db T_BOARD_VER, 0
;T_HOST_GUID_PTR: .db T_HOST_GUID, 0
;T_COPY_RIGHT_PTR: .db T_COPY_RIGHT, 0
; ------------------------------------------------------------------------------------------------------- ;

; ------------------------------------------------------------------------------------------------------- ;
; - Routine:		Main program loop
; - Params:			Void
; - Uses:			void
; - Result:			Void
; - MCU Cycles:		<Infinit>
; - Descreption:	Main program loop
; - Notes:			Main program loop

Main:
	; Maintain status LED..
	sbrc Flags, F_DMX_OK ;(1) Skip if DMX != OK.
		sbi LED_S_PORT, LED_S_PIN ;(1) LED on.
	sbrs Flags, F_DMX_OK ;(1) Skip if DMX == OK.
		cbi LED_S_PORT, LED_S_PIN ;(1) LED off.

	jmp Main ;(3) Loop.
; ------------------------------------------------------------------------------------------------------- ;

; ------------------------------------------------------------------------------------------------------- ;
; - Routine:		DMXInit
; - Params:			Void
; - Uses:			r16
; - Result:			Void
; - MCU Cycles:		-
; - Descreption:	Initialize DMX reciption
; - Notes:			-

DMXInit:
	push r16 ;(2) Save register.
	;push r17
	
	;DMX buffer will be initialized by a ramp from 0 to 255 & so on..
	ldi r16, 0 ;(1) Will be 0 in the loop.
	;ldi r17, 64

	ldi XL, Low(DMXBuffer) ;(1) DMX buffer ptr low.
	ldi XH, High(DMXBuffer) ;(1) DMX buffer ptr hi.

	DMXI_CB_LOOP:
		;adc r16, r17 ;(1) Inc DMX byte value.

		st X+, r16 ;(2) Store DMX byte to DMX buffer.

		cpi XH, High(DMXBuffer + DMX_BUFFER_SIZE) ;(1) XH = end of DMX buffer hi.
		brne DMXI_CB_LOOP ;(1,2) XH != end of DMX buffer hi -> return.
		;XH = end of DMX buffer hi -> check YL.
		cpi XL, Low(DMXBuffer + DMX_BUFFER_SIZE) ;(1) XL = end of DMX buffer low.
		brne DMXI_CB_LOOP ;(1,2) XL != end of DMX buffer low -> less than (513 bytes sent), return.
		;XL = end of DMX buffer low -> overflow(513 bytes sent) -> Reset via FrameError.

	;Reset X ptr..
	ldi XL, Low(DMXBuffer) ;(1) DMX buffer ptr low.
	ldi XH, High(DMXBuffer) ;(1) DMX buffer ptr hi.

	; Initialize USART to receive DMX packets..
	;Set baud rate..
	ldi r16, high(DMX_UBBER_VALUE) ;(1)
	out UBRRH, r16 ; (1)
	ldi r16, low(DMX_UBBER_VALUE) ;(1)
	out UBRRL, r16 ; (1)
	;Set frame format: Data bits = 8, Stop bits = 2, Parity = 0..
	ldi r16, (1<<URSEL)|(3<<UCSZ0)|(1<<USBS) ;(1)
	out UCSRC, r16 ;(1)
	;Reset UDR..
	in r16, UDR ;(1)
	;Reset UCSRA..
	clr r16 ;(1)
	out UCSRA, r16 ;(1)
	;Enable receiver via inturrupt..
	sbi UCSRB, RXCIE ;(1) RX Complete Interrupt Enable.
	sbi UCSRB, RXEN ;(1) Receiver Enable.

	;Return..
	;pop r17
	pop r16 ;(2) Restore register.
	ret ;(4, 5) Return
; ------------------------------------------------------------------------------------------------------- ;

; ------------------------------------------------------------------------------------------------------- ;
; - Routine:		D54Init
; - Params:			Void
; - Uses:			r16
; - Result:			Void
; - MCU Cycles:		-
; - Descreption:	Initialize D54 transmition
; - Notes:			-

D54Init:
	push r16 ;(2) Save register.
	
	;Use Y as DMX channel data ptr..
	ldi YL, Low(D54Buffer) ;(1) DMX buffer ptr low.
	ldi YH, High(D54Buffer) ;(1) DMX buffer ptr hi.

	;Initialize data port for D54 data out..
	ldi r16, 255 ;(1) All pins out.
	out D54_DATA_DDR, r16 ;(1) Define port dir.
	clr r16 ;(1) Clear port out.
	out D54_DATA_PORT, r16 ; Set port out.

	;Initialize sync pin for D54 out..
	; SYNC signal is reverse logic 1 = Sync OFF, 0 = Sync ON..
	sbi D54_SYNC_DDR, D54_SYNC_DDR_BIT ;(1) Set pin dir as out.
	sbi D54_SYNC_PORT, D54_SYNC_PIN ;(1) Set pin out.

	;Initialize Timer0 CTC value, to be fired every D54_TIME_SAMPLE..
	ldi r16, D54_TIMER_CTC_VAL ;(1)
	out OCR0, r16 ;(1)
	; Enable CTC interrupt..
	in r16, TIMSK ;(1)
	sbr r16, (1<<OCIE0) ;(1)
	out TIMSK, r16 ;(1)
	;Initialize Timer0..
	ldi r16, (1<<WGM01)|(1<<CS00) ;(1) Clear Timer on Compare Match (CTC), No prescaling.
	out TCCR0, r16 ;(1)

	; Start..
	sbi D54_SYNC_PORT, D54_SYNC_PIN ;(1) SYNC off, pin = 1.
	
	ldi r16, 0 ;(1)
	out D54_DATA_PORT, r16 ; Out byte to port.

	ldi D54_cTS, D54_T_START ;(1) Load TS counter.
	ldi D54_State, D54_S_START ;(1) Default to idle.

	; Return..
	pop r16 ;(2) Restore register.
	ret ;(4, 5) Return
; ------------------------------------------------------------------------------------------------------- ;

; ------------------------------------------------------------------------------------------------------- ;
; - Routine:		ISRReset (Interrupt)
; - Params:			Void
; - Uses:			r16
; - Result:			Void
; - MCU Cycles:		?
; - Descreption:	Reset interrupt handler
; - Notes:			Reset interrupt handler

ISRReset:
	;Initialize stack
	ldi r16, low(RAMEND) ;(1)
	out SPL, r16 ; (1)
	ldi r16, high(RAMEND) ;(1)
	out SPH, r16 ;(1)

	;Initialize DMX reciption..
	call DMXInit ;(2)

	;Initialize D54 transmition..
	call D54Init ;(2)

	;Initialize status LED pin for out..
	sbi LED_S_DDR_REG, LED_S_DDR_BIT ;(1) Set LED pin dir as out.
	cbi LED_S_PORT, LED_S_PIN ;(1) LED off.

	;Enable interrupts
	sei ;(1)

	sbi LED_S_PORT, PINC0 ;(1) Sys OK LED on.

	;Goto main loop
	jmp Main ;(2)
; ------------------------------------------------------------------------------------------------------- ;

; ------------------------------------------------------------------------------------------------------- ;
; - Routine:		ISRRXComplete (Interrupt)
; - Params:			Void
; - Uses:			r16, r17
; - Result:			Void
; - MCU Cycles:		?
; - Descreption:	USART RX Complete Handler, DMX packet receiver.
; - Notes:			Reset interrupt handler

ISRRXComplete:
	;Save registers..
	push r16 ;(2)
	in r16, SREG ;(1)
	push r16 ;(2)
	push r17 ;(2)

	;Read USART status befor data..
	in r16, UCSRA ;(1) Get status register.
	in r17, UDR ;(1) Get data register

	;Frame error -> break, MAP..
	sbrc r16, FE ;(1) Check UCSRA for frame error.
	rjmp ISRRXC_FRAME_ERROR ;(2) Frame error -> reset via FrameError.
		
	st X+, r17 ;(2) Store DMX byte to DMX buffer.

	sbr Flags, (1<<F_DMX_OK) ;(1) Set DMX_OK flag.

	;Check buffer overflow..
	cpi XH, High(DMXBuffer + DMX_BUFFER_SIZE) ;(1) XH = end of DMX buffer hi.
	brne ISRRXC_RET ;(1,2) XH != end of DMX buffer hi -> return.
	;YH = end of DMX buffer hi -> check YL.
	cpi XL, Low(DMXBuffer + DMX_BUFFER_SIZE) ;(1) XL = end of DMX buffer low.
	brne ISRRXC_RET ;(1,2) XL != end of DMX buffer low -> less than (513 bytes sent), return.
	;XL = end of DMX buffer low -> overflow(513 bytes sent) -> Reset via FrameError.

 	ISRRXC_FRAME_ERROR:
		ldi XL, Low(DMXBuffer) ;(1) DMX buffer ptr low.
		ldi XH, High(DMXBuffer) ;(1) DMX buffer ptr hi.
		cbr Flags, (1<<F_DMX_OK) ;(1) Clear DMX_OK flag.

	ISRRXC_RET: ;Return.
		;Restore registers..
		pop r17 ;(2)
		pop r16 ;(2)
		out SREG, r16 ;(2)
		pop r16 ;(2)
		reti ;(4,5) Return.
; ------------------------------------------------------------------------------------------------------- ;

; ------------------------------------------------------------------------------------------------------- ;
; - Routine:		ISRTimer0Compare (Interrupt)
; - Params:			Void
; - Uses:			r16
; - Result:			Void
; - MCU Cycles:		?
; - Descreption:	Timer0 compare interrupt handler
; - Notes:			Sends D54 data.

ISRTimer0Compare:
	;Save registers..
	push r16 ;(2)
	in r16, SREG ;(1)
	push r16 ;(2)

	;Switch D54_State..
	cpi D54_State, D54_S_START ;(1) case D54_S_START
	breq ISRT0C_S_START ;(1,2)

	cpi D54_State, D54_S_SYNC ;(1) case D54_S_SYNC
	breq ISRT0C_S_SYNC ;(1,2)

	cpi D54_State, D54_S_BYTE ;(1) case D54_S_BYTE
	breq ISRT0C_S_BYTE ;(1,2)

	cpi D54_State, D54_S_EOF ;(1) case D54_S_EOF
	breq ISRT0C_S_EOF ;(1,2)

	;Default: SYNC out between bytes..
	ISRT0C_SYNC_OUT:
		clr r16 ;(1) Turn off data.
		out D54_DATA_PORT, r16 ; Set port out (Turn off data).

		;SYNC ON..
		cbi D54_SYNC_PORT, D54_SYNC_PIN ;(1) SYNC on, pin = 0.

		ldi D54_cTS, D54_T_SYNC ;(1) Load TS counter.
		ldi D54_State, D54_S_SYNC ;(1) Set state.

		jmp ISRT0C_RET ;(2) Return.

	ISRT0C_S_SYNC: ;D54_S_SYNC.
		dec D54_cTS ;(1) Dec. TS counter.
		brne ISRT0C_RET ;(1,2) Return if TS counter != 0.

		;SYNC time passed -> Turn off sync & out byte.
		sbi D54_SYNC_PORT, D54_SYNC_PIN ;(1) SYNC off, pin = 1.

		ld r16, Y+ ;(2) Get current bute.
		out D54_DATA_PORT, r16 ; Out byte to port.
		
		ldi D54_cTS, D54_T_BYTE ;(1) Load TS counter.
		ldi D54_State, D54_S_BYTE ;(1) Set state.
		
		jmp ISRT0C_RET ;(2) Return.

	ISRT0C_S_START: ;D54_S_START
		dec D54_cTS ;(1) Dec. TS counter.
		brne ISRT0C_RET ;(1,2) Return if TS counter != 0.

		;Redo from start next time..
		ldi YL, Low(D54Buffer) ;(1) DMX buffer ptr low.
		ldi YH, High(D54Buffer) ;(1) DMX buffer ptr hi.

		jmp ISRT0C_SYNC_OUT ;(2) SYNC out.

	ISRT0C_S_BYTE: ;D54_S_BYTE
		dec D54_cTS ;(1) Dec. TS counter.
		brne ISRT0C_RET ;(1,2) Return if TS counter != 0.

		;BYTE time passed -> If it wasn't the last byte -> SYNC between bytes to cont..
		cpi YH, High(D54Buffer + D54_CH_MAX) ;(1) YH = end of DMX buffer hi.
		brne ISRT0C_SYNC_OUT ;(1,2) XH != end of DMX buffer hi -> return.
		;YH = end of DMX buffer hi -> check YL.
		cpi YL, Low(D54Buffer + D54_CH_MAX) ;(1) XL = end of DMX buffer low.
		brne ISRT0C_SYNC_OUT ;(1,2) XL != end of DMX buffer low -> less than (513 bytes sent), return.
		;XL = end of DMX buffer low -> overflow(513 bytes sent) -> Reset via FrameError.

		;It was the last byte -> EOF..
		ISRT0C_S_BYTE_OVERFLOW: ;D54_CH_MAX allread sent -> redo from start..
			; Out EndOfFrame signal
			clr r16 ;(1) Turn off data.
			out D54_DATA_PORT, r16 ; Set port out (Turn off data).

			cbi D54_SYNC_PORT, D54_SYNC_PIN ;(1) SYNC on, pin = 0.

			ldi D54_cTS, D54_T_EOF ;(1) Load TS counter.
			ldi D54_State, D54_S_EOF ;(1) Set state.

			jmp ISRT0C_RET ;(2) Return.

	ISRT0C_S_EOF: ;D54_S_EOF
		dec D54_cTS ;(1) Dec. TS counter.
		brne ISRT0C_RET ;(1,2) Return if TS counter != 0.

		;Start..
		sbi D54_SYNC_PORT, D54_SYNC_PIN ;(1) SYNC off, pin = 1.
	
		ldi r16, 0 ;(1)
		out D54_DATA_PORT, r16 ; Out byte to port.

		ldi D54_cTS, D54_T_START ;(1) Load TS counter.
		ldi D54_State, D54_S_START ;(1) Default to idle.

		;jmp ISRT0C_SYNC_OUT ;(2) SYNC out.

	ISRT0C_RET: ;Return.
		;Restore registers..
		pop r16 ;(2)
		out SREG, r16 ;(2)
		pop r16 ;(2)
		reti ;(2)
; ------------------------------------------------------------------------------------------------------- ;

; ####################################################################################################### ;
; # Included code
; ####################################################################################################### ;

; ####################################################################################################### ;
; # Dynamic Data segment (SRAM)
; ####################################################################################################### ;

.dseg
.org SRAM_START
DMXBuffer: .BYTE DMX_BUFFER_SIZE ; DMX channel buffer.

; ####################################################################################################### ;
; # Static Data segment (EEPROM)
; ####################################################################################################### ;

.eseg
.org 0
T_FW_SERIAL: .db 0, 0, 0, 1 ; 4 bytes Firmware serial no.

T_FW_VER_PTR: .db T_FW_VER, 0 ;Firmware version.
T_FW_AUTHER_PTR: .db T_FW_AUTHER, 0 ;Firmware auther.
T_FW_COPYRIGHT_PTR: .db T_FW_COPYRIGHT, 0 ;Firmaware copyright
T_FW_WWW_PTR: .db T_FW_WWW, 0 ;Contact.

; ####################################################################################################### ;
; # EOF
; ####################################################################################################### ;
