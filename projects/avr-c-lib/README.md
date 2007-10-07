# AVR C++ Lib

Uploaded by totenhose on 2007-10-07 09:34:00 (rating 0 out of 5)

## Summary

It's an open-source object-oriented AVR programming library. All AVR functionality is written on C++ structures in "human" language. User doesn't have to remember all the Atmel's register and bit name abbreviations, only has to know how each feature works. For example, to set up a USART, he writes:  

USART0::Setup(9600, NoParity, OneStop, Bits8);


Beside AVR peripheral functionality, the library includes common data arrays, pin arrays, fifo buffers, etc. These aren't like STL classes, because efficency matters in AVR. Believe it or not, but GCC correctly compiled C++ is no worse than self-written ASM code.


One great feature of AVR C++ Lib is delegates. These are like functors, or delegates from C#. Although they take a lot of ROM, the performance stays tolerable, not mentioning the comfort of using them in interrupts.


Currently, 12 mainstream AVR devices are supported.

## Links

- [AVR C++ Lib Sourceforge releases](http://sourceforge.net/project/showfiles.php?group_id=190998)

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
