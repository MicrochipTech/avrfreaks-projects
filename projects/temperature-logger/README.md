# Temperature Logger

Uploaded by sellis on 2002-01-09 22:02:00 (rating 0 out of 5)

## Summary

This project uses an ATTiny15, a LM75 I2C temperature sensor and a 24LC128 I2C serial EEPROM to implement a small temperature logger. Heavy use is made of the sleep modes on all three devices to extend battery life. There is enough space in the EEPROM to sample every minute for 10 days, and the battery life on a CR2032 lithium coin cell should be over a month. Comments in the code should explain mode of operation and how to get the data out. The schematics assume that the LM75, a surface-mount device, is attached to a small piece of stripboard to adapt to a 8-pin dip socket. By bending pins 2 and 7 up, it is easy to do this as long as pins 1 and 2, and pins 7 and 8 end up swapped. More details coming soon to <http://homepage.ntlworld.com/seanellis/technology.htm>.

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
