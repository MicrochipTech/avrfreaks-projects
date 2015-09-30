# SH1106 reset and init (ASM)

Uploaded by RES on 2015-09-30 09:35:00 (rating 0 out of 5)

## Summary

Reset and initialization ASM routines for the SH1106 (Sino Wealth) OLED display controller.


Reset routine adjusted for 8 MHz AVR clock speed. OLED display: 1.3 inch Heltec 132x64 (connector at top, see picture)


 


Note: lower column address: 0x02 (POR = 0x00)


 


 


;* Reset SH1106 (Vdd1 2.4V - 3.5V)


reset\_SH1106:  

cbi PORTB, RST ;resetting SH1106  

ldi temp, 0x1b  

wait\_pulse:  

dec temp ;wait at least 5 us (during)  

brne wait\_pulse  

sbi PORTB, RST  

nop  

nop  

nop  

nop  

nop ;wait at least 1 us (ending)  

nop  

nop  

nop  

nop  

nop  

ret


 


 


 


 


 


 


 


;* Initialize SH1106: mapping, scan, display RAM


init\_sh1106:  

ldi data, 0xae ;display off  

rcall write\_command  

ldi data, 0xd5 ;set display clock divide ratio  

rcall write\_command  

ldi data, 0x50 ;(POR=0x50)  

rcall write\_command  

ldi data, 0xa8 ;set multiplex ratio  

rcall write\_command  

ldi data, 0x3f ;(POR=0x3f)  

rcall write\_command  

ldi data, 0xd3 ;set display offset  

rcall write\_command  

ldi data, 0x00 ;(POR=0x00)  

rcall write\_command  

ldi data, 0x40 ;set display start line  

rcall write\_command  

ldi data, 0xb0 ;set page (POR=0)  

rcall write\_command  

ldi data, 0x02 ;lower column address (+2)  

rcall write\_command  

ldi data, 0x10 ;higher column address  

rcall write\_command  

ldi data, 0xa1 ;set segment re-map (1=reverse)  

rcall write\_command  

ldi data, 0xc8 ;set COM output scan direction  

rcall write\_command  

ldi data, 0xda ;set common pads hw configuration  

rcall write\_command  

ldi data, 0x12 ;alternate (POR=0x12)  

rcall write\_command  

ldi data, 0x81 ;set contrast control mode  

rcall write\_command  

ldi data, 0x80 ;(POR=0x80)  

rcall write\_command  

ldi data, 0xd9 ;set pre-charge period mode  

rcall write\_command  

ldi data, 0x02 ;(POR=0x02)  

rcall write\_command  

ldi data, 0xdb ;set VCOMH de-select level  

rcall write\_command  

ldi data, 0x35 ;(POR=0x35)  

rcall write\_command  

ldi data, 0xa4 ;output follows RAM content  

rcall write\_command  

ldi data, 0xa6 ;set normal display  

rcall write\_command  

ret


Next, wait at least 100 msec, clear display RAM and turn display on (0xaf)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
- ATtiny45
- sh1106 reset initialization init OLED controller
