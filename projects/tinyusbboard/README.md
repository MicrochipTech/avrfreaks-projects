# tinyUSBboard

Uploaded by matrixstorm on 2013-04-09 17:09:00 (rating 0 out of 5)

## Summary

URL: <http://matrixstorm.com/avr/tinyusbboard/>


TinyUSBboard is an Arduino compatible, minimalistic and cheap AVR microprocessor board with USB interface and simple "do it yourself" capability.  

It can be programmed via assembler, C, Arduino or of course BASCOM.  

All its components you need to construct it are universal and available nearly everywhere - no special PCB needed.  

It is well suited for beginners to learn AVR microcontroller programming and/or it can be used on solderless breadboards very simple to do various projects.


The board in form of a regular stick (approx. 2.5cm x 8.0cm) uses by default an ATmega8-16PU MCU, clocked at 16MHz.  

This IC allows up to 8kib of programmed code within its flash-memory.  

It also has additional 512byte persistent electronical eraseable memory (EEPROM) and 1kib volatile memory RAM.


Of course it is possible to use tinyUSBboard with other microcontrollers (for example ATmega328), too.


Althought there are lots of AVR boards available, tinyUSBboard has because of its special design and the complete revised sourcecode of USBaspLoader some unique advantages against other AVR VUSB boards. A list with the most outstanding ones is collected here.


Hardware advantages:


 - cheap and simple thanks to universal components (veroboard - no special PCB needed)  

 - very compact because of USB-stick form factor  

 - very fast datatransfers (no need for slow serial programming)  

 - klick-based bootloader condition (no jumper required)  

 - helps to conserve flash lifetime (interrupt trampolines combined with no complete chip-erase necessary)  

 - nearly all PINs are routed (for use on breadboards)  

 - USB-powered - no extra powersource needed  

 - do-it-yourself enabled 


Software advantages:


 - completly revised USBaspLoader ([https://github.com/baerwolf/USBa...](https://github.com/baerwolf/USBaspLoader))  

 - lots of space optimizations  

 - bootloader section update-/exchangeable without any further programming device needed  

 - no hardware locking needed


 - lots of new features  

 * bootloader update feature  

 * software lockbits  

 * exit-logic  

 * accepts ISP-clock commands  

 * able to read its fuses/lockbits  

 * fully supported flash-read (not just paged one)  

 * "magic values" to distinguisch different hardware-layouts


 - lots of bug fixes  

 * fixed "fuse changed! Was 1f, and is now 0" bug  

 * fixed "cannot set sck period. " bug  

 * fixed "flash reads always 0x00 in terminal mode" bug

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
