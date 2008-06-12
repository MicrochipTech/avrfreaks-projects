# Accumulated PWM sinewave

Uploaded by krazatchu on 2008-06-12 01:24:00 (rating 0 out of 5)

## Summary

This project generates a PWM Sinewave from a Tiny26...


The 64Mhz PLL clock on the Tiny26 is employed to give a 250Khz base frequency...  

64MHz / 256 = 250kHz


The sine is generated from a 1024 point lookup table...


As the requested frequency may not be a multiple of the base frequency, an accumulator is used to carry the overflow...  

Such that the following wave cycle will have a new set of points offset by the carry-over from the accumulator...


With 8 bits of fractional accumulation and a 1024 point datatable,  

It will realize minimum steps of 0.953674 Hz...


The Tiny26 has built in deadtime between it's inverted and non-inverted outputs..  

This is suitable for driving an H-bridge...


The control interface is parallel signed 8bit (PORTA)...  

Port A sets the step size from -128 to +127, where 1 step is 0.953674Hz  

The frequency step is active on INT0 (PB6)...


The code contains a better description of the accumulation...  

Source, Eagle and JPG schematic are included...


Happy New Years!!


UPDATE: 2008/6/12 -- V1.1  

-- added a zero crossing detector in software...  

-- this will be useful for doing phase measurements...  

-- output is on pin B4...

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
