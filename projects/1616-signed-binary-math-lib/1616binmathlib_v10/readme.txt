
16.16 signed binary math lib for small RAM-less AVR's and all other AVR's.      V1.0 
ALL code by Rolf R Bakke,  September 2008

This library uses only the AVR's registers for usage in RAM-less AVR's such as the ATiny 15.

(Note the reverse byte order, I did it for easy readout in the simulator)

 

-*- 16.16 signed binary addition -*-

Label: b16_add

Register usage:

   sum          augend        addend  
r0:r1.r2:r3 = r0:r1.r2:r3 + r4:r5.r6:r7





-*- 16.16 signed binary subtraction -*-

Label: b16_sub

Register usage:

difference      minuend     subtrahend
r0:r1.r2:r3 = r0:r1.r2:r3 - r4:r5.r6:r7




-*- 16.16 signed binary multiply -*-

Label: b16_mul

Register usage:

  product     multiplicand   multiplier
r0:r1.r2:r3 = r4:r5.r6:r7 * r8:r9.r10:r11

r12,r13,r14,r15,r16 and r17 is destroyed

The result is rounded to nearest LSB.

c-flag is 0: no error
c-flag is 1: overflow



-*- 16.16 signed binary divide -*-

label: b16_div

Register usage:

 quotient      dividend       divisor
r0:r1.r2:r3 = r4:r5.r6:r7 / r8:r9.r10:r11

r12,r13 and r16 is destroyed.

The result is rounded to nearest LSB.

c-flag is 0: no error
c-flag is 1: overflow or divide by zero





Example:  Multiply 0x01ef.8000 by 0x0008.abcd


ldi r16,0x01
mov r4,r16

ldi r16,0xef
mov r5,r16

ldi r16,0x80
mov r6,r16

ldi r16,0x00
mov r7,r16


ldi r16,0x00
mov r8,r16

ldi r16,0x08
mov r9,r16

ldi r16,0xab
mov r10,r16

ldi r16,0xcd
mov r11,r16


rcall b16_mul

brcs error

...


.include "1616binmathlib.asm"




Result is now in r0:r1.r2:r3


Any questions?
Mail modellfly@hotmail.com

