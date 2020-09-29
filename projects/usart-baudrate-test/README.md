# USART Baudrate Test

Uploaded by ki0bk on 2020-09-28 20:22:00 (rating 0 out of 5)

## Summary

Here is a simple USART test program that sends the letter U continuously, the letter U has a 01010101 pattern that can be used to verify the cpu speed and baud rate by measuring the bit time of the TXD pin output.


At a baud rate of 4800, the bit time (pulse duration high or low) should be ~208us using a logic analyzer or O-scope or 4800Hz using a frequency counter.


Any other value indicates the cpu is not running at the expected speed, for this program as compiled, 8MHz, change F\_CPU to change to your actual cpu speed as set by the fuses.


 


Jim

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
- ATmega16
- USART
