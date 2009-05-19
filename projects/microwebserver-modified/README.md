# MicroWebServer, modified

Uploaded by davef on 2009-05-19 09:39:00 (rating 0 out of 5)

## Summary

The source files for MicroWebServer can be found at [http://en.klinkerstein.m-faq.de/...](http://en.klinkerstein.m-faq.de/index.php/MicroWebServer/Downloads). In enc28J60.c modify the #defines for the SPI ports of the uP used (ATmega32, in my case). If you have the ram available bump #define ENC28J60\_MAXFRAME up to 1000, things will work faster.


If using the ethernet module from Futurlec, note the SD0 and SDI connections are incorrectly labelled. 


The original code only allows a change in state on input and output ports of the uP (and changing DDR). This project has three significant modifications:


1 - I wanted to be able to send ADC values (an int or 5 characters), up to a max value of 32767 or with one decimal place 3276.7 to the Web browser.


2 - I wanted to be able to send 2 bytes to an 8bit 6 channel DAC from the Web browser. The main functions that required changes were CopyHttpData() and HttpDCall(), especially in STATE\_PARSEPOST.


3 â€“ I wanted to password protect my web server. See HttpDCall() and static char password[].


The main thing to keep in mind when sending out information, from the web server, is that you have to keep careful track of *ppData, *pDataLeft, pBuffer and nBytesProcessed. Especially note the manual decrement of nMaxBytes in CopyHttpData(). When parsing received information in HttpDCall(), again you have to keep careful track of variables nBytes, pSocket->nParseLeft and pBuffer.


pBuffer contains either the HTML file to transmit or the post message from the web browser. The post message is usually very short. As noted you have to keep ENC28J60 frame size more then the length of your post message, as there is no overrun test in my added code. 


As this TCP/IP implementation does NOT use content-length to determine the length of the POSTed message, always modify PARESBYTES as described in the note near the top of httpd.c before trying to debug the code. N.B. this behaviour also means that DAC values less than 100 need to have one zero entered in first and values less than 10 need to have two zeros entered in first to allow the POSTed message to upload without having to do a  in the web browser. 


The hard coded password is *********, changed as desired.


set and get functions for ADC and DAC data are possibly a bit OTT for a small embedded project.


I am in the process of incorporating uIPâ€™s SMTP example into the project , so that my webserver can send me an email message when alarms are set off.


Have fun.


As file upload doesn't appear to work, search AVR GCC for thread 


davef  

AVRfreaks.net

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
