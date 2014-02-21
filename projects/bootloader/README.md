# Bootloader

Uploaded by dzairo on 2014-02-21 12:48:00 (rating 0 out of 5)

## Summary

I find one [http://sourceforge.net/projects/...](http://sourceforge.net/projects/avrub/) and try it . Work good .. but I make some changes in Software tools. Use serial com port to send data to MCU. After reset MCU .. program waiting some time while not received special char. If not then start program. There is not used special pin to control bootloader mode or program mode. MCU code is tested in WinAVR Software tools is tested in Delphi 7 In software tools I make changes: 1. in menu option is possible to enable - disable Hot Key (Ctrl+Shift+D) to start flash MCU if Windows is not active. 2. in menu option is possible to enable - disable auto reload file before flashing MCU.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
