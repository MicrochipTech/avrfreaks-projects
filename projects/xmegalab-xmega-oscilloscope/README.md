# Xmegalab XMEGA Oscilloscope

Uploaded by ganzziani on 2010-08-24 20:17:00 (rating 0 out of 5)

## Summary

![](http://www.gabotronics.com/components/com_virtuemart/shop_image/product/resized/AVR_Xmegalab_4c2feb4fb3d5d_100x100.jpg)  

Firmware updated to rev. 1.02  

The [Xmegalab](http://www.gabotronics.com/development-boards/avr-xmegalab.htm) is a development board for AVR XMEGA microcontrollers, a dual channel oscilloscope, an arbitrary waveform generator and a scientific calculator.  

**Xmegalab features:**  

- ATXMEGA128A1 microcontroller with 128K Flash, 8K SRAM, 2K EEPROM  

- External serial SRAM 32K  

- Arbitrary Waveform Generator, 1MSPS, using the XMEGA's internal DAC  

- SD Card with FAT File System support  

- 4x3 Keypad plus 5 tactile switches  

- 2 Rotary encoders  

- Serial Port  

- JTAG and PDI interfaces for development  

- User external connection with digital I/Os or SPI interface  

- Graphic LCD 128x128 pixels with backlight  



| [book](http://gabotronics.com/index.php?option=com_content&view=article&id=71:xmegalab-manual&catid=2&Itemid=26) | [sch_icon](http://gabotronics.com/download/xmegalab/xmegalab-schematics.pdf) | [code-icon](http://gabotronics.com/download/xmegalab/xmegalab-source.zip) | [hex-icon](http://gabotronics.com/download/xmegalab/xmegalab-hex.zip) | [spreadsheet](http://gabotronics.com/download/xmegalab/xmegalab-bom.xls) | [movie](http://gabotronics.com/index.php?option=com_content&view=article&id=41&catid=1&Itemid=19) | [faq](http://gabotronics.com/index.php?option=com_content&view=article&id=70:xmegalab-faq&catid=3&Itemid=17) | [forum](http://gabotronics.com/component/option,com_ccboard/Itemid,12/forum,11/view,topiclist/) |
| [Manual](http://gabotronics.com/index.php?option=com_content&view=article&id=71:xmegalab-manual&catid=2&Itemid=26) | [Schematics](http://gabotronics.com/download/xmegalab/xmegalab-schematics.pdf) | [Code](http://gabotronics.com/download/xmegalab/xmegalab-source.zip) | [Hex](http://gabotronics.com/download/xmegalab/xmegalab-hex.zip) | [BOM](http://gabotronics.com/download/xmegalab/xmegalab-bom.xls) | [Videos](http://gabotronics.com/index.php?option=com_content&view=article&id=41&catid=1&Itemid=19) | [FAQ](http://gabotronics.com/index.php?option=com_content&view=article&id=70:xmegalab-faq&catid=3&Itemid=17) | [Forums](http://gabotronics.com/component/option,com_ccboard/Itemid,12/forum,11/view,topiclist/) |




| **Oscilloscope specifications:**
- 2 Analog Channels, 4 Digital Channels
- Sampling rate: 16MSPS
- Analog Bandwidth: 2MHz
- Resolution: 8bits
- Input Impedance: 1MÎ©
- Buffer size per channel: 256 | **AWG specifications:**
- 1 Analog Channel
- Maximum conversion rate: 1MSPS
- Analog Bandwidth: 66kHz
- Resolution: 8bits
- Low output impedance
- Buffer size: 256 |

  

There are five applications programmed on the device:  
  
**1) [SD](http://gabotronics.com/index.php?option=com_content&view=article&id=34:xmultikit-sd-card-demo-manual&catid=2&Itemid=26):** The SD Card demo is based on ChaN's FatFs demo project. The application will initialize the SD card and initialize the FAT system, then, HyperTerminal is used to interface with the SD Card.  

**2) [AWG](http://gabotronics.com/index.php?option=com_content&view=article&id=35:xmultikit-awg-manual&catid=2&Itemid=26):** An Arbitrary Waveform generator that outputs the standard waveforms of a function generator and can adjust the frequency, amplitude, offset and duty cycle of the selected waveform.  

**3) [MSO](http://gabotronics.com/index.php?option=com_content&view=article&id=36:xmultikit-mso-manual&catid=2&Itemid=26):** A Mixed Signal Oscilloscope. It has all the common features found on most digital scopes and more: Individual channel control, trigger control, math, spectrum analyzer, cursors, logic analyzer with parallel and serial decoding, waveform save to SD card, multimeter mode.  



AVR Freaks forum topic for the project [here](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=80681).

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
