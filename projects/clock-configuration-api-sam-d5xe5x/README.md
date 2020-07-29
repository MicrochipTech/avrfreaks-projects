# Clock Configuration API for SAM D5x/E5x

Uploaded by scdoubleu on 2020-07-29 12:44:00 (rating 0 out of 5)

## Summary

Questions regarding how to configure the various clocks on SAM D5x/E5x are a frequent topic on the [SAM forum](https://community.atmel.com/atmel-smart-arm-based-mcus).


 


Many developers get by just fine using the Microchip/Atmel "Start" tool to configure clocking for their project. However, just as many seem to want to go beyond what the tool can offer; it can be somewhat uncooperative at times. This project is targeted at the latter group.


 


The header (sam\_clock.h) and source (sam\_clock.c) files provide a set of API functions to configure most of the clock facilities available on SAM D5x/E5x. Both files are reasonably well commented throughout so I won't go into details here, but the API documentation should be read in conjunction with the relevant sections of the data sheet. Not all hardware features are implemented (sam\_clock.h has a list), and of those which are implemented, not all combinations have been tested.


 


An example code fragment of how the API might be used on the SAME54-XplainedPro board is provided for reference purposes. It's not a complete example, but should be enough to get started.


 


The source code is provided As-Is, with no warranty or guarantee whatsoever that it will work, let alone compile (I use the [Gnu Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm), for reference). Use it entirely at your own risk. A two-clause BSD license applies.


 


Please direct feedback/questions to the forum, or to [steve@maverick-embedded.co.uk](mailto:steve@maverick-embedded.co.uk).


 


Steve (scdoubleu on the forum).

## Tags

- Part-specific Lib.functions
- SAME5x SAMD5x
- same5x samd5x clocks
