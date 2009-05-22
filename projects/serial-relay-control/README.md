# Serial relay control

Uploaded by kng on 2009-05-22 12:24:00 (rating 0 out of 5)

## Summary

This is built for the ATTiny 26, but should be easily ported to almost any processor with TIM0 and INT0.  

This simple program reads data from the serial port and sets output pins according to the input.  

It includes 4 functions, Help, Enable, Disable and Status.  

The text messages are defined in eeprom.


Code included from AVR304: Half Duplex Interrupt Driven Software UART


(Waiting for the upload function to get fixed, 2009-05-22)


Sample terminal output: (input=he1e2e3sd1s)  

Ex = Enable output x (1-8)  

Dx = Disable output x  

S = Status  

ENABLE 1  

ENABLE 2  

ENABLE 3  

STATUS 1=1 2=1 3=1 4=0 5=0 6=0 7=0 8=0  

DISABLE 1  

STATUS 1=0 2=1 3=1 4=0 5=0 6=0 7=0 8=0

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
