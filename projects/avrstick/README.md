# avrstick

Uploaded by matrixstorm on 2014-03-28 14:15:00 (rating 5 out of 5)

## Summary

URL: <http://matrixstorm.com/avr/avrstick/>


AVRSTICK is an USB fullspeed browser programmable prototyping board with Atmel ATxmega128A3U.


The special feature of the board is that it requires no drivers or programming software (such as AVRDUDE).  

The board also provides some GPIO ports. These are routed to the core IC in a specific way, so all technologies on the ATxmega128A3U can be "tasted".  

Therefore the board is suited for beginners, who want to play around with microcontrollers. But it might also be interesting for more experienced users who want to implement and test their own USB devices.


A bootloader is installed by default, which enumerates the board as an USB stick. By pressing the PROG-button during a RESET, you can enter its bootloader mode.  

There all the memories in the boards ATxmega128A3U (FUSES, {production-, user-}signature, FLASH and EEPROM) are accessable as files.  

Changing / overwriting a file then corresponds to the programming of the specific memory. Of course just reading is possible, too!  

Since the bootloader also offers an update functionality, you always can benefit from all upcoming features.


So for example the board (without having any driver software installed on the computer) can a be programmed directly in the browser.  

The assembled firmware is then simply downloaded to the dongle.


However the board is not limited to being programmed in the browser.  

You also can use an AVR compiler to program it in your preferred language with your preferred IDE. To install your firmware on the board, just overwrite FIRMWARE.BIN with the binary file produced by avr-objcopy.  

If you like using Atmels FLIP (Appnote AVR1916) protocol better for transfering your firmware onto the board: There is an update firmware for exchanging the bootloader provided here.


Of course there are also ready-to-use examples provided here, which turn the stick into useful tools (like an AVR ISP MKII PDI programmer).


This project is still at the very beginning (even if a lot of stuff already works)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
