# AVR-GCC+AVR-GDB In System Debugger

Uploaded by birkler on 2003-05-11 20:18:00 (rating 0 out of 5)

## Summary

Target monitor with software and hardware breakpoints. Host program using STK200 connection to target to modify breakpoints and read/write flash and SRAM. Host program uses tcp/ip connection to avr-insight or avr-gdb. Compiled under CYGWIN. Released under GNU GPL, based on Denis Chertykov monitor.  

- No patches to avr-gdb needed  

- Compiled and software breakpoints  

- Uses GDB serial protocol over tcp/ip  

- Uses MOSI/MISO/SCK: No extra pins needed  

- Configurable interrupt, INT0, INT1, Timer0 etc.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
