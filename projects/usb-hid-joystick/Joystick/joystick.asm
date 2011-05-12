;*******************************************************************
;                             USB Joystick
;*******************************************************************
;
; Author:               Gunter Hatze
; Date of creation:     17-APR-2011
; Version:				1.0
; Development Platform: AVR Studio 4, Version 4.13, Build 528
; Revision history:     None
;
;*******************************************************************
; This Assembler listing provides the functionality required to con-
; vert an anaolg style Joystick (CH Flightstick Pro) to a USB version.
; It is a 4 button joystick with a throttle.
;
; The AT90USB162 clock frequency is 16MHz, the
; instruction cycle time is therefor 62.5ns.
;
;*******************************************************************
;
.nolist
;
.INCLUDE	"usb162def.inc"		; Header File for AT90USB162
.list
;
;***********************  PORTB, PORTC, PORTD Reset pin definitions

;Note: 0= Pin LOW, 1= Pin HIGH

.equ	bres_pb = 0x01  ; Port bits for PortB after Reset
                        ; Set: AD_CE
                        ; Cleared: All others
.equ	bres_pc = 0x30  ; Port bits for PortC after Reset
                        ; Set: PC4 (PU_EN), PC5 (TP1)
                        ; Cleared: All others
.equ	bres_pd = 0x00  ; Port bits for PortD after Reset
                        ; Set: None
                        ; Cleared: All 
;***********************  End of PORTX Reset definitions
;
;
;***********************  PORTB, PORTC, PORTD Reset direction definitions

;Note: 0= Input, 1= Output

.equ	bdir_pb = 0xB7  ; Port directions for PortB after Reset
                        ; Inputs: PB3 (MISO), PB6 (TDI)
                        ; Outputs: All others
.equ	bdir_pc = 0x34  ; Port directions for PortC after Reset
                        ; Inputs: All others
                        ; Outputs: PC2 (LED), PC4 (PU_EN), PC5 (TP1)
.equ	bdir_pd = 0x00  ; Port directions for PortD after Reset
                        ; Inputs: All
                        ; Outputs: None
;***********************  End of PORTX Reset Direction Definitions
;
;
;***********************  Constant definitions
.equ	b_buf = 0x60	; 96 byte USB Data Buffer
.equ	ep0_fifo = 0x08	; 8 byte FIFO for EP0
.equ	ep1_fifo = 0x08	; 8 byte FIFO for EP1
.equ	tmr0_on = 0x04	; Timer0 run value
.equ	tmr0_off = 0x00	; Timer0 stop value
;***********************  End of Constant definitions
;
;
;***********************  Constant definitions for TLC0834
.equ	a2d_en = 0x00 	; Sets CS line of A/D LOW through Port Expander
.equ	a2d_dis = 0x10 	; Sets CS line of A/D HIGH through Port Expander
.equ	ad_ch0 = 0x18 	; Address of CH0, single ended measurement
.equ	ad_ch1 = 0x1C 	; Address of CH1, single ended measurement
.equ	ad_ch2 = 0x1A 	; Address of CH2, single ended measurement
.equ	ad_ch3 = 0x1E 	; Address of CH3, single ended measurement
;***********************  End of Constants definitions for TLC0834
;
;
.DSEG
;*************** Variables used by the bootloader:
msg:  
	.BYTE	1		; Set to one if a message needs to be processed 
a2d_ch0:  
	.BYTE	1		; Result of A/D channel 0 
a2d_ch1:  
	.BYTE	1		; Result of A/D channel 1 
a2d_ch2:  
	.BYTE	1		; Result of A/D channel 2 
a2d_ch3:  
	.BYTE	1		; Result of A/D channel 3 
buttons:
	.BYTE	1		; Joystick buttons.
update_joy:
	.BYTE	1		; 1 if joystick measurements need to be polled.
joy_change:
	.BYTE	1		; 1 if joystick measurements changed, 0 otherwise.
en_txini:
	.BYTE	1		; 1 if TXINI Int. needs to be enabled, 0 otherwise.
bbuf_in_ptr:  
	.BYTE	1		; Offset pointer into boot_buf_in
bbuf_out_ptr:  
	.BYTE	1		; Offset pointer into boot_buf_out
num_out:  
	.BYTE	1		; Number of bytes (still) to receive (direction OUT)
num_in:  
	.BYTE	1		; Number of bytes still to send (direction IN)
boot_buf_in:
	.BYTE 	b_buf	; 96 byte USB IN Data buffer (for Device to Host Xfers)
boot_buf_out:
	.BYTE 	b_buf	; 96 byte USB OUT Data buffer (for Host to Device Xfers)
;*************** End of Variables used by the bootloader
 

;*************** Variables used for USB:
dev_status:  
	.BYTE	1		; Device status, as follows:
					;	
					;	bit0:			1 if device in Suspend mode, 0 otherwise
					;	bit1:			1 if device busy, 0 otherwise
					;	bit2:			0 if device in Address state, 1 in 
					;					Configured state
					;	bit3:			1 if device has been issued a bus reset,
					;					0 otherwise
					;	bits 4 to 7:	Reserved

ep0_status:
	.BYTE	1		; Endpoint 0 status, as follows:
					;	
					;	bit0:			Reserved
					;	bit1:			Reserved
					;	bit2:			1 if Endpoint is waiting for more data
					;					from host, 0 otherwise
					;	bit3:			1 if Endpoint is waiting to send more data
					;					to the host, 0 otherwise
					;	bit4:			1 if Endpoint is ready to send ZLP,
					;					0 otherwise
					;	bit5:			1 if Endpoint is waiting to ACK a Control Read
					;					Request by ACKing RXOUTI, 0 otherwise
					;	bit6:			1 if Endpoint is waiting to latch new address,
					;					0 otherwise
					;	bit 7:			1 if we are waiting for a NAKOUTI, 0 otherwise

ep1_status:
	.BYTE	1		; Endpoint 1 (IN, Interrupt) status, as follows:
					;	
					;	bit0:			1 if Endpoint is halted, 0 otherwise
					;	bit1:			1 if Endpoint is stalled, 0 otherwise
					;	bit2:			Reserved
					;	bit3:			1 if Endpoint is waiting to send more data
					;					to the host, 0 otherwise
					;	bit4:			1 if Endpoint is ready to send ZLP,
					;					0 otherwise
					;	bits 5 to 7:	Reserved


stp_req_type:  
	.BYTE	1		; Setup Request Type (byte 0 of Setup Packet) 
stp_request:  
	.BYTE	1		; Setup Request Value (byte 1 of Setup Packet) 
stp_val_lo:
	.BYTE	1		; Low Byte of Setup Value word (byte 2 of Setup Packet) 
stp_val_hi:
	.BYTE	1		; High Byte of Setup Value word (byte 3 of Setup Packet)
stp_index_lo:
	.BYTE	1		; Low Byte of Setup Index word (byte 4 of Setup Packet) 
stp_index_hi:
	.BYTE	1		; High Byte of Setup Index word (byte 5 of Setup Packet) 
stp_length_lo:
	.BYTE	1		; Low Byte of Setup Length word (byte 6 of Setup Packet) 
stp_length_hi:
	.BYTE	1		; High Byte of Setup Length word (byte 7 of Setup Packet) 
ep_number:
	.BYTE	1		; Number of currently selected Endpoint (0 to 3) 
fifo_size:
	.BYTE	1		; Maximum FIFO size of currently selected Endpoint 
;*************** End of Variables used for USB
; 
;
;*************** Macros:
.MACRO set_suspend
	lds		r16, dev_status
	ori		r16, 0x01			; Signal Suspend mode
	sts		dev_status, r16
.ENDMACRO

.MACRO clr_suspend
	lds		r16, dev_status
	andi	r16, 0xFE			; Signal Normal mode
	sts		dev_status, r16
.ENDMACRO

.MACRO set_busy
	lds		r16, dev_status
	ori		r16, 0x02			; Signal Busy mode
	sts		dev_status, r16
.ENDMACRO

.MACRO clr_busy
	lds		r16, dev_status
	andi	r16, 0xFD			; Signal Not-Busy mode
	sts		dev_status, r16
.ENDMACRO

.MACRO clr_fifocon
	lds		r16, UEINTX
	andi	r16, 0x7F
	sts		UEINTX, r16			; Clear FIFO Has Data Int. Flag.
.ENDMACRO

.MACRO clr_nakini
	lds		r16, UEINTX
	andi	r16, 0xBF			; Clear NAKINI Interrupt flag
	sts		UEINTX, r16
.ENDMACRO
	
.MACRO clr_nakouti
	lds		r16, UEINTX
	andi	r16, 0xEF			; Clear NAKOUTI Interrupt flag
	sts		UEINTX, r16
.ENDMACRO
	
.MACRO clr_rxstpi
	lds		r16, UEINTX
	andi	r16, 0xF7
	sts		UEINTX, r16			; Clear Received Setup Int. Flag.
.ENDMACRO

.MACRO clr_rxouti
	lds		r16, UEINTX
	andi	r16, 0xFB
	sts		UEINTX, r16			; Clear Received Data Int. Flag.
.ENDMACRO

.MACRO clr_stalledi
	lds		r16, UEINTX
	andi	r16, 0xFD
	sts		UEINTX, r16			; Clear Stalled Int. Flag.
.ENDMACRO

.MACRO clr_txini
	lds		r16, UEINTX
	andi	r16, 0xFE			; Clear TXINI interrupt flag
	sts		UEINTX, r16
.ENDMACRO

.MACRO enable_txini
	cli
	lds		r16, UEIENX
	ori		r16, 0x01
	sts		UEIENX, r16				; Enable TXINI interrupt
	sei
.ENDMACRO

.MACRO disable_txini
	cli
	lds		r16, UEIENX
	andi	r16, 0xFE
	sts		UEIENX, r16				; Disable TXINI interrupt
	sei
.ENDMACRO

.MACRO enable_tmr0
	clr		r16
	out		TCNT0, r16
	ldi		r16, tmr0_on
	out		TCCR0B, r16				; Enable Timer0
.ENDMACRO

.MACRO init_ep0_tx_status
	clr		r16
	sts		ep0_status, r16
.ENDMACRO

;*************** End of Macros
 

.CSEG
.ORG	LARGEBOOTSTART			; Address $1800

;****************************************************************************
;**********************  Start of bootloader section  ***********************
;****************************************************************************
; This is the entry point for the bootloader. Note that program execution
; begins here by default, as the fuse BOOTRST is set to active (which means
; the reset vector is at the beginning of the bootloader section). The
; first instruction at the reset vector will be a jump, as the Interrupt
; Vector Table (IVT) will be relocated to the Boot sector as well. In order
; not to overwrite the IVT, a jump over this table is required at the reset
; vector address. 
;
boot_start:	
;***********************  Start of Interrupt Vector table
; Note: Vectors are spaced two words apart. The rjmp instruction
; 	requires only one word. The second word is a padding word. Should
; 	the MCU erroneously reach any odd address, it will jump back to the
;	beginning of the boot section.

	rjmp	bootload		; Jump over relocated IVT
	rjmp	boot_start		; Re-boot		
	reti 					; IRQ0 Handler
	rjmp	boot_start		; Re-boot		
	reti 					; IRQ1 Handler
	rjmp	boot_start		; Re-boot		
	reti 					; IRQ2 Handler
	rjmp	boot_start		; Re-boot		
	reti 					; IRQ3 Handler
	rjmp	boot_start		; Re-boot		
	reti 					; IRQ4 Handler
	rjmp	boot_start		; Re-boot		
	reti 					; IRQ5 Handler
	rjmp	boot_start		; Re-boot		
	reti 					; IRQ6 Handler
	rjmp	boot_start		; Re-boot		
	reti 					; IRQ7 Handler
	rjmp	boot_start		; Re-boot		
	reti 					; PCINT0 Handler
	rjmp	boot_start		; Re-boot		
	reti 					; PCINT1 Handler
	rjmp	boot_start		; Re-boot		
	rjmp	usb_gen			; USB General Handler
	rjmp	boot_start		; Re-boot		
	rjmp	usb_ep			; USB Endpoint Handler
	rjmp	boot_start		; Re-boot		
	reti 					; Watchdog Timer Interrupt Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; Timer1 Capture Event Handler
	rjmp	boot_start		; Re-boot		
	reti					; Timer1 Compare Match A Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; Timer1 Compare Match B Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; Timer1 Compare Match C Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; Timer1 Overflow Handler
	rjmp	boot_start		; Re-boot		
	rjmp 	tim0_oca		; Timer0 Compare Match A Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; Timer0 Compare Match B Handler
	rjmp	boot_start		; Re-boot		
	reti					; Timer0 Overflow Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; SPI Serial Transfer Complete Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; USART1 Rx Complete Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; USART1 Data Register Empty Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; USART1 Tx Complete Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; Analog Comparator Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; EEPROM Ready Handler
	rjmp	boot_start		; Re-boot		
	reti 		 			; Store Program Memory Ready Handler
	rjmp	boot_start		; Re-boot		
;***********************  End of Interrupt Vector table
;
;
;
.ORG	LARGEBOOTSTART + 0x040	; Address $1840

;*********************** Entry point for bootloader
bootload:
	ldi		r16,low(RAMEND)
	out		SPL,r16				; Set stack pointer to last internal RAM location
	ldi		r16,high(RAMEND)
	out		SPH,r16


; Relocate IVT to Boot section:
	ldi 	r16, (1<<IVCE)
	out 	MCUCR, r16
	ldi 	r16, (1<<IVSEL)
	out 	MCUCR, r16


; Disable Watchdog:
	clr		r16
	sts		WDTCSR, r16

; Set ports:
	ldi		r16, bres_pb
	out		PORTB, r16
	ldi		r16, bdir_pb
	out		DDRB, r16

	ldi		r16, bres_pc
	out		PORTC, r16
	ldi		r16, bdir_pc
	out		DDRC, r16

	ldi		r16, bres_pd
	out		PORTD, r16
	ldi		r16, bdir_pd
	out		DDRD, r16

; Set up SPI port:
	ldi		r16, 0x52
	out		SPCR, r16			; Master Mode, Tx on rising edge, Rx on falling
								; edge, MSB first, clock rate = fosc/32 = 500kHz
	ldi		r16, 0x01
	out		SPSR, r16			; Select 2X speed
	
; Set up Timer0:
	ldi		r16, 0x02
	out		TCCR0A, r16			
	ldi		r16, tmr0_off
	out		TCCR0B, r16			; CTC Mode, 2ms time period.
	clr		r16
	out		TCNT0, r16
	ldi		r16, 0x7D			; 125 decimal
	out		OCR0A, r16
	ldi		r16, 0x02
	sts		TIMSK0, r16			; Timer0 CMP A INT enabled

; Get initial state of Joystick:
	rcall	meas_joy

; Set up Power Mode:
	clr		r16
	sts		PRR1, r16			; USB Power enabled

; Set up USB PLL:
	ldi		r16, 0x06
	out		PLLCSR, r16			; Divide by two, enable PLL

; Check if PLL has locked:
boot1:
	in		r16, PLLCSR
	sbrs	r16, PLOCK			; Wait for PLL to lock
	rjmp	boot1

; Disable USB interface:
	ldi		r16, 0x20
	sts		USBCON, r16			; Disable USB Controller, disable clock


; Note: for some unknown reason, the AT90USB does not like it when the Controller AND
;		the USB clock are both enabled at the SAME time. The safe way to do this is
;		to first enable the Controller, then the USB clock.


; Enable USB Controller:
	ldi		r16, 0xA0
	sts		USBCON, r16			; Enable only USB Controller, not also the clock!!!
	nop
	nop

; Enable USB Clock:
	ldi		r16, 0x80
	sts		USBCON, r16

; Configure Endpoints:
	rcall	config_ep

;Enable USB General Interrupts:
; Note:	Currently, only the USB End of Reset and Suspend Interrupts are enabled. For unknown
;		reasons, enabling the Wakeup Interrupt leads to frequent, continuous reset
;		of the MCU. The workaround is disable the WAKEUP Interrupt, and to to poll the
;		WAKEUP Interrupt flag in the main loop of the bootloader.

	ldi		r16, 0x29			; Enabled interrupts: End of Resume, End of Reset, Suspend
	sts		UDIEN, r16

;Attach USB device, and disable CPU reset on USB Reset:
	clr		r16
	sts		UDCON, r16

;Clear pending Interrupts:
	ldi		r16, 0x00
	sts		UDINT, r16
	nop

; Initialize variables:
	clr		r16
	sts		bbuf_in_ptr, r16  
	sts		bbuf_out_ptr, r16  
	sts		num_out, r16  
	sts		num_in, r16  
	sts		dev_status, r16
	sts		msg, r16			; No message
	sts		update_joy, r16
	sts		joy_change, r16
	sei							; Enable GLOBAL INT.

;******************** Bootloader Main loop
boot_l:	
	lds		r16, UDINT
	andi	r16, 0x10			; Isolate WAKEUP bit
	tst		r16
	breq	boot11
	rcall	usb_wakeup

boot11:
	lds		r16, msg
	tst		r16
	breq	boot12
	rcall	parse
	rjmp	boot_l

boot12:
	lds		r16, update_joy		; Set to 1 in Timer0 CTC Interrupt routine (every 2ms)
	tst		r16					; If not zero, then send joystick data
	breq	boot_l
	clr		r16
	sts		update_joy, r16
	rcall	meas_joy

	lds		r16, joy_change
	tst		r16
	brne	boot13			; Joystick data has changed
	rjmp	boot_l

boot13:
	ldi		r16, 0x01
	sts		UENUM, r16		; Select Endpoint 1
	sts		ep_number, r16
	clr_txini
	ldi		XH, high(boot_buf_in)		; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)
	ldi		r16, 0x04					; Report length is 4 bytes
	sts		num_in, r16
	lds		r16, a2d_ch2			; Throttle
	st		X+, r16					; Store byte
	lds		r16, a2d_ch0			; X position
	st		X+, r16					; Store byte
	lds		r16, a2d_ch1			; Y position
	st		X+, r16					; Store byte
	lds		r16, buttons			; Buttons
	st		X+, r16					; Store byte
	clr		r16
	sts		bbuf_in_ptr, r16
	inc		r16
	sts		ep_number, r16
	rcall	usb_send_data
	rjmp	boot_l

; Note: The TXINI interrupt for Endpoint 1 (EP1) is disabled. The AT90USB162
;		continuously calls this interrupt if it is enabled, thereby bogging
;		down the firmware. However, we only want to send data if something 
;		has changed. The A/D for the joystick is therefor polled every two
;		milliseconds. If something has changed, then the new data is written
;		to the Tx buffer of EP1, and sent on the next IN token from the host.
;		If there is no new data in the EP1 TX buffer, and the host sends an
;		IN token, then the AT90USB162 hardware will automatically respond
;		with a NAK.

;******************** End of Bootloader Main loop
;
;
;***********************  Timer0 Interrupt Routine 
;
tim0_oca:
	in		r25, SREG
	push	r25				; Context saving of Status register
	ldi		r25, 0x01
	sts		update_joy, r25		; Check joystick data
	pop		r25
	out		SREG, r25
	reti
;*********************  End of Timer0 Interrupt Routine
;
;
;*********************** USB Interrupt Handlers
;
;*** General USB Interrupt Handler
usb_gen:
	in		r25, SREG
	push	r25					; Context saving of Status register
	push	r16
	push	r20
	lds		r16, UDINT
	mov		r20, r16		
	andi	r16, 0x01			; Check for Suspend flag
	tst		r16
	breq	ugen1
	rcall	usb_suspend
	rjmp	ugen_e

ugen1:
	mov		r16, r20
	andi	r16, 0x08			; Check for End of Reset flag
	tst		r16
	breq	ugen_2
	clr		r16					; Clear bit3 (End of Reset flag)
	sts		UDINT, r16
	lds		r16, dev_status
	ori		r16, 0x08
	sts		dev_status, r16		; Signal that the device has been reset
	clr		r16
	sts		bbuf_in_ptr, r16  
	sts		bbuf_out_ptr, r16  
	sts		num_out, r16  
	sts		num_in, r16  
	rcall	config_ep			; Configure Endpoints
	clr		r16
	sts		UDINT, r16			; Clear pending interrupts
	sts		UDCON, r16
	rjmp	ugen_e

ugen_2:
	mov		r16, r20
	andi	r16, 0x20			; Check for End of Resume flag
	tst		r16
	breq	ugen_e
	clr		r16					; Clear all bits
	sts		UDINT, r16
	lds		r16, dev_status
	andi	r16, 0xFE
	sts		dev_status, r16		; Signal that the device is not in suspend mode

ugen_e:
	pop		r20
	pop		r16
	pop		r25
	out		SREG, r25
	reti
;*** End of General USB Interrupt Handler
;
;
;*** USB Endpoint Interrupt Handler
usb_ep:
	in		r25, SREG
	push	r25				; Context saving of Status register
	push	r16
	push	r17
	lds		r16, dev_status
	andi	r16, 0x08		; Isolate Bus reset bit
	tst		r16
	brne	uep4
	rjmp	uep_e			; No reset yet - do not respond

uep4:
	lds		r16, UEINT
	mov		r17, r16		
	andi	r16, 0x01		; Check for Endpoint 0 Interrupt
	tst		r16
	breq	uep_e
	ldi		r16, 0x00
	sts		UENUM, r16		; Select Endpoint 0
	sts		ep_number, r16
	ldi		r16, ep0_fifo
	sts		fifo_size, r16
	rcall	proc_ep0		; Process the Endpoint

uep_e:
	pop		r17
	pop		r16
	pop		r25
	out		SREG, r25
	reti
;*** End of USB Endpoint Interrupt Handler
;
;******************** End of USB Interrupt Handlers
;
;
;*** Process Endpoint 0 Interrupt
proc_ep0:
	lds		r16, UEINTX
	andi	r16, 0x08		; Check for Setup Request
	tst		r16
	breq	proc_ep0_1
	rcall	proc_ep0_su		; Process the Setup request
	rjmp	proc_ep0_e

proc_ep0_1:
	lds		r16, UEINTX
	andi	r16, 0x04		; Check for Rx Data
	tst		r16
	breq	proc_ep0_3
	rcall	proc_ep0_rx		; Process the received data
	rjmp	proc_ep0_e
	
proc_ep0_3:
	lds		r16, UEINTX
	andi	r16, 0x10		; Check for NAKOUT Interrupt
	tst		r16
	breq	proc_ep0_4
	lds		r16, ep0_status
	andi	r16, 0x80		; Are we expecting a NAKOUTI?
	tst		r16
	breq	proc_ep0_4
	rcall	proc_ep0_nakout	; Process the Interrupt
	rjmp	proc_ep0_e

proc_ep0_4:
	lds		r16, UEINTX
	andi	r16, 0x01		; Check for TXINI Interrupt
	tst		r16
	breq	proc_ep0_e
	disable_txini

; Are we waiting to send a ZLP?:
	lds		r16, ep0_status
	andi	r16, 0x10				; Check bit4
	breq	proc_ep0_4a
	
; Yes:
	lds		r16, ep0_status
	andi	r16, 0x27				; Clear bits 3, 4
	ori		r16, 0x80				; Set bit 5 (waiting for NAKOUTI)
	sts		ep0_status, r16
	clr_txini						; Send ZLP
	rjmp	proc_ep0_e

; Are we are waiting to latch the new Device Address?:
proc_ep0_4a:
	lds		r16, ep0_status
	andi	r16, 0x40
	tst		r16
	breq	proc_ep0_5

; Yes, we are:
	lds		r16, ep0_status
	andi	r16, 0xBF			; Clear bit 6
	sts		ep0_status, r16
	lds		r16, UDADDR
	ori		r16, 0x80
	sts		UDADDR, r16
	disable_txini		
	nop
	rjmp	proc_ep0_e

proc_ep0_5:
	lds		r16, ep0_status
	andi	r16, 0x08			; Check bit3 (are we waiting to send more data to the host?)
	tst		r16
	breq	proc_ep0_e

; Yes:
	lds		r16, ep0_status
	andi	r16, 0xF7			; Clear bit 3
	sts		ep0_status, r16
	rcall	usb_send_data

proc_ep0_e:
	ret
;*** End of Process Endpoint 0 Interrupt
;
;
;*** Process Endpoint 0 Setup Request (8 byte packet)
; Note:	Setup packets have a fixed length of 8 bytes.

proc_ep0_su:
	cli
	ldi		YH, high(boot_buf_out)	; Rx receive buffer and Tx Transmit buffer
	ldi		YL, low(boot_buf_out)

	lds		r17, UEBCLX			; Number of bytes in the FIFO
proc_ep0_su_lp1:
	lds		r16, UEDATX
	st		Y+, r16
	dec		r17
	brne	proc_ep0_su_lp1
	ldi		r16, 0x01
	sts		msg, r16			; Signals that a Setup message is ready
	clr_rxstpi
	sei
	ret
;*** End of Process Endpoint Setup Request
;
;	
;*** Process Endpoint 0 Rx Event
proc_ep0_rx:
; Are we are waiting to ACK a Control Read Request by ACKing RXOUTI?:
	cli
	lds		r16, ep0_status
	andi	r16, 0x20
	tst		r16
	breq	proc_ep0_rx1

; Yes, we are:
	lds		r16, ep0_status
	andi	r16, 0x07			; Clear bits 3 to 7
	sts		ep0_status, r16
	clr_nakouti					; Clear NAKOUTI flag
	rjmp	proc_ep0_rxe


; Are we are waiting for the rest of a receive message? If not, reset
; buffer pointer, and read data.
proc_ep0_rx1:
	lds		r16, ep0_status
	andi	r16, 0x04			; Are we waiting for more data?
	tst		r16
	breq	proc_ep0_rx2

; Yes, we are waiting for the rest of a Receive message:
	ldi		YH, high(boot_buf_out)	; Rx receive buffer and Tx Transmit buffer
	ldi		YL, low(boot_buf_out)
	lds		r18, bbuf_out_ptr
	mov		r19, r18
	add		YL, r18					; Add offset to Y
	clr		r18
	adc		YH, r18
	lds		r17, UEBCLX			; Number of bytes received in the FIFO
	mov		r18, r19
	lds		r19, num_out
	lds		r20, fifo_size
	cp		r17, r20
	breq	proc_ep0_rx1_l1		; Buffer is full --> there may be more data
	lds		r16, ep0_status
	andi	r16, 0xFB			; We are NOT waiting for more Receive data
	ori		r16, 0x10			; We are expecting to send a ZLP
	sts		ep0_status, r16

proc_ep0_rx1_l1:
	lds		r16, UEDATX
	st		Y+, r16
	inc		r18
	inc		r19
	dec		r17
	brne	proc_ep0_rx1_l1

	sts		num_out, r19
	sts		bbuf_out_ptr, r18
	rjmp	proc_ep0_rxe

; No, we are not waiting for the rest of a Receive message:
proc_ep0_rx2:
	clr		r16
	sts		num_out, r16
	sts		bbuf_out_ptr, r16
	lds		r16, ep0_status
	andi	r16, 0xEB				; We are NOT waiting for more Receive data, and
									; we are not expecting to send a ZLP.
	sts		ep0_status, r16
	ldi		YH, high(boot_buf_out)	; Rx receive buffer
	ldi		YL, low(boot_buf_out)
	clr		r18
	lds		r17, UEBCLX				; Number of bytes received in the FIFO
	lds		r20, fifo_size
	cp		r17, r20
	brlo	proc_ep0_rx2_l1
	lds		r16, ep0_status
	ori		r16, 0x04			; Assume we are waiting for more data
	sts		ep0_status, r16

proc_ep0_rx2_l1:
	lds		r16, UEDATX			; Read byte from FIFO buffer
	st		Y+, r16				; Store byte
	inc		r18
	dec		r17
	brne	proc_ep0_rx2_l1

	sts		num_out, r18
	sts		bbuf_out_ptr, r18

proc_ep0_rxe:
	clr_rxouti					; Clear Received Out Data Interrupt Flag
	sei
	ret
;*** End of Process Endpoint 0 Rx Event
;
;
;*** Process Endpoint 0 NAKINI Interrupt
; The NAKIN Interrupt occurs after the Host has sent all the OUT data, and it
; now moves to the STATUS phase by issuing an IN Token to the device. The
; AT90USB162 Controller will issue a NAKIN handshake to the Host, and trigger
; the interrupt. This means that the Host is waiting for an ACK from the device,
; which is accomplished by clearing the TXINI flag (thereby sending a ZLP).

proc_ep0_nakin:
	cli
	lds		r16, ep0_status
	andi	r16, 0xEB				; We are NOT waiting for more Receive data, and
									; we are not expecting to send a ZLP.
	sts		ep0_status, r16
	clr_nakini						; Clear NAKINI flag
	clr_txini						; Clear TXINI flag
; User should set some flag here if Receive data needs to be processed.
	sei
	ret
;*** End of Process Endpoint 0 NAKINI Interrupt
;
;	
;*** Process Endpoint 0 NAKOUTI Interrupt
; The NAKOUT Interrupt occurs after the Device has sent all the IN data, and the
; Host now moves to the STATUS phase by issuing an OUT Token to the device. The
; AT90USB162 Controller will issue a NAKOUT handshake to the Host, and trigger
; the interrupt. This means that the Host is waiting for an ACK from the device,
; when the NEXT OUT Token is sent (that is when the RXOUTI flag will be cleared).

proc_ep0_nakout:
	cli
	lds		r16, ep0_status
	andi	r16, 0x60				; We are NOT waiting for more Receive data, and
									; we are not expecting to send a ZLP, we have
									; sent all the IN data, and we are NOT waiting
									; for NAKOUTI.
	ori		r16, 0x20				; We are waiting to ACK a Control Read Request
	sts		ep0_status, r16
	clr_nakouti						; Clear NAKOUTI flag
	clr_rxouti						; Clear RXOUTI flag
	sei
	ret
;*** End of Process Endpoint 0 NAKOUTI Interrupt
;
;	
;*** Configure USB Endpoints
; The Endpoints have the following characteristics:
;
; Endpoint 0:	Default Control Endpoint. Also used for OUT direction for
;				Notification Channel.
;				FIFO size:	8 bytes

; Endpoint 1:	Joystick data Channel, IN direction, Interrupt type
;				FIFO size:	8 bytes

; Endpoint 0:
config_ep:
	cli	

	lds		r16, UERST
	ori		r16, 0x01
	sts		UERST, r16			; Reset Endpoint 0 FIFO

	clr		r16
	sts		UERST, r16			; End reset of Endpoint 0 FIFO

	clr		r16
	sts		ep0_status, r16
	sts		UENUM, r16			; Select Endpoint 0

	ldi		r16, 0x01
	sts		UECONX, r16			; Enable Endpoint 0

	ldi		r16, 0x00
	sts		UECFG0X, r16		; Endpoint type is Control, direction is OUT

	ldi		r16, 0x02
	sts		UECFG1X, r16		; Set size of Endpoint 0 FIFO to 8 bytes, and allocate

	ldi		r16, 0x1C
	sts		UEIENX, r16			; Enable Interrupts: RXSTPE, RXOUTE, NAKOUTE

	ldi		r16, 0x01
	sts		UEINTX, r16			; Clear any pending interrupts

config_ep0:
	lds		r16, UESTA0X		; Check if configuration is okay
	sbrs	r16, 7
	rjmp	config_ep0

; Endpoint 1 (IN, Interrupt):
epoint1:
	lds		r16, UERST
	ori		r16, 0x02
	sts		UERST, r16			; Reset Endpoint 1 FIFO

	clr		r16
	sts		UERST, r16			; End reset of Endpoint 1 FIFO

	clr		r16
	sts		ep1_status, r16

	ldi		r16, 0x01
	sts		UENUM, r16			; Select Endpoint 1

	ldi		r16, 0x09
	sts		UECONX, r16			; Enable Endpoint 1, reset data toggle to 0

	ldi		r16, 0xC1
	sts		UECFG0X, r16		; Endpoint type is Interrupt, direction is IN

	ldi		r16, 0x02
	sts		UECFG1X, r16		; Set size of Endpoint 1 FIFO to 8 bytes, and allocate

	clr		r16
	sts		UEIENX, r16			; Enable Interrupts: none

	clr		r16
	sts		UEINTX, r16			; Clear any pending interrupts

config_ep1:
	lds		r16, UESTA0X		; Check if configuration is okay
	sbrs	r16, 7
	rjmp	config_ep1

	clr		r16
	sts		UENUM, r16			; Select Endpoint 0

	sei
	ret
;*** End of Configure USB Endpoints
;
;
;*** Command parser
parse:	
	clr		r16
	sts		msg, r16
	ldi		YH, high(boot_buf_out)	; Rx receive buffer
	ldi		YL, low(boot_buf_out)	
	ld		r16, Y+				; Load Request Type byte. Y now points to Specific Request byte
	sts		stp_req_type, r16
	ld		r16, Y+				; Load Specific Request byte. Y now points to MSB of wValue word
	sts		stp_request, r16
	ld		r16, Y+				; Load LSB of wValue word. Y now points to MSB of wValue word
	sts		stp_val_lo, r16
	ld		r16, Y+				; Load MSB of wValue word. Y now points to LSB of wIndex word
	sts		stp_val_hi, r16
	ld		r16, Y+				; Load LSB of wIndex word. Y now points to MSB of wIndex word
	sts		stp_index_lo, r16
	ld		r16, Y+				; Load MSB of wIndex word. Y now points to LSB of wLength word
	sts		stp_index_hi, r16
	ld		r16, Y+				; Load LSB of wLength word. Y now points to MSB of wLength word
	sts		stp_length_lo, r16
	ld		r16, Y				; Load MSB of wLength word.
	sts		stp_length_hi, r16


; Parse Standard or Class Requests here:
p_setup5:	
; Check to see if this is indeed a Standard Request Type:
	lds		r16, stp_req_type
	andi	r16, 0x60			; Isolate bits 6 and 5 (must both be zero for
								; a Standard Request)
	breq	p_setup6
	cpi		r16, 0x20			; Check if it is a Class specific request
	breq	c_class1
	rcall	req_error
	ret

p_setup6:
	lds		r16, stp_request
	cpi		r16, 0x0D			; Only 13 Standard Requests defined (0..12)
	brlo	p_setup7
	rcall	req_error
	ret

p_setup7:
	ldi		ZH, high(setup_com_tab*2)
	ldi		ZL, low(setup_com_tab*2)	
	lsl		r16
	add		ZL, r16
	clr		r16
	adc		ZH, r16
	lpm		XL, Z+
	lpm		XH, Z
	movw	ZH:ZL, XH:XL
	icall
	ret

;*** Parse Class specific requests here
c_class1:
	lds		r16, stp_request
	cpi		r16, 0x01			; Get Report request
	brne	c_setup1
	lds		r16, stp_val_hi		; Load MSB of wValue word.
	cpi		r16, 0x01			; Input Report type is requested
	breq	c_get_rep1
	rcall	req_error
	ret

;Process Get Report Request (not the Report Descriptor, but the actual report)
c_get_rep1:
	ldi		XH, high(boot_buf_in)		; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)
	ldi		r16, 0x04					; Report length is 4 bytes
	sts		num_in, r16
	lds		r16, a2d_ch2			; Throttle
	st		X+, r16					; Store byte
	lds		r16, a2d_ch0			; X position
	st		X+, r16					; Store byte
	lds		r16, a2d_ch1			; Y position
	st		X+, r16					; Store byte
	lds		r16, buttons			; Buttons
	st		X+, r16					; Store byte
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret

;Process Set Idle Request
c_setup1:
	lds		r16, stp_request
	cpi		r16, 0x0A			; Set Idle request
	brne	c_setup2
	rcall	s_send_zlp
	ret

; Note: technically, this command is not being implemented correctly,
;		as it ignores the delay value. We are assuming an idle
;		state of indefinite, meaning that the joystick will only
;		report data when data has changed.


c_setup2:
	rcall	req_error
	ret

;*** End of Command parser
;
;
;
.INCLUDE	"descriptor_joystick.asm"		; Descriptor files
;
;*********************** Setup Command Lookup table
; The Setup Command parser loads the beginning of this table into the Z pointer and adds the
; Command byte to the pointer. The parser then makes an indirect call to the routine
; pointed to by the entry in the table that Z is pointing to.

setup_com_tab:

	.DW	s_get_stat			; Status Request
	.DW	s_clear_feature		; Clear Feature Request
	.DW	not_supported		; Unsupported (reserved) Command
	.DW	s_set_feature		; Set Feature Request
	.DW	not_supported		; Unsupported (reserved) Command
	.DW	s_set_address		; Set Address Request
	.DW	s_get_descriptor	; Get Descriptor Request
	.DW	not_supported		; Set Descriptor Request not supported
	.DW	s_get_configuration	; Get Configuration Request
	.DW	s_set_configuration	; Set Configuration Request
	.DW	not_supported		; Get Interface Request
	.DW	not_supported		; Set Interface Request
	.DW	not_supported		; SYNC FRAME Request

;*********************** End of Setup Command Lookup table
;
;
;*********************** Setup Command routines
;
;****** GET_STATUS command
s_get_stat:
	lds		r16, stp_val_hi		; Load MSB of wValue word.
	tst		r16
	breq	s_get_stat11
	rcall	req_error
	ret

s_get_stat11:
	lds		r16, stp_val_lo		; Load LSB of wValue word.
	tst		r16
	breq	s_get_stat12
	rcall	req_error
	ret

s_get_stat12:
	lds		r16, stp_index_hi	; Load MSB of wIndex word.
	tst		r16
	breq	s_get_stat13
	rcall	req_error
	ret

s_get_stat13:
	lds		r16, stp_length_hi	; Load MSB of wLength word.
	tst		r16
	breq	s_get_stat14
	rcall	req_error
	ret

s_get_stat14:
	lds		r16, stp_length_lo	; Load LSB of wLength word (must be 2).
	cpi		r16, 0x02
	breq	s_get_stat15
	rcall	req_error
	ret

s_get_stat15:
	lds		r16, stp_req_type
	mov		r17, r16
	andi	r16, 0x80		; Bit 7 must be 1 (Device-to-Host transfer)
	tst		r16
	brne	s_get_stat1
	rcall	req_error
	ret

s_get_stat1:
	mov		r16, r17
	andi	r16, 0x60		; Bits 6,5 must be 0 (Standard Request)
	tst		r16
	breq	s_get_stat2
	rcall	req_error
	ret

s_get_stat2:
	mov		r16, r17
	andi	r16, 0x07		; Recipient specified in Bits 0-2
	cpi		r16, 0x03
	brlo	s_get_stat3		; Valid range is 0 to 2
	rcall	req_error
	ret

s_get_stat3:
	tst		r16
	brne	s_get_stat5

; Recipient is the Device - process it here:
	lds		r18, stp_index_lo
	tst		r18
	breq	s_get_stat4				; Must be 0 for a Device recipient
	rcall	req_error
	ret

s_get_stat4:
	ldi		XH, high(boot_buf_in)	; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)	; Recipient Address is zero = Device
	ldi		r16, 0x01				; Device is self-powered, no remote wakeup
	st		X+, r16					; Store byte
	clr		r16
	st		X+, r16					; Store byte
	ldi		r16, 0x02				; Send two bytes
	sts		num_in, r16
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret

s_get_stat5:
	cpi		r16, 0x01
	brne	s_get_stat7

; Recipient is the Interface - process it here:
	lds		r18, stp_index_lo
	cpi		r18, 0x02
	brlo	s_get_stat6				; Valid range is 0 to 1
	rcall	req_error
	ret

s_get_stat6:
	ldi		XH, high(boot_buf_in)	; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)	; Recipient Address is zero = Device
	clr		r16
	st		X+, r16					; Store byte
	st		X+, r16					; Store byte
	ldi		r16, 0x02				; Send two bytes
	sts		num_in, r16
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret

; Recipient is an Endpoint - process it here:
s_get_stat7:
	lds		r16, dev_status
	andi	r16, 0x04
	tst		r16
	brne	s_get_stat9

; We are in the Address phase:
	lds		r16, stp_index_lo
	tst		r16
	breq	s_get_stat8				; Must be 0 (Endpoint 0)
	rcall	req_error
	ret

s_get_stat8:
	ldi		XH, high(boot_buf_in)	; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)	; Recipient Address is zero = Device
	clr		r16
	st		X+, r16					; Store byte (do not use HALT bit for EP0)
	st		X+, r16					; Store byte
	ldi		r16, 0x02				; Send two bytes
	sts		num_in, r16
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret

; We are in the Configured phase:
s_get_stat9:
	lds		r18, stp_index_lo
	sts		UENUM, r18				; Select correct Endpoint
	lds		r18, UECONX
	andi	r18, 0x20				; Isolate STALLRQ bit
	clr		r16
	tst		r18
	breq	s_get_stat10
	inc		r16						; Endpoint is STALLed (HALTed)

s_get_stat10:
	clr		r18
	sts		UENUM, r18				; Select Endpoint 0
	ldi		XH, high(boot_buf_in)	; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)	; Recipient Address is zero = Device
	st		X+, r16					; Store byte (do not use HALT bit for EP0)
	clr		r16
	st		X+, r16					; Store byte
	ldi		r16, 0x02				; Send two bytes
	sts		num_in, r16
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret
;****** End of GET_STATUS command
;
;
;****** CLEAR_FEATURE command
s_clear_feature:
	lds		r16, stp_val_hi		; Load MSB of wValue word.
	tst		r16
	breq	s_clr_feat1
	rcall	req_error
	ret

s_clr_feat1:
	lds		r16, stp_val_lo		; Load LSB of wValue word.
	tst		r16
	breq	s_clr_feat2
	rcall	req_error			; Only supports feature 0 = Endpoint HALT
	ret

s_clr_feat2:
	lds		r16, stp_index_hi	; Load MSB of wIndex word.
	tst		r16
	breq	s_clr_feat3
	rcall	req_error
	ret

s_clr_feat3:
	lds		r18, stp_index_lo	; Load LSB of wIndex word.
	lds		r16, dev_status
	andi	r16, 0x04
	tst		r16
	brne	s_clr_feat5

; We are in the Address phase:
	tst		r18
	breq	s_clr_feat4			; Must be 0 (Endpoint 0)
	rcall	req_error
	ret

s_clr_feat4:
	clr		r18
	sts		UENUM, r18			; Select Endpoint 0
	lds		r18, UECONX
	ori		r18, 0x10			; Disable STALL Request mechanism	
	sts		UECONX, r18
	rcall	s_send_zlp
	ret

; We are in the Configured phase:
s_clr_feat5:
	lds		r18, stp_index_lo
	cpi		r18, 0x04
	brlo	s_clr_feat6
	rcall	req_error			; Only Endpoints 0 to 3 exist
	ret

s_clr_feat6:
	sts		UENUM, r18			; Select correct Endpoint
	lds		r18, UECONX
	ori		r18, 0x10			; Disable STALL Request mechanism	
	sts		UECONX, r18
	clr		r18
	sts		UENUM, r18			; Select Endpoint 0

	rcall	s_send_zlp
	ret
;****** End of CLEAR_FEATURE command
;
;
;****** SET_FEATURE command
s_set_feature:
	lds		r16, stp_val_hi		; Load MSB of wValue word.
	tst		r16
	breq	s_set_feat1
	rcall	req_error
	ret

s_set_feat1:
	lds		r16, stp_val_lo		; Load LSB of wValue word.
	tst		r16
	breq	s_set_feat2
	rcall	req_error			; Only supports feature 0 = Endpoint HALT
	ret

s_set_feat2:
	lds		r16, stp_index_hi	; Load MSB of wIndex word.
	tst		r16
	breq	s_set_feat3
	rcall	req_error
	ret

s_set_feat3:
	lds		r16, stp_index_lo	; Load LSB of wIndex word.
	tst		r16
	brne	s_set_feat4
	rcall	req_error			; HALT feature not supported for Endpoint 0
	ret

s_set_feat4:
	cpi		r16, 0x04
	brlo	s_set_feat5
	rcall	req_error			; Only Endpoints 0 to 3 exist
	ret

s_set_feat5:
	sts		UENUM, r16			; Select correct Endpoint
	lds		r18, UECONX
	ori		r18, 0x20			; Enable STALL Request mechanism	
	sts		UECONX, r18
	clr		r18
	sts		UENUM, r18			; Select Endpoint 0

	rcall	s_send_zlp
	ret
;****** End of SET_FEATURE command
;
;
;****** SET_ADDRESS command
s_set_address:
	lds		r16, stp_val_hi		; Load MSB of wValue word.
	tst		r16
	breq	s_set_add1
	rcall	req_error
	ret

s_set_add1:
	lds		r16, stp_val_lo		; Load LSB of wValue word.
	tst		r16
	brne	s_set_add2
	rcall	req_error			; Address may not be set to 0
	ret

s_set_add2:
	sbrs	r16, 7				; Address range is 0 to 127
	rjmp	s_set_add3
	rcall	req_error
	ret

s_set_add3:
	sts		UDADDR, r16
	lds		r16, ep0_status
	ori		r16, 0x40			; We need to latch new address AFTER Status stage
	sts		ep0_status, r16
	lds		r16, ep0_status
	andi	r16, 0xEF			; No need to send ZLP, as we are doing it here
	sts		ep0_status, r16
	clr_txini					; Send ZLP

	enable_txini				; Enable TXINI Interrupt so we can latch new address (we
								; cannot do this here, otherwise the next IN token will be
								; incorrectly decoded, and the device will not respond)
	ret
;****** End of SET_ADDRESS command
;
;
;****** GET_DESCRIPTOR command
s_get_descriptor:
	lds		r16, stp_val_hi		; Load MSB of wValue word.
	cpi		r16, 0x01			; If 1, the host is requesting the device descriptor
	brne	s_get_des2
	ldi		XH, high(boot_buf_in)		; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)
	ldi		ZH, high(device_descriptor*2)
	ldi		ZL, low(device_descriptor*2)
	lpm		r21, Z						; Descriptor length
	sts		num_in, r21

; Process Device Descriptor
s_get_des_l1:
	lpm		r16, Z+				; Descriptor length
	st		X+, r16
	dec		r21
	brne	s_get_des_l1 
	rcall	check_desc_length
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret

; Process Configuration Descriptor
s_get_des2:
	cpi		r16, 0x02			; If 2, the host is requesting the configuration descriptor
	brne	s_get_des3
	ldi		XH, high(boot_buf_in)		; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)
	ldi		ZH, high(configuration_descriptor*2)
	ldi		ZL, low(configuration_descriptor*2)
	lpm		r21, Z						; Configuration Descriptor length
	sts		num_in, r21

s_get_des_l2:					; Copy Configuration Descriptor to buffer
	lpm		r16, Z+
	st		X+, r16
	dec		r21
	brne	s_get_des_l2 


; Copy Interface Descriptor to buffer
	ldi		ZH, high(interface_descriptor*2)
	ldi		ZL, low(interface_descriptor*2)
	rcall	append_data

; Copy HID Descriptor Header to buffer
	ldi		ZH, high(hid_descriptor*2)
	ldi		ZL, low(hid_descriptor*2)
	rcall	append_data

; Copy Endpoint l Descriptor to buffer
	ldi		ZH, high(endpoint1_descriptor*2)
	ldi		ZL, low(endpoint1_descriptor*2)
	rcall	append_data

; Check total length of Descriptor data to be sent
	rcall	check_desc_length
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret


; Process String Descriptor
s_get_des3:
	cpi		r16, 0x03				; If 3, the host is requesting a String Descriptor
	brne	s_get_des4
	ldi		XH, high(boot_buf_in)	; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)
	lds		r16, stp_val_lo			; Index of requested string descriptor
	cpi		r16, 0x03
	brlo	s_get_des_3d
	rcall	req_error				; Only three String Descriptors (Index 0..2)
	ret

s_get_des_3d:
	tst		r16
	brne	s_get_des_3a

	ldi		ZH, high(string0_descriptor*2)
	ldi		ZL, low(string0_descriptor*2)


	rjmp	s_get_des_3c

s_get_des_3a:
	cpi		r16, 0x01
	brne	s_get_des_3b
	ldi		ZH, high(string1_descriptor*2)
	ldi		ZL, low(string1_descriptor*2)
	rjmp	s_get_des_3c

s_get_des_3b:
	cpi		r16, 0x02
	brne	s_get_des_3c
	ldi		ZH, high(string2_descriptor*2)
	ldi		ZL, low(string2_descriptor*2)

s_get_des_3c:
	lpm		r21, Z						; Descriptor length
	sts		num_in, r21

s_get_des_l3:
	lpm		r16, Z+
	st		X+, r16
	dec		r21
	brne	s_get_des_l3 
	rcall	check_desc_length
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret


; Process HID Descriptor
s_get_des4:
	cpi		r16, 0x21			; If 21h, the host is requesting the HID descriptor
	brne	s_get_des5
	ldi		XH, high(boot_buf_in)		; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)
	ldi		ZH, high(hid_descriptor*2)
	ldi		ZL, low(hid_descriptor*2)
	lpm		r21, Z						; Descriptor length
	sts		num_in, r21

; Process HID Descriptor
s_get_des_l4:
	lpm		r16, Z+				; Descriptor length
	st		X+, r16
	dec		r21
	brne	s_get_des_l4 
	rcall	check_desc_length
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret

; Process Report Descriptor
s_get_des5:
	cpi		r16, 0x22			; If 22h, the host is requesting the Report descriptor
	brne	s_get_des6
	ldi		XH, high(boot_buf_in)		; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)
	ldi		ZH, high(rd_length*2)
	ldi		ZL, low(rd_length*2)
	lpm		r21, Z						; Descriptor length
	sts		num_in, r21

	ldi		ZH, high(report_descriptor*2)
	ldi		ZL, low(report_descriptor*2)

; Process Report Descriptor
s_get_des_l5:
	lpm		r16, Z+
	st		X+, r16
	dec		r21
	brne	s_get_des_l5 
	rcall	check_desc_length
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	enable_tmr0
	ret


; Request error
s_get_des6:
	rcall	req_error
	ret
;****** End of GET_DESCRIPTOR command
;
;
;****** GET_CONFIGURATION command
s_get_configuration:
	lds		r16, stp_val_hi		; Load MSB of wValue word.
	tst		r16
	breq	s_get_conf1
	rcall	req_error
	ret

s_get_conf1:
	lds		r16, stp_val_lo		; Load LSB of wValue word.
	tst		r16
	breq	s_get_conf2
	rcall	req_error
	ret

s_get_conf2:
	lds		r16, stp_index_hi
	tst		r16
	breq	s_get_conf3
	rcall	req_error
	ret

s_get_conf3:
	lds		r16, stp_index_lo
	tst		r16
	breq	s_get_conf4
	rcall	req_error
	ret

s_get_conf4:
	lds		r16, stp_length_hi
	tst		r16
	breq	s_get_conf5
	rcall	req_error
	ret

s_get_conf5:
	lds		r16, stp_length_lo
	cpi		r16, 0x01
	breq	s_get_conf6
	rcall	req_error
	ret

s_get_conf6:
	lds		r16, dev_status
	andi	r16, 0x04
	tst		r16
	brne	s_get_conf7

; We are in the Address phase:
	ldi		XH, high(boot_buf_in)	; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)	; Recipient Address is zero = Device
	clr		r16
	st		X, r16					; Store byte
	ldi		r21, 0x01				; Send one byte
	sts		num_in, r21
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret

s_get_conf7:
	ldi		XH, high(boot_buf_in)	; Tx Transmit buffer
	ldi		XL, low(boot_buf_in)	; Recipient Address is zero = Device
	ldi		r16, 0x01
	st		X, r16					; Store byte
	ldi		r21, 0x01				; Send one byte
	sts		num_in, r21
	clr		r16
	sts		bbuf_in_ptr, r16
	init_ep0_tx_status
	clr_rxouti
	clr_fifocon
	clr_nakouti
	rcall	usb_send_data
	ret
;****** End of GET_CONFIGURATION command
;
;
;****** SET_CONFIGURATION command
s_set_configuration:

	ldi		r16, 0x84
	rcall	snd_test

	lds		r16, stp_val_lo		; Load LSB of wValue word.
	cpi		r16, 0x02
	brlo	s_set_conf2			; Only one configuration
	rcall	req_error
	ret

s_set_conf2:
	lds		r16, stp_index_hi
	tst		r16
	breq	s_set_conf3
	rcall	req_error
	ret

s_set_conf3:
	lds		r16, stp_index_lo
	tst		r16
	breq	s_set_conf4
	rcall	req_error
	ret

s_set_conf4:
	lds		r16, stp_length_hi
	tst		r16
	breq	s_set_conf5
	rcall	req_error
	ret

s_set_conf5:
	lds		r16, stp_length_lo
	tst		r16
	breq	s_set_conf6
	rcall	req_error
	ret

s_set_conf6:
	lds		r16, stp_val_lo
	tst		r16
	brne	s_set_conf7
	lds		r16, dev_status
	andi	r16, 0xFB
	sts		dev_status, r16		; Signal that we are in the Address State
	rjmp	s_set_conf8

s_set_conf7:
	lds		r16, dev_status
	ori		r16, 0x04
	sts		dev_status, r16		; Signal that we are in the Configured State

s_set_conf8:
	ldi		r16, 0x01
	sts		UENUM, r16			; Select Endpoint 1

	ldi		r16, 0x09
	sts		UECONX, r16			; Reset data toggle to 0

	ldi		r16, 0x00
	sts		UENUM, r16			; Select Endpoint 0

	rcall	s_send_zlp
	ret
;****** End of SET_CONFIGURATION command
;
;
;****** Unsupported command
not_supported:	
	rcall	req_error
	ret
;****** End of Unsupported command
;*********************** End of Setup Command routines
;
;
;*********************** Handler for sending ZLP
; Check if we can send a ZLP:
s_send_zlp:
	lds		r16, UEINTX
	andi	r16, 0x01
	tst		r16
	brne	s_send_zlp1

; No, so enable Interrupt, and set flag in EP Status byte:
	lds		r16, ep0_status
	ori		r16, 0x10			; We are ready to send ZLP
	sts		ep0_status, r16
	enable_txini
	ret

s_send_zlp1:
	clr_txini					; Send ZLP
	disable_txini
	ret
;*********************** End of Handler for sending ZLP
;
;
;*********************** Handler for checking how many bytes
;*********************** of the data structure need to be sent
; This routine will check the GET_DESCRIPTOR request to see how many bytes have been
; requested. It will first check to see if the HIGH byte is not zero. If it is not zero,
; then num_in will be left as is, because none of the descriptors have a length longer than
; $FF bytes. If less than the length of the descriptor have been requested, then only
; the requested amount will be sent. If the requested amount is more than the desriptor
; length, then only the descriptor length will be sent.
; This routine receives the desriptor length in num_in. It will return the actual amount to
; send in num_in.

check_desc_length:
	lds		r17, stp_length_hi	; High byte of number of bytes requested by host
	tst		r17
	breq	c_desc1
	ret

c_desc1:
	lds		r17, stp_length_lo	; Number of bytes requested by host
	lds		r21, num_in
	cp		r17, r21			; r17 - r21
	brlo	c_desc2				; Branch if r17 < r21
	ret
	 
c_desc2:
	sts		num_in, r17			; Only part of descriptor has been requested
	ret
;*********************** End of Handler


;*********************** Handler for appending data

append_data:
	lpm		r21, Z
	lds		r20, num_in
	add		r20, r21
	sts		num_in, r20

append_l1:
	lpm		r16, Z+
	st		X+, r16
	dec		r21
	brne	append_l1 
	ret
;*********************** End of Handler
;
;
;****** Generate Request Error (STALL)
req_error:
	lds		r17, UECONX
	ori		r17, 0x20
	sts		UECONX, r17		; Set STALL Request bit
	ret
;****** End of Process Request Error
;
;
;*********************** Handler for USB Suspend
usb_suspend:
	set_suspend		
	ldi		r16, 0x88
	sts		USBCON, r16		; Freeze USB clock ("Resume Detection" still active)
	clr		r16
	sts		UDINT, r16
	ret
;*********************** End of Handler for USB Suspend
;
;
;*********************** Handler for USB Wakeup
usb_wakeup:
	ldi		r16, 0x80
	sts		USBCON, r16		; Enable USB clock
	clr_suspend		
	clr		r16
	sts		UDINT, r16
	ret
;*********************** End of Handler for USB Wakeup
;
;
;*********************** Handler for sending n bytes on USB port
; This routine will send n bytes (as specified in num_in) to the USB port.
; It expects the data to be in boot_buf_in, pointed to by bbuf_in_ptr.

usb_send_data:
; Check if we are dealing with EP0:
	lds		r16, ep_number
	tst		r16
	breq	send_ep0				; Yes, so process EP0
	rjmp	send_ep1				; No. Process EP1

send_ep0:
	ldi		r16, 0x00
	sts		UENUM, r16			; Select Endpoint 0
	disable_txini					; Disable TXINI interrupt (Assume last packet)
	clr		r16
	sts		en_txini, r16			; Assume no TXINI interrupt handling is necessary

	lds		r17, num_in
	tst		r17
	brne	usb_send_data1a
	ret

usb_send_data1a:
	ldi		r20, ep0_fifo
	cp		r17, r20
	brne	usb_send_data2

; They are equal --> set ZPL flag in Status byte:
	lds		r16, ep0_status
	ori		r16, 0x10				; Set ZPL flag.
	sts		ep0_status, r16
	ldi		r16, 0x01
	sts		en_txini, r16			; Enable TXINI AFTER we have sent the data,
									; otherwise we might prematurely trigger it if
									; we enable it here.
	rjmp	usb_send_data4

usb_send_data2:
	cp		r17, r20
	brlo	usb_send_data3			; branch if r17 < r20 (7 bytes or less to send)

	lds		r16, ep0_status
	ori		r16, 0x08				; We are waiting to send more data to the host
	sts		ep0_status, r16
	ldi		r16, 0x01
	sts		en_txini, r16			; Enable TXINI AFTER we have sent the data,
									; otherwise we might prematurely trigger it if
									; we enable it here.
	mov		r17, r20				; Used to count down the amount of bytes we send
	rjmp	usb_send_data4

usb_send_data3:
	lds		r16, ep0_status
	ori		r16, 0x80				; Endpoint is waiting for NAKOUTI
	sts		ep0_status, r16

usb_send_data4:
	cli
	ldi		YH, high(boot_buf_in)	; Tx Transmit buffer
	ldi		YL, low(boot_buf_in)	
	lds		r18, bbuf_in_ptr
	mov		r19, r18
	add		YL, r18					; Add offset to Y
	clr		r18
	adc		YH, r18
	mov		r18, r19
	lds		r19, num_in

usb_send_data_5:
	ld		r16, Y+
	sts		UEDATX, r16
	inc		r18
	dec		r19
	dec		r17
	brne	usb_send_data_5

	sts		num_in, r19
	sts		bbuf_in_ptr, r18
	sei

	clr_txini						; Clear TXINI interrupt flag. This will send the data,
									; and clear the buffer.
	lds		r16, en_txini
	sbrc	r16, 0
	enable_txini					; Enable TXINI interrupt
	ret


; Process EP1 here:
send_ep1:
	clr_txini						; Clear TXINI interrupt flag.
	cli
	ldi		YH, high(boot_buf_in)	; Tx Transmit buffer
	ldi		YL, low(boot_buf_in)	
	lds		r19, num_in

send_ep1a:
	ld		r16, Y+
	sts		UEDATX, r16
	dec		r19
	brne	send_ep1a
	sei
	clr_fifocon						; This will send the data
	ret
;*********************** End of Handler for sending n bytes on USB port
;
;
;***********************  Routine to measure Joystick parameters
;
; This routine is used to measure the following Joystick parameters:
;
;	- X deflection (CH0)
;	- Y deflection (CH1)
;	- Throttle (CH2)
;	- Buttons
;
meas_joy:
	clr		r19
	sts		joy_change, r19		; Assume no change

	ldi		r19, ad_ch0		; X deflection
	rcall	get_channel
pos1:  	
	lds		r20, a2d_ch0
	cp		r20, r19
	breq	pos1a

	sts		a2d_ch0, r19
	ldi		r19, 0x01
	sts		joy_change, r19

pos1a:
	ldi		r19, ad_ch1		; Y deflection
	rcall	get_channel
pos2:  	
	lds		r20, a2d_ch1
	cp		r20, r19
	breq	pos2a

	sts		a2d_ch1, r19
	ldi		r19, 0x01
	sts		joy_change, r19

pos2a:
	ldi		r19, ad_ch2		; Throttle
	rcall	get_channel
pos3:  	
	lds		r20, a2d_ch2
	cp		r20, r19
	breq	pos3a

	sts		a2d_ch2, r19
	ldi		r19, 0x01
	sts		joy_change, r19

pos3a:
	in		r19, PIND
	mov		r20, r19
	andi	r20, 0x0F		; Isolate bits 0 to 3
	tst		r20
	breq	pos_e
	cpi		r20, 0x01
	brne	pos4
	andi	r19, 0xF0		; Set bits 0 to 3 to 0
	rjmp	pos_e1

pos4:
	cpi		r20, 0x02
	brne	pos5
	andi	r19, 0xF0		; Set bits 0 to 3 to 0
	ori		r19, 0x01		; Set lower nibble to 1
	rjmp	pos_e1

pos5:
	cpi		r20, 0x04
	brne	pos6
	andi	r19, 0xF0		; Set bits 0 to 3 to 0
	ori		r19, 0x02		; Set lower nibble to 2
	rjmp	pos_e1

pos6:
	cpi		r20, 0x08
	brne	pos_e
	andi	r19, 0xF0		; Set bits 0 to 3 to 0
	ori		r19, 0x03		; Set lower nibble to 3
	rjmp	pos_e1
	
pos_e:
	ori		r19, 0x04		; Set to 4 --> hat switch
							; is in the null state
pos_e1:
	lds		r20, buttons
	cp		r20, r19
	breq	pos_e2

	sts		buttons, r19
	ldi		r19, 0x01
	sts		joy_change, r19
	
pos_e2:
	ret
;*********************  End of Routine to measure Joystick parameters
;
;
;***********************  Routine to get one measurement cycle from A/D
;
; This routine is used to get one measurement from one of the four
; channels of the A/D converter. The channel must be in r19.
; The result will be returned in r19.
;
get_channel:
		cbi		PORTB,0				; Set CS_ of A/D LOW
		out		SPDR, r19			; Sends Setup data
get_c1:	in		r16, SPSR
		andi	r16, 0x80			; Isolate SPIF flag
		tst		r16
		breq	get_c1				; When SPIF goes HIGH, then the transfer is complete

		out		SPDR, r19			; Sends Setup data again (dummy send)
get_c2:	in		r16, SPSR
		andi	r16, 0x80			; Isolate SPIF flag
		tst		r16
		breq	get_c2				; When SPIF goes HIGH, then the transfer is complete

		in		r19, SPDR
		sbi		PORTB,0				; Set CS_ of A/D HIGH
		ret
;*********************  End of Routine to get one measurement cycle from A/D
;
;
;***********************  Routine for Sending of test data 
;
; This routine will send a byte of data on PORTC.5 (TP1). It expects the data to be in r16.
; The Idle level is HIGH. Next, a start bit (HIGH to LOW transition) is generated, then
; 8 bits of data (MSB first), then the Idle state (HIGH).
; The bit time is 64 cycles = 64 x 62.5ns = 4us.
;
#define startbit 25
#define databit 24

snd_test:
	push	r16
	push	r17
	push	r20

	ldi		r17, 0x08
	ldi		r20, startbit
	cbi		PORTC,5			; Start bit

del_t1:	
	dec		r20
	brne	del_t1
	clc

test1:	
	lsl		r16
	brcc	test2
	sbi		PORTC,5
	rjmp	test3

test2:
	cbi		PORTC,5	

test3:
	ldi		r20, databit

del_t2:	
	dec		r20
	brne	del_t2

	dec		r17
	brne	test1	

	sbi		PORTC,5
	pop		r20
	pop		r17
	pop		r16
	ret
;*********************  End of Routine for Sending of test data


