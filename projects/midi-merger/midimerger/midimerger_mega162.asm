;####################################################################
;# ATmega162 - Midi Merger
;#
;#  Based on dual uart uC Atmel ATmega162 (AVR 8bit).
;#		+ Merge all messages except SysEx
;#		+ Running Status (RX only)
;#
;#	Created by:
;#		del_gr
;#		SiC
;#
;#	Info:
;#	- This is our first project, so don't expect much. ;)
;#	- It has no FIFO buffer, and you shouldn't need one under normal circumstanses
;#	- It works! (i think.)
;#	- Use code and especially the schematic at your own risk.
;#
;#	Feel free to send comment or questions to delgr@hotmail.com
;#	
;####################################################################

.include "m162def.inc"


.def baud=r16			; baud rate reg
.def temp=r17			; temp reg
.def temp2=r25			; temp reg
.def mbyte0=r18			; store register (statusbyte)	;
.def mbyte1=r19			; store register (databyte1)	; Uart0 & Uart1
.def mbyte2=r15			; store register (databyte2)	;
.def mes0_pointer=r21	; Message Pointer - Uart0
.def mes0_count=r22		; Message Counter - Uart0
.def mes1_pointer=r23	; Message Pointer - Uart1
.def mes1_count=r24		; Message Counter - Uart1
.def RStatbyte0=r14		; running status store register - uart0
.def RStatbyte1=r13		; running status store register - uart1
.def RScount0=r12		; running status message counter store register - uart0
.def RScount1=r11		; running status message counter store register - uart1

.equ brate=15			; baudrate (31250bps) 15 for 8Mhz
.equ UART0_busy=7
.equ UART1_busy=6

; ###################################################################




.CSEG						; start code segment
.ORG 0						; reset interrupt
	rjmp RESET			
	rjmp RESET
	rjmp RESET


.ORG USART0RXCaddr			; uart0 receive interrupt
	rjmp uart0_rx
.ORG USART1RXCaddr			; uart1 receive interrupt
	rjmp uart1_rx

; ######################################

RESET:
ldi temp, high(RAMEND)		;
out SPH, temp				;	Setup
ldi temp, low(RAMEND)		;	Stack
out SPL, temp				;

rcall set_uart
;set uart(s) to allow interrupt handling
sbi UCSR0B,RXCIE0		; 1
cbi UCSR0B,TXCIE0		; 0
cbi UCSR0B,UDRIE0		; 0
sbi UCSR0B,RXEN0		; 1
cbi UCSR0B,TXEN0		; 0

sbi UCSR1B,RXCIE1		; 1
cbi UCSR1B,TXCIE1		; 0
cbi UCSR1B,UDRIE1		; 0
sbi UCSR1B,RXEN1		; 1
sbi UCSR1B,TXEN1		; 1

clr temp
clr temp2
clr baud
clr mbyte0
clr mbyte1
clr mbyte2
clr mes0_pointer
clr mes1_pointer
clr mes0_count
clr mes1_count
clr RStatbyte0
clr RStatbyte1
clr RScount0
clr RScount0

; Initiate WatchdogTimer
ldi temp,0b0011110		; 1 sec timeout
out WDTCR,temp


rcall flush_udr0
rcall flush_udr1
;rcall ActiveSensing
rcall allsoundsoff
rcall allsoundsoff
rcall allsoundsoff
rcall ledInit

;set global interrupts on
sei


; #######################################


LOOP:			; main program loop
wdr
rjmp LOOP


set_uart: ; (s)
ldi baud,brate		; set baudrate
out UBRR0L,baud		; to ubrr0
ldi baud,brate		; set baudrate
out UBRR1L,baud		; to ubrr1
ret

; ####################################################################################
; ####################################################################################


uart0_rx:

cbi PORTA,6 ; Activity Led ON

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Receive 1st byte (status)

rx_1_uart0:
	sbis UCSR0A,RXC0				; Check if byte is received
	rjmp rx_1_uart0					;
		in mbyte0,UDR0				; get it from the UDR
		
		sbrc mbyte0,7				; check if its a status byte
		mov RStatbyte0,mbyte0		; and store it for use in Running Status
		
	inc mes0_pointer				; increase uart0 pointer
	rcall checkbyte0				; go check what we received
		sbrs mbyte0,7				; if got here by mistake
		rjmp finish_tx0				; finish

		cp mes0_pointer,mes0_count	; if the midi message is complete with this one
		breq uart0_tx				; send it.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Receive 2nd byte (data1)

rx_2_uart0:
	sbis UCSR0A,RXC0				; Check if byte is received
	rjmp rx_2_uart0					;
		in mbyte1,UDR0				; get it from the UDR

	RSreturn0:
	inc mes0_pointer				; increase uart0 pointer
									;
		cp mes0_pointer,mes0_count	; if the midi message is complete with this one
		breq uart0_tx				; send it.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Receive 3nd byte (data2)

rx_3_uart0:
	sbis UCSR0A,RXC0				; Check if byte is received
	rjmp rx_3_uart0					;
		in mbyte2,UDR0				; get it from the UDR

	inc mes0_pointer				; increase uart0 pointer
									;
		cp mes0_pointer,mes0_count	; if the midi message is complete with this one
		breq uart0_tx				; send it.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	

reti

; ####################################################################################
; ####################################################################################

uart1_rx:

cbi PORTA,6	; Activity Led ON

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Receive 1st byte (status)

rx_1_uart1:
	sbis UCSR1A,RXC1				; Check if byte is received
	rjmp rx_1_uart1					;
		in mbyte0,UDR1				; get it from the UDR
	
		sbrc mbyte0,7				; check if its a status byte
		mov RStatbyte1,mbyte0		; and store it for use in Running Status
	
	inc mes1_pointer				; increase uart0 pointer
	rcall checkbyte1				; go check what we received
		sbrs mbyte0,7				; if got here by mistake
		rjmp finish_tx1				; finish

		cp mes1_pointer,mes1_count	; if the midi message is complete with this one
		breq uart1_tx				; send it.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Receive 2nd byte (data1)

rx_2_uart1:
	sbis UCSR1A,RXC1				; Check if byte is received
	rjmp rx_2_uart1					;
		in mbyte1,UDR1				; get it from the UDR

	RSreturn1:
	inc mes1_pointer				; increase uart0 pointer
									;
		cp mes1_pointer,mes1_count	; if the midi message is complete with this one
		breq uart1_tx				; send it.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Receive 3nd byte (data2)

rx_3_uart1:
	sbis UCSR1A,RXC1				; Check if byte is received
	rjmp rx_3_uart1					;
		in mbyte2,UDR1				; get it from the UDR

	inc mes1_pointer				; increase uart0 pointer
									;
		cp mes1_pointer,mes1_count	; if the midi message is complete with this one
		breq uart1_tx				; send it.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	

reti

; ####################################################################################
; ####################################################################################


uart0_tx:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Transmit 1st byte (status)

tx_1_uart0:
	sbis UCSR1A,UDRE1				; Check if UDR is empty
	rjmp tx_1_uart0					;
									;
		out UDR1,mbyte0				; send through UDR1
									;
			cpi mes0_count,1		; Check if we are done
			breq finish_tx0			; finish

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Transmit 2nd byte (data1)

tx_2_uart0:
	sbis UCSR1A,UDRE1				; Check if UDR is empty
	rjmp tx_2_uart0					;
									;
		out UDR1,mbyte1				; send through UDR1
									;
			cpi mes0_count,2		; Check if we are done
			breq finish_tx0			; finish

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Transmit 3rd byte (data2)

tx_3_uart0:
	sbis UCSR1A,UDRE1				; Check if UDR is empty
	rjmp tx_3_uart0					;
									;
		out UDR1,mbyte2				; and send it to UDR1
									;
			cpi mes0_count,3		; Check if we are done
			breq finish_tx0			; finish

ret


; ####################################################################################
; ####################################################################################

uart1_tx:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Transmit 1st byte (status)

tx_1_uart1:
	sbis UCSR1A,UDRE1				; Check if UDR is empty
	rjmp tx_1_uart1					;
									;
		out UDR1,mbyte0				; and send it to UDR1
									;
			cpi mes1_count,1		; Check if we are done
			breq finish_tx1			; finish

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Transmit 2nd byte (data1)

tx_2_uart1:
	sbis UCSR1A,UDRE1				; Check if UDR is empty
	rjmp tx_2_uart1					;
									;
		out UDR1,mbyte1				; and send it to UDR1
									;
			cpi mes1_count,2		; Check if we are done
			breq finish_tx1			; finish

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	Transmit 3rd byte (data2)

tx_3_uart1:
	sbis UCSR1A,UDRE1				; Check if UDR is empty
	rjmp tx_3_uart1					;
									;
		out UDR1,mbyte2				; and send it to UDR1
									;
			cpi mes1_count,3		; Check if we are done
			breq finish_tx1			; finish

ret

; ####################################################################################
; ####################################################################################

; ####################################### Help routines

finish_tx0:
clr mes0_pointer	;
clr mes0_count		;
clr mbyte0			;	Clear registers
clr mbyte1			;	except running status
clr mbyte2			;
sbi PORTA,6			; Activity Led OFF
reti

finish_tx1:
clr mes1_pointer	;
clr mes1_count		;
clr mbyte0			;	Clear registers
clr mbyte1			;	except running status
clr mbyte2			;
sbi PORTA,6			; Activity Led OFF
reti

flush_udr0:			; Read UDR until it is empty
in temp,UDR0
sbic UCSR0A,RXC0
rjmp flush_udr0
clr temp
ret

flush_udr1:			; Read UDR until it is empty
in temp,UDR1
sbic UCSR1A,RXC1
rjmp flush_udr1
clr temp
ret

;------------------------------------------- MMerger created messages
ActiveSensing:		;
nop					;
sbis UCSR1A,UDRE1	; Send one active
rjmp ActiveSensing	; sensing message.
ldi temp,0xFE		; (for testing
out UDR1,temp		; midi_tx).
clr temp			;
ret					;

allsoundsoff:		;all sounds off  176-120-64
test_tx0:
nop
sbis UCSR1A,UDRE1
rjmp test_tx0
ldi temp,176
out UDR1,temp

test_tx1:
nop
sbis UCSR1A,UDRE1
rjmp test_tx1
ldi temp,120
out UDR1,temp

test_tx2:
nop
sbis UCSR1A,UDRE1
rjmp test_tx2
ldi temp,64
out UDR1,temp

clr temp
ret
;-------------------------------------------

ledInit:			; Initiate port driving leds
ldi temp,255		;
out PORTA,temp		; 
ldi temp,255		;
out DDRA,temp		; define as output
cbi PORTA,7			; Status Led ON
clr temp			;
ret

; ####################################################################################
; ####################################################################################


; ####################### Midi Message Check ########################
checkbyte0:

sbrc mbyte0,7			; if status byte on mbyte0
rjmp msgcheck0			; skip running status retrieve
mov mbyte1,mbyte0		; else: get data1 where it should be
mov mbyte0,RStatbyte0	; get the last status byte
mov mes0_count,RScount0	; the last message counter

rjmp RSreturn0			; and continue RX process

msgcheck0:
mov temp,mbyte0
mov temp2,temp
lsr temp2
lsr temp2
lsr temp2
lsr temp2

programcheck0:
cpi temp2,0x0c		; check if program change
breq send01data

aftertouchcheck0:
cpi temp2,0x0d		; check if Channel Pressure (After-touch)
breq send01data

SPPcheck0:
cpi temp,0xf2		; check if SPP
breq send02data

songcheck0:			; check if Song Select
cpi temp,0xf3
breq send01data

syscheck0:			; 
cpi temp,0xf6
brsh send00data

chancheck0:			; 
cpi temp2,0x08
brsh chancheck01
rjmp RESET

chancheck01:		;
cpi temp2,0x0f		;
brlo send02data		;
rjmp RESET

; RESULTs ;
send00data:
ldi mes0_count,1
mov RScount0,mes0_count
ret

send01data:
ldi mes0_count,2	; =N+1  (result of databytes + statusbyte)
mov RScount0,mes0_count
ret

send02data:
ldi mes0_count,3	; =N+1
mov RScount0,mes0_count
ret

; ####################################################################################
; ####################################################################################

checkbyte1:

sbrc mbyte0,7			; 
rjmp msgcheck1			;	*See comment on label checkbyte0
mov mbyte1,mbyte0		;
mov mbyte0,RStatbyte1	;
mov mes1_count,RScount1	;

rjmp RSreturn1			;

msgcheck1:
mov temp,mbyte0
mov temp2,temp
lsr temp2
lsr temp2
lsr temp2
lsr temp2

programcheck1:
cpi temp2,0x0c		; check if program change
breq send11data

aftertouchcheck1:
cpi temp2,0x0d		; check if Channel Pressure (After-touch)
breq send11data

SPPcheck1:
cpi temp,0xf2		; check if SPP
breq send12data

songcheck1:			; check if Song Select
cpi temp,0xf3
breq send11data

syscheck1:			; 
cpi temp,0xf6
brsh send10data

chancheck10:			; check if status byte ???
cpi temp2,0x08
brsh chancheck11
rjmp RESET

chancheck11:		;
cpi temp2,0x0f		;
brlo send12data		;
rjmp RESET

; RESULTs ;
send10data:
ldi mes1_count,1
mov RScount1,mes1_count
ret

send11data:
ldi mes1_count,2	; =N+1  (result of databytes + statusbyte)
mov RScount1,mes1_count
ret

send12data:
ldi mes1_count,3	; =N+1
mov RScount1,mes1_count
ret

;#######################
;##############################################
;##################################################################### The End.
;############################################################################################