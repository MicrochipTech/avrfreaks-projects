# Wireless RF Temperature Transmitter

Uploaded by Don Carveth on 2002-06-20 18:27:00 (rating 0 out of 5)

## Summary

Uses RF radio modules to transmit temperature data back to a PC. The PC runs my "Mailbox" Windows software (included) which decodes the data, eliminates noise and presents the the data in a tabular format. Mailbox is also an ActiveX EXE which means it is a component which can be called by other applications. The transmitter is based on an AT90S4433. Data is transmitted at a switch selectable rate between 1 second and 2 hours. The long delay is obtained by cascading TCNT1 into TCNT0. The device enters "idle" mode when the tranmission is complete to save power. Temperature measurement is done using thermistors - it also measures light and battery voltage. The dowload file includes the PC Windows program and is about 2M total. Includes article, C code, schematics, Visual Basic .exe and test program source code. Don Carveth, [www.botgoodies.com](http://www.botgoodies.com), [don@botgoodies.com](mailto:don@botgoodies.com)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
