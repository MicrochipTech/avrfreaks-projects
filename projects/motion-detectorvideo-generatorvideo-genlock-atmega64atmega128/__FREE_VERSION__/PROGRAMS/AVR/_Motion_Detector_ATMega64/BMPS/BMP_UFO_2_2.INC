;+------------------------------------------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : UFO_2_2.BMP
;|
;| Destiny
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : BMP_UFO_2_2.INC
;|
;| Destiny Image Properties
;|     Size   :    35 Bytes
;|     Width  :    40 Pixels
;|     Height :     7 Pixels
;|     WidthB :     5 Bytes
;|     Colors :     2 Levels (1 bit)
;+------------------------------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                                             
;               1         2         3         
;     0123456789012345678901234567890123456789
;     |         |         |         |         
;   0-                                        -   0
;   1-                                        -   1
;   2-                                        -   2
;   3-                                        -   3
;   4-                                        -   4
;   5-                                        -   5
;   6-########  ####      #       ##   #######-   6
;     |         |         |         |         
;                                             
;               1         2         3         
;     0123456789012345678901234567890123456789

;( IMAGE DATA )

BMP_UFO_2_2:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     40 ;Width Pixels
	.DW      7 ;Height Pixels
	.DW      5 ;Width Bytes
	.DW     35 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	.DB 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xC3
	.DB 0xF7,0xF3,0x80,0x00

