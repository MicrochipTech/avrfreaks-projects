# 8086 emulator on Arduino UNO (atmega328p)

Uploaded by raspiduino on 2021-03-24 04:36:00 (rating 0 out of 5)

## Summary

Intel 8086 emulator on Arduino UNO (atmega328p)


1. What is this?


*avr-x86* is an 8086 emulator running on Arduino UNO (atmega328p) and upper. The emulator core is based on *fake86*and a port of *corax89 at* [https://github.com/corax89/Ardui...](https://github.com/corax89/Arduino_8086_emulator. )*avr-x86*can load assembly programs from the SD card and run it.


![Proteus simulation](https://user-images.githubusercontent.com/68118236/112129528-1415f400-8bfa-11eb-93f2-7bdf84fe7214.png) 


As you can see, *avr-x86* is running Brainfuck on Proteus simulation.


2. What can it do?


* Emulating a complete 8086/80186 CPU
* Text/Console interface through serial
* Load program from SD card
* Virtual RAM feature (Virtual RAM file on SD card, not the AVR's RAM, so you can have more ram than AVR support)

3. Todo


* Add BIOS/HD support
* Reduce sketch size

4. Credit


* Fake86 by Mike Chambers
* Fake86 port to Arduino by corax89
* Fat16 library by William Greiman

5. Link to GitHub: [https://github.com/raspiduino/av...](https://github.com/raspiduino/avr-x86)


6. Note: There are prebuilt hex for an Arduino UNO bellow



> Sketch uses 32130 bytes (99%) of program storage space. Maximum is 32256 bytes.  
> 
> Global variables use 1176 bytes (57%) of dynamic memory, leaving 872 bytes for local variables. Maximum is 2048 bytes. 
> 
> 
>

## Links

- [Github link](https://github.com/raspiduino/avr-x86)

## Compilers

- Arduino IDE

## Tags

- Complete code with HW
- Arduino IDE
- # Atmega328P #arduino
- #Arduino #emulator #x86 #8086 #sd #atmega328p #avr
- Portable Game Consoles
