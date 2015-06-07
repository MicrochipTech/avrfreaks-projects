# avr32webboot

Uploaded by lsilberbauer on 2015-06-06 17:39:00 (rating 0 out of 5)

## Summary

**avr32webboot**


A HTML5 webinterface and bootloader for AVR32


Allows to flash an AVR32 via a nice looking HTML5 webinterface.


Simply upload a .srec file, test the new firmware and apply or reject changes. Uses FreeRTOS, LwIP, ASF and Miniport.


Contributions and ports welcome!


 


Features


* Runs on an EVK1100 Development Board
* FreeRTOS V8.2.0
* LwIP 1.4.1 + httpserver\_raw app + SSI + POST support
* .srec File Parser (checks parity and start address)
* Automatically creates a backup of the running program in AT45DBX external flash memory
* Implements a simply registry: Change parameters like IP address via the webinterface and tune your application on-the-fly
* Easy to use SSI (Server Side Includes) and HTTP-POST handling functions
* Application stub for easy extensibility

Requirements


* AVR Studio 6.2 with ASF (Atmel Software Framework)
* Atmel EVK1100 Demo Board
* JTAG programmer
* Ethernet cable from your PC to the Demo Board
* Recommended: Internet connection of your PC (via a second LAN adapter or WiFi) for HTML5 features
* Optional: A serial cable connected to UART1 with 115200 baud, 8N1 to display debug info

More on <https://github.com/lsilberbauer/avr32webboot>


![](https://www.avrfreaks.net/sites/default/files/start_0.png)


 


![](https://www.avrfreaks.net/sites/default/files/registry.png)


 


![](https://www.avrfreaks.net/sites/default/files/update.png)

## Links

- [GIT Repo](https://github.com/lsilberbauer/avr32webboot)

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
- AT32UC3A0512AU
- evk1100
