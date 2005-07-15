# Driving a Crystalfontz LCD with a Mega 128

Uploaded by admin on 2005-07-15 08:36:00 (rating 0 out of 5)

## Summary

Recently i had a reason to use a Crystalfontz graphics LCD on a Mega 128, and had a few problems finding an easy (down to earth) driver anywhere on the net, so i bit the bullet and wrote my own. I thought i would share it with everyone else as i'm sure that someone else out there might want to do the same. It was written with the ICCAVR v6 compiler.


I happened to be using the Crystalfontz graphic LCD model (CFAG12232DYYHN) which can be seen at: [http://www.crystalfontz.com/prod...](http://www.crystalfontz.com/products/12232d/CFAG12232DYYHN.pdf)  

It is very cheap and reliable.


I was driving it with a Mega 128 clocked at 8Mhz, but i think the code should be flexible enough to handle any clock speed (no guarantees though!!).


After reading the manual for the LCD, i should tell you how i had the pins connected. (Warning - this will probably not make sense unless you have read the manual in the link provided above ^):


LCD Pin connectod to M128 Pin  

--------------------------------------  

R/W to PORTD6  

A0 to PORTG3  

E1 to PORTD1  

E2 to PORTG4  

D0->D7 to PORTF0->PORTF7


I have attached the files to this post, so you can download them. You will need to make your own main function from which you should call the lcd\_init() function which sets the lcd up, and then you can go ahead and print stuff to the LCD by using my:


void lcd\_printstring(int row, int col, char* string)  

function, or my


void lcd\_printnum(int row, int col, int num)  

function.


My routines allow you to print to 4 lines (rows) and up to 20 characters (colums). It does not word wrap, so don't bother trying to print more than 20 chars per line.


I apologise that the code is not well commented, but i'm sure it will save you a heap of time anyway.


Please feel free to contact me with any questions about it (or any feedback in general).


[les060@csiro.au](mailto:les060@csiro.au)

## Compilers

- ImageCraft ICCAVR V6

## Tags

- Part-specific Lib.functions
- ImageCraft ICCAVR V6
