# Software UART with FIFO

Uploaded by danni on 2009-02-14 16:00:00 (rating 0 out of 5)

## Summary

Following a software UART with buffered transmit and receive.  

It can be used to get a second UART or for ATtiny without UART.


It uses a timer with clear on compare feature to determine the transmission baud rate.  

Another compare interrupt of the same timer was used for the receive interrupt.  

To catch the start bit, the input capture interrupt was used.  

On AVRs without ICP, also an external interrupt can be used, but then on high baud rates the interrupt delay should be substract from the bit scan time.


Peter

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
