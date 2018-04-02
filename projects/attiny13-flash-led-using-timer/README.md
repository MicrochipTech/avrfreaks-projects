# ATtiny13 Flash Led Using The Timer

Uploaded by twogreysquirrels on 2018-04-02 14:16:00 (rating 0 out of 5)

## Summary

;--------------------------------------------------------------------------------------  

; Name:        t13timer2.asm  

; Author:    twogreysquirrels  

; Date:        20180401  

; Purpose:    Flash an LED on PB0 using the timer  

; Compiler:    gavrasm ([http://www.avr-asm-tutorial.net/...](http://www.avr-asm-tutorial.net/gavrasm/index_en.html))  

;--------------------------------------------------------------------------------------  

; Circuit diagram for ATtiny 13  

;  

;                    -----  

;                PB5    |    | VCC (2.7V TO 5.5V)  

;                PB3    |    | PB2  

;                PB4    |    | PB1  

;                GND    |    | PB0 --| 220 |-short leg-LED--VCC  

;                    -----              

;                                          

;  

; As per the diagram connect a 220 ohm resistor to PB0 which is pin 5. The short leg of an LED is connected to the resistor. The long leg of the LED is connected to the positive power supply.  

;--------------------------------------------------------------------------------------  

;  

; tell the assembler about the processor we are using  

.DEVICE ATtiny13  

;  

;--------------------------------------------------------------------------------------  

; The Warehouse  

;--------------------------------------------------------------------------------------  

.def carrie = R16 ; Multi purpose register  

;  

;--------------------------------------------------------------------------------------  

; The Setup  

;--------------------------------------------------------------------------------------  

; set PORTB0 PB0 for output  

            sbi DDRB,DDB0


; Set the value we want the timer A to compare  

; This may be any value of 255 or less  

; When the timer reaches this value it will reset  

            ldi carrie,16  

            out OCR0A,carrie


; When the eight bit timer reaches the compare value it toggles the state of PB0  

            ldi carrie,1<<COM0A0  

            out TCCR0A,carrie


; set timer prescaler (the prescaler is a divider) to 1024  

; this will also start the timer  

            ldi carrie,(1<<CS02)|(1<<CS00)  

            out TCCR0B,carrie  

;--------------------------------------------------------------------------------------  

;The Play  

;--------------------------------------------------------------------------------------  

mloop:  

; go back Jack, do it again  

            rjmp mloop  

;--------------------------------------------------------------------------------------

## Tags

- Complete code
- ATTtiny13
- Simple Timer Example
