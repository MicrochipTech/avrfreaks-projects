# AVR_XMODEM_BOOTLOADER

Uploaded by admin on 2004-02-04 01:06:00 (rating 0 out of 5)

## Summary

When I am working with the STK500 to do some projects,I have to connect two serial cables(one for code dowload,another for printing the debug information)with the board.And as my computer has only one serial port , I work with two cables alternatively.  

So I decide to write the bootloader using the serial port to download the code.That I can just use one serial cable for download and debug.As I am not so good at writing PC side software for download,I choose the HYPERTERMINAL(a windows build-in communication program) as the code download and debug program,and use the XMODEM protocol to download the code.  

As the object file to download through the XMODEM should be plain binary and the most compilers just output the hex file,I search the web and find the open source project HEX2BIN(you can find the project at the GNU.ORG) to convert the hex file to the plain binary file.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
