Source files for AVCX project:

	http://www.circuitcellar.com/avr2004/grand.html
	http://www.dutchtronix.com/AVCX-LCD-Controller.htm

Updated November 21, 2008

The source files for the AVR processors originate from the Circuit Cellar 2004 AVR Design Contest and are used with permission from Circuit Cellar.

MCU1:	Atmega8515 AVR, the master CPU which drives the LCD and controls MCU2
MCU2:	Atmega128. This CPU updates the shared memory with data coming from the Serial Interface

AVCXAccess: Visual C++ 9.00 Express Edition application sending text and a bitmap to the AVCX controller.
This is a quick and dirty application, using a lot of code borrowed from other sample applications.
You will have to change the serial port for your own situation.

Pict_01.BMP. This is an image created with Adobe Photoshop elements 3.0. It has 160 lines of 240 chars each and a colordepth of 12 pixels (X4 R4 G4 B4). The AVCXAccess application only uses the upper 3 bits of each color.

for discussions, email jdr@dutchtronix.com
