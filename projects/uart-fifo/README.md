# UART with FIFO

Uploaded by danni on 2008-08-06 17:20:00 (rating 0 out of 5)

## Summary

Following an example code with interrupt driven UART with FIFO.


Typically a FIFO size of about 256 Bytes was sufficient for most AVR projects. Thus the FIFO was realized with index instead pointer, which saves code space and CPU-time, because the index can be hold in a char (8bit).  

Also because 8bit access is atomic, no interrupts must be disabled during the main loop functions.  

The maximum size is 257 (TX) and 258 (RX) byte, because also the hardware buffers are used.


Especially care was taken to support all the different AVR derivates.  

Thus are some definitions for the different vectors, bytes and bits inside the uart0.h.


Also all functions are named **0, so its easy to copy the code for the other UARTs and rename it to **1 ... **3.


Peter

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
