
;This code interfaces with basic stamp code and a 2 wire i2c type buss.
;The basic stamp relays the databyte to the monitor via serial interface
;The stamp also holds the SDA low untill it is ready to process another byte
;This allows the basic stamp to controll execution of the AVR chip
;Not implimented here but you could do the following very easy
;A level interrupt can be set up on an AVR to interrupt every instruction
;the interrupt can call DEBUG. By poping the address off the stack (and returning it) 
;you can output the PC address of the code and what ever variables you wish
;through the basic stamp. Single stepping through the AVR code.

;I just used it to verify my FRAM read and write routines
;Speed is about 30cps because the stamp only executes 2000
;lines per second. A dedicated AVR with a crystal and serial output
;could work at higher speed or course but I had the stamp sitting there
;doing nothing and looking at lines on the scope as bits was getting to
;be too much thinking.



.include "tn45def.inc"  ; change if an other device is used

.equ BSSCL = 3  ;PB3 clock line pull high with 2.2k resistor
.equ BSSDA = 4  ;PB4 data line pull high with 2.2k resistor

.def i2cdata  = r17   ; I2C data transfer register

main:
i2cdata=$FF  ;data in i2cdata variable
rcall debug  ;send it to basic stamp for output
rjmp main


BSPAUSE:
push r16
push r17
ldi r16,0
ldi r17,15 ;increase for longer delay at 8Mhz this is about 1.5 mS
BSPAUSELOOP:
dec r16
brne BSPAUSELOOP
dec r17
brne BSPAUSELOOP
pop r17
pop r16
ret

DEBUG:            ; Basic stamp output one byte echos to screen through basic stamp
cbi ddrb,BSSCL    ;release clock
cbi ddrb,BSSDA    ;release SDA
waitforoktosend:  ;Stamp holds SDA low until it is ready to accept byte
rcall BSPAUSE
sbi ddrb,BSSCL
rcall BSPAUSE
cbi ddrb,BSSCL
sbis pinb,BSSDA
rjmp waitforoktosend
rcall BSPAUSE
sbi ddrb,BSSDA     ;send start condition
rcall BSPAUSE
sbi ddrb,BSSCL     ;pull clock low
rcall BSPAUSE
sec
rol i2cdata
writebits:
brcc notset
nop
cbi ddrb,BSSDA      ;release SDA bit is 1
rjmp BSCLOCKIT
notset:
sbi ddrb,BSSDA      ;pull SDA low bit is 0
rjmp BSCLOCKIT
BSCLOCKIT:
cbi ddrb,BSSCL      ;release clock
rcall BSPAUSE
sbi ddrb,BSSCL      ;pull clock low
rcall BSPAUSE       ;bit is sent
lsl i2cdata         ;shift next bit into carry flag
brne writebits      ;when i2cdata=0 all bits are sent
cbi ddrb,BSSDA      ;Release the buss
clc                 ; clear carry flag
sbic pinb,BSSDA     ; if BSSDA is high
sec                 ; set carry flag NO ACK, chip did not respond
cbi ddrb,BSSCL      ;release the clock
rcall BSPAUSE
ret
