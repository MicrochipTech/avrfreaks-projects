# AVR Console

Uploaded by cprhodin on 2014-05-26 13:30:00 (rating 0 out of 5)

## Summary

Serial I/O with stdio-like canonical processing. Canonical processing means no input is available until the user presses "Return" and some limited line editing can be done before that. To erase the last character press "Backspace" to kill the entire line press "Cntl-U". The goal of this project is to make standard I/O available with a minimum impact on resources. By "minimum" I mean only USART0 and its associated I/O pins and interrupts. There is no use of timers and no need to call any polling routines. As long as interrupts are enabled you can type until the buffer fills, backspace over everything you just typed, and then type some more. When you aren't typing and the output buffer is empty, exactly zero cycles will be consumed. With a typical Unix like "tty" there are a multitude of terminal attributes available. With this code you get five. These are: ICRNL - Translate received CR to NL. ECHO - Echo received characters to transmitter. ICANON - Enable canonical mode processing. INONBLOCK - Do not block until operation is complete. ONLCR - Translate transmitted NL to CR-NL. By default the only attribute not set is INONBLOCK. This means a call to getchar() will wait for you to enter something. With INONBLOCK set, a call to getchar() will return EOF (-1) when nothing is available. Setting the terminal attributes to 0 disables the extra processing and leaves you with a buffered, interrupt driven serial interface. Target device(s): ATmega328P Arduino Nano Resources: USART0, pins, interrupts 1.5KB ROM 109B RAM Toolchain: gcc version 4.7.2 (AVR\_8\_bit\_GNU\_Toolchain\_3.4.2\_939) avr-libc-1.8.0 Programming tool: AVRISP mkII How to build (Linux 64-bit system): $cd librb $make $cd .. $make $make burn # Flash an Arduino Nano with avrdude. How to run (Linux 64-bit system): The default baud rate is 250K. The baud rate is set by the preprocessor variable "BAUD". $setserial /dev/ttyUSB0 spd\_cust divisor 96 # make 38.4K into 250K $minicom -D /dev/ttyUSB0 -b 38400 Complete source code can be downloaded from Github. Use the command: git clone -b console-only [https://github.com/cprhodin/avr-...](https://github.com/cprhodin/avr-console.git)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
