This is a demo 02 of AVR++ avr-gcc port.

The demo can display an 8-LED counter counting from 0 to 255.
The counter LEDs can be attached (bit 7 to 0) to PB3, PA3,
PB2, PA2, PB1, PA1, PB0, PA0, respectively. Open a 02-counter.prj
file as a VMLAB project, then see Scope (View->Scope) for graphs.
Elder bits are on top of scope.

Alternately, you may build a real device with flashing LEDs,
use a breadboard. ;-) Just remember about resistors.

FILES:
1. main.cpp -- the main program (what else can this one be?)
2. makefile -- just type "make" from console
3. 02-counter.prj -- the VMLAB project
