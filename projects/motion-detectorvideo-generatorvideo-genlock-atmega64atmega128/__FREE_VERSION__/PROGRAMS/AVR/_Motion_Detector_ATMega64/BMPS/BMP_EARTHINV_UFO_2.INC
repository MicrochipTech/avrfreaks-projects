;+------------------------------------------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : EARTHINV_UFO_2.BMP
;|
;| Destiny
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : BMP_EARTHINV_UFO_2.INC
;|
;| Destiny Image Properties
;|     Size   :    14 Bytes
;|     Width  :    11 Pixels
;|     Height :     7 Pixels
;|     WidthB :     2 Bytes
;|     Colors :     2 Levels (1 bit)
;+------------------------------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                
;               1
;     01234567890
;     |         |
;   0-###########     -   0
;   1-###########     -   1
;   2-## ##### ##     -   2
;   3-###     ###     -   3
;   4-##  # #  ##     -   4
;   5-# #  #  # #     -   5
;   6-###########     -   6
;     |         |
;                
;               1
;     01234567890

;( IMAGE DATA )

BMP_EARTHINV_UFO_2:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     11 ;Width Pixels
	.DW      7 ;Height Pixels
	.DW      2 ;Width Bytes
	.DW     14 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x00,0x00,0x20,0x80,0x1F,0x00,0x35,0x80,0x5B,0x40,0x00,0x00

