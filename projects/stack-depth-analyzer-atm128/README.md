# Stack depth analyzer (atm128)

Uploaded by johnkim20 on 2009-10-15 22:09:00 (rating 0 out of 5)

## Summary

The code is compiled using gcc. It works for ".lss" file from avr-gcc 3.4.6.


Run sd-atm.exe, then it shows usage, such as:  

 >sd-atm mycode.lss -a 0x2000 > dump.txt  

 >sd-atm mycode.lss -a app\_main >> dump.txt


1. Target core is atmega 128.  

2. Indirect call and jump are detected, but not processed. You have to do math: "[stack depth till the break] + [stack depth of indirectly referenced function]".  

3. Perhaps useful: Detects hidden recursive calls.


//  

Wanted to add capability to handle indirect call & jump, along with ".elf" for source file. But, ... so, decided to let it go as it is.  

I would be glad to see "Hi" @ [john-k.home@rcn.com](mailto:john-k.home@rcn.com).

## Links

- [Stack Dept Analyzer (atm128)](http://users.rcn.com/john-k.home/)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
