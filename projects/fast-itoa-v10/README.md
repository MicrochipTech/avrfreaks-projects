# fast itoa()  v1.0

Uploaded by admin on 2009-02-12 21:37:00 (rating 0 out of 5)

## Summary

\_itoa16() & \_itoa32() completely written in assembly language based on atmel's math32 app.note and they are much faster than original C itoa() & ltoa() functions(they are not completely functional as itoa() and ltoa()). the result for both functions is a null\_ended string with leading zeros placed in abuff[11] defined in "asm\_math.h" . with a little modifications in asm program you can provide more fuctionality from your routines.  

add asm\_math.c and asm\_math.h to your project(like sample fast\_itoa project)  

 Sim results(at 16Mhz):  

 itoa(32767,buff,10); 72.75 uS  

 \_itoa16(32767); 11.81 uS  

 ltoa(2123456789,buff,10); 397 uS  

 \_itoa32(2123456789); 62.38 uS


it's hard to add files to projects in avrfreaks!

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
