# Lib For Buffer

Uploaded by Morgoth2600 on 2008-02-22 12:18:00 (rating 0 out of 5)

## Summary

This lib include next function:


CommandSystemCreateNormalBuffers\_  

CommandSystemClearNormalBuffers\_  

CommandSystemPopFromAdressNormalBuffers\_  

CommandSystemPopNormalBuffers\_  

CommandSystemPushToAdressNormalBuffers\_  

CommandSystemPushNormalBuffers\_


CommandSystemCreateCircleBuffers\_  

CommandSystemPushCircleBuffers\_  

CommandSystemPopCircleBuffers\_  

CommandSystemClearCircleBuffers\_


This lib is for 2 tipe of buffer:  

1) Sample buffer  

2) Circle buffer


All the routines return some flags:  

1) OverFlowFlag  

2) FlagBufferFull  

3) FlagBufferEmpty


Description of flags:


-Sample Buffer:


--For Push Pop autoincrement conut  

1) If conut Push => Dimension of buffer = Return R17,Bit OverFlowFlag = 1


2) If count Pop => Count Push = Return R17,Bit OverFlowFlag = 1


--For Push Pop to/from location  

1) If Location Push => Dimension of buffer = Return R17,Bit OverFlowFlag = 1


2) If Location Pop => Dimension of buffer = Return R17,Bit OverFlowFlag = 1


-Circle Buffer  

1) If count count Push - count Pop => Length Buffer Return R17,Bit FlagBufferFull = 1


2) If count Pop - count Push => 0 Return R17,Bit FlagBufferEmpty = 1


Example of use lib:


.include "m1280def.inc"; or any core


.Equ ClokFrecvency = 16000000  

.Equ BeginRam = SRAM\_START  

.Equ EndRam = RAMEND  

.Equ BeginStiva = EndRam  

.Equ StivaLength = 64


.org 0  

 Rjmp Reset


.Org INT\_VECTORS\_SIZE  

Reset:


Ldi R16,Low(BeginStiva)  

Out SPL,R16  

Ldi R16,High(BeginStiva)  

Out SPH,R16


Create simple buffer:


Ldi Zl,Low(BeginRam);Zl:Zh = Adress of buffer  

Ldi Zh,High(BeginRam)  

Ldi R16,CommandSystemCreateNormalBuffers\_  

Mov R15,R16;R15 = command to lib  

Ldi Xl,Low(A);A = Buffer length  

Ldi Xh,High(A)  

Rcall SystemNormalBuffers


Push to sample Buffer:


Ldi R16,CommandSystemPushNormalBuffers\_  

Mov R15,R16;R15 = command to lib  

Ldi R16,1;R16 = Byte to be write  

Rcall SystemNormalBuffers  

Ldi R16,2;R16 = Byte to be write  

Rcall SystemNormalBuffers  

Ldi R16,3;R16 = Byte to be write  

Rcall SystemNormalBuffers  

Ldi R16,4;R16 = Byte to be write  

Rcall SystemNormalBuffers  

Ldi R16,5;R16 = Byte to be write  

Rcall SystemNormalBuffers  

Ldi R16,6;R16 = Byte to be write  

Rcall SystemNormalBuffers


Pop from sample Buffer:


Ldi R16,CommandSystemPopNormalBuffers\_  

Mov R15,R16;R15 = command to lib  

Rcall SystemNormalBuffers;R16 = Byte read  

Rcall SystemNormalBuffers;R16 = Byte read  

Rcall SystemNormalBuffers;R16 = Byte read  

Rcall SystemNormalBuffers;R16 = Byte read  

Rcall SystemNormalBuffers;R16 = Byte read  

Rcall SystemNormalBuffers;R16 = Byte read  

Rcall SystemNormalBuffers;R16 = Byte read


Push to location to simple buffer:


Ldi R16,CommandSystemPushToAdressNormalBuffers\_  

Mov R15,R16;R15 = command to lib  

Ldi Xl,Low(B);B = Adress in buffer  

Ldi Xh,High(B)  

Ldi R16,100;R16 = Byte to be write  

Rcall SystemNormalBuffers


Pop from location from sample buffer:


Ldi R16,CommandSystemPopFromAdressNormalBuffers\_  

Mov R15,R16;R15 = command to lib  

Ldi Xl,Low(B);B = Adress in buffer  

Ldi Xh,High(B)  

Rcall SystemNormalBuffers


Clear sample buffer:


Ldi R16,CommandSystemClearNormalBuffers\_  

Mov R15,R16;R15 = command to lib  

Rcall SystemNormalBuffers  

/**********************************************************************/  

/**********************************************************************/  

Create circle buffer:


Ldi Zl,Low(BeginRam));Zl:Zh = Adress of buffer  

Ldi Zh,High(BeginRam)  

Ldi R16,CommandSystemCreateCircleBuffers\_  

Mov R15,R16;R15 = command to lib  

Ldi Xl,Low(A);A = Buffer length  

Ldi Xh,High(A)  

Rcall SystemCircleBuffers


Push to circle buffer until the buffer is full:


Ldi R16,CommandSystemPushCircleBuffers\_  

Mov R15,R16;R15 = command to lib  

Ldi R16,1;R16 = Byte to be write  

Rcall SystemCircleBuffers  

Ldi R16,2;R16 = Byte to be write  

Rcall SystemCircleBuffers  

Ldi R16,3;R16 = Byte to be write  

Rcall SystemCircleBuffers  

Ldi R16,4;R16 = Byte to be write  

Rcall SystemCircleBuffers  

Ldi R16,5;R16 = Byte to be write  

Rcall SystemCircleBuffers  

Ldi R16,6;R16 = Byte to be write  

Rcall SystemCircleBuffers


Pop from circle buffer until buffer is empty:


Ldi R16,CommandSystemPopCircleBuffers\_  

Mov R15,R16;R15 = command to lib  

Rcall SystemCircleBuffers;R16 = Byte read  

Rcall SystemCircleBuffers;R16 = Byte read  

Rcall SystemCircleBuffers;R16 = Byte read  

Rcall SystemCircleBuffers;R16 = Byte read  

Rcall SystemCircleBuffers;R16 = Byte read  

Rcall SystemCircleBuffers;R16 = Byte read  

Rcall SystemCircleBuffers;R16 = Byte read


Push to circle buffer:


Ldi R16,CommandSystemPushCircleBuffers\_  

Mov R15,R16;R15 = command to lib  

Ldi R16,10;R16 = Byte to be write  

Rcall SystemCircleBuffers  

Ldi R16,20;R16 = Byte to be write  

Rcall SystemCircleBuffers


Pop from circle buffer:


Ldi R16,CommandSystemPopCircleBuffers\_  

Mov R15,R16;R15 = command to lib  

Rcall SystemCircleBuffers;R16 = Byte read  

Rcall SystemCircleBuffers;R16 = Byte read  

Rcall SystemCircleBuffers;R16 = Byte read  

Rcall SystemCircleBuffers;R16 = Byte read


Clear circle buffer:


Ldi R16,CommandSystemClearCircleBuffers\_  

Mov R15,R16;R15 = command to lib  

Rcall SystemNormalBuffers

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
