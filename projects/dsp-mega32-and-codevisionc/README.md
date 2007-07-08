# DSP for Mega32 and CodevisionC

Uploaded by admin on 2007-07-08 16:22:00 (rating 0 out of 5)

## Summary

Floating point arithmetic is too slow for small, 8-bit processors to handle, except when human interaction is involved. However in fast loops, such as IIR filters or animation, you are going to need to use fixed point arithmetic. This page  

[http://instruct1.cit.cornell.edu...](http://instruct1.cit.cornell.edu/courses/ee476/Math/index.html)  

implements an optimized assembly-level multiply, assembly-level digital filtering, Fast Fourier Transform, Fast Walsh Transform, Discrete Cosine Transform, and sine/cosine functions. All calculations are done in 8:8 format (16 bits with 8 bits of fraction), 2's complement arithmetic. The FFT runs in real time at 8kHz sampling rate and 128 point transforms.

## Links

- [DSP for Mega32](http://instruct1.cit.cornell.edu/courses/ee476/Math/avrDSP.htm)

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
