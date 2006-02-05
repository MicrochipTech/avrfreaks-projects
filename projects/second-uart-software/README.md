# A second UART in software

Uploaded by danni on 2006-02-04 19:50:00 (rating 0 out of 5)

## Summary

My software UART use T1 and the ICP input to receive and one compare output to transmit. Thus no jitter and no jitter accumulation occur during transmit.


Other interrupts are allowed as long the receive interrupt was no longer delayed as 1/2 bit time and the transmit interrupt no longer as one bit time.


Peter

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
