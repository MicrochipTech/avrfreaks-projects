# Dallas one-wire interface

Uploaded by Spamiam on 2008-10-06 22:43:00 (rating 0 out of 5)

## Summary

Dallas One-Wire low-level and high-level interface. The technique to address the physical I/O pins is tailored to the Oak Micros OM128 device, but more standard pin addressing can substituted easily. The timing was carefully measured to meet the 1-wire standard. Changes to the pin addressing may change the speed (probably ending up slightly faster), and this could affect the most time-critical functions.


Also, it is tailored for 14.7MHz, but there are comments on the clock counts for certain sections so that macros could be added to calculate the actual times.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
