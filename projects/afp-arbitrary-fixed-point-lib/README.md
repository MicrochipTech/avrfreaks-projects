# AFP - Arbitrary Fixed Point Lib

Uploaded by avra on 2009-12-14 12:18:00 (rating 0 out of 5)

## Summary

This is library to work with fast fixed point math. I've called it AFP (Arbitrary Fixed Point Math Library). In AFP the place of a decimal point can be set by you (that's why it's called arbitrary), but once you set it in initialization you are practically working with fixed point numbers. You can add fixed point numbers, subtract them, multiply, divide, compare, calculate sine, cosine and square root. That's quite a solid base, so it shouldn't be too hard to add anything you need to the library. If you have any additions, code size or speed improvements, bug reports or bug fixes, please let me know.


These are the examples of s15.16 and s21.10:


s15.16: fractional part is 2^16 = 65536 (each increment is 1/65536), and integer part is 2^15-1 = 32767  

S-IIIIIIIIIIIIIII-FFFFFFFFFFFFFFFF (each bit is represented by one letter)  

1 bit for sign, 15 bits for integer part, 16 bits for fractional part  

Initialization is done with a call to fixInit(16);


s21.10: fractional part is 2^10 = 1024 (each increment is 1/1024), and integer part is 2^21-1 = 2097151  

S-IIIIIIIIIIIIIIIIIIIII-FFFFFFFFFF (each bit is represented by one letter)  

1 bit for sign, 21 bits for integer part, 10 bits for fractional part  

Initialization is done with a call to fixInit(10);


You are not limited to just s15.16 and s21.10 - you can choose any number of bits for fractional part - from zero to 26 bits (at least 5 integer bits are needed for simple PI calculation used in code). This means that you are free to determine your precision.


Performance tips (a and b are fixed point numbers, n is integer):  

1. Instead of the fixAdd(a,b) or fixSub(a,b) just add or subtract fixed point integers as if they were normal long integers: c:=a+b or c:=a-b.  

2. Multiplying a fixed point integer with a natural number n (this is a normal or long integer) should not be done by converting the number n to a fixed point using IntToFix(n) or LongToFix() and then use the fixMul(a,IntToFix(n) or fixMulInt(a,n). Instead one should just multiply the fixed point integer with n. For example n * a = a + a + ... + a + a (add n times a to zero).  

3. Similar to point 2 dividing by a natural number (a normal int) can be done without converting the natural number to a fixed-point integer. For example a / n = fixDiv(a,IntToFix(n)) = fixDivInt(a,n).  

4. Zero in fixed point representation is 0. So a = 0 equals a = IntToFix(0) = LongToFix(0) = FloatToFix(0.0);  

5. One in fixed point representation is 2^(number of bits used for fractional part), so use fixOne instead of IntToFix(1). Since 1 is neutral for multiplication and division, this means that a = fixMul(a,fixOne) and a = fixDiv(a,fixOne). Remember that a = a * 1, but a <> a * fixOne since fixOne is also fixed point number. Basically, you have to use fixMul(a,b) and fixDiv(a,b) only when both a and b are fixed point numbers.  

6. Use fixPI, fixPIHALF, fixTWOPI and other math "constants" available in the unit instead of calculating them on your own. 


HISTORY:  

========  

v0.1 Initial Release  

v0.2 Fixed bug in FixToStr()  

v0.3 Fixed bugs in fixGetFractPart(), fixMul(), fixDiv()  

v0.4 Implemented Merlin's suggestions, added fixSin(), fixCos(), fixRadToDeg(), fixDegToRad(), fixGetPI()  

v0.5 Improved fixDiv(), added fixDivInt(), fixDivLong(). Other small changes.  

v0.6 Improved code size - Sin() and Cos() share the same code  

v0.7 Added fixCosD(), fixSinD(), fixMod(), fixModInt()  

v0.8 Small code size and speed optimizations, exposed math constants to the public  

v0.9 Added fixSin() and fixCos() now support any angle  

v1.0 Fixed fixSin() and fixCos() calculation, cleaned code a little  

v1.1 Added fixSqrt()


More details here:  

[http://forum.e-lab.de/topic.php?...](http://forum.e-lab.de/topic.php?t=2387&page=1#post6)  

[http://forum.e-lab.de/attachment...](http://forum.e-lab.de/attachment_processor.php?mode=implement_file&id=897)

## Compilers

- AVRco

## Tags

- General Lib.functions
- AVRco
