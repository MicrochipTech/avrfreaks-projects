# Binary data I/O for GCC and AVR

Uploaded by janne102 on 2007-09-29 15:27:00 (rating 0 out of 5)

## Summary

Lowest level byte operations in C for AVR. Several useful functions are included that in a compact and fast way perform binary data transfer to and from an AVR.


This code:  

a. waits until the read buffer is full (UART1)  

b. converts four consequtive buffered bytes into an integer  

c. puts a header into the TX buffer  

d. cuts the integer into four pieces and adds it into the TX buffer  

e. transmits the TX buffer through UART0  

f. includes a function that cuts an integer into bytes and transmitts it to UART directly, unbuffered  

g. includes a function that copies the whole content of a receive buffer into a transmission buffer directly


The code assumes you have setup UART0 and UART1 for buffered operation  

using the AVRlib functions "buffer" and "uart2".


Furthermore, it shows you how to manipulate the buffer on the lowest level, in addition to the usual library functions.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
