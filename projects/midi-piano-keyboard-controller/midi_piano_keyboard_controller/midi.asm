; midi piano keyboard controller
;
; Author Joachim Beyer achim@itmin.com
; Schematic: midi.sch
; PCB lauout in tif available
;
; yamaha YP-40 clavicord adr scheme, 7 octaves, 12 semitones: c# ...c
; 76 keys E..G'with velocity detection

; each key has an upper switch N/C and a lower N/O
; this enables to measure the time from the still position upto touch down.
; the time is a measure of velocity and volume control.
; Timings have been tested with the help of a logic controller.
; it is between 20 and 5 ms (slow...fast)
; the sitches bounce a lot and are both debounced by the routines.
;
; low nibble port-a = semitones 1..12 , 0 and 13..15 not used.
; high nibble = octave 1...7 ,0 adr not used
; adr lowest key E= $14 = 28midi; highest key G=$77 = 103 midi
; keys adr porta $14-$77
;
; For each key is a staus byte provided in RAM
; Pointer X for status bytes: Xh=1 XL=keyadr equ to porta
;
; status code scheme:
;A $ff adr not used since only 12 tones are used of 16 addresses
;B $00 key is still
;C 18...1 velocity (counting down from 'key star't until 'key down')
; velocity is multiplied by factor 7 for a range of 7..126 as
; defined by the midi standard (1...128 = 7 bits)
;D status bit7= debounce period after key down: count down from 1000 0111b
;E status bit7 + 6 = complement of velocity after debouncing. the complement
; sets automaticly both bits to define the key down status
;F status bit5 (status= 00100000b)= debounce period after key release
; to 'key still'.
;
; velocity is stored at (adr + 100) to accommodate for
; debouncing count down during status D
;
; A FIFO is provided in the lower RAM as a TX buffer at RAM $60 upwards with
; pointer Y since a transmission requires at least 2 bytes.
;
; Look-up table
; the midi keynumber is looked up with Z for which the key adr in XL
; is copied to zl with zh=2 as table pointer.
;
; timer 1 generates an intr of 1ms time base to for the main routine
; of keyboard scan and to determine key velocity.
;
; constants are placed in registers for fast execution
;
; a scan takes 500us without any keys activated. This leaves 500us=2000 mcls
; for status byte, look-up operations and midi transmission initiations.

; note, the main routine is not a interrupt routine - even though it is initiated
; by the timer time base of 1 ms - for the very reason to enable serial port
; interrupts for fast message and data transmission which would otherwise be
; inhibited by a non-completed interrupt routine.

; output
; midi format channel messages: bit7= message high nibble, low nibble=channel1

; status byte *** Data Byte1*** Data Byte2
; note on= $91*** note number*** velocity
; note off=$81*** note number*** velocity
; control= $B1*** pedal=$40  ***0=ON ; 7=OFF
;

.equ basel= $78 ;77+1 highest key adr
.equ baseh= 1
.equ fifo= $60




.def t1= r16 ;temporary
.def t2= r17
.def stat= r18 ;status register copied from RAM
.def read= r19 ;port a input
.def flag= r20 ;pedal flags
.def factor= r21 ;mul factor of velocity

.def last= r1 ;last, lowest key adr
.def max= r2
.def offset= r3
.def mesg1= r4 ;midi messages
.def mesg2= r5
.def contrchange= r6 ;midi message
.def pedon= r7 ;pedal
.def pedoff=r8
.def hundred=r9 ;delay
.def bounce= r10
.def off= r11


.include "D:\avrtools\appnotes\8515def.inc"

.cseg

         rjmp init
         reti                 ;int0
         reti                 ;int1
         reti                 ;t1capt
         rjmp Tflag           ;t1 comp match a
         reti                 ;t1 comp match b
         reti                 ;t1 ovfl
         reti                 ;t0 ovfl
         reti                 ;spi complete
         reti                 ;rxd complete
         reti                 ;rxd empty
         rjmp tx              ;txd complete
         reti                 ;an com 


init:    
         ldi t1,low(ramend)
         out spl,t1
         ldi t1,high(ramend)
         out sph,t1
         rcall raminit

         ldi xl,basel
         ldi xh,baseh
         ldi t1,24
         mov offset,t1
         clr Yh
         ldi yl,fifo
         ldi t1,0b10010001    ;key on midi
         mov mesg1,t1
         ldi t1,0b10000001    ;key off midi
         mov mesg2,t1
         ldi t1,23            ; 5ms min, 20ms max
         mov max,t1           ;18 x 7 =126

         ldi zh,2

         ldi t1,$b1
         mov contrchange,t1
         clr pedoff
         ldi t1,$7f
         mov pedon,t1
         ldi t1,100
         mov hundred,t1
         ldi t1,0b10000111
         mov bounce,t1
         ldi t1,0b11100000
         mov off,t1
         ldi t1,7             ;mul factor 7 ############
         mov factor,t1
         ldi t1,$14           ;last key (lowest address)
         mov last,t1
         ser t1
         out ddra,t1

;UART 4Mhz,31250 Baud
;preset=7
         ldi t1,7
         out ubrr,t1
         ldi t1,0b01001000
         out ucr,t1

;******************************
;timer1b cycles 4000= $fa0 1ms
;outp compare regb

         ldi t1,$0f
         out ocr1ah,t1
         ldi t1,$a0
         out ocr1al,t1
         ldi t1,0b01001
         out tccr1b,t1
         ldi t1,0b01000000
         out timsk,t1         ;timer intr mask reg
         sei
wait:    
         brts scan
         rjmp wait

;*******************************
;intr t1 1ms
Tflag:   
         set
         reti

;*******************************
;main keyboard routine
;time base 1ms to determine the velocity of each key press
;starts decrementing from 23...1
;starts with highest adr $01 77 + 1
;ends with lowest= last
;operates status byte for each key in RAM
;00 still
;ff not used
;18..1 velocity decrement
;1000xxxxb debounce
;1111xxxxb velocity complement after decrement
;oo100000b debounce till key still detection


scan:    
         ldi xl,basel
         ldi xh,baseh
         clt

next:    
         sei
         cp xl,last
         breq finish
         ld stat,-x
         out porta,xl
         cpi stat,$ff
         breq next

         sbrc stat,6
         rjmp keyoff
         sbrc stat,7
         rjmp debounc
         sbrc stat,5
         rjmp keyend
         cpi stat,0
         breq keystart

         dec stat
         BRNE touchdown       ;avoid underflow
         inc stat
         rjmp touchdown

finish:  

         rjmp wait

;******************************************
;key start? transition from start - down
;pinc,0 = N/C if high= key pressed, start
;if low = not pressed
;pinc,1 = N/O if high = normal state

keystart:
         cli
         in read,portc
         cpi read,0b011
         brne next
         st x,max
         rjmp next

;***********************************************
;key touch down?, final velocity count
;
;pinc,1 = low = pressed down
;pinc,0 = high = pressed
;store codes in tx FIFO buffer with Y

touchdown:
         cli
         in read,portc
         cpi read,0b01
         brne store

;transmit FIFO
         cpi yl,fifo          ;buffer empty?
         breq dir
         st y+,mesg1
hjk:                          ;FIFO
         rcall keynumber
         st y+,r0             ;key number to fifo

         rcall multpl         ;velocity
         st y+,r0

;initiate debounce
;store bounce in status byte
;store velocity in temp upper ram = adr + 100 !!

         st x,bounce
         add xl,hundred
         st x,stat            ;velocity
         sub xl,hundred
         rjmp next
dir:     
         out udr,mesg1

         rjmp hjk

store:   st x,stat            ;save new count velocity
         rjmp next

;*********************************************************
;bouncing period after touch down
;dec bounce to zero
;status 1000xxxxb
;in the end stores compl of the velocity as status byte
;

debounc: cli
         dec stat
         cpi stat,0b10000000
         brne opl

;end debounce period
;store complement of velocity as status
         ADD xl,hundred
         ld stat,x
         com stat
         sub xl,hundred
         st x,stat
         rjmp next
opl:     
         st x,stat            ;store decr bounce
         rjmp next


;*************************************************************
;key released ? status off = 00100000b
;This detects whether the key is released and in transition up.
;The 'end of key' message is transmitted along with key number and
;previous velocity. The staus is changed to "off" until the
;key returns into still position to accomodate bouncing again.

keyoff:  
         CLI
         in read,portc
         cpi read,0b011
         breq iop
         rjmp next
;FIFO
iop:     
         cpi yl,fifo
         breq direct
balance: 
         st y+,mesg2
         rcall keynumber
         st y+,r0

;velocity FIFO

         com stat
         rcall multpl
         st y+,r0

         st x,off
         rjmp next

direct:  
         out udr,mesg2

         rjmp balance

;***********************************
;test for key back in still position
;and write zero to status
; = final change to zero status.

keyend:  cli
         in read,portc
         cpi read,0b010
         breq ytu
         rjmp next
ytu:     
         clr stat
         st x,stat
         rjmp next

;********************************
;subroutine
;look up midi key number
;xl copied to zl

keynumber:
         ldi zh,2
         mov zl,xl
         lpm
         ret

;******************************************
; tx intr routine operated by tx completed
; keeps on transmitting bytes from $60
; and shifts FIFO
tx:      
         cpi yl,fifo
         breq empty

         lds t2,$0060
         out udr,t2
         rcall shift
;SBIS usr,udre ;udr empty?
empty:   reti
         rjmp tx

;***********************
; shift FIFO
; sub routine
; yl pointer to last byte +1

shift:   
         cpi yl,$61
         brne eg
         dec yl
         rcall display
         ret
eg:      
         mov t1,yl            ;save adr
         ldi yl,$61           ;next adr
         ldi zl,$60           ;tx buffer
         clr zh
cont:    
         ld t2,y+
         st z+,t2
         cp yl,t1
         breq fini

         rjmp cont
fini:    
         dec yl
         rcall display
         ret

;******************************
;subroutine to compute velocity
;inp stat, outp r0
;limit range between 10..126

multpl:  
         cpi stat,19
         BRLO cor
         ldi stat,18          ;18x7=126
cor:     
         mov t1,stat
         mov r0,stat
         mov t2,factor
vbn:     add r0,t1
         dec t2
         brne vbn
         ret
;************************
;flag 0 = pedal on

pedal:   
         SBRC flag,0
         rjmp ped_off
         sbic portb,4
         ret
         cpi yl,fifo
         breq sdf
         st y+,contrchange
         rjmp mnb
sdf:     out udr,contrchange
mnb:     
         st y+,pedon
         sbr flag,1
         ret
ped_off: 
         sbis portb,4
         ret
         cpi yl,fifo
         breq bvc
         st y+,contrchange
         rjmp rty
bvc:     out udr,contrchange
rty:     st y+,pedoff
         cbr flag,1
         ret

;****************************************
;RAM initalisation of status bytes
;copies table complement to RAM
;for a 0 $FF is stored.
;for any used key a zero is stored as status

raminit: 

         ldi yh,1
         ldi zh,2
         ldi zl,$10
         mov yl,zl
         ldi t1,105           ;105 table bytes

uio:     lpm
         ADIW zl,1
         com r0
         mov t2,r0
         cpi t2,$ff
         BREQ cvb
         clr t2
cvb:     st y+,t2
         dec t1
         BRNE uio
         ret

;*********************************************************************
; this is a led display on portb as an indicator of the FIFO queue
; for debugging purpose as well as to indicate wether the serial midi
; port can cope with the input
; 7 keys in queue shows all led lit

display:
         mov t1,yl
         subi t1,$60
         cpi t1,23
         brlo bit7
         clr t2
         out portb,t2
         ret
bit7:    cpi t1,20
         brlo bit6
         ldi t2,0b10000000
         out portb,t2
         ret
bit6:    cpi t1,17
         brlo bit5
         ldi t2,0b11000000
         out portb,t2
         ret
bit5:    cpi t1,14
         brlo bit4
         ldi t2,0b11100000
         out portb,t2
         ret
bit4:    cpi t1,11
         brlo bit3
         ldi t2,0b11110000
         out portb,t2
         ret
bit3:    cpi t1,8
         brlo bit2
         ldi t2,0b11111000
         out portb,t2
         ret
bit2:    cpi t1,5
         brlo bit1
         ldi t2,0b11111100
         out portb,t2
         ret
bit1:    cpi t1,2
         brlo bit0
         ldi t2,0b11111110
         out portb,t2
         ret
bit0:    ser t2
         out portb,t2
         ret





;***********************************************
;look up table of midi key number, zero=unused adr
;yamaha YP-40 clavicord adr scheme
;pointer Y= $0110 for RAM
;pointer Z= $0210 = 2 x $0108 for Table

.org $0108

table:   

.db 0,00,00,00,28,29,30,31,32,33,34,35,36,0,0,0
.db 0,37,38,39,40,41,42,43,44,45,46,47,48,0,0,0
.db 0,49,50,51,52,53,54,55,56,57,58,59,60,0,0,0 ;c=60
.db 0,61,62,63,64,65,66,67,68,69,70,71,72,0,0,0
.db 0,73,74,75,76,77,78,79,80,81,82,83,84,0,0,0
.db 0,85,86,87,88,89,90,91,92,93,94,95,96,0,0,0
.db 0,97,98,99,100,101,102,103,0


