# tinyAVR upgrade for ATtiny4..10

Uploaded by Norman256256 on 2010-05-23 16:28:00 (rating 0 out of 5)

## Summary

ICCtiny does not support ATtiny 4,5,9,10 (SOT23-6 case). If you select  

custom for other tiny's, the counting of interrupt vectors are limited  

(only 9 vectors). I have change limit to 31 vectors.


installation:  

-install ICCtiny 6.11A  

-goto folder bin  

-rename ICCTINY.exe in *.org  

-copy from ZIP-file patched ICCTINY.exe  

-goto folder lib  

-copy from ZIP-file crttinyxxx.o in this folder  

-goto folder include  

-copy from ZIP-file iotiny10.h


Create a new project:  

-change project options  

 (custom,stack = 10,global variables = 7,vector entries = 10)  

 -> set as default  

-insert in your C-Source #include "iotiny10.h"  

-now compile your project  

-programming your device with TPI-flasher (programmer)  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_id=2461&item_type=project)


Add in main-routine as first: asm("ldi R30,0x4F");  

now you can give parameter for subroutines


I have a big problem:  

The tiny10 has only registers R16..31. After 2nd. subroutines the  

C-Compiler will use registers R14,R15 (R0..15),but the registers  

are not available and the registers are mapping to R16..31. The  

register R30 (Z-Pointer) is just overwrite. After 2nd. subroutines  

you can not access to any variables.

## Compilers

- ImageCraft ICCtiny V6

## Tags

- Complete code
- ImageCraft ICCtiny V6
