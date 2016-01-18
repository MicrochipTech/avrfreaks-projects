# Eclipse plugin for UART communication using USBasp+ programmer

Uploaded by stevep on 2016-01-18 08:25:00 (rating 0 out of 5)

## Summary

I use Eclipse for AVR development. For programming the MCU I use the USBasp device. Inspired by the USPasp+ project, which extends the original USBasp firmware by adding UART support, I developed an Eclipse plugin which shows UART communication in the Console view.


 


I use UART for reporting the current state of the MCU to the a device connected to my circuits (in production I connect my Android phone). So during development the plugin is very convenient because you don't have to unplug the programmer and plugin an USB serial converter after programming the MCU to see the UART communication. Especially if you are interessted in the output of the booting-period. Another advantage: The plugin works on Mac OSX (my prefered environment), Linux (also ARM -> Raspberry Pi) and Windows.


 


I hope you will find it useful.

## Links

- [Eclipse USBasp+ Console plugin](https://github.com/RasPelikan/USBaspPlusConsole)
- [Eclipse USBasp+ Console plugin upate site](http://raspelikan.github.io/USBaspPlusConsole)
- [Eclipse AVR plugin](http://avr-eclipse.sourceforge.net/wiki/index.php/The_AVR_Eclipse_Plugin)
- [Eclipse AVR plugin update site](http://avr-eclipse.sourceforge.net/updatesite)
- [USBasp programmer](http://www.fischl.de/usbasp/)
- [USBasp+ project](https://community.atmel.com/projects/usbasp-tty-usbasp-programmer-modified-serial-support-and-terminal-program)

## Tags

- General Lib.functions
- usbasp
- Eclipse
- UART
- usbasp+
