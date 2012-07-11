# Light tower control via SMS

Uploaded by bbiandov on 2012-07-11 16:15:00 (rating 0 out of 5)

## Summary

For the longest time I wanted to build something using the Light Tower I got from HSC for $25 bucks â€“ you know one of those industrial tubes with stacked lights to indicate machine status based on colors just like a street light â€“ red means â€œwholly crapâ€, then you have yellow, green and blue. Who knows what blue is supposed to indicate but the tower is cool and very high-tech, if you know what I mean ïŠ


I also had Wavecom WMOi3 old GSM module, which is good for nothing these days of 3G and 4G so I figured I can use it as a cheap SMS terminal (I got it from eBay for $10 bucks back in the day)


Lastly since the tower is NOT based on LEDs I had to do something about the light bulb control and that happened to be yet another Opto22 board with 12v optical switches, that again came from the good old eBay for probably less than $10 although I really donâ€™t remember when I got it and what project it was supposed to â€œbeautifyâ€ back then.


The key of course is AT90S2313 running simple firmware to parse the SMS text and then turn ON or OFF various pins which control the Opto22 TTL switches. The switches then control the high current light bulbs. Very simple!


To summarize, the 3 main components are:


Futurlec board with Atmel AT90S2313 CPU and bunch of headers. Got it from ETTeam.com


Wavecom WMOi3 GSM module


Opto22 board for 12v high current lamp control (5A max I think)


If only the file upload feature would work I would love to post pictures and firmware :(


Till then the project will reside here:


[http://s193925491.onlinehome.us/...](http://s193925491.onlinehome.us/lighttower/)


Added 7/23/2010 regarding the 50-pin WMOi3 connector:


I get asked often the key question about Wavecom WMOi3 GSM module: where does one find that super small 50 pin board-to-board connector in such configuration as to solder those individual wires meaning the connector has the usual leads exposed after it fits into the module? Well I found it at HSC Electronics Supply, <http://www.halted.com/> for $0.20 each LOL


Added 7/10/2012 regarding how the WMOi3 module is configured:


Here is the dump straight out the module:


ATI6


DATA RATES: AUTOBAUD,300,1200,1200/75,2400,4800,9600,14400  

DATA MODES: T/NT,ASYNCHRONOUS  

FAX CLASS: 1,2


OK


ATI5


Q:0 V:1 S0:000 S2:043 S3:013 S4:010 S5:008  

+CR:0 +CRC:0 +CMEE:0 +CBST:0,0,1  

+SPEAKER:0 +ECHO:1,3 &C:1 &D:2 %C:0  

+IPR:9600 +ICF:3,4 +IFC:2,2


OK


ATI3


420\_09gm.2C 1171736 042301 17:30


OK


ATI0


 WAVECOM MODEM


 MULTIBAND 900E 1900


OK


When the module boots up here is what you should get if it registers successfully with the GSM carrier:


+WIND: 7


+CREG: 1,"178D","1678"


+WIND: 3


+WIND: 1


+CREG: 2


+WIND: 7


+CREG: 1,"178D","1678"

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
