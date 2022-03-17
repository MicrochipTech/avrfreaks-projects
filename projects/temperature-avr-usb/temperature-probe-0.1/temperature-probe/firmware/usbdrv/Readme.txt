This is the Readme file to Objective Development's firmware-only USB driver
for Atmel AVR microcontrollers. For more information please visit
http://www.obdev.at/avrusb/

This directory contains the USB firmware only. Copy it as-is to your own
project and add all .c and .S files to your project (these files are marked
with an asterisk in the list below). Then copy usbconfig-prototype.h as
usbconfig.h to your project and edit it according to your configuration.


TECHNICAL DOCUMENTATION
=======================
The technical documentation (API) for the firmware driver is contained in the
file "usbdrv.h". Please read all of it carefully! Configuration options are
documented in "usbconfig-prototype.h".

The driver consists of the following files:
  Readme.txt ............. The file you are currently reading.
  Changelog.txt .......... Release notes for all versions of the driver.
  usbdrv.h ............... Driver interface definitions and technical docs.
* usbdrv.c ............... High level language part of the driver. Link this
                           module to your code!
* usbdrvasm.S ............ Assembler part of the driver. This module is mostly
                           a stub and includes one of the usbdrvasm*.S files
                           depending on processor clock. Link this module to
                           your code!
  usbdrvasm*.inc ......... Assembler routines for particular clock frequencies.
                           Included by usbdrvasm.S, don't link it directly!
  asmcommon.inc .......... Common assembler routines. Included by
                           usbdrvasm*.inc, don't link it directly!
  usbconfig-prototype.h .. Prototype for your own usbdrv.h file.
* oddebug.c .............. Debug functions. Only used when DEBUG_LEVEL is
                           defined to a value greater than 0. Link this module
                           to your code!
  oddebug.h .............. Interface definitions of the debug module.
  iarcompat.h ............ Compatibility definitions for IAR C-compiler.
  usbdrvasm.asm .......... Compatibility stub for IAR-C-compiler. Use this
                           module instead of usbdrvasm.S when you assembler
                           with IAR's tools.
  License.txt ............ Open Source license for this driver.
  CommercialLicense.txt .. Optional commercial license for this driver.
  USBID-License.txt ...... Terms and conditions for using particular USB ID
                           values for particular purposes.

(*) ... These files should be linked to your project.


CPU CORE CLOCK FREQUENCY
========================
We supply assembler modules for clock frequencies of 12 MHz, 15 MHz, 16 MHz and
16.5 MHz. Other clock rates are not supported. The actual clock rate must be
configured in usbdrv.h unless you use the default 12 MHz.

12 MHz Clock
This is the traditional clock rate of AVR-USB because it's the lowest clock
rate where the timing constraints of the USB spec can be met.

15 MHz Clock
Similar to 12 MHz, but some NOPs inserted. On the other hand, the higher clock
rate allows for some loops which make the resulting code size somewhat smaller
than the 12 MHz version.

16 MHz Clock
This clock rate has been added for users of the Arduino board and other
ready-made boards which come with a fixed 16 MHz crystal. It's also an option
if you need the slightly higher clock rate for performance reasons. Since
16 MHz is not divisible by the USB low speed bit clock of 1.5 MHz, the code
is somewhat tricky and has to insert a leap cycle every third byte.

16.5 MHz Clock
The assembler module for this clock rate differs from the other modules because
it has been built for an RC oscillator with only 1% precision. The receiver
code inserts leap cycles to compensate for clock deviations. 1% is also the
precision which can be achieved by calibrating the internal RC oscillator of
the AVR. Please note that only AVRs with internal 64 MHz PLL oscillator can be
used since the 8 MHz RC oscillator cannot be trimmed up to 16.5 MHz. This
includes the very popular ATTiny25, ATTiny45, ATTiny85 series as well as the
ATTiny26.

See the EasyLogger example at http://www.obdev.at/avrusb/easylogger.html for
code which calibrates the RC oscillator based on the USB frame clock.

20 MHz Clock
This module is for people who won't do it with less than the maximum. Since
20 MHz is not divisible by the USB low speed bit clock of 1.5 MHz, the code
uses similar tricks as the 16 MHz module to insert leap cycles.


USB IDENTIFIERS
===============
Every USB device needs a vendor- and a product-identifier (VID and PID). VIDs
are obtained from usb.org for a price of 1,500 USD. Once you have a VID, you
can assign PIDs at will.

Since an entry level cost of 1,500 USD is too high for most small companies
and hobbyists, we provide some VID/PID pairs for free. See the file
USBID-License.txt for details.

Objective Development also has some license offerings which include product
IDs. See http://www.obdev.at/avrusb/ for details.


DEVELOPMENT SYSTEM
==================
This driver has been developed and optimized for the GNU compiler version 3
(gcc 3). It does work well with gcc 4, but with bigger code size. We recommend
that you use the GNU compiler suite because it is freely available. AVR-USB
has also been ported to the IAR compiler and assembler. It has been tested
with IAR 4.10B/W32 and 4.12A/W32 on an ATmega8 with the "small" and "tiny"
memory model. Not every release is tested with IAR CC and the driver may
therefore fail to compile with IAR. Please note that gcc is more efficient for
usbdrv.c because this module has been deliberately optimized for gcc.


USING AVR-USB FOR FREE
======================
The AVR firmware driver is published under the GNU General Public License
Version 2 (GPL2). See the file "License.txt" for details.

If you decide for the free GPL2, we STRONGLY ENCOURAGE you to do the following
things IN ADDITION to the obligations from the GPL2:

(1) Publish your entire project on a web site and drop us a note with the URL.
Use the form at http://www.obdev.at/avrusb/feedback.html for your submission.
If you don't have a web site, you can publish the project in obdev's
documentation wiki at
http://www.obdev.at/goto.php?t=avrusb-wiki&p=hosted-projects.

(2) Adhere to minimum publication standards. Please include AT LEAST:
    - a circuit diagram in PDF, PNG or GIF format
    - full source code for the host software
    - a Readme.txt file in ASCII format which describes the purpose of the
      project and what can be found in which directories and which files
    - a reference to http://www.obdev.at/avrusb/

(3) If you improve the driver firmware itself, please give us a free license
to your modifications for our commercial license offerings.


COMMERCIAL LICENSES FOR AVR-USB
===============================
If you don't want to publish your source code under the terms of the GPL2,
you can simply pay money for AVR-USB. As an additional benefit you get
USB PIDs for free, licensed exclusively to you. See the file
"CommercialLicense.txt" for details.

