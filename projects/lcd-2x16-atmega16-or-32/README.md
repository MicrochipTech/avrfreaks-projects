# lcd 2x16 atmega16 or 32

Uploaded by panaras on 2011-05-08 13:32:00 (rating 0 out of 5)

## Summary

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;;;;;;;;;;;;; 8-bit LCD INTERFACING with ATMEGA16 ;;;;;;  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;;;|PD0->RS|PD1->RW|PD2->E|PB0-PB7->DATA|;;;PB0=LSB|PB7= MSB;;;;;;;;;;;;  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;Created by Panagiotis Georgiou - ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

.device ATmega16  

.include "m16def.inc"


.def temp=r16 ;general purpose register  

.def data=r17 ;register responsible for data 


.equ LCD\_RS=0 ;PortD,0  

.equ LCD\_RW=1 ;PortD,1  

.equ LCD\_E=2 ;PortD,2


.org 0x0000  

rjmp reset


;*** Stack Initialization *** NOT REALLY USED HERE!


reset:  

ldi temp, low(RAMEND)  

out SPL, temp  

ldi temp, high(RAMEND)  

out SPH, temp


;*** Port D (LCD Register Port) Initialization ***


ldi temp,0x07 ;  

out DDRD,temp ;make only Command Port bits D0, D1 and D2 outputs.


;*** Main LCD program manipulation ***


rcall lcdinit ;Commands for LCD display preparation


//Fill the first line with characters from A to P


ldi data,'I'  

rcall datatext  

ldi data,'S'  

rcall datatext  

ldi data,' '  

rcall datatext  

ldi data,'T'  

rcall datatext  

ldi data,'H'  

rcall datatext  

ldi data,'A'  

rcall datatext  

ldi data,'T'  

rcall datatext  

ldi data,' '  

rcall datatext  

ldi data,'Y'  

rcall datatext  

ldi data,'O'  

rcall datatext  

ldi data,'U'  

rcall datatext  

ldi data,'?'  

rcall datatext


ldi data,0xC0 ;Command to go to the 2nd line beginning  

rcall command


//Fill the second line with characters from Q to `


ldi data,'Y'  

rcall datatext  

ldi data,'E'  

rcall datatext  

ldi data,'S'  

rcall datatext


ldi data,0xCE ;move to the second line start  

rcall command


ldi data,'N'  

rcall datatext  

ldi data,'O'  

rcall datatext


ldi data,0x80 ;Command to go to the 1st line beginning  

rcall command


sleep  

loop: jmp loop


;*** LCD Initialization ***


lcdinit:  

 ldi data,0x38 ;2 lines, 5*8 font, 8-BIT MODE  

 rcall command  

 ldi data,0x0F ;Display on, cursor on, blinking  

 rcall command  

 ldi data,0x01 ;clear display, cursor -> home  

 rcall command  

 ldi data,0x06 ;auto-inc cursor  

 rcall command  

 ldi data,0x80 ;move cursor to the beginning of f.line  

 rcall command  

ret


;*** Send Command to the LCD ***


command:  

 rcall busyflag  

 ldi temp,0xFF ;  

 out DDRB,temp ;make whole Data PortB output.  

 cbi PortD, LCD\_RS ;RS = 0 for Command  

 cbi PortD, LCD\_RW ;RW = 0 for Write  

 out PortB, data ;and write the Data Port value.  

 sbi PortD, LCD\_E ;High E (going...to low pulse)  

 nop  

 nop  

 nop  

 nop  

 nop  

 nop  

 cbi PortD, LCD\_E ;and then Low E  

ret


;*** Send DataText to the LCD ***


datatext:  

 rcall busyflag  

 ldi temp,0xFF ;  

 out DDRB,temp ;make whole Data PortB output.  

 sbi PortD, LCD\_RS ;RS = 1 for Text input  

 cbi PortD, LCD\_RW ;RW = 0 for Write  

 out PortB, data ;and write the Data Port value.  

 sbi PortD, LCD\_E ;High E (going...to low pulse)  

 nop  

 nop  

 nop  

 nop  

 nop  

 nop  

 cbi PortD, LCD\_E ;and then Low E  

ret


;*** Check busy flag ***


busyflag:  

 ldi temp,0x00 ;  

 out DDRB,temp ;make whole Data PortB input.  

 cbi PortD, LCD\_RS ;RS = 0 for Command input  

 sbi PortD, LCD\_RW ;RW = 1 for Read  

 busy:  

 sbi PortD, LCD\_E  

 nop  

 nop  

 nop  

 in temp,PinB ;Input data from the LCD  

 nop  

 nop  

 nop ;wait  

 cbi PortD, LCD\_E  

 nop  

 nop  

 sbrc temp,7 ;check if busy flag (DB7) is cleared  

 rjmp busy ;if not cleared redo checking  

ret

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
