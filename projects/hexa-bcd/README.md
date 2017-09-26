# hexa to bcd

Uploaded by hasitha kotta on 2017-09-25 19:12:00 (rating 0 out of 5)

## Summary

.


include"m8535def.inc"


.org 00


     lds r17,$61


     mov r16,r17


     ldi r18,$0A


back:sub r17,r18


     inc r19


     brcc back


     dec r19


     neg r17


     ldi r20,$0A


     sub r20,r17


     ldi r21,$10


     mul r19,r21


     add r0,r20


     sts $62,r0


here:rjmp here

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
