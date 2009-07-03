# Hexadecimal Display Decoder/Driver for 7 Segment LED on ATTiny26

Uploaded by saundby on 2009-07-03 01:33:00 (rating 0 out of 5)

## Summary

Use the ATTiny26 to provide a latched 1 digit hexadecimal display (0 to F) with 4 bits of input data. Decodes input value and puts out an 8-bit signal to place the digit on a 7-segment display (eighth line drives the decimal point to signify an alpha character, for those like me who sometimes mistake their b's for 6's.)  

Pinout is provided in assembly source file, each line of PORTA directly drives one element of the 7 segment display. Outputs are arranged in a PCB-friendly layout the same as many commercial 7-segment decoder/driver chips.  

When the latch line is low, the displayed value tracks the inputs. When the latch line is high, the last value sampled when latch was low is retained.  

One line is used to select outputs that are active high for common cathode displays or active low for common anode displays, so either type of display may be driven.


[My Electronics Webpage](http://saundby.com/electronics.html)


January 4th 2007: I have added a second version of the program that reduces the power utilization of the LED by reducing its duty cycle to about 1/3. With this version, the current requirements for seven illuminated segments (an '8') drop below the current requirements for two segments (a '1') in the original program. Overall current requirements are about 40% of the original version. (Actual current requirements will vary by display used, that is why I am only providing relative measurements.) The brightness of the LED is reduced as well, of course, but should be plenty bright for almost all applications.


Sample rates in the reduced power (RP) version are about the same as the original, so no significant change in performace should result through the use of the lower power version.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
