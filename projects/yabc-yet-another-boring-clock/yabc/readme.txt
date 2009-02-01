This is a project which I've been working on for some time. I call it "yet another boring clock" (YABC). "Yet another", because it's just another LED clock; "boring" since there is nothing significantly unique about it.

This grew out of some learning experiences:

* using the Z registers
* multiplexing LED displays
* hardware debouncing

Actually most of those were subsequent to each other. Once I started to understand the Z registers, it was only a natural to build a table for LED segments. Ultimately it grew into a clock.

When I had the firmware to a point of basic functionality, I decided to go full-bore and get a PCB fabbed. This was actually the hardest (and longest) part. I went thru three revisions.

I'd like to get some *constructive* feedback on this. I am sure there are a lot of things I did, which I should not have done; I am also sure there are things which I did not do, which I should have. There is undoubtedly a significant amount of improvement and optimization, but I would like to know that the basic structure works. It works for me, which is fine, but will it work for you? Does it work by design, or by coincidence? That's important.

I've included a few files in this archive:

* yabc.sch - Eagle (5.3.0) schematic
* yabc.brd - Eagle (5.3.0) board from above
* yabc.pdf - PDF of the schematic
* yabc-final-1.jpg - top view of the "final" project
* yabc-final-2.jpg - solder side view of the "final" project
* yabc-bom.htm - Eagle BOM
* yabc-bom.txt - an English interpretation of the Eagle BOM
* yabc.asm - AVR assembler code
* yabc.hex - Assembled code, suitable for flashing to a Mega32
