# Code generator for math-function

Uploaded by Trolll on 2013-04-02 18:59:00 (rating 0 out of 5)

## Summary

When programming microcontrollers sometimes there is a problem of calculation of math functions. And despite the fact that the standard AVR GCC library includes support for float numbers and basic math calculations, in practice is not always possible to use these routines. Because AVR microcontrollers do not have a math coprocessor and instructions for operations with float numbers, the use of standard library routines leads to the generation of relatively cumbersome and slow code. Therefore, to calculate the "complex" mathematical functions often resort to the table method. In this case function pre-calculated and the values are written to the array, and then taken out of it. To reduce the size of the array, you can use linear interpolation. This online-calculator allows generate source code for any functions. It automatically selects the type of the argument and the function return value based on the entered data.


 


Home page: [http://trolsoft.ru/en/articles/a...](http://trolsoft.ru/en/articles/avr-calc-table-functions/)

## Links

- [AVR math functions code generator](http://trolsoft.ru/en/articles/avr-calc-table-functions/)

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
