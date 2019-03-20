# Lots of AVR C projects on AVR Mega1284p/644p and W5500 Ethernet adapter.

Uploaded by maxx_ir on 2019-03-20 10:00:00 (rating 0 out of 5)

## Summary

**Lots of AVR C projects on AVR Mega1284p/644p and Ethernet Wiznet W5500 SPI network adapter.**


 


 


 


[m1284p\_wiz5500](https://github.com/maxxir/m1284p_wiz5500)


 


[Kicad open hardware](https://github.com/maxxir/m1284p_wiz5500/tree/master/KiCad_M644_breakout_v1.2d)


 


**This is my own m644p/m1284p custom board looks like:**


![My  M1284p board](https://community.atmel.com/sites/default/files/M644_breakout_v1.2d-a_top.png "My  M1284p board")


 


**Tested system photo:**


![tested_system_photo](https://community.atmel.com/sites/default/files/tested_system_photo_02_tiny_0.png "tested_system_photo")


 


**Briefly:**


Lots of projects using ATMEGA 1284p(644p partially) and Ethernet Wiznet 5500 (based on W5500 EVB examples,


but heavily modified). Building in Eclipse Kepler with AVR-Eclipse plugin and avr-gcc 4.9.2 toolchain.


 


**What done ( In order of increasing complexity and date of creation ):**


 


1. [Wiznet Loopback TCP/UDP Static IP](https://github.com/maxxir/m1284p_wiz5500/tree/master/03_m1284p_WIZNET_loopback_STATIC_IP)


2. [Wiznet Loopback TCP/UDP DHCP IP](https://github.com/maxxir/m1284p_wiz5500/tree/master/04_m1284p_WIZNET_loopback_DHCP)


3. [DNS example](https://github.com/maxxir/m1284p_wiz5500/tree/master/05_m1284p_WIZNET_DNS_client)


4. [SNTP +  DNS example](https://github.com/maxxir/m1284p_wiz5500/tree/master/06_m1284p_WIZNET_DNS_SNTP_client)


5. [Telnet server example](https://github.com/maxxir/m1284p_wiz5500/tree/master/07_m1284p_WIZNET_telnets_basic)


6. [ICMP aka ping example](https://github.com/maxxir/m1284p_wiz5500/tree/master/08_m1284p_WIZNET_ICMP_aka_ping)


7. [Simple Web Server (one page with HTTP POST/GET queries)](https://github.com/maxxir/m1284p_wiz5500/tree/master/09_m1284p_WIZNET_simple_webserver)


8. [HTTPD Web Server with all contents in AVR FLASH (with AJAX queries)](https://github.com/maxxir/m1284p_wiz5500/tree/master/11_m1284p_WIZNET_HTTPServer_FLASH_pages)


9. [HTTPD Web Server with all content on SD card (Chang FAT FS lib using) (with AJAX queries)](https://github.com/maxxir/m1284p_wiz5500/tree/master/12_m1284p_WIZNET_HTTPServer_SDCARD_pages)


10. [FTPC example (only active mode sorry) with store content on SD card (console dialog from serial terminal like putty, terraterm etc..)](https://github.com/maxxir/m1284p_wiz5500/tree/master/14_m1284p_WIZNET_FTPC_FATFS)


11. [FTPD example (both active-passive modes working) with store content on SD card, checked on FTP clients: Windows 7 cmd, FAR, Total commander, WinSCP.](https://github.com/maxxir/m1284p_wiz5500/tree/master/15_m1284p_WIZNET_FTPD_FATFS)


12. [Combined HTTPD + FTPD for  dynamic upload WEB server pages, with all content on SD card (Chang FAT FS lib using) (with AJAX queries)](https://github.com/maxxir/m1284p_wiz5500/tree/master/16_m1284p_WIZNET_HTTPD_FTPD_FATFS_SDCARD)


13. ZEVERO SD PetitFS Bootloader for both [M1284p](https://github.com/maxxir/m1284p_wiz5500/tree/master/bootloader_zevero_sd_m1284p_make)/[M644p](https://github.com/maxxir/m1284p_wiz5500/tree/master/bootloader_zevero_sd_m644p_make)


14. Bootloaded code: Combined Wiznet Loopback TCP/UDP Static IP + FTPD server (for bootloading via FTP client) + SD-bootloader ZEVERO, working on [M644p](https://github.com/maxxir/m1284p_wiz5500/tree/master/18_m644p_BTLD_WIZNET_LOOPBACK_FTPD_FATFS_SDCARD)/[M1284p](https://github.com/maxxir/m1284p_wiz5500/tree/master/18_m1284p_BTLD_WIZNET_LOOPBACK_FTPD_FATFS_SDCARD)


15. [Bootloaded code: Combined HTTPD server (with AJAX) + FTPD server (for bootloading via FTP client and upload WEB server pages) + SD-bootloader ZEVERO, working on M1284p only](https://github.com/maxxir/m1284p_wiz5500/tree/master/17_m1284p_BTLD_WIZNET_HTTPD_FTPD_FATFS_SDCARD)


16. [IOT Blynk client Example with Blynk smartphone application (checked only Android side) - NO Arduino CODE, used Wiznet Sockets.](https://github.com/maxxir/m1284p_wiz5500/tree/master/19_m1284p_WIZNET_blynk)


17. [Bootloaded code: IOT BLYNK client combined with FTPD server (for bootloading via FTP client)+ SD-bootloader ZEVERO, working on M1284p only](https://github.com/maxxir/m1284p_wiz5500/tree/master/20_m1284p_BTLD_WIZNET_BLYNK_FTPD_FATFS_SDCARD)


18. [TFTP client](https://github.com/maxxir/m1284p_wiz5500/tree/master/21_m1284p_WIZNET_TFTP_client_FATFS)


19. [**(NEW)**MQTT client](https://github.com/maxxir/m1284p_wiz5500/tree/master/22_m1284p_WIZNET_MQTT)


 


[>>Blynk discussion your are welcome here.](https://community.blynk.cc/t/port-from-w5500-evb-to-atmega1284p-w5500-wiznet-sockets-library-without-arduino/35235)


 


**Blynk application screenshot:**


![Blynk AtMEGA1284+W5500 App](https://www.avrfreaks.net/sites/default/files/Screenshot_2019-03-18-13-37-20-278_cc.blynk_.png "Blynk AtMEGA1284+W5500 Application screenshot")


 


**What TODO:**


All done.


 


**Remark:**


[**m1284P schematic**](https://github.com/maxxir/m1284p_wiz5500/blob/master/KiCad_M644_breakout_v1.2d/Pictures/M644_breakout_v1.2d_schematic.png)


[**m1284P typical connection diagram**](https://github.com/maxxir/m1284p_wiz5500/blob/master/KiCad_M644_breakout_v1.2d/Pictures/M644_connection_schematic.png)


**[m1284P Board pinmap](https://github.com/maxxir/m1284p_wiz5500/blob/master/KiCad_M644_breakout_v1.2d/Pictures/M644_breakout_v1.2d_pinmap.png)**


**[Blynk IOT Application QR-code](https://github.com/maxxir/m1284p_wiz5500/blob/master/19_m1284p_WIZNET_blynk/Blynk_application/app2_m1284p_and_W5500_QR.png)**

## Links

- [Github software](https://github.com/maxxir/m1284p_wiz5500)
- [Github hardware](https://github.com/maxxir/m1284p_wiz5500/tree/master/KiCad_M644_breakout_v1.2d)
- [Blynk part discussion](https://community.blynk.cc/t/port-from-w5500-evb-to-atmega1284p-w5500-wiznet-sockets-library-without-arduino/35235)
- [Software and Hardware discussion (without Blynk)](https://www.avrfreaks.net/forum/need-w5500-example-c-tcp)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
- ATmega1284P
- ATmega644P
- Industrial Automation
- W5500
- Blynk
- FTP
- HTTP
- bootloader
- IoT
- WIZnet
