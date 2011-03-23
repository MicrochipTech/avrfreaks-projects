# Bitmap Converter w/ RLE compress

Uploaded by ganzziani on 2011-03-23 00:21:00 (rating 0 out of 5)

## Summary

So you want to store one or a few bitmaps in your microcontroller but you find out that they need a lot of memory?  

For example, a 128x64 bitmap will need 1KB of memory, that is a lot memory for low pin count micros. A simple solution: compress the image. Note that the decoding algorithm has to be simple, there is no point on having a complex algorithm that will take a lot of program memory anyway. The algorithm of choice for simple monochrome bitmaps is Run Length Encoding (RLE). Like the name suggests, Run Length Encoding compresses the data by replacing runs of the same data with a counter.


More information can be found here: [Bitmap Converter](http://www.gabotronics.com/tutorials/run-length-encoding-for-lcd.htm)

## Tags

- General Lib.functions
