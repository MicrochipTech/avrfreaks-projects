Swfltek Atmega Boot Loader Features:

    Arduino compatible, STK500V1 protocol subset
    Fits in 512 byte boot section *
    Programs application flash up to 128 Kbytes
    Programs Eeprom too!

Limitations
    No 'eye candy'
    Requires hardware USART
    Requires AVRDUDE version 5.10 or higher

Though written for atmega328p, it is easily adaptable to many devices. It has
been tested by the author on atmega168, atmega328p, atmega1280 and atmega1284p.



* When compiled for devices with 64Kbyte or less of program memory.
=====================================================================================

            BUILDING

Open the file 'make.conf' with a text editor, and edit the entries to match your ISP
programmer and the MCU you want to create a bootloader for. Be very careful about the
fuse settings so that you don't disable ISP programming! Save your changes.

Then, cd to the swfltek_bootloader directory, and type...

    make load
    make fuse
    make lock

            TESTING

To verify your boot loader works properly, a test application is provided. To build and
load the test application, type the following at the command line...

    cd test
    make load

Connect to the AVR's serial port at 38400/N/8/1, and reset the AVR. You should see the
string '0123456789'. This indicates that both flash and eeprom burned properly.
