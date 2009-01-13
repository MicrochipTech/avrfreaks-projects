# taskswitch

Uploaded by Mickjc75 on 2009-01-13 14:01:00 (rating 0 out of 5)

## Summary

I wanted to be able to have 2 or 3 threads running, in the simplest way possible.


No operating system  

No timer ticks  

No thread priority  

No event posting  

No seperate stack for ISR's  

Minimum assembler


This is my example of how to do it, I was looking for something like this and didn't find it, so I thought it might help somebody else if I shared it.


My example is a WinAVR project for atmega128, it creates two additional threads with stack sizes 256 and 150 and simply spits some strings out of uart1.


Feel free to ask any q's or suggest improvements, and PLEASE rate it! as it's my first thing to put on this site.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
