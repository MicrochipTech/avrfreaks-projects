# LCD2USB Button Fixed for Windows

Uploaded by MODASSAR027 on 2008-09-07 00:57:00 (rating 0 out of 5)

## Summary

It is actualy very simple. Just add this one function to the lcd2usb.cpp file:  

Code:  

unsigned short \_\_stdcall DISPLAYDLL\_ReadKey(void)  

{  

 int keymask = lcd\_get(LCD\_GET\_KEYS); 


 if(keymask > 0)  

 return (keymask & 3) + '0';  

 else  

 return 0xFF00;  

} 


And this line to LCD2USB.DEF 


Code:  

DISPLAYDLL\_ReadKey @11 


Recompile and that's it. 


Do thank MR Henk, as this fix was actully done by him i just found his linked and compled it for others as he was unable to upload the file. but provided the code modification. the original link of MR Henk is  

[http://forums.lcdsmartie.org/vie...](http://forums.lcdsmartie.org/viewtopic.php?t=1562)

## Tags

- Part-specific Lib.functions
