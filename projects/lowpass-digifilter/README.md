# lowpass DigiFilter

Uploaded by admin on 2007-06-20 15:44:00 (rating 0 out of 5)

## Summary

This is A tiny lowpass digital filter to remove unwanted signals on discrete inputs such as keypad or optoIsolated inputs.  

also it generates risinge edge and falling edge,so u can easily handle your keypad events. I used this routine in my keypad.asm and it works good.  

how it generates rising edge?  

consider that we have an single input at the state of 0.  

then the state of this inputs chenges from 0 to 1. we can do the following steps to generate a rasing edge, or falling edge.  

input state: 1 2 3 4  

 ------------------------------------------------  

prev\_status: 00000000 00000001 00000001 00000001  

cur\_status: 00000001 00000001 00000001 00000000


eor prev,cur 00000001 00000000 00000000 00000001  

and prev,cur 00000001 00000000 00000000 00000000


key0\_pressed: 00000001 00000001 00000001 00000000  

key0\_redge: 00000001 00000000 00000000 00000000  

so we have the edge!(at state 1)  

*for information on macros uesed in this article refer to my other programs.  

happy programming.

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
