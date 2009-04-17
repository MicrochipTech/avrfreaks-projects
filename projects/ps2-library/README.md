# PS/2 library

Uploaded by janpieter on 2009-04-16 22:39:00 (rating 0 out of 5)

## Summary

Update:  

jdiwnab found a bug in the code:  

The write function incorrectly calculates parity.  

The code currently calculates even parity, while the PS/2 protocol specifies odd parity.  

A simple fix is to switch the CBI and SBI calls in the parity bit sending block.


Since I can't (and don't have the time to) test this, I just copied jdiwnab's comment without any further testing, and I will not update the code.


JP


Update:  

The webpages mentioned in the sourcefiles are moved to:  

<http://www.computer-engineering.org>


JP


OK, I have updated the files to work with AVRStudio 4.12 and  

WinAVR-20060125.  

Build completed successfull here, though I can't test it since I do not have a test board around anymore.  

But it should still work (Hint: disable optimization because of timing)


JP


> Hello  

>  

> I wanted to use a at90s8515 to show the position of a ps/2 mouse on > an LCD.  

> But I couldn't find anything for ps/2 on a atmel microcontroller.  

> So I made it myself.  

> Maybe someone can use it.  

> Please mail your experience about the library to me  

>  

> Bye Jan Pieter de Ruiter  

> [janpieterd@lycos.nl](mailto:janpieterd@lycos.nl)

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
