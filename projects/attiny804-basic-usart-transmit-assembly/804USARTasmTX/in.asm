;
; 804USARTasmTX.asm
;
; Created: 3/24/2021 8:09:40 PM
; Author : pappan
;
.include "tn804def.inc"
.equ fclk = 3333333
.equ BAUD = 9600
.equ fBAUD = ((64 * fclk) /(16 * BAUD)+0.5)
.def temp = r16

.macro millis
ldi YL,low(@0)
ldi YH,high(@0)
rcall delayYx1mS
.endm


reset:
lds r16,PORTB_DIR	
ori r16,(1<<2)
sts PORTB_DIR,r16			;set portB PB2 direction as output
lds r16,PORTB_OUT
ori r16,(1<<2)
sts PORTB_OUT,r16			;set portB PB2 as 1 or +ve
ldi r16,low(fBAUD)			;load low value of fBAUD as calculated in the formula provided above
ldi r17,high(fBAUD)			;load high value of fBAUD as calculated in the formula provided above
sts USART0_BAUD,r16			;store low fBAUD in BAUD register
sts USART0_BAUD + 1,r17		;store low fBAUD in BAUD register
ldi r16,USART_TXEN_bm	    ;0b01000000 ,(1<<6) ,bitmask value to enable USART transmit 
sts USART0_CTRLB,r16		;store TXEN in USART_CTRLB register
rjmp TXUSART				;jump to TXUSART address (main routine)

sendbyte:
lds r16,USART0_STATUS		;copy USART status register to r16
andi r16,USART_DREIF_bm     ;(0b00100000) AND with DATA REGISTER EMPTY FLAG bitmask (position5) to check flag status 0= not empty 1= empty 
sbrs r16,5					;skip next instruction if bit 5 is 1 (means flag set for data transmit buffer ready to receive new data )
rjmp sendbyte				;if DREIF = 0 ,bit 5 in r16 is 0 then loop back to sendbyte until DREIF = 1
mov r16,r17					;copy data to be transmitted from r17 to r16
sts USART0_TXDATAL,r16		;store r16 in TXDATAL transmit data low register 
ret

TXstring:
ldi ZL,low(2*string)		;load low value of string address in low register of Z pointer
ldi ZH,high(2*string)		;load low value of string address in low register of Z pointer
loop:
lpm r17,Z+					;load from program memory value poited by Z (first byte/character of the string) and increment address
and r17,r17					;AND r17 with itself, if null/0x00 or not
breq exit					;if null/0x00 the end of string has reached
rcall sendbyte				;if r17 value not null call sendbyte routine to transmit 1 character
rjmp loop					;jump to loop back through the string until a null is encountered
exit:ret



TXUSART:
rcall TXstring				;call routine to transmit a null terminated string
millis 250					;delay routine that calls a delay of 250ms
rjmp TXUSART				;infinite loop that keeps transmitting the string


string:
.db "My first UART assembly code!",'\r','\n',0       ; string in "   " ,'\r' is carriage return , '\n' is new line

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;DELAY SUBROUTINES
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
     ; ============================== Time Delay Subroutines =====================
; Name:     delayYx1mS
; Purpose:  provide a delay of (YH:YL) x 1 mS
; Entry:    (YH:YL) = delay data
; Exit:     no parameters
; Notes:    the 16-bit register provides for a delay of up to 65.535 Seconds
;           requires delay1mS

delayYx1mS:
    rcall    delay1mS                        ; delay for 1 mS
    sbiw    YH:YL, 1                        ; update the the delay counter
    brne    delayYx1mS                      ; counter is not zero

; arrive here when delay counter is zero (total delay period is finished)
    ret
; ---------------------------------------------------------------------------
; Name:     delayTx1mS
; Purpose:  provide a delay of (temp) x 1 mS
; Entry:    (temp) = delay data
; Exit:     no parameters
; Notes:    the 8-bit register provides for a delay of up to 255 mS
;           requires delay1mS

delayTx1mS:
    rcall    delay1mS                        ; delay for 1 mS
    dec     temp                            ; update the delay counter
    brne    delayTx1mS                      ; counter is not zero

; arrive here when delay counter is zero (total delay period is finished)
    ret

; ---------------------------------------------------------------------------
; Name:     delay1mS
; Purpose:  provide a delay of 1 mS
; Entry:    no parameters
; Exit:     no parameters
; Notes:    chews up fclk/1000 clock cycles (including the 'call')

delay1mS:
    push    YL                              ; [2] preserve registers
    push    YH                              ; [2]
    ldi     YL, low(((fclk/1000)-18)/4)     ; [1] delay counter              (((fclk/1000)-18)/4)
    ldi     YH, high(((fclk/1000)-18)/4)    ; [1]                            (((fclk/1000)-18)/4)

delay1mS_01:
    sbiw    YH:YL, 1                        ; [2] update the the delay counter
    brne    delay1mS_01                     ; [2] delay counter is not zero

; arrive here when delay counter is zero
    pop     YH                              ; [2] restore registers
    pop     YL                              ; [2]
    ret                                     ; [4]
; ============================== End of Time Delay Subroutines ==============  
