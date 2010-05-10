# Binary to ASCII Formatter

Uploaded by drmpftemp on 2010-05-10 08:49:00 (rating 0 out of 5)

## Summary

Binary to Ascii formatter


These routines provide a simple signed binary to Ascii formatter.  

The first routine converts a signed binary number upto 13bits to ASCII  

in a variety of formats, leading zeros, left or right justified  

The second routine converts 0 to 1000 in binary to 0.0% to 100.0%


The source code RemoteLedDriver contains the source and examples of use.


These are not the fastest or smallest routines but they are simple to understand and easy to modify to your needs.


Description of the code can be found at  

<http://www.forward.com.au>  

/MobilePhoneControlledLedDriver/MobilePhoneControlledLedDriverPart3.html 


//-----------------------------------------------------  

// CONVERT\_13BITS  

// Converts the, possiably signed, 13bit number in ZH:ZL  

// (actually -9999 to +9999)  

// and save acsii char in global SRAM whose start is pointed  

// to by XH:XL,  

// XH:XL decremented BEFORE each store of a char  

// If ZH:ZL is negative a leading '-' is stored  

//  

// Format of number is determined by uncommenting  

// one of the 3 #defines  

// #define CONVERT\_13BITS\_LEFT  

// left justifies the result eg |-21 and |2  

// #define CONVERT\_13BITS\_RIGHT  

// right justifies the result eg | -21 and | 2  

// #define CONVERT\_13BITS\_ZEROS  

// right justifies with leading zeros eg |-0021 and | 0002  

// if you uncomment more then one you will get a compile error (duplicate label)  

//  

// Numbers >9999 are output as 9999  

// Numbers <-9999 are output as -9999  

//  

// On return ZH:ZL is zero and XH:XL is updated to point to  

// the last char stored  

//  

// all used registers pushed and popped (saved and restored)  

//-----------------------------------------------------


Code update --- Here is a more compact test for negative and negate  

 // check if number to be converted is neg  

 // if so prepend - and negate  

 tst ZH  

 brpl CONVERT\_13BITS\_PAD\_NEG\_SIGN


 // else negate the input  

 // Temp zero from above  

 neg ZH  

 neg ZL  

 sbci ZH,0 // handle carry from neg ZL

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
