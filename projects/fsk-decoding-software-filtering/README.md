# FSK decoding with software filtering

Uploaded by barnacle on 2006-01-15 19:26:00 (rating 0 out of 5)

## Summary

edit: posted also on my web-site at [http://www.nailed-barnacle.co.uk...](http://www.nailed-barnacle.co.uk/callerid/avr/callerid.zip) as they don't seem to have stuck...


These are code snippets from an otherwise defunct project to decode BELL300 FSK  

for a US-style telephone caller ID. It includes the original C-code for an 80486  

from which the AVR assembly was derived.


Of particular interest is the very efficient (and unusual) implementation of the  

necessary DSP routines; a single sample from the 1200 baud code is decoded in  

approximately 50uS on a 16MHz Mega8 - approximately 50% processor usage.  

I have never seen this algorithm anywhere else, but I do know I had major  

headaches while I was trying to derive it.


To decode FSK, you need to delay the incoming signal by half a bit-period (the  

easy bit) and then low-pass filtering the result (the hard bit). At the end of  

the process the resulting signal is passed through a bit-slicer to determine the  

logic level of the original bit. In this case, the input signal is a unipolar  

8-bit sample but this is promoted to a signed 16-bit result; otherwise,  

signal-to-noise issues swamp the filter. Internally, the filter uses 16-bit  

signed scaled values; temporary accumulators and multiplication results are 32  

bits.


Digital filters can be implemented as either finite impulse response (FIR) or  

infinite impulse response (IIR) filters. IIR filters are difficult to design but  

 simple and fast to implement; FIR filters are easy to design but difficult to  

implement quickly. However, unconditionally stable IIR filters can be  

particularly hard to design so I used FIR.


FIR filters work by having a fifo shift register containing sequential samples  

of the signal to be filtered; and a similar sized array containing the filter  

coefficients. For each sample, the entire contents of the samples array is  

multiplied by the equivalent entry in the coefficient array. The results of  

these multiplications are then summed to provide the output term; thereafter,  

the contents of the fifo are shifted along one space and the next sample shifted  

in.


There are two slow functions here on a non-dsp processor; the  

multiply-accumulate stage and the shifting. There's not much you can do about  

the time it takes to complete a 16*16 -> 32 multiply, but it is possible to  

improve the basic algorithm.


A dedicated DSP chip will have both Multiply-Accumulate (MAC) instructions and  

variable length circular buffers. The AVR has neither. :(


Here's the basic algorithm to process a single sample... I assume a buffer  

length of 21 samples, which is about the minimum that works in this application.



```
<br />
on\_interrupt:<br />
{<br />
    for (q=19, q>=0, q--)<br />
    {<br />
        buffer[q+1] = buffer[q];<br />
    }<br />
    buffer[0] = new\_data\_sample;<br />
    output = 0;<br />
    for (q=0; q<21; q++)<br />
    {<br />
        output += buffer[q] * coeffs[q];<br />
    }<br />
    return output;<br />
}<br />

```



So the first attempt is to get rid of the shift operation. We don't have  

circular buffers but perhaps we can simulate one? In the original algorithm a  

single value acts as a pointer for both the data buffer and the coefficients,  

but we can decouple those and use a separate pointer for each:



```
<br />
{<br />
    // we don't need to shift any more<br />
    buffer[next\_input] = new\_data\_sample;<br />
    next\_input++;<br />
    // but we have to ensure we don't overflow<br />
    if (next\_input == num\_of\_entries)<br />
    {<br />
        next\_input = 0;<br />
    }<br />
    for (q=0; q<21; q++)<br />
    {<br />
        output += buffer[next\_input+q] * coeffs[q];<br />
        // check we're not out of range<br />
        if (next\_input+q > 20)<br />
        {<br />
            next\_input -= 21;<br />
        }<br />
    }<br />
    return output;<br />
}<br />

```



Which is an improvement but it's still not good enough. Apart from anything  

else, there's that horrible comparison required between every MAC cycle, takes  

precious time.


But if you take *two* copies of the coefficients, and butt-join them, then you  

no longer need to perform those comparisons. This is easiest to explain with an  

example - for now, let's assume that the filter length is just five elements:


We have two copies of the filter coefficients butted to each other


P Q R S T P' Q' R' S' T'


and one copy of the data buffer


A B C D E


we start with a pointer to P', and do the summation  

AP'+BQ'+CR'+DS'+ET'


next time we add new data to replace A


F B C D E 


decrement the pointer to T, and do the summation  

FT+BP'+CQ'+DR'+ES'


and so on. When the coefficient filter passes P we aim it at P' and in this way  

need not move any data.



```
<br />
// asssuming 21 coefficients again...</p>
<p>    buffer[next\_input] = new\_data\_sample;<br />
    next\_input++;<br />
    next\_coeff--;<br />
    // we still have to ensure we don't overflow the input buffer<br />
    if (next\_input == num\_of\_entries)<br />
    {<br />
        next\_input = 0;<br />
        next\_coeff = 21;<br />
    }<br />
    for (q=0; q<21; q++)<br />
    {<br />
        output += buffer[q] * coeffs[next\_coeff+q];<br />
    }<br />
    return output;<br />
}<br />

```



See the attached code for implementation details.


Neil

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
