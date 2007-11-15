# VB terminal

Uploaded by newbie123 on 2007-11-15 11:42:00 (rating 0 out of 5)

## Summary

This terminal will transmit and receive hex files from your computer to your micro controller. The port and settings are pre-set to run on  

Com1, 9600, 8, n, 1.


I did this because I just couldn't find a half decent terminal that would  

stop the data from coming in too fast, causing lost data. In my avr code I buffer 16 bytes of data coming in from the application, then process the 16 bytes.


What this app will do is send one line of the hex file (16 bytes), and then wait 100mS before sending another one. This gives most avr programs enough time to do something with the data before being bombarded with more data.


I didn't want it too complicated, its very easy. 


If you need some library files then get the whole package here  

its 10MB!  

[http://rapidshare.com/files/6987...](http://rapidshare.com/files/69876174/Terminal_Setup1.5.zip)  

It is a full install and includes everything with all  

the library files for MSCOMM and progress bar, etc, etc....

## Tags

- Complete code
