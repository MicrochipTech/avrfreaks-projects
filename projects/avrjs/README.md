# AVRjs

Uploaded by swavr on 2016-08-16 20:28:00 (rating 5 out of 5)

## Summary

AVRjs is an in-browser AVR simulator, it is mostly written in C and compiled to JavaScript using Emscripten.


 


There is a JavaScript terminal attached to UART0 of each processor which serves as the main, IO for the simulator. The code for the default loaded app is on GitHub and is a good place to get started as it implements the UART functions and initialises stdout allowing the use of printf.


 


Currently supported MCUs:


* ATmega328
* ATmega128
* ATtiny1634

 


Writing to flash is currently unsupported.


 


Note: avrjs.com will pretty much max out one core of your CPU if the program you load onto it doesn't sleep (at least it does on my machine), this may be undesirable, especially on older hardware. The default program tries its best to sleep all the time.

## Links

- [AVRjs](http://avrjs.com)
- [Code on GitHub](http://github.com/avrjs)
- [Default app Atmel Studio project on Github](http://github.com/avrjs/avrjs-demo-terminal)

## Tags

- #ATmega328 #ATmega128 #ATtiny1634
- #Simulator #JavaScript
