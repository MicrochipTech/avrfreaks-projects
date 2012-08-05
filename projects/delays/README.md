# _Delays

Uploaded by A-men on 2012-08-04 17:25:00 (rating 0 out of 5)

## Summary

.INCLUDE "\_Delays.asm" enables you to use the following macro in your code


\_Waste\_us n (with n = 1 to 25.769.803.779)


This macro will generate a wait loop from 1 ï¿½s to 25.769.803.779 ï¿½s. 


\_Waste\_us is actually limited to minimum (1.000.000/F\_CPU) us because we can not loop for less then 1 instruction cycle. If you request a delay below this limit \_Waste\_us will insert no instruction in your code.


\_Waste\_us is absolutely exact down to (1/Your\_CPU\_frequency\_in\_MHz) ï¿½s


\_Waste\_us will pick the smallest code for your delay (1 to 13 words).  

Well that is not 100% true. It will pick the shortest usable loop 8-,16-,24- or 32-bit and add a 2nd 8-bit loop and maybe 2 NOP's to match exactly the delay you requested. In some cases that results in code that is 1 word longer than the minimum code that would have generated the requested delay. (Fixing that would make the macro really complex and I decided the benefit was not worth the effort)


2012 august 4  

I have removed all the old files and left just the one you need to use


Jan Huygh

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
