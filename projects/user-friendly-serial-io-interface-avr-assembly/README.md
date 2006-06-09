# "user friendly" Serial I/O Interface in AVR assembly

Uploaded by Drive_C: on 2006-06-08 18:56:00 (rating 0 out of 5)

## Summary

Assembly code (AVR Studio 4) for serial I/O with AVR controller. Code adapted to 8535.inc definition file, but is probably MCU-independent, as long as it has a USART.


Commands follow the format:  

"W 0x34 0b00001000" or "W 0d019 0xFF"


help text included in program code, and displayed to user when "H" is pressed; this help text can be removed if using program on ATTiny or small flash memory controllers.


Upcoming version will have bit clearing/setting of data space or IO space registers, and TWI send/receive buffering...

## Links

- [Assembly Code](http://pantheon.yale.edu/~mhd26/basic232IO.asm)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
