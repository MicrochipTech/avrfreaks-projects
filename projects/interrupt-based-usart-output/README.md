# Interrupt-based USART output

Uploaded by Simonetta on 2008-05-01 14:46:00 (rating 0 out of 5)

## Summary

 This program shows how to use an output queue and the Transmit\_Buffer\_Empty interrupt. This technique allows the main program to run at full speed when transmitting multi-byte messages out of a USART.  

 Sample code for USARTs invariably show the main program polling the USART output\_ready flag when transmitting. While that technique works, it suspends the operation of the main program for the duration of the USART output.  

 The code below (for the Atmel AVR Mega48) shows how to write the output bytes to a queue at full speed and return to the main code. The USART transmit interrupt routine then outputs each byte in the background.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
