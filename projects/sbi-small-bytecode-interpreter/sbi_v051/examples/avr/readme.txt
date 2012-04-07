---- AVR Example of SBI -------------------------------------------------------

This is an AVR project that loades SBI programs from a MicroSD card.
In this version are used buffered functions to read the SD, so the reading
process will take less time. Besides, the user variables limit is set to 16,
user variables limit to 8 and labels maximum number to 16 (to save AVR RAM).

Sucessfull tested on an ATmega16 DIP running at 16 mHz.

Be sure that on the root of the SD exists a file named 'program.sbi', which
must be one of the compiled examples in the folder 'programs' (or a your
program).

Use a serial terminal to check running status and to see debug's output
(required by 'pow.sbi', 'counter.sbi' and 'jumps.sbi').

Note that 'pow.sbi', 'counter.sbi' and 'jumps.sbi' are the same of PC's
examples! This demostrates that the code is very portable!
