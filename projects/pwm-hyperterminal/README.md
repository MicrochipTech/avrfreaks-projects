# PWM via HyperTerminal

Uploaded by maquad on 2011-02-02 17:49:00 (rating 0 out of 5)

## Summary

;-----------------------------------------------------------------------------  

;Atmega8:  

; 1 - PC6 (RESET) -RESET  

; 2 - PD0 (RXD) -RXD  

; 3 - PD1 (TXD) -TXD  

; 4 - PD2 (INT0) -SWITCH ON/OFF  

; 5 - PD3 (INT1) -STEP DOWN out  

; 6 - PD4 (XCK/T0) -  

; 7 - VCC  

; 8 - GND  

; 9 - PB6 (XTAL1/TOSC1) -8 MHz  

; 10 - PB7 (XTAL2/TOSC2)-8 MHz  

; 11 - PD5 (T1) -OUT LED ROSSO  

; 12 - PD6 (AIN0) -OUT LED GIALLO  

; 13 - PD7 (AIN1) -OUT LED VERDE2  

; 14 - PB0 (ICP1) -OUT LED VERDE1  

; 15 - PB1 (OC1A) -OUT PWM1 LED LUXEON  

; 16 - PB2 (SS/OC1B) -  

; 17 - PB3 (MOSI/OC2) -  

; 18 - PB4 (MISO) -  

; 19 - PB5 (SCK) -  

; 20 - AVCC -VCC  

; 21 - AREF -VCC  

; 22 - GND  

; 23 - PC0 (ADC0) -  

; 24 - PC1 (ADC1) -  

; 25 - PC2 (ADC2) -  

; 26 - PC3 (ADC3) -  

; 27 - PC4 (ADC4/SDA) -  

; 28 - PC5 (ADC5/SCL) -POTENZIOMETRO ESTERNO  

;-----------------------------------------------------------------------------


.include "m8def.inc"


.def R0X10=R16  

.def R0X11=R17  

.def R0X12=R18  

.def R0X13=R19  

.def R0X14=R20  

.def R0X15=R21  

.def R0X16=R22  

.def R0X17=R23  

.def R0X18=R24  

.def R0X19=R25  

.def R0X1A=R26  

.def R0X1B=R27  

.def R0X1C=R28  

.def R0X1D=R29  

.def R0X1E=R30  

.def R0X1F=R31


.equ v\_reg = 167 ;target voltage * 33.33  

.equ v\_sdown = 150 ;shutdown voltage * 50  

.equ max\_pwm = 230 ;PWM maximum  

.equ min\_pwm = 4 ;PWM minimum


.cseg  

.org 0x0000  

// interrupt vectors  

rjmp Start ; Reset Handler  

rjmp EXT\_INT0 ; IRQ0 Handler  

rjmp EXT\_INT1 ; IRQ1 Handler  

rjmp Start;TIM2\_COMP ; Timer2 Compare Handler  

rjmp Start;TIM2\_OVF ; Timer2 Overflow Handler  

rjmp Start;TIM1\_CAPT ; Timer1 Capture Handler  

rjmp Start;TIM1\_COMPA ; Timer1 CompareA Handler  

rjmp Start;TIM1\_COMPB ; Timer1 CompareB Handler  

rjmp Start;TIM1\_OVF ; Timer1 Overflow Handler  

rjmp Start;TIM0\_OVF ; Timer0 Overflow Handler  

rjmp Start;SPI\_STC ; SPI Transfer Complete Handler  

rjmp USART\_RXC ; USART RX Complete Handler  

rjmp Start;USART\_UDRE ; UDR Empty Handler  

rjmp Start;USART\_TXC ; USART TX Complete Handler  

rjmp ADConverter; ADC Conversion Complete Handler  

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

ldi r16, 0b00000011  

out DDRB, r16 //set port B0,B1 as output  

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


/*  

// initialization ADC  

;SET ADMUX BIT  

;7=REFS1(R/W);6=REFS0(R/W);5=ADLAR(R/W);4=NotUsed;3=MUX3(R/W);2=MUX2(R/W);1=MUX1(R/W);0=MUX0(R/W)  

ldi R16, 0b01100101  

out ADMUX, R16  

;SET ADCSRA BIT  

;7=ADEN(R/W);6=ADSC(R/W);5=ADFR(R/W);4=ADIF(R/W);3=ADIE(R/W);2=ADPS2(R/W);1=ADPS1(R/W);0=ADPS0(R/W)  

ldi R16, 0b10000000  

out ADCSRA, R16


*/  

// initialization PWM  

;SET TCCR1A BIT  

;7=COM1A1(R/W);6=COM1A0(R/W);5=COM1B1(R/W);4=COM1B0(R/W);3=FOC1A(W);2=FOC1B(W);1=WGM11(R/W);0=WGM10(R/W)  

ldi r16,0b10000001 ;Fast PWM, 8-bit  

out TCCR1A,r16  

;SET TCCR1B BIT  

;7=ICNC1(R/W);6=ICES1(R/W);5=NotUsed;4=WGM13(R/W);3=WGM12(R/W);2=CS12(R/W);1=CS11(R/W);0=CS10(R/W)  

ldi r16,0b00001001  

out TCCR1B,r16  

ldi r16,0x00 ;0% PWM at start  

out OCR1AL,r16 ;set PWM value


// main routine loop  

Main:  

// spampa valore duty cycle e prescale  

 in r16,OCR1AL  

 rcall CONVASCIIBIN  

 ;spazio  

 ldi r19,32  

 rcall INVIA  

 //stampa prescale  

 in r16,TCCR1B  

 rcall CONVASCIIBIN  

 //torna all'inizio riga  

 ldi r19,15  

 rcall INVIA  

 ldi r19,13  

 rcall INVIA  

 ldi r19,10  

 rcall INVIA


//ricezione input da tastiera  

 rcall RICEVI  

//Se premuto + incrementa duty cycle  

 cpi r19,43 ; 43 valore ascii del pulsante '+'  

 brne loop\_main10  

 in r17,OCR1AL  

 inc r17  

 out OCR1AL,r17  

loop\_main10:  

//Se premuto - riduci duty cycle  

 cpi r19,45 ; 45 valore ascii del pulsante '-'  

 brne loop\_main20  

 in r17,OCR1AL  

 dec r17  

 out OCR1AL,r17  

loop\_main20:  

//se hai premuto * incrementa il prescale  

 cpi r19,42 ; 42 valore ascii del pulsante '*'  

 brne loop\_main30  

 in r16,TCCR1B  

 inc r16  

 out TCCR1B,r16  

loop\_main30:  

//se hai premuto / riduce il prescale  

 cpi r19,47 ; 47 valore ascii del pulsante '/'  

 brne loop\_main40  

 in r16,TCCR1B  

 dec r16  

 out TCCR1B,r16  

loop\_main40:  

 rjmp Main  

;====================================================================  

// INTERRUPT  

;====================================================================


EXT\_INT0: ;collegato il trasformatore esterno  

 reti


EXT\_INT1: ;Switch LED ON/OFF  

; sbic ; interrutore spento  

; sbis ;interruttore acceso  

 reti


USART\_RXC: ;entra in modalitÃ  controllo da Hyperterminal  

 reti


ADConverter: ;acquisizione effettuata  

 ;aggiorniamo  

 reti  

;====================================================================  

// SUBROUTINES  

;====================================================================  

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

; in r16,ADCL  

 in r16,ADCH  

 ret

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
