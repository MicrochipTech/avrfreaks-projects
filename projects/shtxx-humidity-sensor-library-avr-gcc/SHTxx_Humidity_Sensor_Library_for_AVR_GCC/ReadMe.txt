
-----------------------------------------------------

Sensirion SHTxx Humidity Sensor Library

Brian Low (mportobello@hotmail.com)

Jul 6, 2003

-----------------------------------------------------

Hardware Setup:
- ATmega8
- 8MHz internal oscillator
- LEDs on pins D5, D6, D7 for power, receive, transmit
- SHTxx clock and data connected to pins B1 and B2
- RXD, TXD pins connected to computer COM port via MAX232 chip

Files of interest:
- Sht.c:            the SHT functions. See comments for library usage
- NetTest.c:        includes main(), demonstrates the library
- NetTest.hex:      compiled project for the ATmega8

To make:
- Project was built with WinAvr GCC 3.3 (http://sourceforge.net/projects/winavr) or see www.AVRFreaks.net
- Run "make" in the project folder
- Output is NetTest.hex


  
