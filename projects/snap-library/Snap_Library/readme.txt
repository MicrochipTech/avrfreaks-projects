


-----------------------------------------------------

Snap Library 

Brian Low (mportobello@hotmail.com)

Aug 17, 2003

-----------------------------------------------------

Hardware Setup:
- ATmega8
- 8MHz internal oscillator
- RXD, TXD pins connected to computer COM port via MAX232 chip
- Pin C5 connected to RXD

Files of interest:
- Snap.c            the Snap library including documentation
- SnapTest.c        test project demonstrating the library
- SnapTest.hex      test project compiled for ATmega8
- ClientTest.zip    C# project for communicating with an ATmega8 using the Snap protocol

To make:
- Project was built with WinAvr GCC 3.3 (http://sourceforge.net/projects/winavr) or see www.AVRFreaks.net
- Run "make" in the project folder
- Output is SpanTest.hex


  
