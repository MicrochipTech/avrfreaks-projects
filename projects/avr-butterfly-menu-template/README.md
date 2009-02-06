# AVR Butterfly menu template

Uploaded by jremington on 2009-02-06 04:20:00 (rating 0 out of 5)

## Summary

Several large applications are available for the AVR Butterfly that employ a nice menu system for choosing program modes and other options. For a new application, one could either start from scratch, or from one of the existing APIs and get a different menu-driven application running. However, it is pretty daunting to throw away thousands of lines of code and keep the key user interface intact!


I've done that for my latest application, beginning with Dean Camera's fantastic BUTTLOAD package, and produced a template file for general development purposes. It presents three dummy "MAIN MENU" routines, with labels and actions. Menu selection of the three routines is driven by the joystick. I've included an interrupt-driven joystick driver and a tone generator. Note that I had previously added to Dean's LCD drivers some new features like flashing digits, the use of which can be seen in my Real Time Clock project example (Butterfly\_RTC).


I hope this is useful for others. Of course, bug notices and tweaks are welcome. 


Jim

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
