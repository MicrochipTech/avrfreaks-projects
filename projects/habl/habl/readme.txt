


Henriks AVR Boot Loader for AVR ATMEGA88 with xmodem and autoboot.



How to use

See avrdude_readme.txt on how to set fuse bits and write the boot loader to the device.

Default configuration assumes 8 Mhz and 19200 baud.

When the boot loader starts it will write: 
habl <version>
for help goto www.eit.se/habl

Then it will wait 3 seconds for user cancel autoboot by pressing Esc exactly 5 times.

If Esc is pressed 5 times within 3 seconds it will start sending ready for xmodem 
download: CCCCCC...
A binary file with an application can now be sent using xmodem protocol.

If you have a hex file you need to convert it to binary first, hex2bin.exe
can be used for that.

If any other key is pressed during the 3 seconds or esc is pressed more or less than 5 times then the application is started (autoboot).


If there is no application loaded it will probably go into cyclic reboot
until an application is loaded.




This bootloader enables the watchdog so the applications loaded with this
bootloader needs to support that.




