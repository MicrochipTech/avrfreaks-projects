# SW-UART up to 460800 Baud

Uploaded by danni on 2008-06-13 18:44:00 (rating 0 out of 5)

## Summary

On small ATtiny devices there exist no hardware UART.


Thus I wrote a fast software UART e.g. for data logging an ADC, reading serial Flash/EEPROM on SPI or I2C.


A crystal of 14.7456MHz was used, 18.432MHz may be used also.  

For fast baud rates the program was written in assembler.  

The bit transmit interrupt need 23 cycle, so you can use an interrupt rate of 32 cycle (14.7456MHz / 32 = 460800 baud).  

Furthermore in assembler you can atomic access to 32 bit register variables (MOVW), which was needed, because the FIFO size was above 256 byte (this saves using of CLI+SEI instead).


The test program runs on an ATtiny85 and counts a 32 bit value and send it as hexadecimal over the UART.  

On using Attiny25 or 45 you must reduce the FIFO size.  

Also ATiny84, ATtiny861 can be used, with small changes on the timer usage.


On my tests the onboard UART of my PC was unable to handle 460800 baud, but the USB-RS232 converter can do it.


Peter

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
