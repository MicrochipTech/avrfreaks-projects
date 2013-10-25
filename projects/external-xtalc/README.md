# external xtal.c

Uploaded by colldecarrera on 2013-10-24 20:43:00 (rating 0 out of 5)

## Summary

The code sets the proper oscillation registers to configure external clock. The frequency range of this external crystal should be comprised between 9 and 12 Mhz. Once the production signature row has been modified, the microcontroller will recognise the external crystal. After delay created by a for loop, pin D0 will toggle. To check if it really runs at 10 Mhz, rub out the functions concerning the production signature row. Once it is compiled and downloaded it will run at 2Mhz (default)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
