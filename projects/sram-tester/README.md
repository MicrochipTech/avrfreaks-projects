# SRAM Tester

Uploaded by sellis on 2002-03-13 21:51:00 (rating 0 out of 5)

## Summary

This is a SRAM tester I put together to test my external SRAM board, and new SRAM chip on STK501. It repeatedly tests all RAM addresses from RAMSTART to user-definable value RAMTOP (the stack is placed just before RAMSTART so the default value of 0x100 should be the minimum), and reports any errors to the UART for display on a connected PC. It does a read/write on each byte in the RAM, and then a separate read back pass across the whole RAM again to check for erroneous overwrites. Each block of 256 bytes gets a different sequence, and this sequence is again changed for each round of testing. PD2 can be attached to A16 on the RAM to check paged operation up to 128K. PORTB can be connected to LEDs to display simple status "running lights". More details in the comments in the code.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
