# MAX7219 LED MATRIX clock with DS3231 and ATTINY 804 in ASSEMBLY

Uploaded by darth vader on 2021-12-06 05:19:00 (rating 0 out of 5)

## Summary

Chip attiny 804


atmel studio 7


assembler


DS3231 clock module


MAX7219 Lattice modules 4 in one display Digital tube display module Single chip module 8x8 common cathode (aliexpress) 2 pieces joined together for 8 blocks


[https://www.aliexpress.com/item/...](https://www.aliexpress.com/item/32907492270.html?spm=a2g0s.9042311.0.0.5bf74c4dHA30ix)


Menu function is available with momentary contact switches for "MENU" , "NEXT", " SAVE" buttons


;press menu button for a moment to enter menu function  

;press next button to change values of of the selected/displayed option (values increment and start from begining)  

;press save button to save selected value , the next option will be automatically loaded one by one (hour,min,day,month,date,year > time)


The clock displays time in HOUR:MINUTE:SECONDS format


After every 30-50 seconds the date is displayed in DATE:MONTH:YEAR format for 2 seconds and followed by day of week in SUNDAY......FRIDAY format for 2 seconds and again time will be displayed


Alarm function not implemented (may be in future)


 


; PA1 = MOSI (data)  

; PA3 = SCK   (clock)  

; PA4 = SS     (chip select)


; PA5 next button active 0 idle 1  

; PA6 save button active 0 idle 1  

; PA7 menu button active 0 idle 1


; PB0 I2C   SCL


; PB1 I2C   SDA

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
- ATTINY 804
- assembly
- MAX7219
- LED matrix
- ATTINY 804
- assembly
- MAX7219
- LED matrix
- Comfort and Control
