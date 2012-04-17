What is Etherfeed ? 

The etherfeed is a small standalone network device, it connects to the Internet and displays statistical data about your website like number 
of unique visitors, number of pageviews, PayPal sales (SUM of sales via the PayPal IPN), Facebook likes for your brand / group and weather data 
like temperature, humidity for your location.

How it works?

The system consist of two parts. The first part is a set of php scripts responsible for collecting data from Google Analytics, PayPal, Facebook 
and Google weather data. These scripts shall be installed on the target website.

The second part is the etherfeed device which connects to the internet via a router, and access one of the above mentioned php scripts, collects 
the data and displays it on the etherfeed 16×1 Blue backlight LCD Display.

Key features / benefits:

- Displays Google Analytics data (visits, pageviews etc)
- Displays weather Data (source Google)
- Displays sum of PayPal IPN Sales
- open source and open hardware project


etherfeed version 1.0 firmware source code
------------------------------------------

The etherfeed project is based on the avr-uip port of the UIP protocol stack for the AVR microcontrollers (http://code.google.com/p/avr-uip/)

The current source code works on the etherfeed v1.0 hardware which includes:

- atmega644 microcontroller from ATMEL
- ENC28J60 Ethernet phy from Microchip
- 16x1 LCD (9.66mm characters with blue backlight) HD44780 based 
- power supply via USB / USB Adapter 5V DC

The etherfeed v1.0 hardware and software was completely tested.


Directory Structure
-------------------

hardware - contains the schematics and the production files of the etherfeed version 1.0

uip - The UIP TCPIP port of UIP for AVR microcontrollers

photos - sample photos of a etherfeed prototype device

libs - various libraries for lcd uart etc

drivers - contains the enc28j60 drivers

apps - the apps which run on the etherfeed (dhcpc, resolver web server and webclient)

project / etherfeed - the main project directory including the Makefiles

Compiler
--------

WinAVR - Download the latest version of WinAVR to compile / build the project





