;***************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;*
;* Number               : AVR314
;* File Name            : "dtmf.asm"
;* Title                : DTMF Generator
;* Date                 : 00.06.27
;* Version              : 1.1
;* Target MCU           : Any AVR with SRAM, 8 I/O pins and PWM
;*
;* DESCRIPTION
;* This Application note describes how to generate DTMF tones using a single
;* 8 bit PWM output.
;*
;***************************************************************************
;
; Modified with a kludge to adjust 697 tone only.
; Error now +0.06% from true. (Was +1.10% from true.)
;
;***************************************************************************

.include "4414def.inc"

;**************************************************************************
;  REGISTERS
;**************************************************************************
.def   ptr_Ltmp = r1                    ; low  part temporary register
.def   ptr_Htmp = r2                    ; high part temporary register
.def   ptr_Lb   = r3                    ; low  part low frequency pointer
.def   ptr_Hb   = r4                    ; high part low frequency pointer

.def   ptr_La   = r5                    ; low  part high frequency pointer
.def   ptr_Ha   = r6                    ; high part high frequency pointer

.def   addb_out = r7                    ; low  frequency value to be written                            
.def   adda_out = r8                    ; High frequency value to be written

.def   x_SWa = r9                    	; high frequency x_SW
.def   x_SWb = r10                   	; low  frequency x_SW

.def   count    = r18                   ; temporary counter register
.def   tmp      = r16                   ; temp register

.def   input    = r17                   ; input from portB

;**************************************************************************
;**************************************************************************
.equ     Xtal      = 8000000            ; system clock frequency
.equ     prescaler = 1                  ; timer1 prescaler
.equ     N_samples = 128                ; Number of samples in lookup table
.equ     Fck = Xtal/prescaler           ; timer1 working frequency
.equ	 delaycyc  = 10			; port B setup delay cycles

;**************************************************************************
;  PROGRAM START - EXECUTION STARTS HERE
;**************************************************************************
.cseg
.org    $0
    rjmp    start                       ; Reset handler
.org    OVF1addr
    rjmp    tim1_ovf                    ; Timer1 overflow Handle

;**************************************************************************
; Interrupt timer1
;**************************************************************************
tim1_ovf:
   push   tmp    			; Store temporary register
   in     tmp,SREG
   push   tmp                           ; Store status register
   push   ZL
   push   ZH                            ; Store Z-Pointer
   push   r0                            ; Store R0 Register

;high frequency
   mov    ptr_Ltmp,ptr_La
   mov    ptr_Htmp,ptr_Ha
   rcall  getsample                     ; read from sample table
   mov    adda_out,r0                   ; adda_out = high frquency value
   add    ptr_La,x_SWa
   clr    tmp                           ; (tmp is cleared, but not the carry flag)
   adc    ptr_Ha,tmp                    ; Refresh pointer for the next sample

;low frequency
   mov    ptr_Ltmp,ptr_Lb
   mov    ptr_Htmp,ptr_Hb
   rcall  getsample                     ; read from sample table
   mov    addb_out,r0                   ; addb_out = low frequency value
   add    ptr_Lb,x_SWb
;                                       ; MOD
; MODS to improve 697Hz precision       ; MOD
; Error 697 changes from +1.1% to +.06% ; MOD
;  clr    tmp                           ; MOD ; (tmp is cleared, but not the carry flag)
;  adc    ptr_Hb,tmp                    ; MOD ; refresh pointer for the next sample
brcc DoneLowFreq                        ; MOD
   inc    ptr_Hb                        ; MOD
   mov    tmp,x_Swb                     ; MOD ; Kludge to improve 697Hz freq precision
   cpi    tmp,46                        ; MOD ; Is this the 697 x_Swb ?
   brne   DoneLowFreq                   ; MOD
   mov    tmp,3                         ; MOD
   cp     tmp,ptr_Lb                    ; MOD ; Is there room to make an adjustment w/o a carry?
   brsh   DoneLowFreq                   ; MOD ; If adjustment will not cause a carry
   sub    ptr_Lb,tmp                    ; MOD ; subtract adjustment from low freq ptr
DoneLowFreq:                            ; MOD

; scale amplitude
   ldi     tmp,2
   add     addb_out,tmp
   lsr     addb_out
   lsr     addb_out                     ; divide 4 and round off
   sub     r0,addb_out                  ; 4/4 - 1/4 = 3/4
   mov     addb_out,r0                  ; now addb_out has the right amplitude

   clr     tmp
   out     OCR1AH,tmp
   mov     tmp,adda_out
   add     tmp,addb_out
   out     OCR1AL,tmp                   ; send the sum of the two amplitudes to PWM

   pop     r0                           ; Restore R0 Register
   pop     ZH
   pop     ZL                           ; Restore Z-Pointer
   pop     tmp
   out     SREG,tmp                     ; Restore SREG
   pop     tmp                          ; Restore temporary register;
   reti

;*********************************
; RESET Interrupt
;*********************************
start:
    sbi   DDRD,PD5                      ; Set pin PD5 as output
    ldi   tmp,low(RAMEND)
    out   SPL,tmp
    ldi   tmp,high(RAMEND)
    out   SPH,tmp                       ; Initialize Stackpointer

;Initialization of the registers
    clr   ptr_La
    clr   ptr_Ha
    clr   ptr_Lb
    clr   ptr_Hb                        ; Set both table ponters to 0x0000

;enable timer1 interrupt
    ldi   tmp,(1<<TOIE1)
    out   TIMSK,tmp                     ; Enable Timer1_ovf interrupt

;set timer1 PWM mode
     ldi   tmp,(1<<PWM10)+(1<<COM1A1)
     out   TCCR1A,tmp                   ; 8 bit PWM not reverse (Fck/510)
     ldi   tmp,(1<<CS10)
     out   TCCR1B,tmp                   ; prescaler = 1
     sei                                ; Enable interrupts

;**************************************************************************
; MAIN
; Read from portB (eg: using evaluation board switch) which
; tone to generate, extract mixing high frequency
; (column) and low frequency (row), and then
; fix x_SWa and x_SWb
; row    -> PINB high nibble
; column -> PINB low nibble
;**************************************************************************

main:
;high frequency (Esteem only high nibble that is row)
;PB_High_Nibble:
   ldi   tmp,0x0F
   out 	 DDRB,tmp			; High nibble Input / Low nibble. Outp. 
   ldi   tmp,0xF0
   out   PORTB,tmp			; High nibble PullUp / Low nibble Zero Outp. 
   rcall delay
   clr   count
   in    input,PINB

main10:
   inc   count
   lsl   input	    
   brcc  main20
   ldi   tmp,4
   cp    count,tmp
   brne  main10
   clr   x_SWb
   clr   count
   rjmp  main30

main20:
   dec   count
   ldi   ZL,low(frequencyL*2)
   ldi   ZH,high(frequencyL*2)
   add   ZL,count
   clr   tmp
   adc   ZH,tmp
   lpm
   mov   x_SWb,r0                    	; this is low frequency x_SW
   clr   count
   
;low frequency
;PB_Low_Nibble:
main30:
   ldi   tmp,0xF0
   out 	 DDRB,tmp			; High nibble. Outp. / Low nibble Input  
   ldi   tmp,0x0F
   out   PORTB,tmp			; High nibble Zero Outp. / Low nibble PullUp 
   rcall delay
   in    input,PINB
   swap  input

main40:
   inc   count
   lsl   input	    
   brcc  main50
   ldi   tmp,4
   cp    count,tmp
   brne  main40
   clr   x_SWa
   rjmp  main

main50:                                 ; there is a zero bit in count-1  position
   dec   count
   ldi   ZL,low(frequencyH*2)
   ldi   ZH,high(frequencyH*2)
   add   ZL,count
   clr   tmp
   adc   ZH,tmp
   lpm
   mov   x_SWa,r0                    	; this is high frequency x_SW
   rjmp  main


;******************   DELAY   ***********************************
;****************************************************************
delay:
   ldi	  tmp,delaycyc
loop:
   dec	  tmp
   brne	  loop
ret


;******************   GET SAMPLE   ******************************
;****************************************************************
getsample:
   ldi    tmp,0x0f
   and    ptr_Htmp,tmp

; ROUND	
   ldi    tmp,4
   add    ptr_Ltmp,tmp
   clr    tmp
   adc    ptr_Htmp,tmp

;shift (divide by eight):
   lsr    ptr_Htmp
   ror    ptr_Ltmp
   lsr    ptr_Htmp
   ror    ptr_Ltmp
   lsr    ptr_Htmp
   ror    ptr_Ltmp

   ldi    tmp,0x7f
   and    ptr_Ltmp,tmp                  ; module 128 (samples number sine table)

   ldi    ZL,low(sine_tbl*2)
   ldi    ZH,high(sine_tbl*2)
   add    ZL,ptr_Ltmp
   clr    tmp
   adc    ZH,tmp                        ; Z is a pointer to the correct
                                        ; sine_tbl value
   lpm
   ret

;*************************** SIN TABLE *************************************
; Samples table : one period sampled on 128 samples and
; quantized on 7 bit
;******************************************************************************
sine_tbl:
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

;*******************************  x_SW  ***********************************
;Table of x_SW (excess 8): x_SW = ROUND(8*N_samples*f*510/Fck)
;**************************************************************************

;high frequency (coloun)
;1209hz  ---> x_SW = 79
;1336hz  ---> x_SW = 87
;1477hz  ---> x_SW = 96
;1633hz  ---> x_SW = 107

frequencyH:
.db 107,96
.db 87,79

;low frequency (row)
;697hz  ---> x_SW = 46
;770hz  ---> x_SW = 50
;852hz  ---> x_SW = 56
;941hz  ---> x_SW = 61

frequencyL:
.db 61,56
.db 50,46




