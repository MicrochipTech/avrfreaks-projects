# T1compare.c

Uploaded by colldecarrera on 2013-10-25 17:50:00 (rating 0 out of 5)

## Summary

Timer C1 increments at 1us. The actual value of this timer is compared with the PER register's value. When both are equal, the microcontroller generates an interruption toggling bit C0. As the PER register is 128, the period of the output signal should be 256 us or a frequency of 3906 Hz. This can be check easily with the help of an oscilloscope.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
