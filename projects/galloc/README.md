# galloc

Uploaded by curtavr on 2013-09-06 18:04:00 (rating 0 out of 5)

## Summary

So I am implementing some comm protocols to go to peripherals (and USB) and needed some generic memory allocation.


tried malloc.. mother of frank what a huge monstrosity that was. both in code size and memory overhead, not to mention the real problem of fragmentation.


so I wrote my own, it's very small, very low overhead, cannot fragment and is part of my larger project, a DNA board (which is not quite ready for prime-time) 


files (since that 'attach' doesn't seem to be working)  

[https://github.com/jingoro2112/d...](https://github.com/jingoro2112/dna/blob/master/firmware/dna/galloc.c)  

[https://github.com/jingoro2112/d...](https://github.com/jingoro2112/dna/blob/master/firmware/dna/galloc.h)


Docs:  

[http://northarc.com/ps/index.php...](http://northarc.com/ps/index.php/DNA_GALLOC)

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
