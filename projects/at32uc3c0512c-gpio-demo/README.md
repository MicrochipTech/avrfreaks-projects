# AT32UC3C0512C GPIO Demo

Uploaded by drift on 2011-10-21 19:02:00 (rating 0 out of 5)

## Summary

This project was created with AVR Studio 5 and the AT32UC3C-EK development kit. There are no board specific dependencies, so it should be easy enough to port to any other development setup by changing the AVR Studio device configuration settings.


The demonstration introduces the registers and concepts required to configure GPIO as outputs to drive some LEDs. Most of the program is straight forward, except the strange GPIO pin grouping. Instead of using the port names from the datasheet like in the 8-bit AVR (PORTA, PORTB...) the GPIO are assigned an abstract number that starts at 0 for PORTA0 and increases as port number and letter increase. Another tricky part is that pins that don't exist on the port are still counted. PORTA30 and PORTA31 don't exist on the UC3C0512C for example, but PORTB0 is still assigned number 32. These numbers are used to group the pins into 32-bit GPIO ports in the software, where each bit on the GPIO port represents a pin. Check the datasheet for more information and the chip specific header file for "AVR32\_PIN\_xxxx" defines that give the pin numberings.


UPDATE: Seems I can't upload a file larger than 20 KB, so that means I can't include an AVR Studio project that includes the AVR Software Framework (ASF). So, just the main.c file is attached. Feel free to PM me if you need help setting up your project. All you need from the ASF is the delay utility.

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
