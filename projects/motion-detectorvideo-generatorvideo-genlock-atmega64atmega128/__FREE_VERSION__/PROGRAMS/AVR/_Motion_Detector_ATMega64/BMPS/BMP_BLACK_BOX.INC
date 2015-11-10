;+------------------------------------------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : BLACK_BOX.BMP
;|
;| Destiny
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : BMP_BLACK_BOX.INC
;|
;| Destiny Image Properties
;|     Size   :    20 Bytes
;|     Width  :    12 Pixels
;|     Height :    10 Pixels
;|     WidthB :     2 Bytes
;|     Colors :     2 Levels (1 bit)
;+------------------------------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                 
;               1 
;     012345678901
;     |         | 
;   0-############    -   0
;   1-############    -   1
;   2-############    -   2
;   3-############    -   3
;   4-############    -   4
;   5-############    -   5
;   6-############    -   6
;   7-############    -   7
;   8-############    -   8
;   9-############    -   9
;     |         | 
;                 
;               1 
;     012345678901

;( IMAGE DATA )

BMP_BLACK_BOX:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     12 ;Width Pixels
	.DW     10 ;Height Pixels
	.DW      2 ;Width Bytes
	.DW     20 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.DB 0x00,0x00,0x00,0x00

