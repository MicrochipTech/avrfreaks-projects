# Nokia 3310 LCD Driver

Uploaded by mjag17 on 2013-08-10 07:15:00 (rating 5 out of 5)

## Summary

[20041009 -- Update]  

Included some dependencies that were missing (typedefs from procyon avrlib). Also added some other code I was working on but didn't include originally.


Driver for PCD8544, the controller used in the Nokia 3310 lcd. Plans on how to connect it are available on the net. Look in the pcd8544.h file for config. Sample file lcd\_test.c included.


Includes two tiny unreadable (nearly anyway;) fonts, though you can create your own as well.


The code is quite bloated (well framebuffer takes up quite a bit too :) so you'll need an AVR with 1k ram (you can get away with 512b, with some tricks)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
