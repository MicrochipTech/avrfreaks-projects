# Pato is a HD44780 display driver for Atmel's mega and tiny AVRs

Uploaded by nab00 on 2015-04-12 20:11:00 (rating 0 out of 5)

## Summary

Yeah, another driver for HD44780 :)


 


In fact it is more than just a driver library. 


I am trying to create complete throw in solution that will turn any 


HD44780-based display in conjunction with atmega controller


into a simple and configurable terminal. 


 


I am posting this on AVR-Freaks in hope the project will be useful for others and 


in hope to get a good feedback.


 


What is in right now:


* Works on atmega328, atmega2560, attiny2313


* Communication drivers - UART or I2C (TWI in Atmel terms) for mega chips, I2C only for tiny.


* Output buffering. Pato stores text to be displayed in memory and allows delta modifications only for parts that should change.   


* String formatting - based on printf library from Atmel.


* Basic terminal control characters (\n\r\t\f\b\v) are supported.


 


New stuff:


* I2C USI driver for attiny2313, please report bugs on github


* Since tiny chips are limited by amount of flash the extended printf API is not in, so only direct 


   HD44780 commands are only available.


 


Planned:


* More VT100 stuff


* PWM contrast and backlight control


* Custom fonts


* Bi-Di support


 


The project code is at Github <https://github.com/kloper/pato>


Since I do not know whether the project will be interesting to the public I am not releasing any binaries at this point.

## Links

- [Github](https://github.com/kloper/pato)

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
- hd44780 display
