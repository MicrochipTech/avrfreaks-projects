# Morse Code ID

Uploaded by AllenW on 2009-02-10 18:37:00 (rating 0 out of 5)

## Summary

;This program will send a Morse code string of characters.  

;In the program Morse code characters are represented by numbers. For example, the  

;letter V is represented by 20. In Morse, V = ...\_ and in the system used in this  

;program, a dit is represented by a binary 0 and a dah by a 1. Therefore V = 0001.  

;Some method of indicating the number of elements to be sent is also required, however, and  

;this is coded in the last three bits of a byte. Since V has four elements, the last three  

;bits are 100. Thus the whole byte representing a V is 00010100 where the 0 between the two 1s  

;acts as a spacer. The maximum number of elements in a Morse letter or number  

;is 5 so in general the first five bits of a byte are all that are necessary to code for the  

;character. Commas, periods and question marks require 6 bits, however, so for these  

;symbols the 6 element bits overlap with the last three bits that indicate the number of elements  

;to be sent. The subroutines adj1 and adj2 get around this problem. Six character elements  

;ending in a dit (like the question mark) are logically ANDed with 11111011 whereas  

;periods and commas can be sent as is. Spaces between words are represented by 0. For  

;further explanation of this coding method and a table of numbers representing letters and  

;digits 0-9 see the "BASIC Stamp Programming Manual 1.8" that is available from Parallax on  

;the internet.  

;Adjustments in the frequency and speed can be made by changing "tone", "time", "Tj" and  

;"Tk" as indicated in the program. As written the frequency is about 800 Hz and the Morse  

;code is sent at about 25 wpm.


.include "tn13def.inc"


.def Index = r16 ;number of characters to be sent  

.def Character = r17 ;number representing character to be sent  

.def Elements = r18 ;number of dits and dahs in character  

.def Time = r19 ;lengthen for slower code  

.def Tone = r20 ;determines frequency  

.def Tj = r21 ;outer loop constant for space and cspace  

.def Tk = r22 ;inner loop constant for space and cspace  

.def Temp = r23 ;sets DDRB to output


Message:  

.DB 197, 29, 0, 131, 1, 0, 20, 1, 197, 132, 212, 227 ;VE7BQO  

 Ldi Index, 0 ;first letter to be sent


Read: 


 lpm Character, z+ ;Load 1st # from prog mem, inc. z.  

 ldi Elements, 0b00000111  

 and Elements, Character ;Logical AND elem & char; results in element.  

 cpi Elements, 0 ;If character = 0 then a word space will be sent.  

 breq Wspace  

 cpi Elements, 6 ;6 element character ending in dit  

 breq adj1  

 cpi Elements, 7 ;6 element character ending in dah  

 breq adj2  

 rjmp Key 


adj1:  

 andi Character, 0b11111011 ;Adjust character to send dit as 6th element.  

 rjmp Key


adj2:  

 ldi Elements, 6 ;Adjust character to send 6 elements.


Key:  

 cpi Character, 128 ;If character starts with a 1, skip to dah.  

 brsh Dah


Dit:  

 ldi Time, 60 ;Change the 60 to get a different speed.  

 ldi Temp, 0b00000001 ;temp register to hold 0b00000001  

 out DDRB, Temp ;so as to set DDRB0 as output


Toggle:  

 SBI PINB, 0b00000000 ;toggle pin B0  

 ldi Tone, $FF ;$FF sets the tone, smaller #, higer freq


Delay:  

 dec Tone ;Time delay for toggling pin B0.  

 brne Delay  

 dec Time  

 brne Toggle  

 rjmp Space


Dah:  

 ldi Time, 180 ;Time for dah = 3x dit, adjust as  

 ldi Temp, 0b00000001 ;required.  

 out DDRB, Temp


Toggle1:  

 SBI PINB, 0b00000000 ;toggle pin B0, as for dit  

 ldi Tone, $FF


Delay1:  

 dec Tone ;delay loop for dah  

 brne Delay1  

 dec Time  

 brne Toggle1  

 rjmp Space


Shift:  

 lsl Character ;Shift character to left so as to send the  

 dec Elements ;next element of the character; dec elements  

 brne Key ;so as to send just the correct # of elements.  

 rjmp Cspace


Space: ;Space between elements = dit time.  

 ldi Tj, 78 ;if Time =60, dit = 0.039 s  

 L1: ldi Tk, 199 ;j(3k+3) = (tiny13A frequency x dit time)  

 L2: dec Tk ;j(3k+3) = 1200000 x 0.039  

 brne L2 ;k=199, j=78  

 dec Tj ;Change for a different code speed.  

 brne L1  

 rjmp Shift


Cspace: ;Space between characters = dah time  

 ldi Tj, 156 ;dit time space already, therefore  

 L3: ldi Tk, 199 ;character space is only 2x dit time.  

 L4: dec Tk  

 brne L4  

 dec Tj  

 brne L3  

 inc Index  

 cpi Index, 12 ;Change this number if a longer or shorter  

 brne Read ;string of characters is to be sent.


Wspace:  

 ldi Tj, 255 ;These Tj and Tk values give approximately  

 L5: ldi Tk, 255 ;5x a dit time for the space between words.  

 L6: dec Tk ;If a longer space is required another loop  

 brne L6 ;must be added.  

 dec Tj  

 brne L5  

 inc Index  

 rjmp Read 


End:


 ldi r16, (1<<SE) ;Enable sleep mode.  

 out MCUCR, r16  

 sleep ;Put MCU in sleep mode.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
