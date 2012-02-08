# mickboot

Uploaded by Mickjc75 on 2012-02-08 02:54:00 (rating 0 out of 5)

## Summary

Bootloader for Atmega128, 1281, 1280, and AT90can128 with the following:


Pros:  

 No special PC software required (just a terminal).  

 No handshaking required at all.  

 Works fine with USB adapter type com ports.  

 Auto selection of UART0/1  

 Simplex operation (Device only needs to rx data, not tx it)  

 Programmed pages are verified (up to 10 retry if it fails).  

 No restrictions on fragmented hex files or page boundaries.


Cons:  

 Pretty big, (~3.6k)  

 Ram hungry, although not a problem for Atmega128.  

 No auto-baud, you have to build it for your clock freq.  

 No eeprom or lock bits programming, (maybe I'll add it l8r)


Operation:  

 Burn the bootloader with fuses selecting the largest (4k) bootloader section, and enable the boot reset vector.


 To activate the loader, send a space (0x20) within 200mS after reset. Then send 'Z' (UPPER CASE) within 15sec. Then send hex file as ascii, using 9600baud.


 If the operation is successful, application will run on completion. Otherwise, you have 15 seconds to send 'Z' and try again.


Thats it! If your product has bi-directional comms, you will see prompts and progress on the terminal. If not, you might want to make sure the uart responses do not upset your hardware.


Note hyperterminal is a P.O.S and will not send a hex file intact, use virtually any other terminal program, I use Hercules.


Feel free to ask me any q's if you have trouble, or If you don't use avrgcc and you want me to upload a hex file for a specific clock/baud just ask.


Good luck!


Version 1.1 changes:


Added builds for Atmega1280, 1281, 64 (un-tested), AT90can128.  

Fixed occasional page retry, by disabling ISR's during page load.  

Major speed improvements, a 4MHz clock can upload at 38400 baud.


Added feature which allows boot loader to be invoked through a terminal console. Simply leave 1 only UART enabled, disable all other interrupts (due to vector switch), and call the boot loader address like this:


void (*bootloader)(void)= (void(*)(void))0x1E000;  

(*bootloader)();


The boot loader will start using the enabled com port at it's current baud rate. This can be used to do remote firmware uploads over a GPRS modem in transparent mode.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
