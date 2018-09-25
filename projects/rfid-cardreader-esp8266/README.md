# RFID CardReader with ESP8266

Uploaded by sergei_iliev on 2018-09-25 07:39:00 (rating 0 out of 5)

## Summary

RFID Card Reader powered by Acorn kernel RTOS. 


WiFi over ESP8266 is used to transfer card data.


The kernel reads data from the card and construct a REST service request to a remote web based server. 


REST service response is parsed to inspect validity of the code.


Android program is used to set proper TCP configuration values(IP,port,uri root,device id) into CPU eerpom.


 


Schematic and board pcb made by myNetPCB.


[Download](https://github.com/sergei-iliev/acorn-kernel-samples/tree/master/WiFiWebCardReader)


 


![](https://github.com/sergei-iliev/acorn-kernel-samples/raw/master/screenshots/wifiwebcardreader.jpg)

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
- acorn kernel
- acorn kernel
