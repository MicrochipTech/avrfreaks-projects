# gpio - General Purpose IO Macros

Uploaded by heinrichs.hj on 2007-07-24 09:56:00 (rating 0 out of 5)

## Summary

This include file provides symbolic access to GPIO pins through a few  

generic macros and a configuration table. The configuration table holds  

information about the GPIO layout and mapping to the hardware ports of  

the AVR chip. Using a table to describe the GPIOs in a symbolic way has  

several advantages:  

1. All bit mappings can be made at a central project place, in one file.  

2. Changes to the layout of the GPIOs do not enforce any changes to other  

 source code places - simply change the config table.  

3. In the program itself there is no need to worry about bit positions  

 or bit masks - bits are handled in a symbolic way.  

4. No more worry about anding/oring bits and bitfields.  

5. ...


The configuration table is made up from preprocessor #defines. The syntax  

is as below:


 #define SYMBOL PORT, MSB, LSB


 // SYMBOL : A unique symbolic name for the GPIO (or GPIO group).  

 // PORT : AVR Hardware Port (A,B,C,D,...).  

 // MSB : Bit# of the (M)ost (S)ignificant (B)it in this GPIO group.  

 / /LSB : Bit# of the (L)east (S)ignificant (B)it in this GPIO group.


Once the table is created, the GPIOs can be accessed by several macros  

(please find the exact calling conventions in the source code below):


 gpio\_conf(cfg,io,p\_da) - Configures the GPIOs (set PORT/DDR reg.)  

 gpio\_wr(px,cfg,da) - Writes to GPIO registers  

 gpio\_rd(px,cfg) - Reads GPIO registers (right aligned)  

 gpio\_rd8(px,cfg) - Reads GPIO registers (unaligned)  

 gpio\_toggle(cfg) - Toggles GPIOs (write 1's to PIN reg.)  

 gpio\_rd\_inv(cfg) - Reads GPIO regs. right aligned, and inversed  

 gpio\_set(cfg) - Sets GPIO registers (write 1's to PORT reg.)  

 gpio\_clr(cfg) - Clears GPIO registers (write 0's to PORT reg.)

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
