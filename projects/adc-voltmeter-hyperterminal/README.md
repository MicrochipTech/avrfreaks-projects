# ADC Voltmeter via HyperTerminal

Uploaded by maquad on 2011-02-02 17:48:00 (rating 0 out of 5)

## Summary

.include "m8def.inc"


.cseg  

.org 0x0000  

// interrupt vectors  

rjmp Start ; Reset Handler  

rjmp Start;EXT\_INT0 ; IRQ0 Handler  

rjmp Start;EXT\_INT1 ; IRQ1 Handler  

rjmp Start;TIM2\_COMP ; Timer2 Compare Handler  

rjmp Start;TIM2\_OVF ; Timer2 Overflow Handler  

rjmp Start;TIM1\_CAPT ; Timer1 Capture Handler  

rjmp Start;TIM1\_COMPA ; Timer1 CompareA Handler  

rjmp Start;TIM1\_COMPB ; Timer1 CompareB Handler  

rjmp Start;TIM1\_OVF ; Timer1 Overflow Handler  

rjmp Start;TIM0\_OVF ; Timer0 Overflow Handler  

rjmp Start;SPI\_STC ; SPI Transfer Complete Handler  

rjmp Start;USART\_RXC ; USART RX Complete Handler  

rjmp Start;USART\_UDRE ; UDR Empty Handler  

rjmp Start;USART\_TXC ; USART TX Complete Handler  

rjmp Start;ADC ; ADC Conversion Complete Handler  

rjmp Start;EE\_RDY ; EEPROM Ready Handler  

rjmp Start;ANA\_COMP ; Analog Comparator Handler  

rjmp Start;TWSI ; Two-wire Serial Interface Handler  

rjmp Start;SPM\_RDY ; Store Program Memory Ready Handler


Start:


// initialize stack pointer to point to end of memory


ldi r16, low(RAMEND)  

out SPL, r16  

ldi r16, high(RAMEND)  

out SPH, r16


// initialization PORT B  

ldi r16, 0b00000001  

out DDRB, r16 //set port B0 as output  

clr r16  

out PORTB, r16 //set portb without pullup  

// initialization PORT C  

clr r16  

out DDRC, r16 //set port input  

out PORTC, r16 //set portc without pullup  

// initialization PORT D  

out DDRD, r16 //set port input  

out PORTD, r16 //set portd without pullup


// initialization USART  

;SET UCSRA BIT  

;7=RXC(R);6=TXC(R/W);5=UDRE(R);4=FE(R);3=DOR(R);2=PE(R);1=U2X(R/W);0=MPXM(R/W)  

ldi R16, 0b00100000  

out UCSRA, R16  

;SET UCSRB BIT  

;7=RXCIE(R/W);6=TXCIE(R/W);5=UDRIE(R/W);4=RXEN(R/W);3=TXEN(R/W);2=UCSZ2(R/W);1=RXB8(R);0=TXB8(R/W)  

ldi R16, 0b00011000  

out UCSRB, R16  

;Baud rate = 51 ;9600 BPS  

ldi R16, 0b00000000  

out UCSRC, R16  

ldi R16, 0b00000000  

out UBRRH, R16  

ldi R16, 0b00110011  

out UBRRL, R16  

;SET UCSRC BIT  

;7=URSEL(R/W);6=UMSEL(R/W);5=UPM1(R/W);4=UPM0(R/W);3=USBS(R/W);2=UCSZ1(R/W);1=UCSZ0(R/W);0=UCPOL(R/W)  

ldi R16, 0b10000110  

out UCSRC, R16


// initialization ADC  

;SET ADMUX BIT  

;7=REFS1(R/W);6=REFS0(R/W);5=ADLAR(R/W);4=NotUsed;3=MUX3(R/W);2=MUX2(R/W);1=MUX1(R/W);0=MUX0(R/W)  

ldi R16, 0b01000101  

out ADMUX, R16  

;SET ADCSRA BIT  

;7=ADEN(R/W);6=ADSC(R/W);5=ADFR(R/W);4=ADIF(R/W);3=ADIE(R/W);2=ADPS2(R/W);1=ADPS1(R/W);0=ADPS0(R/W)  

ldi R16, 0b10000000  

out ADCSRA, R16


// main routine loop  

Main:  

//ricezione input da tastiera  

 rcall RICEVI  

 mov R16,R19  

//visualizza =  

 ldi R19,'='  

 rcall INVIA  

//aquisizione  

 rcall ADCRESULT  

//conversione e visualizzazione  

 mov r16,r18  

 rcall CONVASCIIBIN  

 mov r16,r17  

 rcall CONVASCIIBIN  

 //visualizza ' '  

 ldi R19,' '  

 rcall INVIA  

//segnalazione led  

 ldi R19,13  

 rcall INVIA  

 rcall LED  

rjmp Main  

;====================================================================  

// SUBROUTINES


LED:  

 sbi PORTB,0  

 rcall delay  

 cbi PORTB,0  

 rcall delay  

ret


delay: //delay 1s  

 ldi r16, 255  

 ldi r17, 255  

 ldi r18, 1  

Loop\_delay:  

 dec r16 ; fast count  

 brne Loop\_delay  

 dec r17 ; medium count  

 brne Loop\_delay  

 dec r18 ; slow count  

 brne Loop\_delay  

 ret ; done


RICEVI:


; Wait for data to be received  

 sbis UCSRA, RXC  

 rjmp RICEVI  

; Get and return received data from buffer  

 in r19, UDR  

 ret


INVIA:  

; Wait for empty transmit buffer  

 sbis UCSRA,UDRE  

 rjmp INVIA  

; Put data (r16) into buffer, sends the data  

 out UDR,r19  

 ret


;Conversione ASCII BINARIO  

;IN= R16  

CONVASCIIBIN:  

;if R16>128  

 cpi r16,128  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,128  

 rjmp loop\_convasciibin10  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin10:  

;if R16>64  

 cpi r16,64  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,64  

 rjmp loop\_convasciibin20  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin20:  

;if R16>32  

 cpi r16,32  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,32  

 rjmp loop\_convasciibin30  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin30:  

;if R16>16  

 cpi r16,16  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,16  

 rjmp loop\_convasciibin40  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin40:  

;if R16>8  

 cpi r16,8  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,8  

 rjmp loop\_convasciibin50  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin50:  

;if R16>4  

 cpi r16,4  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,4  

 rjmp loop\_convasciibin60  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin60:  

;if R16>2  

 cpi r16,2  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,2  

 rjmp loop\_convasciibin70  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin70:  

;if R16>1  

 cpi r16,1  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,1  

 rjmp loop\_convasciibin80  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin80:  

 ret


ADCRESULT:  

 ;start conversion  

 sbi ADCSRA, ADSC  

 ;end conversion  

 sbic ADCSRA,ADSC  

 rjmp PC-1  

 ;carica il volore in r16  

 in r17,ADCL  

 in r18,ADCH  

 ret

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
