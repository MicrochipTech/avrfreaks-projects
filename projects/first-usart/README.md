# first USART

Uploaded by Benski on 2009-09-21 08:12:00 (rating 0 out of 5)

## Summary

This program is to send a ASCII character out of the ATMEGA 8 (or any other AVR with USART capability) and in series with a MAX232 be received by a computer.  

Pretty simple compared to some projects out their but is REALLY satisfying to see it work especially if most of your projects like mine end up taking you a month or so!! :D 


;=====================================================================


.device ATmega8  

.nolist  

.include "C:\Program Files\Atmel\AVR Tools\AvrAssembler2\Appnotes\m8def.inc"  

.list  

;==============================


;TxD is PD1  

;RxD is PD0  

;UCSRA â€“ USART Control and Status Register A  

;Mostly status, however 2 bits control speed and mode  

;UCSRB â€“ USART Control and Status Register B  

;Mostly control bits, however 2 are data bits  

;UCSRC â€“ USART Control and Status Register C  

;All control related bits


;set baudrate to ???  

.equ brp = 23 ;Baud rate parameter  

ldi R17, high(brp)  

ldi R16, low(brp)  

out UBRRH, R17  

out UBRRL, R16


 ;UCSRC  

 ;bit 7: URSEL (register select (1=UCSRC)  

 ;bit 6: UMSEL (mode select) (0=asynchronous)  

 ;bits 5:4: UPM1:0 (parity mode)  

 ;00 disabled, 10 even, 11, odd  

 ;bit 3: USBS (stop bits) (0=1 stop bit)  

 ;bits 2:1: UCSZ1:0 (data size) (11 = 8 bit data)  

 ;There is one more bit at UCSRB2 (UCSZ2) which is set to 1 for 9-bit data  

 ;bit 0: UCPOL (clock polarity) (synch mode only)  

ldi R16, 0b10000110 ;set output to 8N1 - not strictly requried as default is 8N1  

out UCSRC, R16


;Set the transmit and receive enable bits, clearing others  

;ldi R16, (1<<RXEN)|(1<<TXEN)  

;out UCSRB, R16  

;Or simply set the two enable bits, leaving everything else in the register unchanged  

sbi UCSRB, RXEN  

sbi UCSRB, TXEN


;transmit  

;Byte to send is  

;in R19  

transmit: 


;=====================================================================

## Tags

- Complete code
