# Wireless Sensor Network-Soil-Moisture Monitoring System

Uploaded by abytom87 on 2018-01-15 07:29:00 (rating 0 out of 5)

## Summary

**Summary**


This project consists of two parts, one part is the sensor node which measures and processes sensor data and sends it to the cordinator node which sends data to LCD indicating the stataus of available moisture in the soil.


**Description**


Soilmoisture Sensornode - Soil moisture sensor is connected to an Arduino UNO, which is connected to an XBEE S1 unit, that transmits the measured,processed data via the Zigbee Protocol. The XBEE connected to this Arduino is configured via the XCTU Tool. The parameters needed to be configured are Coordinator/Endpoint (in this case its configured as Endpoint), PAN ID, Baudrate and Destination address. The rest of the parameters were not changed.


Receiver/Coordinator node - The second XBEE is connected to another Arduino and configured as Coordinator, the other parameters like PAN ID, Baudrate, Destination address should be same as endpoint. The Coordinator receives the payload and displays the value to LCD. The received data provides information to user as to how much moist/humid the soil is.


**Hardware used:**


Endpoint node:


- FC28 is used as a soil moisture sensor,


- Arduino UNO,


- XBEE Pro S1, with XBEE Arduino shield,


- Powerbank to supply energy for the node.


Coordinator node:


- Arduino UNO,


- XBEE Pro S1, with XBEE Arduino shield,


- LCD 1604 Display with PCF8574 chip,


- 2.2k Ohm Pull-up resistors, for I2C bus


Software used:


- Eclipse CDT ,


- AVR-GCC compiler, AVRDUDE to program,


- XCTU Software from Digikey was used to configure the XBEEs as either Endpoint/Coordinator.

## Links

- [Wireless Sensor Network-Soil-Moisture Monitoring System](https://github.com/abtom87/WirelessSensorNetwork-Soil-MoistureMonitoringsystem)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
- #Atmega328P
- #Atmega
- #Arduino
- xbee pro s1
- #moisture-sensor
- Industrial Automation
- Xbee
- #wireless #communication #multipletransmitters #multichannelreceiver #advice #newbie
