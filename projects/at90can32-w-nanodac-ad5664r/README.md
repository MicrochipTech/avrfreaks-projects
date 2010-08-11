# AT90CAN32 w/ nanoDAC AD5664R

Uploaded by jh.chen1 on 2010-08-11 02:05:00 (rating 0 out of 5)

## Summary

Developing a CAN node to output QUAD precision analogs. It was developed with build-in PWM channels (6 inside AT90CAN32): The PWM's frequency with 16bits PWM output was okay for directly output as DAC, but the PWM's ripple needs a long RC-time-constant to smooth the ripples, along with the in-accuracy issues while controlling the outer KW power peripherals. We have to utilize the AD5664R (ADI/nanoDAC) to accomplish the goal for precision/stability.


Looking into ADI's web site, we found nothing about the example codes, therefore we develop by ourselves. Here it is.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Part-specific Lib.functions
- WinAVR (GNU GCC)
