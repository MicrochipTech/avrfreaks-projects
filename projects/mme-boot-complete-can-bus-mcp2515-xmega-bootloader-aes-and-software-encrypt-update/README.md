# MME-BOOT - Complete CAN BUS (MCP2515) XMega Bootloader with AES and software to encrypt & update

Uploaded by cmcraeslo on 2017-01-23 15:55:00 (rating 5 out of 5)

## Summary

 


I've recently migrated one of my devices to CAN BUS which meant that I had to build the firmware upgrade tool for my customers as well. I've looked all around the web and found few different versions, but none so complete that I could use. This is the reason I decided to make one and share this project with anyone that is doing the same.


 


**Main bootloader features:**


* Any ATXMega firmware upgrade over CAN BUS (MCP2515)
* Up to 255 different devices on the same bus can be updated using the same software
* AES CBC encrypted firmware
* Firmware version up to 4 bytes
* Firmware version stored in XMEGA's signature area
* CRC
* Bootloader waits 500ms after the boot to receive the signal from can bus by our upgrade tool. If not received, it boots to application.
* Any part of this firmware/tool can be used personal/commercial project, but it's not allowed to charge customers for this service (only upgrades)

 


**Windows C# (.NET 2.0) firmware encryption & update tool with full source included:**


* Encrypt any .hex flash file with AES KEY & Initialization vector (IV)
* Stores device id and authorization code into firmware file so single software can be used to update all devices on the can bus
* Options to specify old firmwares (saved into firmware file) from which upgrade can be done. - Option to generate random key & iv

 


You need any "USB to CAN Converter" that will allow reading and writing to CAN BUS over serial port. Device used in this project was <http://www.voblox.com/ucan>.


 


**Getting started:**


1. **​**Edit bootloader/config.h according to your needs, compile & write to device.   

 Don't forget to set fuses BOOTRST=BOOTLDR and the LOCK BITS to prevent firmware to be read back. BLBB=RLOCK, BLBA=RLOCK.   

 If you're not building firmware from my .cproj file, you need to add "-Wl,--section-start=.text=0x030000" in Toolchain/Linker/Miscellaneous.
2. Encrypt a .hex file using EncryptFirmware project. It will create .bin file that you will need when using update tool. Make sure the parameters from config.h of your device match!
3. Open UpgradeTool project.

* Add code for your USB to CAN converter.
* For reading, make sure OnPacketRead(id, dlc, data) is called whenever can bus packet is received. Same goes for SendPacket(id, dlc, data[])
* Other methods to look for (modify): FirmwareToString(), DeviceIdToString(), FirmwareIsAllowedToUpgradeTo()

 


You're done :)


 


If you use this project in your commercial project, consider donating any amount you see fit (my paypal address is [marko@mlakar.si](mailto:marko@mlakar.si)). Thanks :)


 


Screenshots:


[![](http://shrani.si/t/40/AQ/25XIcLLG/upgrade-tool.jpg)](http://shrani.si/?40/AQ/25XIcLLG/upgrade-tool.png)


[![](http://shrani.si/t/15/3X/3ia1zB2M/encrypt-tool.jpg)](http://shrani.si/?15/3X/3ia1zB2M/encrypt-tool.png)

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
- ATxmega
- bootloader canbus mcp2515 can-bus
