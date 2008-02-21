Dutchtronix AVR Oscilloscope Terminal V3.0a

A program to use your (analog) oscilloscope with XY mode as a display device using the Dutchtronix AVR Oscilloscope clock hardware.

February 19, 2008

This project needs to be build with the Atmel AVR Studio tools, freely available from Atmel. I am using V4.13 SP1, build 557 but expect older versions to work as well. See (http://www.atmel.com/dyn/products/tools_card.asp?tool_id=2725).
The only 2 files needed from the distribution are "avrasm2.exe" and "m168def.inc". Use the file "build.bat" to assemble the software (it expects the AVR Studio files to be installed in the default locations). Alternatively, you can create a new AVR Studio "Atmel AVR Assembler" project and add the file "ScopeTerm.asm" as the source file; the other source files will be included.

Programming the AVR clock with the Atmel Dragon is possible by uncommenting the second command line in the batch file (assuming you have installed the 10-pin header on the AVR clock board and have installed the proper cable, incl. a 6-pin to 10-pin adapter)
Of course, you can use your favorite AVR Programmer. Just keep in mind that you will lose the bootstrap loader present on the AVR clock using this method. I suggest saving the existing firmware first, so you can reload the clock, complete with the bootstrap loader, later on.

If you don't have an AVR programmer, you can use the serial port on your PC to update the AVR clock. Updating the Dutchtronix AVR Oscilloscope Clock, either with updated clock firmware or with this application is described in detail on the Dutchtronix website:

	http://dutchtronix.com/ClockFirmwareUpdate.htm

Operating instructions are listed on the AVR Oscilloscope Terminal page:

	http://dutchtronix.com/ScopeTerm.htm


Some notes about the source code.

The Dutchtronix AVR Oscilloscope Clock hardware uses an Atmega168, which is the default build target. However, this application will also work on an Atmega32, but you'll have to add the RTC, DAC and RS-232 level converter hardware yourself.  See the "Pin Configuration" section for one possible way of connecting the pins.

Due to the design of the Atmega168, no port has all 8 bit available for I/O if you want to use the serial port and a crystal. This is the reason for the splitting of the DAC access across 2 ports. Some of the bits of PORTC, PC4 and PC5, are used to access the Real Time Clock using the I2C protocol. By setting these bits to INPUT when the RTC is not being accessed, we can set the lower 4 bits of PORTC while ignoring the effect on the upper 4 bits, thus speeding up access to the DAC. The code for the ATmega32, which I used for development (it support JTAG which is a much faster debugging environment than "debugwire") is simpler since the ATmega32 has a full port dedicated to the DAC (PORTA). 

The source code is formatted for tabs=4, the default in AVR Studio.

The file "avr.inc" is used for various handy macro definitions. This file originates from ChaN (http://elm-chan.org/)

ScopeTerm reserves several AVR registers for global use; these registers (r8, r9, r14, r15, r25) cannot be used for any other purpose.

ScopeTerm uses timer2 for internal timing purposes, specifically for the blinking cursor.

Timer0 is used to debounce the push button switch, together with the function "PinChangeInterrupt", triggered by a change in Pin23.

Serial Port I/O is completely interrupt driven and buffered. Incoming characters are stored in a 128 byte buffer which the program can retrieve characters from when it has time to do so. Serial output is achieved by passing a ptr to a string (both flash based and SRam based strings are supported); the call will return immediately (assuming the serial out port is ready and the string is sent out as the serial out port finishes sending each character. See the file "Term-uart.asm" for all the UART functionali
ty. The flag "fRcvdChar" in the global status register "G_FLAGS" is set to indicate whether one or more incoming characters are ready.

After init, the program will enter the main loop which calls "ClockWorks" to keep the display and the clock going, followed by tests for input, either the push button switch or the serial port. Serial port input is processed depending on the current clock mode (Terminal or Command mode). When in Terminal mode, up to 40 characters are processed at once (remember, they are already received and buffered) in "ProcessBunchedChars".

"ProcessSerialInput" processes 1 character and does the terminal emulation, including the delayed handling of the LF and CR characters.

The screen is represented by 13 text buffers of 20 characters each. Pointers to these buffers are located in an array of text buffers, "ScanTbl", which also includes a ptr to the numeric time buffer. Each entry also contains a horizontal and a vertical position for that buffer. The "ScreenRefresh" routine runs through these pointers, rendering each buffer on the oscilloscope screen (see below). Scrolling is achieved by rotating all text buffers one place in the ScanTbl, clearing the new bottom line of the 
screen.

Cursor blinking is done by using the "flash" mechanism: an alternate buffer for the line that contains the area to blink is constructed (for ScopeTerm this is just the current char location) and the area to blink is replaced with an underscore character. The ScanTbl ptr, the original buffer ptr, and the alternate buffer are submitted to "AddFlashItem" which store all pertinent information in the FlashTable after which the combination of Timer2 and ClockWorks makes sure the area selected seems to blink.

The actual rendering is done by "DrawChar", which uses the character bitmap table "Newfonttbl" in Term-font8x5.asm to display the appropriate pixels. Each character pixel is actually a combination of 2 screen pixels. My original design used a combination of 4 pixels but this results in much lower refresh times and the image improvement is only marginal. You can experiment with the number of screen pixels per character pixel by changing the constant "DOTSPERCHARPIXEL" to 4.

Painting one pixel on the scope screen is achieved by clocking a value between 0 and 255 into the DAC for both the X and the Y axis. These values will be transformed by the DAC to a voltage between 0 and 4V (approximately) in a linear fashion (hopefully). The X-Y mode of the oscilloscope will send the beam to the location corresponding to the voltages on the X-Y inputs.

Burn-in prevention on the oscilloscope is achieved by dedicating 2 registers to a grid offset value, one for each axis. The registers are added to the final pixel location everytime a pixel is sent to the DAC, thus allowing a very small variation by changing these dedicated registers once in a while. The function "RotateXYOffset" does this.

Reading and updating the Real Time Clock is done by functions in the file "Term-rtc.asm". This is a bit banging implementation of the I2C protocol.

The rest of the code in ScopeTerm.asm is pretty straighforward, updating time, utility functions and doing command line processing. One interesting mechanism is seen in the functions starting with "SpecialUART". These functions need to wait for the serial port to be ready (to start sending text or to receive a character). Waiting would mean that the display is not refreshed so these functions each call ClockWorks repeatedly while waiting for the serial port, emulating part of the main program loop.

Please respect the license conditions included in each file; see the file LICENSE.TXT for details.

Suggestions for improvements and notification of bugs are always welcome: jdr@dutchtronix.com

Good luck

Jan de Rie
Dutchtronix
