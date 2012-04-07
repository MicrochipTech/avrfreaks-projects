---- AVR Example of SBI -------------------------------------------------------

This is an AVR project that loades SBI programs from a MicroSD card.

Sucessfull tested on an ATmega16 DIP running at 16 mHz.

Be sure that on the root of the SD exists a file named 'program.sbi', which
must be one of the compiled examples in the folder 'programs' (or a your
program).

Use a serial terminal to check running status and to see debug's output
(required by 'pow.sbi' and 'counter.sbi').

Note that 'pow.sbi', 'counter.sbi' and 'jumps.sbi' are the same of PC's
examples! This demostrates that the code is very portable!
