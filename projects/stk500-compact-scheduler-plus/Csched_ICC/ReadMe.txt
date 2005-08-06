An STK500/90S8515 demo of a compact task scheduler. Included 
are the.hex files for direct loading to the STK500 (Just a better
LED/Switch demo). The original IAR source (msched1.c), An 
ImageCraft verion of the same (Csched1_ICC.c) and an expanded 
demo including multiple independent timing loops (Csched2_ICC.c)

I have just finished the awful process of reviewing the many 
options for RTOS's for the AVR. I was primarily Interested in 
Imagecraft C compatibility but I looked at the whole range. 
I ended up choosing  Salvo because it offered RTOS services 
in a cooperative, non-preemptive OS.  The thought of a 
preemptive scheduler chopping my tightly crafted little tasks 
into arbitrary pieces was very scary.

However, Salvo is huge.  and it's one of those products that 
releases one version for all supported hardware and compiler 
choices. Many chips, many compilers, hundreds of compiler 
switches. It's a big investment in time, money and brain space. 
Use it if you need it.

However most of my needs are really just task scheduling with  
delays.  GD Technik in England wrote a very simple C function 
pointer based task scheduler described in the Atmel Application
Journal:
http://www.atmel.com/dyn/resources/prod_documents/CompactScheduler.pdf

I've added a very simple asychronous delay mechanism and a 
restart capability to the list so that any task (not just the 
first one on the list) can stop any other task, including itself.  
This means you can build linked lists of routines with delays 
in between.

The Zip has  their original IAR code (msched1.c), a version 
just adapted for ICCAVR (Csched1_ICC.c)  and a version that adds 
the features described above (Csched2_ICC.c).

My two versions are setup for the original STK500 with the 
initial demo jumpers and the old 90S8515. Except for the 
interrupt pragma and the names of the include files this should
all be usable with any C compiler and any AVR with SRAM and a 
Timer0.

John Edwards - August 2005