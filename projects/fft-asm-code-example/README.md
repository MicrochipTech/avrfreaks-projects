# FFT - asm code example

Uploaded by plasmaenergy on 2004-11-21 11:51:00 (rating 0 out of 5)

## Summary

FFT program written completely in assembly.  

627 words code  

352 word constants (including 128 words example-data)  

execution time: 86.1ms per MHz  

Input: 128 data-points  

Output: 64 Frequencies (complex data = Real and Imaginary part)  

Written an tested using AVR-Studio4 using ATMega8. Other MEGA-AVRs should work the same.


Program structure and variable names are taken from the FFT-algorithm found in "Numerical recipes in C".  

There the theory and mathematics are explained in detail (look for realft !).  

Code works with FFT-lenghts of nn2=128 and less.  

Input data is nn2 real data points with values ranging from -16384 to +16384.  

Output data overwrites input data and has the same range (-2^14 ... +2^14).  

Output-array is:  

f 0,f N/2), *  

Re(f 1),Im(f 1),  

Re(f 2),Im(f 2),  

Re(f 3),Im(f 3),  

.  

.  

.  

Re(f N/2-1),Im(f N/2-1)


*Note: the first 2 data-points in the Output data is already real-valued! 


To achieve simpler and faster code, I have omitted correct rounding when dividing by two. I have simply right-shifted the values. This is one reason why the accuracy is not as with your pentium8-processor. Nevertheless you can expect accuracy of +/-8 which gives you more dynamic range than the ADC of the AVR.  

I would recommend scaling the input-data until it fits approximately the input range. Then performing FFT. Afterwards scale back if necessary.


Your comments and questions are welcome. But don't ask me about FFT's in general! There are plenty of resources on the internet!

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
