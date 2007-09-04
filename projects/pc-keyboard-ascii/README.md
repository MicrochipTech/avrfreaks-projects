# PC keyboard to ASCII

Uploaded by Simonetta on 2007-09-04 15:45:00 (rating 0 out of 5)

## Summary

 A simple program demonstrating how to interface a standard PC keyboard to an AVR. In assembler for Tiny11 but adaptable to all AVRs. There are two routines. An interrupt routine that collects the bits sent from the keyboard into a byte, and the routine to parse these bytes into ASCII.


 The shell demonstration program takes the data received from the keyboard and converts it into an ASCII character that corresponds to the keypress. Then this ASCII byte is sent out as an RS232 byte at 38400 baud using a software UART. 


 The PS2 keyboard sends one or two bytes for every keypress down and two or three bytes for every keypress release. Each of these bytes has to be examined individually as they arrive to see how they relate to the previous characters sent by the PS2 keyboard. Then, the scancode byte has to be mapped against a table to get the ASCII value of the key pressed. The key presses corresponding to SHIFT, CONTROL, and ALT keys have to be evaluated also if any of these keys are pressed in addition to an alphanumeric key.

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
