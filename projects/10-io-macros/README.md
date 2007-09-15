# 10 I/O Macros

Uploaded by valyor on 2007-09-15 05:10:00 (rating 0 out of 5)

## Summary

The macros with comments are listed below:


clp - CLear bit in Port data register (PORT)  

sep - SEt bit in Port data register (PORT)  

spc - Skip if bit in Port data register (PORT) is Cleared  

sps - Skip if bit in Port data register (PORT) is Set


cld - Clear bit in data Direction register (DDR)  

sed - Set bit in data Direction register (DDR)  

sdc - Skip if bit in data Direction register (DDR) is cleared  

sds - Skip if bit in data Direction register (DDR) is set


sic - Skip if bit in Input pins address (PIN) is Cleared  

sis - Skip if bit in Input pins address (PIN) is Set


Macros definitions are stored in the include fail IOPORTS.INC.  

Device pins must be defined, as two char strings. Left nibble is port name, right nibble is pin number.  

For example, G3 means - Pin 3 on Port G.


Features are, easier code writing and simplified I/O ports registers access. Only one argument, wich consists information for both - port name and pin number, is necessary. These conditional assembly macros can be used with any AVR device, without other modifications when changing the device definition file. Conditional directives are present in AVR Assembler version 1.74 (and later), but code with these macros, will be assembled properly, only with AVR Assembler 2.


IOMEGA64.INC is less universal modification, that don’t support all AVR devices, but is compatible with AVR Assembler 1 (v.1.74 and later) as well. This file was written for ATmega64, but many others, more conventional AVR devices, will be assembled correctly.


For more help information, see AVR Studio Assembler help, Atmel AVR001 application note and comments in include files.


Note 1: Be careful, when use skip instruction before these macros. Some of them, are three instructions long.


Note 2: These macros, are not 100% tested, for all AVR devices.


Example 1: Define device pin named OUTPUT1 as output, and set


.include "m64def.inc"  

.include "IOPORTS.INC"


.EQU OUTPUT1 = G3 ;Pin 3 on Port G


 sed OUTPUT1 ;output  

 sep OUTPUT1 ;set


;The same is:


 lds r16,DDRG  

 sbr r16,1<<3  

 sts DDRG,r16


 lds r16,PORTG  

 sbr r16,1<<3  

 sts PORTG,r16


Example 2: Configure device Pin 7 on Port B as tri-state input, and skip next instruction, if low level is present on the pin.


.include "m64def.inc"  

.include "IOMEGA64.INC"


 cld B7 ;DDRB7 = 0, input  

 clp B7 ;PORTB7 = 0, tri-state  

 sic B7 ;skip if PINB7 = 0


;The same is:


 cbi DDRB,7  

 cbi PORTB,7  

 sbic PINB,7

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
