# I2C (TWI) Sniffer

Uploaded by danni on 2008-08-04 16:53:00 (rating 5 out of 5)

## Summary

If problems on the I2C bus occur, it was needed to see all data transfer on it. The easiest way was using an AVR and send data over the UART.


But no ATmega can be used, because they can not listen only, not signal a start and not receive different addresses.  

So only the Attiny with USI can be used.


Thus the sniffer use the ATtiny85.  

The output was done as text (hex numbers) at 115200 baud with 255 byte FIFO.  

A crystal of 14.7456MHz was used to match the baud rate.


Version 2 added with some improvements. The C source can work with up to 230400 baud, the assembler source up to 460800 baud.


Peter

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
