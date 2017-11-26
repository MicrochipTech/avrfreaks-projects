This is a package for project USB Macro_keyboard.

Main purpose of this project is to automate keboard presses with external tool that doesn't 
require install aditional software (so it works even on login screen of Windows OS or in BIOS).
It uses default HID keyboard drivers.

Sequence of keyboard signals is stored in eeprom. It can be uploaded via commandline tool or 
via standard SPI programming interface (although it might not be easy to get a proper addressing 
and also the attached schematic doesn't implement any programming connector on SPI interface).

Before programing make sure that your OS keyboard setting does match key mapping at the end of hid.c file.
To alter the code for your keyboard layout you can use table in Appendix C of Keyboard_scancodes_specification.pdf.
Additionals codes are in hid_keys.h that are also stored in eeprom memory but are used for device control
(e.g. timer delay - See function parseKeyBuffer in main.c file).

Original goal was to also create Windows program that can upload data to this device.
Unfortunately Windows handle keyboard drivers in a specific way and doesn't allow access to the device.
Because of that I decided to use signal on Num- and Caps-lock LEDs which are part of most of the keyboards.
After several presses of Num- and Caps-lock (or their combinations) it triggers corresponding procedure.

The commandline tool included is operating without too much problem on Linux system.
User has to have an access to USB ports or have root privilege. Also libusb-1.0 library is required.

Other limitation of this device is that it can only send key press and imidiate release. It can't
hold the key pressed for longer duration. Also it can send only one key at the time (not counting
modifiers keys like ctrl, alt and shift).

Special options can be selected in main_config.h (for example remove print function and some of the keys
to fit the code in attiny45) and usbconfig.h (change vendor/device name and VID/PID).


===Note for tiny85 clock source======================

Attiny85 can use its internal oscillator (with some calibration) to generate 16.5 MHz clock source.
I personaly can't recommend it since it was not reliable in my case. Sometimes the calibration
was not done correctly and the device didn't properly connect to USB driver. Therefore I decided
to use 12 MHz crystal instead.

At first I used whole calibration range but it never connected. USB specification probably require
faster device connection.
On second try I used only very small part of it but that required first to find out close to optimal
value of OSCCAL register. This was quite different between 2 chips I tested so the build phase
of the device was not simple.

Some of that old code is located in misc folder if you want to try it.


===Thanks goes to====================================

Objective development for V-USB library   https://www.obdev.at/products/vusb/index.html
Frank Zhao for explanation of HID descriptors   http://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/
Joonas Pihlajamaa and his password generator example   http://codeandlife.com/2012/03/03/diy-usb-password-generator/


===Licensing========================================

Files in usbdrv folders are part of V-USB library from OBJECTIVE DEVELOPMENT Software GmbH - see License.txt
in that folder.

Other files are released under GNU GPLv3 license. See license.txt in root folder of this project.


===Content of the package===========================

circuit
	KiCad
		CustomPCD.pretty
			Diode_Zener.kicad_mod
			SOIC-8S2.kicad_mod
			USB_A_male.kicad_mod
		fp-lib-table
		Macro_keyboard-cache.lib
		Macro_keyboard.kicad_pcb
		Macro_keyboard.net
		Macro_keyboard.pro
		Macro_keyboard.sch
	PCB_bottom_mirrored.pdf
	Schematic.pdf
commandline
	macro_keyboard.cpp
	Makefile
firmware
	hid.c
	hid.h
	hid_keys.h
	hid_rq.h
	main.c
	main_config.h
	main.h
	Makefile
	timer.h
	usbconfig.h
	usbdrv
		asmcommon.inc
		CommercialLicense.txt
		Changelog.txt
		License.txt
		oddebug.c
		oddebug.h
		Readme.txt
		usbconfig-prototype.h
		usbdrvasm.asm
		usbdrvasm.lst
		usbdrvasm.o
		usbdrvasm.S
		usbdrvasm12.inc
		usbdrvasm128.inc
		usbdrvasm15.inc
		usbdrvasm16.inc
		usbdrvasm165.inc
		usbdrvasm18-crc.inc
		usbdrvasm20.inc
		usbdrv.c
		usbdrv.h
		USB-ID-FAQ.txt
		USB-IDs-for-free.txt
		usbportability.h
license.txt
misc
	bottom.jpg
	HID1_11.pdf
	Keyboard_scancodes_specification.pdf
	Old_code
		hid.c
		usbconfig.h
	side.jpg
	top.jpg
readme.txt

===================================================