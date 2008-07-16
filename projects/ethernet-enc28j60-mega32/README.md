# ethernet enc28j60 mega32

Uploaded by guofang on 2008-07-16 14:31:00 (rating 0 out of 5)

## Summary

AVRVI Ethernet IO kit - USE AVR and ENC28J60


Introduction: 


Have you ever thought about connecting an 8bit uC to the Internet? There are already some projects out there that connect an avr uC to the Ethernet by using an old network card or the plain RTL8019 Ethernet controller. 


This board is a web server show how to use AVR and ENC28J60 to the Internet , remote Io control , file upload and download ,also Webcamra use MCA25 is here. 


What's on the board:


CPU: mega32(can expand to mega644)  

Ethernet control ：Enc28j60  

4M dataflash AT45DB041 and SD card ,for restore html or other files  

One RS232 interface  

2 relays for output  

2 P521 for input  

8 10bit AD interface  

SPI interface  

TWI interface  

PWM interface  

Features: 


The following key features have been implemented:


* Ethernet, ARP, IP, TCP, UDP Protocol  

* HTTP server  

* NTP client  

* webcam I/O (640x480px)  

* Servo motor control  

* Temperature logging and graphical output  

* I/O port control (read/write any pin)  

* second uart (using software, 19200baud)  

The main parts of the HTTP protocol are implemented in the webserver. You can view any stored website by accessing the webserver with any browser using HTTP GET. The server also supports file upload to the internal DataFlash storage by using HTTP POST, the same as uploading any file with a standard browser. Another important feature is the support of HTTP AUTH: In order to configure the webserver or to upload any file you need to authenticate yourself by using HTTP AUTH. If you request any website starting with "/adm/", your webbrowser will ask you log in with your password, which can be configured by the user.


The NTP client can update the internal clock by connecting to any NTP time server on the internet. The webserver is able to connect to any server in your  

local network as well as to any other server on the internet by using a configurable default gateway.


The webcam has a resolution of 640x480px and the image will be a JPG image in an html website. The download of a picture takes about 5 seconds. 


Another feature is the control of the webserver's I/O pins with any standard webbrowser. If you hook up a small relay to the I/O pins you can control your coffee machine from everywhere around the world (and watch it brew the coffee by using the webcam function, of course).


<http://www.avrvi.com>

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
