README for Tinyserial Library version 1.0

Copyright (C) 2010 Timothy Jon Fraser <tfraser@alum.wpi.edu>
See LICENSE.txt file for details.

[ What is the Tinyserial library? ]

The Tinyserial library is a space-saving alternative to the Arduino
software distribution's libraries for reading and writing characters
and strings to the USART0 serial port on the Atmel ATmega168 and
ATmega328p MCUs found on Arduino Diecimilla and Duemilanove boards.

 
While the Arduino software distribution's libraries provide
interrupt-driven serial I/O with far more features and support more
MCUs, the Tinyserial library provides only the most basic
polling-based serial I/O.  However, due to its use of assembly rather
than C++, the Tinyserial library uses far less Flash and SRAM, thereby
giving you room to implement larger and more complicated applications
on your boards.

The Tinyserial library respects the GNU libc ABI, so you can call
into it from C and C++ programs.


[ When should I consider using the Tinyserial library? ]

Concurrency:

    If your program must perform other processing while waiting for
    user input, or while bytes are passing back in forth over the
    serial port, use the standard Arduino libraries, as they are
    interrupt-drivern.

    If it is OK if your program does nothing while waiting for user
    input or other serial I/O, then consider using Tinyserial.

Storing immutable data in Flash to save SRAM:

    If you want to store immutable data structures other than strings
    in Flash in order to save SRAM, use the standard Arduino
    Libraries.

    If you simply want to store immutable strings in Flash in order to
    save SRAM, then consider using Tinyserial.

Hardware support:

    If you are using an MCU other than the Atmel ATmega168 or -328p,
    you may still be able to use Tinyserial.  However, if your MCU has
    more than one USART and you need to use more than one, you should
    use the standard Arduino Libraries as Tinyserial supports only
    one.

    If you are using an Arduino Diecimilla or Duemilanove, consider
    using the Tinyserial library.


[ How tiny is tiny? ]

    Hello World programs built using the Tinyserial library consume an
    order of magnitude less Flash and SRAM than functionally
    equivalent Hello World programs built with the Arduino-0017
    software distribution makefiles and libraries.

                              .text size    .data + .bss size
                              (Flash)       (SRAM)

 Arduino-0017 Hello World     4358 bytes    196 bytes
(string in SRAM)

 Arduino-0017 Hello World     4420 bytes    197 bytes
(string in Flash)

 Tinyserial Hello World        244 bytes     16 bytes
(string in SRAM)

 Tinyserial Hello World        260 bytes      0 bytes
(string in Flash)

The Hello World programs and Makefiles used for this comparison are
included in the Tinyserial distribution.


[ Building the Tinyserial library ]

Follow these steps to build the Tinyserial library.  These
instructions should work on both Windows and UNIX-like platforms.
These instructions describe how to build on the command line.  You do
not need the Arduino integrated development environment UI.

1. First, you'll need to download and install the Arduino software
   distribution.  Although the Tinyserial library does not depend on
   any of the libraries contained in this distribution, it does use
   some of the headers and avr-gcc tools.  See http://arduino.cc.

2. Put the directories containing the avr-gcc tool binaries into your
   PATH environment variable.  In the arduno-0017 distribution, there
   were two directories:
    
       arduino-0017/hardware/tools/avr/bin and
       arduino-0017/hardware/tools/avr/utils/bin.
    
3. Make sure your Arduino software distribution has an avrdude.conf
   file.  My Windows distribution of arduino-0017 did not; I had to
   download the GNU/Linux version of the arduino-0017 distribution and
   copy it from there.

4. Modify the configuration.mk file provided in the Tinyserial distribution.
   See the instructions in that file.

5.  Type make to build the Tinyserial library and its test programs.


[ Building and deploying your own programs ]

The tinyserial.h file contains documentation on how to use all of the
Tinyserial library functions.  tinyserial.h and libtinyserial.a reside
in the Lib directory after you have built the library.

See the test programs for further examples on how to use the Tinyserial
library.  Adapt their Makefiles for your own purposes.

Use "make upload" to load programs into flash.  I usually press the
reset button on my Arduino board the instant before I hit enter on the
make upload command.  The timing is tricky - sometimes I get a "the
programmer is not responding" error.  Keep trying; it can take several
tries for make upload to work.


[ Contents of this distribution ]

Lib              - Source for the Tinyserial library.
Test             - Programs for testing the Tinyserial library.
Compare          - Hello world programs used to due above size comparison.
configuration.mk - Customize this to fit your local configuration.
Makefile         - Use this to make library and test programs.
LICENSE.txt      - LGPL v3.
README           - This file.


[ Feedback and bug reports ]

Please send feedback and bug reports to Tim Fraser at tfraser@alum.wpi.edu.

