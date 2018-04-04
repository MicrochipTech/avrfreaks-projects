# Mega & XMega runtime self-test library

Uploaded by dev.null on 2018-04-04 08:38:00 (rating 0 out of 5)

## Summary

Hi!


 


I would like to introduce an already almost completed self-test library for the Mega and XMega families. The goal of this library is to test the microcontroller's ALU to that extent that it can be trusted that it is still capable to correctly execute binaries compiled from high-level languages by continuously monitoring the health of the ALU.


 


It was already tested in two major environments, one proprietary involving XMega microcontrollers, and one public involving an ATmega644 (the Uzebox open-source game console), where it demonstrated its capabilities of detecting anomalies (the XMega is quite easy to overclock to see what happens :) ).


 


I hope this could prove to be useful for meeting related demands (such as standards requiring such self-tests). The license (Mozilla Public License 2.0) is chosen to allow for proprietary uses while still ensuring the freedom of the library as best as possible while meeting this demand.


 


I used the avr-gcc toolchain for development on Linux, command-line, controlled by Makefiles. I dunno what it would take to use it in other environments, although I tried to ensure that it has minimal dependencies. It is fully written in assembler (this is simply required due to the nature of the task), with limited use of the C preprocessor.

## Links

- [XMBurner's main GitHub repository](https://github.com/Jubatian/xmburner)

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
- #Atmega
- ATxmega
- ATmega644
- Fire and Security
- Home Appliances
- Industrial Automation
