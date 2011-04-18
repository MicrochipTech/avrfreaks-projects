This is a demo 01 of AVR++ avr-gcc port.

The demo generates an 1Hz signal on PA0. Open a 01-blink.prj
file as a VMLAB project, then see Scope (View->Scope) for the graph.

Alternately, you may build a real device with a flashing LED attached
to PA0 of ATmega16 through a 300 to 500 ohm resistor and enjoy
the view. ;-) 

FILES:
1. main.cpp -- the main program (what else can this one be?)
2. makefile -- just type "make" from console
3. 01-blink.prj -- the VMLAB project
