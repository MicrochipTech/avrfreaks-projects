# DTMF with More Accurate Tones

Uploaded by djdyyz on 2010-11-14 15:47:00 (rating 0 out of 5)

## Summary

This code is a modification of AVR314 code to improve frequency accuracy.


The tone frequency accuracy for the existing code is -0.70% to +1.10%.  

This is not satisfactory, but it is barely adequate.  

However, the accuracy can be improved significantly with a small modification.


Tones are produced by incrementing a phase counter,  

and using that counter to point to a table to sinewave applitudes.  

The phase counter increments are different for each of the 8 tones.  

The 697 tone has a phase counter increment of 46 in the AVR314 code.  

The precise value for the 697 tone phase increment is:


697 * 8 * 128 * 510 / 8000000


This is 45.50016 exactly, but 46 is the closest integer.


When the increment is added to the pointer,  

a carry occurs from the low byte of the pointer to the high byte.  

This occurs every (256/increment-size),  

For the 697 tone, this should occur every 256/45.50016 additions, or 5.62 times per 256.


Carries are an opportunity to make adjustments to improve the frequency resolution.  

If an increment of 45 where used instead of 46, then the needed increment adjustment would be:  

 45.50016 - 45 = 0.50016  

On each carry, the adjustment would be 0.50016 * 5.62, or about 2.81, for a closest integer of 3.  

This produces a result of 45 + 3/5.62 = 45.534.  

The error from exact is 45.534/45.50016 - 1 = 0.0007437, or about +0.074%.  

The error for the existing AVR314 code for 697 is +1.100%.


The tone frequency accuracy for the modified code is -0.17% to +.14%.  

The accuracies for each tone are listed below:


Tone AVR314 Modified  

697 +1.10% +0.07%  

770 -0.52% -0.14%  

852 +0.68% +0.05%  

941 -0.70% +0.10%  

1209 +0.10% +0.01%  

1336 -0.24% +0.14%  

1477 -0.44% -0.04%  

1633 +0.37% -0.17%


*** Here is the code! ***


;***************************************************************************  

;* A P P L I C A T I O N N O T E F O R T H E A V R F A M I L Y  

;*  

;* Number : AVR314  

;* File Name : "dtmf.asm"  

;* Title : DTMF Generator  

;* Date : 00.06.27  

;* Version : 1.1  

;* Target MCU : Any AVR with SRAM, 8 I/O pins and PWM  

;*  

;* DESCRIPTION  

;* This Application note describes how to generate DTMF tones using a single  

;* 8 bit PWM output.  

;*  

;***************************************************************************  

;  

; (Mods to improve precision for frequencies, by Daryl Diamond; 2010.11.14)  

;  

;***************************************************************************


.include "4414def.inc"


;**************************************************************************  

; REGISTERS  

;**************************************************************************  

.def ptr\_Ltmp = r1 ; low part temporary register  

.def ptr\_Htmp = r2 ; high part temporary register  

.def ptr\_Lb = r3 ; low part low frequency pointer  

.def ptr\_Hb = r4 ; high part low frequency pointer


.def ptr\_La = r5 ; low part high frequency pointer  

.def ptr\_Ha = r6 ; high part high frequency pointer


.def addb\_out = r7 ; low frequency value to be written  

.def adda\_out = r8 ; High frequency value to be written


.def x\_SWa = r9 ; high frequency x\_SW  

.def x\_SWb = r10 ; low frequency x\_SW


.def d\_Swa = r11 ; ADDED ; high frequency delta adjustment  

.def d\_Swb = r12 ; ADDED ; low frequency delta adjustment


.def count = r18 ; temporary counter register  

.def tmp = r16 ; temp register


.def input = r17 ; input from portB


;**************************************************************************  

;**************************************************************************  

.equ Xtal = 8000000 ; system clock frequency  

.equ prescaler = 1 ; timer1 prescaler  

.equ N\_samples = 128 ; Number of samples in lookup table  

.equ Fck = Xtal/prescaler ; timer1 working frequency  

.equ delaycyc = 10 ; port B setup delay cycles


;**************************************************************************  

; PROGRAM START - EXECUTION STARTS HERE  

;**************************************************************************  

.cseg  

.org $0  

 rjmp start ; Reset handler  

.org OVF1addr  

 rjmp tim1\_ovf ; Timer1 overflow Handle


;**************************************************************************  

; Interrupt timer1  

;**************************************************************************  

tim1\_ovf:  

 push tmp ; Store temporary register  

 in tmp,SREG  

 push tmp ; Store status register  

 push ZL  

 push ZH ; Store Z-Pointer  

 push r0 ; Store R0 Register


;high frequency  

 mov ptr\_Ltmp,ptr\_La  

 mov ptr\_Htmp,ptr\_Ha  

 rcall getsample ; read from sample table  

 mov adda\_out,r0 ; adda\_out = high frquency value  

 add ptr\_La,x\_SWa  

brcc SkipCarryAdjustH ; CHANGE ;  

; clr tmp ; CHANGE ; (tmp is cleared, but not the carry flag)  

 inc ptr\_Ha ; CHANGE ; Refresh pointer for the next sample  

 add ptr\_La,d\_Swa ; CHANGE ; add frequency correction delta  

SkipCarryAdjustH: ; CHANGE ;


;low frequency  

 mov ptr\_Ltmp,ptr\_Lb  

 mov ptr\_Htmp,ptr\_Hb  

 rcall getsample ; read from sample table  

 mov addb\_out,r0 ; addb\_out = low frequency value  

 add ptr\_Lb,x\_SWb  

brcc SkipCarryAdjustL ; CHANGE ;  

; clr tmp ; CHANGE ; (tmp is cleared, but not the carry flag)  

 inc ptr\_Hb ; CHANGE ; refresh pointer for the next sample  

 add ptr\_Lb,d\_Swb ; CHANGE ; add frequency correction delta  

SkipCarryAdjustL: ; CHANGE ;


; scale amplitude  

 ldi tmp,2  

 add addb\_out,tmp  

 lsr addb\_out  

 lsr addb\_out ; divide 4 and round off  

 sub r0,addb\_out ; 4/4 - 1/4 = 3/4  

 mov addb\_out,r0 ; now addb\_out has the right amplitude


 clr tmp  

 out OCR1AH,tmp  

 mov tmp,adda\_out  

 add tmp,addb\_out  

 out OCR1AL,tmp ; send the sum of the two amplitudes to PWM


 pop r0 ; Restore R0 Register  

 pop ZH  

 pop ZL ; Restore Z-Pointer  

 pop tmp  

 out SREG,tmp ; Restore SREG  

 pop tmp ; Restore temporary register;  

 reti


;*********************************  

; RESET Interrupt  

;*********************************  

start:  

 sbi DDRD,PD5 ; Set pin PD5 as output  

 ldi tmp,low(RAMEND)  

 out SPL,tmp  

 ldi tmp,high(RAMEND)  

 out SPH,tmp ; Initialize Stackpointer


;Initialization of the registers  

 clr ptr\_La  

 clr ptr\_Ha  

 clr ptr\_Lb  

 clr ptr\_Hb ; Set both table ponters to 0x0000


;enable timer1 interrupt  

 ldi tmp,(1<<TOIE1)  

 out TIMSK,tmp ; Enable Timer1\_ovf interrupt


;set timer1 PWM mode  

 ldi tmp,(1<<PWM10)+(1<<COM1A1)  

 out TCCR1A,tmp ; 8 bit PWM not reverse (Fck/510)  

 ldi tmp,(1<<CS10)  

 out TCCR1B,tmp ; prescaler = 1  

 sei ; Enable interrupts


;**************************************************************************  

; MAIN  

; Read from portB (eg: using evaluation board switch) which  

; tone to generate, extract mixing high frequency  

; (column) and low frequency (row), and then  

; fix x\_SWa and x\_SWb  

; row -> PINB high nibble  

; column -> PINB low nibble  

;**************************************************************************


main:  

;high frequency (Esteem only high nibble that is row)  

;PB\_High\_Nibble:  

 ldi tmp,0x0F  

 out DDRB,tmp ; High nibble Input / Low nibble. Outp.  

 ldi tmp,0xF0  

 out PORTB,tmp ; High nibble PullUp / Low nibble Zero Outp.  

 rcall delay  

 clr count  

 in input,PINB


main10:  

 inc count  

 lsl input  

 brcc main20  

 ldi tmp,4  

 cp count,tmp  

 brne main10  

 clr x\_SWb  

 clr count  

 rjmp main30


main20:  

 dec count


 ldi ZL,low(frequencyL*2)  

 ldi ZH,high(frequencyL*2)  

 add ZL,count  

 clr tmp  

 adc ZH,tmp  

 lpm  

 mov x\_SWb,r0 ; this is low frequency x\_SW


 ldi ZL,low(deltaL*2) ; ADDED  

 ldi ZH,high(deltaL*2) ; ADDED  

 add ZL,count ; ADDED  

 clr tmp ; ADDED  

 adc ZH,tmp ; ADDED  

 lpm ; ADDED  

 mov d\_SWb,r0 ; ADDED ; this is low frequency d\_SW


 clr count


;low frequency  

;PB\_Low\_Nibble:  

main30:  

 ldi tmp,0xF0  

 out DDRB,tmp ; High nibble. Outp. / Low nibble Input  

 ldi tmp,0x0F  

 out PORTB,tmp ; High nibble Zero Outp. / Low nibble PullUp  

 rcall delay  

 in input,PINB  

 swap input


main40:  

 inc count  

 lsl input  

 brcc main50  

 ldi tmp,4  

 cp count,tmp  

 brne main40  

 clr x\_SWa  

 rjmp main


main50: ; there is a zero bit in count-1 position  

 dec count


 ldi ZL,low(frequencyH*2)  

 ldi ZH,high(frequencyH*2)  

 add ZL,count  

 clr tmp  

 adc ZH,tmp  

 lpm  

 mov x\_SWa,r0 ; this is high frequency x\_SW


 ldi ZL,low(deltaH*2) ; ADDED  

 ldi ZH,high(deltaH*2) ; ADDED  

 add ZL,count ; ADDED  

 clr tmp ; ADDED  

 adc ZH,tmp ; ADDED  

 lpm ; ADDED  

 mov d\_SWa,r0 ; ADDED ; this is high frequency d\_SW


 rjmp main


;****************** DELAY ***********************************  

;****************************************************************  

delay:  

 ldi tmp,delaycyc  

loop:  

 dec tmp  

 brne loop  

ret


;****************** GET SAMPLE ******************************  

;****************************************************************  

getsample:  

 ldi tmp,0x0f  

 and ptr\_Htmp,tmp


; ROUND  

 ldi tmp,4  

 add ptr\_Ltmp,tmp  

 clr tmp  

 adc ptr\_Htmp,tmp


;shift (divide by eight):  

 lsr ptr\_Htmp  

 ror ptr\_Ltmp  

 lsr ptr\_Htmp  

 ror ptr\_Ltmp  

 lsr ptr\_Htmp  

 ror ptr\_Ltmp


 ldi tmp,0x7f  

 and ptr\_Ltmp,tmp ; module 128 (samples number sine table)


 ldi ZL,low(sine\_tbl*2)  

 ldi ZH,high(sine\_tbl*2)  

 add ZL,ptr\_Ltmp  

 clr tmp  

 adc ZH,tmp ; Z is a pointer to the correct  

 ; sine\_tbl value  

 lpm  

 ret


;*************************** SIN TABLE *************************************  

; Samples table : one period sampled on 128 samples and  

; quantized on 7 bit  

;******************************************************************************  

sine\_tbl:  

.db 64,67  

.db 70,73  

.db 76,79  

.db 82,85  

.db 88,91  

.db 94,96  

.db 99,102  

.db 104,106  

.db 109,111  

.db 113,115  

.db 117,118  

.db 120,121  

.db 123,124  

.db 125,126  

.db 126,127  

.db 127,127  

.db 127,127  

.db 127,127  

.db 126,126  

.db 125,124  

.db 123,121  

.db 120,118  

.db 117,115  

.db 113,111  

.db 109,106  

.db 104,102  

.db 99,96  

.db 94,91  

.db 88,85  

.db 82,79  

.db 76,73  

.db 70,67  

.db 64,60  

.db 57,54  

.db 51,48  

.db 45,42  

.db 39,36  

.db 33,31  

.db 28,25  

.db 23,21  

.db 18,16  

.db 14,12  

.db 10,9  

.db 7,6  

.db 4,3  

.db 2,1  

.db 1,0  

.db 0,0  

.db 0,0  

.db 0,0  

.db 1,1  

.db 2,3  

.db 4,6  

.db 7,9  

.db 10,12  

.db 14,16  

.db 18,21  

.db 23,25  

.db 28,31  

.db 33,36  

.db 39,42  

.db 45,48  

.db 51,54  

.db 57,60


;******************************* x\_SW ***********************************  

;Table of x\_SW (excess 8): x\_SW = ROUND(8*N\_samples*f*510/Fck)  

;**************************************************************************


;high frequency (coloun)  

;1209hz ---> x\_SW = 79  

;1336hz ---> x\_SW = 87  

;1477hz ---> x\_SW = 96  

;1633hz ---> x\_SW = 107


frequencyH:  

.db 106,96 ; CHANGED 107 to 106  

.db 87,78 ; CHANGED 79 to 78


deltaH: ; ADDED  

.db 1,1 ; ADDED  

.db 1,3 ; ADDED


;low frequency (row)  

;697hz ---> x\_SW = 46  

;770hz ---> x\_SW = 50  

;852hz ---> x\_SW = 56  

;941hz ---> x\_SW = 61


frequencyL:  

.db 61,55 ; CHANGED 56 to 55  

.db 50,45 ; CHANGED 46 to 45


.deltaL: ; ADDED  

.db 2,3 ; ADDED  

.db 1,3 ; ADDED

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
