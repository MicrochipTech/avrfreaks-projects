This is a demo 03 of AVR++ avr-gcc port.

The demo can display a moving light on a 4-LED line by doing
an "external bit shift" (no shift in code, shift is done by
using two buses, an output one and an input one that are
connected with a single bit shift). The LEDs are attached to
PA3-PA0 (just remember about resistors). Some extra connections
are required: PA0 to PB1, PA1 to PB2, PA2 to PB3, PA3 to PB0.
In VMLAB design the pins are connected through 1K resistors,
though those resistors are not necessary, you may connect the
pins directly. Open a 03-counter.prj as a VMLAB project, then
see Scope (View->Scope) for graphs.

Alternately, you may build a real device with flashing LEDs,
use a breadboard. ;-)

FILES:
1. main.cpp -- the main program (what else can this one be?)
2. makefile -- just type "make" from console
3. 03-counter.prj -- the VMLAB project
